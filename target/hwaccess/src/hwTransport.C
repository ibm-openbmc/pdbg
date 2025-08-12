#include "hwTransport.H"

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <cinttypes>
#include <cerrno>
#include <endian.h>
#include <stdlib.h>
#define OPENFSI_LEGACY_PATH "/sys/bus/platform/devices/gpio-fsi/"
#define OPENFSI_PATH "/sys/class/fsi-master/"

namespace HWTRANSPORT
{
const char *fsi_base;

const char *kernel_get_fsi_path(void)
{
    int rc;

    if (fsi_base)
        return fsi_base;

    rc = access(OPENFSI_PATH, F_OK);
    if (rc == 0) {
        fsi_base = OPENFSI_PATH;
        return fsi_base;
    }

    rc = access(OPENFSI_LEGACY_PATH, F_OK);
    if (rc == 0) {
        fsi_base = OPENFSI_LEGACY_PATH;
        return fsi_base;
    }

    /* This is an error, but callers use this function when probing */
    printf("Failed to find kernel FSI path \n");
    return NULL;
}

static int kernel_fsi_getcfam(int fd, uint32_t addr64, uint32_t *value)
{
    int rc;
    uint32_t tmp, addr = (addr64 & 0x7ffc00) | ((addr64 & 0x3ff) << 2);

    rc = lseek(fd, addr, SEEK_SET);
    if (rc < 0) {
        rc = errno;
        printf("seek failed: %s\n", strerror(errno));
        return rc;
    }

    rc = read(fd, &tmp, 4);
    if (rc < 0) {
        rc = errno;
        if ((addr64 & 0xfff) != 0xc09)
            /* We expect reads of 0xc09 to occasionally
             * fail as the probing code uses it to see
             * if anything is present on the link. */
            printf("Failed to read from 0x%08" PRIx32 " (%016" PRIx32 ")\n", (uint32_t) addr, addr64);
        return rc;
    }
    *value = be32toh(tmp);

    return 0;
}

static void kernel_fsi_scan_devices(void)
{
    const char one = '1';
    const char *kernel_path = kernel_get_fsi_path();
    char *path;
    int rc, fd;

    if (!kernel_path)
        return;

    rc = asprintf(&path, "%s/fsi0/rescan", kernel_path);
    if (rc < 0) {
        printf("Unable to create fsi path\n");
        return;
    }

    fd = open(path, O_WRONLY | O_SYNC);
    if (fd < 0) {
        printf("Unable to open %s\n", path);
        free(path);
        return;
    }

    rc = write(fd, &one, sizeof(one));
    if (rc < 0)
        printf("Unable to write to %s\n", path);

    free(path);
    close(fd);
}

int kernel_fsi_probe(int& fd)
{
    int tries = 5;
    int rc;
    const char *kernel_path = kernel_get_fsi_path();
    const char *fsi_path = "fsi0/slave@00:00/raw";
    char *path;
    static bool first_probe = true;

    if (!kernel_path)
        return -1;

    rc = asprintf(&path, "%s%s", kernel_get_fsi_path(), fsi_path);
    if (rc < 0) {
        printf("Unable to create fsi path\n");
        return rc;
    }

    while (tries) {
        fd = open(path, O_RDWR | O_SYNC);
        if (fd >= 0) {
            free(path);
            first_probe = false;
            return 0;
        }
        tries--;


        /*
         * On fsi bus rescan, kernel re-creates all the slave device
         * entries.  It means any currently open devices will be
         * invalid and need to be re-opened.  So avoid scanning if
         * some devices are already probed.
         */
        if (first_probe) {
            kernel_fsi_scan_devices();
            sleep(1);
        } else {
            break;
        }
    }

    printf("Unable to open %s\n", path);
    free(path);
    return -1;
}

static void kernel_fsi_release(int& fd)
{
    if (fd != -1) {
        close(fd);
        fd = -1;
    }
}

int HwTransportIntf::getCfam(HWACCESS::TargetPtr_t target, uint32_t addr, uint32_t& val)
{
    int fd = -1;
    int ret = kernel_fsi_probe(fd);

    if(ret != 0)
    {
        std::cout << "Failed to proble fsi target" << std::endl;
        return ret;
    }
    ret = kernel_fsi_getcfam(fd, addr, &val);

    if(ret == 0)
    {
        printf("successfully read cfam value \n");
        printf("successfully read cfam value 0x%08" PRIx32 "\n", val);
    }
    kernel_fsi_release(fd);
  	return 0;
}

int HwTransportIntf::putCfam(HWACCESS::TargetPtr_t target, uint32_t addr, uint32_t val)
{
    return 0;
}

int HwTransportIntf::getScom(HWACCESS::TargetPtr_t target, uint64_t addr, uint64_t& val)
{
    return 0;
}
int HwTransportIntf::putScom(HWACCESS::TargetPtr_t target, uint64_t addr, uint64_t val)
{
    return 0;
}

} // namespace HWTRANSPORT
