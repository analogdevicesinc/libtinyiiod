/*
 * libtinyiiod - Tiny IIO Daemon Library
 *
 * Copyright (C) 2016 Analog Devices, Inc.
 * Author: Paul Cercueil <paul.cercueil@analog.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

#include "tinyiiod-private.h"

#include "compat.h"

struct tinyiiod {
	const char *xml;
	const struct tinyiiod_ops *ops;
};

struct tinyiiod * tinyiiod_create(const char *xml,
				  const struct tinyiiod_ops *ops)
{
	struct tinyiiod *iiod = malloc(sizeof(*iiod));

	if (!iiod)
		return NULL;

	iiod->xml = xml;
	iiod->ops = ops;

	return iiod;
}

void tinyiiod_destroy(struct tinyiiod *iiod)
{
	free(iiod);
}

int32_t tinyiiod_read_command(struct tinyiiod *iiod)
{
	char buf[128];
	int32_t ret;

	ret = tinyiiod_read_line(iiod, buf, sizeof(buf));
	if (ret < 0)
		return ret;

	ret = tinyiiod_parse_string(iiod, buf);
	if (ret < 0)
		tinyiiod_write_value(iiod, ret);

	return ret;
}

char tinyiiod_read_char(struct tinyiiod *iiod)
{
	char c;

	iiod->ops->read(&c, 1);
	return c;
}

ssize_t tinyiiod_read(struct tinyiiod *iiod, char *buf, size_t len)
{
	return iiod->ops->read(buf, len);
}

ssize_t tinyiiod_read_line(struct tinyiiod *iiod, char *buf, size_t len)
{
	int32_t i;
	bool found = false;

	if (iiod->ops->read_line)
		return iiod->ops->read_line(buf, len);

	for (i = 0; i < len - 1; i++) {
		buf[i] = tinyiiod_read_char(iiod);

		if (buf[i] != '\n' && buf[i] != '\r')
			found = true;
		else if (found)
			break;
	}

	if (!found || i == len - 1) {
		/* No \n found -> garbage data */
		return -EIO;
	}

	buf[i] = '\0';

	return i;
}

ssize_t tinyiiod_write_char(struct tinyiiod *iiod, char c)
{
	return iiod->ops->write(&c, 1);
}

ssize_t tinyiiod_write(struct tinyiiod *iiod, const char *data, size_t len)
{
	return iiod->ops->write(data, len);
}

void tinyiiod_write_string(struct tinyiiod *iiod, const char *str)
{
	tinyiiod_write(iiod, str, strlen(str));
}

void tinyiiod_write_value(struct tinyiiod *iiod, int32_t value)
{
	char buf[16];

	snprintf(buf, sizeof(buf), "%"PRIi32"\n", value);
	tinyiiod_write_string(iiod, buf);
}

void tinyiiod_write_xml(struct tinyiiod *iiod)
{
	size_t len = strlen(iiod->xml);

	tinyiiod_write_value(iiod, len);
	tinyiiod_write(iiod, iiod->xml, len);
	tinyiiod_write_char(iiod, '\n');
}

void tinyiiod_do_read_attr(struct tinyiiod *iiod, const char *device,
			   const char *channel, bool ch_out, const char *attr, bool debug)
{
	char buf[128];
	ssize_t ret;

	if (channel)
		ret = iiod->ops->ch_read_attr(device, channel,
					      ch_out, attr, buf, sizeof(buf));
	else
		ret = iiod->ops->read_attr(device, attr,
					   buf, sizeof(buf), debug);

	tinyiiod_write_value(iiod, (int32_t) ret);
	if (ret > 0) {
		buf[ret] = '\n';
		tinyiiod_write(iiod, buf, (size_t) ret + 1);
	}
}

void tinyiiod_do_write_attr(struct tinyiiod *iiod, const char *device,
			    const char *channel, bool ch_out, const char *attr,
			    size_t bytes, bool debug)
{
	char buf[128];
	ssize_t ret;

	if (bytes > sizeof(buf) - 1)
		bytes = sizeof(buf) - 1;

	tinyiiod_read(iiod, buf, bytes);
	buf[bytes] = '\0';

	if (channel)
		ret = iiod->ops->ch_write_attr(device, channel, ch_out,
					       attr, buf, bytes);
	else
		ret = iiod->ops->write_attr(device, attr, buf, bytes, debug);

	tinyiiod_write_value(iiod, (int32_t) ret);
}

void tinyiiod_do_open(struct tinyiiod *iiod, const char *device,
		      size_t sample_size, uint32_t mask)
{
	int32_t ret = iiod->ops->open(device, sample_size, mask);
	tinyiiod_write_value(iiod, ret);
}

void tinyiiod_do_close(struct tinyiiod *iiod, const char *device)
{
	int32_t ret = iiod->ops->close(device);
	tinyiiod_write_value(iiod, ret);
}

int32_t tinyiiod_do_readbuf(struct tinyiiod *iiod,
			    const char *device, size_t bytes_count)
{
	int32_t ret;
	char buf[256];
	uint32_t mask;
	bool print_mask = true;

	ret = iiod->ops->get_mask(device, &mask);
	if (ret < 0) {
		return ret;
	}

	while(bytes_count) {
		size_t bytes = bytes_count > sizeof(buf) ? sizeof(buf) : bytes_count;

		ret = iiod->ops->read_data(device, buf, bytes);
		tinyiiod_write_value(iiod, ret);
		if (ret < 0)
			return ret;

		if (print_mask) {
			char buf_mask[10];

			snprintf(buf_mask, sizeof(buf_mask), "%08"PRIx32"\n", mask);
			tinyiiod_write_string(iiod, buf_mask);
			print_mask = false;
		}

		tinyiiod_write(iiod, buf, (size_t) ret);
		bytes_count -= (size_t) ret;
	}

	return ret;
}
