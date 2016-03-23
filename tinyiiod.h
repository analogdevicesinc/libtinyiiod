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

#ifndef TINYIIOD_H
#define TINYIIOD_H

#include <stdbool.h>
#include <stdlib.h>

struct tinyiiod;

struct tinyiiod_ops {
	/* Read a character from the input stream */
	char (*read_char)(void);

	/* Write a character to the output stream */
	void (*write_char)(char c);

	ssize_t (*read_attr)(const char *device,
			const char *attr, char *buf, size_t len);
	ssize_t (*write_attr)(const char *device,
			const char *attr, const char *buf, size_t len);

	ssize_t (*ch_read_attr)(const char *device, const char *channel,
			bool ch_out, const char *attr, char *buf, size_t len);
	ssize_t (*ch_write_attr)(const char *device, const char *channel,
			bool ch_out, const char *attr,
			const char *buf, size_t len);
};

struct tinyiiod * tinyiiod_create(const char *xml,
		const struct tinyiiod_ops *ops);
void tinyiiod_destroy(struct tinyiiod *iiod);
int tinyiiod_read_command(struct tinyiiod *iiod);

#endif /* TINYIIOD_H */
