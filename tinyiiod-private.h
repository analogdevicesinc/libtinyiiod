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

#ifndef TINYIIOD_PRIVATE_H
#define TINYIIOD_PRIVATE_H

#include "tinyiiod.h"

char tinyiiod_read_char(struct tinyiiod *iiod);
ssize_t tinyiiod_read(struct tinyiiod *iiod, char *buf, size_t len);
ssize_t tinyiiod_read_line(struct tinyiiod *iiod, char *buf, size_t len);

ssize_t tinyiiod_write_char(struct tinyiiod *iiod, char c);
ssize_t tinyiiod_write(struct tinyiiod *iiod, const char *data, size_t len);
ssize_t tinyiiod_write_string(struct tinyiiod *iiod, const char *str);
ssize_t tinyiiod_write_value(struct tinyiiod *iiod, int32_t value);

void tinyiiod_write_xml(struct tinyiiod *iiod);

void tinyiiod_do_read_attr(struct tinyiiod *iiod, const char *device,
			   const char *channel, bool ch_out, const char *attr, bool debug);

void tinyiiod_do_write_attr(struct tinyiiod *iiod, const char *device,
			    const char *channel, bool ch_out, const char *attr,
			    size_t bytes, bool debug);

void tinyiiod_do_open(struct tinyiiod *iiod, const char *device,
		      size_t sample_size, uint32_t mask);
void tinyiiod_do_close(struct tinyiiod *iiod, const char *device);

int32_t tinyiiod_do_open_instance(struct tinyiiod *iiod);

int32_t tinyiiod_do_close_instance(struct tinyiiod *iiod);

int32_t tinyiiod_do_readbuf(struct tinyiiod *iiod,
			    const char *device, size_t bytes_count);

int32_t tinyiiod_do_writebuf(struct tinyiiod *iiod, const char *device,
			     size_t bytes_count);

int32_t tinyiiod_parse_string(struct tinyiiod *iiod, char *str);

int32_t tinyiiod_set_timeout(struct tinyiiod *iiod, uint32_t timeout);

#endif /* TINYIIOD_PRIVATE_H */
