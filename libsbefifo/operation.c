/* Copyright 2016 IBM Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * 	http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
 * implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <endian.h>

#include "libsbefifo.h"
#include "sbefifo_private.h"

static const uint16_t SBEFIFO_MAX_FFDC_SIZE = 0x8000;

static const uint32_t ffdcDataRaw0[] = {
    0xFBAD0038, 0x0000A801, 0x003d7eb6, 0x00000000, 0x7A951049,
    0xFF000000, 0x00000004, 0x00041038, 0x00020000, 0x5342455F, 0x54524143,
    0x45000000, 0x00000000, 0x0000300F, 0x38131000, 0xFE2329AF, 0x23C34600,
    0x00000000, 0xFFFFFFFF, 0xEF73FA8F, 0x00000018, 0x00015FD8, 0x00000000,
    0x00000000, 0x2C8D0101, 0x9D5C3E06, 0x5FD60000, 0x9D5C4311, 0xA6390006,
    0x9F4D53A9, 0x0000DA7A, 0x00000000, 0xD0A40102, 0x9F4D561E, 0x00000000,
    0x00000001, 0xC49C0102, 0x9F4D58DA, 0x91A00006, 0x9F4D5AF1, 0xC9200000,
    0x9F4D6021, 0x00000001, 0x00000000, 0xEB800102, 0x9F4D61C6, 0x000000A2,
    0x00000001, 0x1F770102, 0x9F62C136, 0x000000A2, 0x00000001, 0xF0F10102,
    0x9F62C392, 0x00000003, 0xDEADDEAD, 0xC0DEA801, 0x00000000, 0x00000003
};

static int sbefifo_read(struct sbefifo_context *sctx, void *buf, size_t *buflen)
{
    assert(buf != NULL && buflen != NULL && *buflen > 0);

    // Use stat as a fake trigger for testing mode
    struct stat dummy;
    if (stat("/tmp/ffdc", &dummy) == 0)
    {
        size_t raw_size = sizeof(ffdcDataRaw0); // in bytes
        if (*buflen < raw_size)
            return EMSGSIZE;

        memcpy(buf, ffdcDataRaw0, raw_size);
        *buflen = raw_size;
        return 0;
    }
    // Fallback to real read if not in test mode
    ssize_t n = read(sctx->fd, buf, *buflen);
    if (n < 0)
        return errno;

    *buflen = n;
    return 0;
}

static int sbefifo_write(struct sbefifo_context *sctx, void *buf, size_t buflen)
{
	ssize_t n;

	assert(buflen > 0);

	n = write(sctx->fd, buf, buflen);
	if (n < 0)
		return errno;

	if (n != buflen)
		return EIO;

	return 0;
}

static int sbefifo_transport(struct sbefifo_context *sctx, uint8_t *msg, uint32_t msg_len, uint8_t *out, uint32_t *out_len)
{
	int rc;
	size_t buflen;


	buflen = msg_len;
	rc = sbefifo_write(sctx, msg, buflen);
	if (rc) {
		LOG("write: cmd=%08x, rc=%d\n", be32toh(*(uint32_t *)(msg+4)), rc);
		return rc;
	}

	buflen = *out_len;
	rc = sbefifo_read(sctx, out, &buflen);
	if (rc) {
		LOG("read: cmd=%08x, buflen=%zu, rc=%d\n", be32toh(*(uint32_t *)(msg+4)), buflen, rc);
		return rc;
	}

	*out_len = buflen;
	return 0;
}

int sbefifo_parse_output(struct sbefifo_context *sctx, uint32_t cmd,
			 uint8_t *buf, uint32_t buflen,
			 uint8_t **out, uint32_t *out_len)
{
	uint32_t offset_word, header_word, status_word;
	uint32_t offset;

	sbefifo_ffdc_clear(sctx);

	/*
	 * At least 3 words are expected in response
	 *   - header word
	 *   - status word
	 *   - header offset word
	 */
	if (buflen < 3 * sizeof(uint32_t)) {
		LOG("reply: cmd=%08x, len=%u\n", cmd, buflen);
		return EPROTO;
	}

	/* Last word is header offset (in words) */
	offset_word = be32toh(*(uint32_t *) &buf[buflen-4]);
	offset = buflen - (offset_word * 4);

	*out_len = offset;

	header_word = be32toh(*(uint32_t *) &buf[offset]);
	offset += 4;

	status_word = be32toh(*(uint32_t *) &buf[offset]);
	offset += 4;

	if (header_word != (0xc0de0000 | cmd)) {
		LOG("reply: cmd=%08x, len=%u, header=%08x\n", cmd, buflen, header_word);
		return EPROTO;
	}

	LOG("reply: cmd=%08x, len=%u, status=%08x\n", cmd, buflen, status_word);

	if (status_word) {
		sbefifo_ffdc_set(sctx, status_word, buf + offset, buflen - offset-4);
		return ESBEFIFO;
	}

	if (*out_len > 0) {
		*out = malloc(*out_len);
		if (! *out)
			return ENOMEM;

		memcpy(*out, buf, *out_len);
	} else {
		*out = NULL;
	}

	//if there is ffdc data for success store it in internal buffer
	if((buflen - offset-4) > *out_len) {
		sbefifo_ffdc_set(sctx, status_word, buf + offset, buflen - offset-4);
	}
	return 0;
}

int sbefifo_operation(struct sbefifo_context *sctx,
		      uint8_t *msg, uint32_t msg_len,
		      uint8_t **out, uint32_t *out_len)
{
	uint8_t *buf;
	uint32_t buflen;
	uint32_t cmd;
	int rc;

	assert(msg);
	assert(msg_len > 0);

	if (!sctx->transport && sctx->fd == -1)
		return ENOTCONN;

	/*
	 * Allocate extra memory for FFDC (SBEFIFO_MAX_FFDC_SIZE = 0x8000)32kb
	 * Use *out_len as a hint to expected reply length
	 */
	buflen = (*out_len + SBEFIFO_MAX_FFDC_SIZE + 3) & ~(uint32_t)3;
	buf = malloc(buflen);
	if (!buf)
		return ENOMEM;

	cmd = be32toh(*(uint32_t *)(msg + 4));

	LOG("request: cmd=%08x, len=%u\n", cmd, msg_len);

	if (sctx->transport)
		rc = sctx->transport(msg, msg_len, buf, &buflen, sctx->priv);
	else
		rc = sbefifo_transport(sctx, msg, msg_len, buf, &buflen);

	if (rc) {
		free(buf);

		if (rc == ETIMEDOUT) {
			uint32_t status;

			status = SBEFIFO_PRI_UNKNOWN_ERROR | SBEFIFO_SEC_HW_TIMEOUT;
			sbefifo_ffdc_set(sctx, status, NULL, 0);
		}

		return rc;
	}

	rc = sbefifo_parse_output(sctx, cmd, buf, buflen, out, out_len);
	free(buf);
	return rc;
}
