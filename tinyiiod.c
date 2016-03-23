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

#include "parser.h"
#include "tinyiiod-private.h"

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct tinyiiod {
	const char *xml;
	const struct tinyiiod_ops *ops;
	yyscan_t scanner;
};

struct tinyiiod * tinyiiod_create(const char *xml,
		const struct tinyiiod_ops *ops)
{
	struct tinyiiod *iiod = malloc(sizeof(*iiod));

	if (!iiod)
		return NULL;

	iiod->xml = xml;
	iiod->ops = ops;

	yylex_init_extra(iiod, &iiod->scanner);

	return iiod;
}

void tinyiiod_destroy(struct tinyiiod *iiod)
{
	yylex_destroy(iiod->scanner);
	free(iiod);
}

int tinyiiod_read_command(struct tinyiiod *iiod)
{
	char buf[256];
	void *ptr;
	int ret;

	ret = (int) tinyiiod_read_line(iiod, buf, sizeof(buf));
	if (ret < 0)
		return ret;

	ptr = yy_scan_string(buf, iiod->scanner);
	ret = yyparse(iiod->scanner);

	yy_delete_buffer(ptr, iiod->scanner);
	return ret;
}

char tinyiiod_read_char(struct tinyiiod *iiod)
{
	return iiod->ops->read_char();
}

void tinyiiod_read(struct tinyiiod *iiod, char *buf, size_t len)
{
	while (len--)
		*buf++ = tinyiiod_read_char(iiod);
}

ssize_t tinyiiod_read_line(struct tinyiiod *iiod, char *buf, size_t len)
{
	unsigned int i;
	bool found = false;

	for (i = 0; i < len - 1; i++) {
		buf[i] = tinyiiod_read_char(iiod);

		if (buf[i] != '\n')
			found = true;
		else if (found)
			break;
	}

	if (!found) {
		/* No \n found -> garbage data */
		return -EIO;
	}

	buf[i + 1] = '\0';
	return (ssize_t) i;
}

void tinyiiod_write_char(struct tinyiiod *iiod, char c)
{
	iiod->ops->write_char(c);
}

void tinyiiod_write(struct tinyiiod *iiod, const char *data, size_t len)
{
	while (len--)
		tinyiiod_write_char(iiod, *data++);
}

void tinyiiod_write_string(struct tinyiiod *iiod, const char *str)
{
	tinyiiod_write(iiod, str, strlen(str));
}

void tinyiiod_write_value(struct tinyiiod *iiod, int value)
{
	char buf[128];

	snprintf(buf, sizeof(buf), "%i\n", value);
	tinyiiod_write_string(iiod, buf);
}

void tinyiiod_write_xml(struct tinyiiod *iiod)
{
	size_t len = strlen(iiod->xml);

	tinyiiod_write_value(iiod, len);
	tinyiiod_write(iiod, iiod->xml, len);
}

void tinyiiod_do_read_attr(struct tinyiiod *iiod, const char *device,
		const char *channel, bool ch_out, const char *attr)
{
	char buf[1024];
	ssize_t ret;

	if (channel)
		ret = iiod->ops->ch_read_attr(device, channel,
				ch_out, attr, buf, sizeof(buf));
	else
		ret = iiod->ops->read_attr(device, attr, buf, sizeof(buf));

	tinyiiod_write_value(iiod, (int) ret);
	if (ret > 0) {
		tinyiiod_write(iiod, buf, (size_t) ret);
		tinyiiod_write_char(iiod, '\n');
	}
}

void tinyiiod_do_write_attr(struct tinyiiod *iiod, const char *device,
		const char *channel, bool ch_out, const char *attr,
		size_t bytes)
{
	char buf[1024];
	ssize_t ret;

	if (bytes > sizeof(buf))
		bytes = sizeof(buf);

	tinyiiod_read(iiod, buf, bytes);

	if (channel)
		ret = iiod->ops->ch_write_attr(device, channel, ch_out,
				attr, buf, bytes);
	else
		ret = iiod->ops->write_attr(device, attr, buf, bytes);

	tinyiiod_write_value(iiod, (int) ret);
}
