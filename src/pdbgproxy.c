#define _GNU_SOURCE
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <inttypes.h>
#include <assert.h>
#include <getopt.h>
#include <errno.h>
#include <byteswap.h>
#include <ccan/array_size/array_size.h>

#include <bitutils.h>
#include <hwunit.h>

#include "pdbgproxy.h"
#include "main.h"
#include "optcmd.h"
#include "debug.h"
#include "path.h"
#include "sprs.h"

#ifndef DISABLE_GDBSERVER

/* Maximum packet size */
#define BUFFER_SIZE    	8192

/* GDB packets */
#define STR(e) "e"
#define ACK "+"
#define NACK "-"
#define OK "OK"
#define TRAP "S05"
#define ERROR(e) "E"STR(e)

#define TEST_SKIBOOT_ADDR 0x40000000

static struct pdbg_target *thread_target = NULL;
static struct pdbg_target *adu_target;
static struct timeval timeout;
static int poll_interval = 100;
static int fd = -1;
static int littleendian = 1;
enum client_state {IDLE, SIGNAL_WAIT};
static enum client_state state = IDLE;

static void destroy_client(int dead_fd);

static uint8_t gdbcrc(char *data)
{
	uint8_t crc = 0;
	int i;

	for (i = 0; i < strlen(data); i++)
		crc += data[i];

	return crc;
}

static void send_response(int fd, char *response)
{
	int len;
	char *result;

	len = asprintf(&result, "$%s#%02x", response, gdbcrc(response));
	PR_INFO("Send: %s\n", result);
	send(fd, result, len, 0);
	free(result);
}

void send_nack(void *priv)
{
	PR_INFO("Send: -\n");
	send(fd, NACK, 1, 0);
}

void send_ack(void *priv)
{
	PR_INFO("Send: +\n");
	send(fd, ACK, 1, 0);
}

static void set_thread(uint64_t *stack, void *priv)
{
	send_response(fd, OK);
}

static void stop_reason(uint64_t *stack, void *priv)
{
	send_response(fd, TRAP);
}

static void detach(uint64_t *stack, void *priv)
{
	PR_INFO("Detach debug session with client. pid %16" PRIi64 "\n", stack[0]);
	send_response(fd, OK);
}

#define POWER8_HID_ENABLE_ATTN			PPC_BIT(31)

#define POWER9_HID_ENABLE_ATTN			PPC_BIT(3)
#define POWER9_HID_FLUSH_ICACHE			PPC_BIT(2)

static int set_attn(bool enable)
{
	uint64_t hid;

	if (thread_getspr(thread_target, SPR_HID, &hid))
		return -1;

	if (pdbg_target_compatible(thread_target, "ibm,power8-thread")) {
		if (enable) {
			if (hid & POWER8_HID_ENABLE_ATTN)
				return 0;
			hid |= POWER8_HID_ENABLE_ATTN;
		} else {
			if (!(hid & POWER8_HID_ENABLE_ATTN))
				return 0;
			hid &= ~POWER8_HID_ENABLE_ATTN;
		}
	} else if (pdbg_target_compatible(thread_target, "ibm,power9-thread")) {
		if (enable) {
			if (hid & POWER9_HID_ENABLE_ATTN)
				return 0;
			hid |= POWER9_HID_ENABLE_ATTN;
		} else {
			if (!(hid & POWER9_HID_ENABLE_ATTN))
				return 0;
			hid &= ~POWER9_HID_ENABLE_ATTN;
		}
		hid |= POWER9_HID_FLUSH_ICACHE;
	} else {
		return -1;
	}

	if (thread_putspr(thread_target, SPR_HID, hid))
		return -1;

	return 0;
}

/* 32 registers represented as 16 char hex numbers with null-termination */
#define REG_DATA_SIZE (32*16+1)
static void get_gprs(uint64_t *stack, void *priv)
{
	char data[REG_DATA_SIZE] = "";
	struct thread_regs regs;
	int i;

	if(thread_getregs(thread_target, &regs))
		PR_ERROR("Error reading gprs\n");

	for (i = 0; i < 32; i++) {
		PR_INFO("r%d = 0x%016" PRIx64 "\n", i, regs.gprs[i]);
		snprintf(data + i*16, 17, "%016" PRIx64 , be64toh(regs.gprs[i]));
	}

	send_response(fd, data);
}

static void get_spr(uint64_t *stack, void *priv)
{
	char data[REG_DATA_SIZE];
	uint64_t value;

	switch (stack[0]) {
	case 0x40:
		/* Get PC/NIA */
		if (thread_getnia(thread_target, &value))
			PR_ERROR("Error reading NIA\n");
		snprintf(data, REG_DATA_SIZE, "%016" PRIx64 , be64toh(value));
		send_response(fd, data);
		break;

	case 0x41:
		/* Get MSR */
		if (thread_getmsr(thread_target, &value))
			PR_ERROR("Error reading MSR\n");
		snprintf(data, REG_DATA_SIZE, "%016" PRIx64 , be64toh(value));
		send_response(fd, data);
		break;

	case 0x42:
		/* Get CR */
		if (thread_getcr(thread_target, (uint32_t *)&value))
			PR_ERROR("Error reading CR \n");
		snprintf(data, REG_DATA_SIZE, "%016" PRIx64 , be64toh(value));
		send_response(fd, data);
		break;

	case 0x43:
		/* Get LR */
		if (thread_getspr(thread_target, 8, &value))
			PR_ERROR("Error reading LR\n");
		snprintf(data, REG_DATA_SIZE, "%016" PRIx64 , be64toh(value));
		send_response(fd, data);
		break;

	case 0x44:
		/* Get CTR */
		if (thread_getspr(thread_target, 9, &value))
			PR_ERROR("Error reading CTR\n");
		snprintf(data, REG_DATA_SIZE, "%016" PRIx64 , be64toh(value));
		send_response(fd, data);
		break;

	case 0x45:
		/* We can't get the whole XER register in RAM mode as part of it
		 * is in latches that we need to stop the clocks to get. Probably
		 * not helpful to only return part of a register in a debugger so
		 * return unavailable. */
		send_response(fd, "xxxxxxxxxxxxxxxx");
		break;

	default:
		send_response(fd, "xxxxxxxxxxxxxxxx");
		break;
	}
}

#define MAX_DATA 0x1000

/* Returns a real address to use with mem_read or -1UL if we
 * couldn't determine a real address. At the moment we only deal with
 * kernel linear mapping but in future we could walk that page
 * tables. */
static uint64_t get_real_addr(uint64_t addr)
{
	if (GETFIELD(PPC_BITMASK(0, 3), addr) == 0xc)
		/* Assume all 0xc... addresses are part of the linux linear map */
		addr &= ~PPC_BITMASK(0, 1);
	else if (addr < TEST_SKIBOOT_ADDR)
		return addr;
	else
		addr = -1UL;

	return addr;
}

/*
 * TODO: Move write_memory and read_memory into libpdbg helper and advertise
 * alignment requirement of a target, or have the back-ends use it directly
 * (latter may have the problem that implicit R-M-W is undesirable at very low
 * level operations if we only wanted to write).
 */

/* WARNING: _a *MUST* be a power of two */
#define ALIGN_UP(_v, _a)	(((_v) + (_a) - 1) & ~((_a) - 1))
#define ALIGN_DOWN(_v, _a)	((_v) & ~((_a) - 1))

static int write_memory(uint64_t addr, uint64_t len, void *buf, size_t align)
{
	uint64_t start_addr, end_addr;
	void *tmpbuf = NULL;
	void *src;
	int err = 0;

	start_addr = ALIGN_DOWN(addr, align);
	end_addr = ALIGN_UP(addr + len, align);

	if (addr != start_addr || (addr + len) != end_addr) {
		tmpbuf = malloc(end_addr - start_addr);
		if (mem_read(adu_target, start_addr, tmpbuf, end_addr - start_addr, 0, false)) {
			PR_ERROR("Unable to read memory for RMW\n");
			err = -1;
			goto out;
		}
		memcpy(tmpbuf + (addr - start_addr), buf, len);
		src = tmpbuf;
	} else {
		src = buf;
	}

	if (mem_write(adu_target, start_addr, src, end_addr - start_addr, 0, false)) {
		PR_ERROR("Unable to write memory\n");
		err = -1;
	}

out:
	if (tmpbuf)
		free(tmpbuf);

	return err;
}

static int read_memory(uint64_t addr, uint64_t len, void *buf, size_t align)
{
	uint64_t start_addr, end_addr;
	void *tmpbuf = NULL;
	void *dst;
	int err = 0;

	start_addr = ALIGN_DOWN(addr, align);
	end_addr = ALIGN_UP(addr + len, align);

	if (addr != start_addr || (addr + len) != end_addr) {
		tmpbuf = malloc(end_addr - start_addr);
		dst = tmpbuf;
	} else {
		dst = buf;
	}

	if (mem_read(adu_target, start_addr, dst, end_addr - start_addr, 0, false)) {
		PR_ERROR("Unable to read memory\n");
		err = -1;
		goto out;
	}

	if (addr != start_addr || (addr + len) != end_addr)
		memcpy(buf, tmpbuf + (addr - start_addr), len);

out:
	if (tmpbuf)
		free(tmpbuf);

	return err;
}


static void get_mem(uint64_t *stack, void *priv)
{
	uint64_t addr, len, linear_map;
	int i, err = 0;
	uint64_t data[MAX_DATA/sizeof(uint64_t)];
	char result[2*MAX_DATA];

	/* stack[0] is the address and stack[1] is the length */
	addr = stack[0];
	len = stack[1];

	if (len > MAX_DATA) {
		PR_INFO("Too much memory requested, truncating\n");
		len = MAX_DATA;
	}

	if (!addr) {
		err = 2;
		goto out;
	}

	linear_map = get_real_addr(addr);
	if (linear_map != -1UL) {
		if (read_memory(linear_map, len, data, 1)) {
			PR_ERROR("Unable to read memory\n");
			err = 1;
		}
	} else {
		/* Virtual address */
		for (i = 0; i < len; i += sizeof(uint64_t)) {
			if (thread_getmem(thread_target, addr, &data[i/sizeof(uint64_t)])) {
				PR_ERROR("Fault reading memory\n");
				err = 2;
				break;
			}
		}
	}

out:
	if (!err)
		for (i = 0; i < len; i ++) {
			sprintf(&result[i*2], "%02x", *(((uint8_t *) data) + i));
		}
	else
		sprintf(result, "E%02x", err);

	send_response(fd, result);
}

static void put_mem(uint64_t *stack, void *priv)
{
	uint64_t addr, len;
	uint8_t *data;
	uint8_t attn_opcode[] = {0x00, 0x00, 0x02, 0x00};
	uint8_t gdb_break_opcode[] = {0x7d, 0x82, 0x10, 0x08};
	int err = 0;

	if (littleendian) {
		attn_opcode[1] = 0x02;
		attn_opcode[2] = 0x00;
	}

	addr = stack[0];
	len = stack[1];
	data = (uint8_t *)(unsigned long)stack[2];

	addr = get_real_addr(addr);
	if (addr == -1UL) {
		PR_ERROR("TODO: No virtual address support for putmem\n");
		err = 1;
		goto out;
	}

	if (len == 4 && !memcmp(data, gdb_break_opcode, 4)) {
		/* According to linux-ppc-low.c gdb only uses this
		 * op-code for sw break points so we replace it with
		 * the correct attn opcode which is what we need for
		 * breakpoints.
		 *
		 * TODO: Upstream a patch to gdb so that it uses the
		 * right opcode for baremetal debug. */
		PR_INFO("Breakpoint opcode detected, replacing with attn\n");
		memcpy(data, attn_opcode, 4);

		/* Need to enable the attn instruction in HID0 */
		if (set_attn(true)) {
			err = 2;
			goto out;
		}
	}

	if (write_memory(addr, len, data, 8)) {
		PR_ERROR("Unable to write memory\n");
		err = 3;
	}

out:
	free(data); // allocated by gdb_parser.rl

	if (err)
		send_response(fd, ERROR(EPERM));
	else
		send_response(fd, OK);
}

static void v_conts(uint64_t *stack, void *priv)
{
	thread_step(thread_target, 1);
	send_response(fd, TRAP);
}

#define VCONT_POLL_DELAY 100000
static void v_contc(uint64_t *stack, void *priv)
{
	thread_start(thread_target);
	state = SIGNAL_WAIT;
	poll_interval = 1;
}

static void interrupt(uint64_t *stack, void *priv)
{
	struct thread_state status;

	PR_INFO("Interrupt from gdb client\n");

	thread_stop(thread_target);

	status = thread_status(thread_target);
	if (!(status.quiesced)) {
		PR_ERROR("Could not quiesce thread\n");
		return;
	}
	state = IDLE;
	poll_interval = VCONT_POLL_DELAY;
	send_response(fd, TRAP);
}

static void poll(void)
{
	uint64_t nia;
	struct thread_state status;

	thread_target->probe(thread_target);
	status = thread_status(thread_target);

	switch (state) {
	case IDLE:
		break;

	case SIGNAL_WAIT:
		if (!(status.quiesced))
			break;

		set_attn(false);

		state = IDLE;
		poll_interval = VCONT_POLL_DELAY;
		if (!(status.active)) {
			PR_ERROR("Thread inactive after trap\n");
			send_response(fd, ERROR(EPERM));
			return;
		}

		/* Restore NIA */
		if (thread_getnia(thread_target, &nia))
			PR_ERROR("Error during getnia\n");
		if (thread_putnia(thread_target, nia - 4))
			PR_ERROR("Error during putnia\n");
		send_response(fd, TRAP);
		break;
	}
}

static void cmd_default(uint64_t *stack, void *priv)
{
	uintptr_t tmp = stack[0];
	if (stack[0]) {
		send_response(fd, (char *) tmp);
	} else
		send_response(fd, "");
}

static void create_client(int new_fd)
{
	PR_INFO("Client connected\n");
	fd = new_fd;
}

static void destroy_client(int dead_fd)
{
	PR_INFO("Client disconnected\n");
	close(dead_fd);
	fd = -1;
}

static int read_from_client(int fd)
{
	char buffer[BUFFER_SIZE];
	int nbytes;

	nbytes = read(fd, buffer, sizeof(buffer) - 1);
	if (nbytes < 0) {
		perror(__FUNCTION__);
		return -1;
	} else if (nbytes == 0) {
		PR_INFO("0 bytes\n");
		return -1;
	} else {
		int i;

		buffer[nbytes] = '\0';
		PR_INFO("Recv: %s\n", buffer);
		pdbg_log(PDBG_DEBUG, " hex: ");
		for (i = 0; i < nbytes; i++)
			pdbg_log(PDBG_DEBUG, "%02x ", buffer[i]);
		pdbg_log(PDBG_DEBUG, "\n");

		parse_buffer(buffer, nbytes, &fd);
	}

	return 0;
}

static command_cb callbacks[LAST_CMD + 1] = {
	cmd_default,
	get_gprs,
	get_spr,
	get_mem,
	stop_reason,
	set_thread,
	v_contc,
	v_conts,
	put_mem,
	interrupt,
	detach,
	NULL};

int gdbserver_start(struct pdbg_target *thread, struct pdbg_target *adu, uint16_t port)
{
	int sock, i;
	struct sockaddr_in name;
	fd_set active_fd_set, read_fd_set;

	parser_init(callbacks);
	thread_target = thread;
	adu_target = adu;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror(__FUNCTION__);
		return -1;
	}

	name.sin_family = AF_INET;
	name.sin_port = htons(port);
	name.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock, (struct sockaddr *) &name, sizeof(name)) < 0) {
		perror(__FUNCTION__);
		return -1;
	}

	if (listen(sock, 1) < 0) {
		perror(__FUNCTION__);
		return -1;
	}

	printf("gdbserver: listening on port %d\n", port);

	FD_ZERO(&active_fd_set);
	FD_SET(sock, &active_fd_set);

	while (1) {
		read_fd_set = active_fd_set;
		timeout.tv_sec = 0;
		timeout.tv_usec = poll_interval;
		if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, &timeout) < 0) {
			perror(__FUNCTION__);
			return -1;
		}

		for (i = 0; i < FD_SETSIZE; i++) {
			if (FD_ISSET(i, &read_fd_set)) {
				if (i == sock) {
					char host[NI_MAXHOST];
					struct sockaddr saddr;
					socklen_t slen;
					int new;

					new = accept(sock, &saddr, &slen);
					if (new < 0) {
						perror(__FUNCTION__);
						return -1;
					}

					if (getnameinfo(&saddr, slen,
							host, sizeof(host),
							NULL, 0,
							NI_NUMERICHOST) == 0) {
						printf("gdbserver: connection from gdb client %s\n", host);
					}

					if (fd > 0) {
						/* It only makes sense to accept a single client */
						printf("gdbserver: another client already connected\n");
						close(new);
					} else {
						create_client(new);
						FD_SET(new, &active_fd_set);
					}
				} else {
					if (read_from_client(i) < 0) {
						destroy_client(i);
						FD_CLR(i, &active_fd_set);
						printf("gdbserver: ended connection with gdb client\n");
					}
				}
			}
		}

		poll();
	}

	return 1;
}

static int gdbserver(uint16_t port)
{
	struct pdbg_target *target, *adu, *thread = NULL;
	uint64_t msr;
	int rc;

	for_each_path_target_class("thread", target) {
		if (pdbg_target_probe(target) != PDBG_TARGET_ENABLED)
			continue;

		if (!thread) {
			thread = target;
		} else {
			fprintf(stderr, "GDB server cannot be run on multiple threads at once.\n");
			return 0;
		}
	}

	if (!thread) {
		fprintf(stderr, "No thread selected\n");
		return 0;
	}

	if (!pdbg_target_compatible(thread, "ibm,power8-thread") &&
	    !pdbg_target_compatible(thread, "ibm,power9-thread")) {
		PR_ERROR("GDBSERVER is only available on POWER8 and POWER9\n");
		return -1;
	}

	if (pdbg_target_compatible(thread, "ibm,power9-thread")) {
		/*
		 * XXX: If we advertise no swbreak support on POWER9 does
		 * that prevent the client using them?
		 */
		PR_WARNING("Breakpoints may cause host crashes on POWER9 and should not be used\n");
	}

	/* Check endianess in MSR */
	rc = thread_getmsr(thread, &msr);
	if (rc) {
		PR_ERROR("Couldn't read the MSR. Are all threads on this chiplet quiesced?\n");
		return 1;
	}
	littleendian = 0x01 & msr;

	/* Select ADU target */
	pdbg_for_each_class_target("mem", adu) {
		if (pdbg_target_probe(adu) == PDBG_TARGET_ENABLED)
			break;
	}

	if (!adu) {
		fprintf(stderr, "No ADU found\n");
		return 0;
	}

	gdbserver_start(thread, adu, port);
	return 0;
}
#else

static int gdbserver(uint16_t port)
{
	return 0;
}
#endif
OPTCMD_DEFINE_CMD_WITH_ARGS(gdbserver, gdbserver, (DATA16));
