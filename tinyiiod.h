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
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#if !defined(__ssize_t_defined) && !defined(_SSIZE_T_DEFINED)
typedef long int ssize_t;
#define __ssize_t_defined
#define _SSIZE_T_DEFINED
#endif

struct tinyiiod;

struct tinyiiod_ops {
	/* Read from the input stream */
	void (*read)(char *buf, size_t len);

	/* Write to the output stream */
	void (*write)(const char *buf, size_t len);

	ssize_t (*read_attr)(const char *device, const char *attr,
			     char *buf, size_t len, bool debug);
	ssize_t (*write_attr)(const char *device, const char *attr,
			      const char *buf, size_t len, bool debug);

	ssize_t (*ch_read_attr)(const char *device, const char *channel,
				bool ch_out, const char *attr, char *buf, size_t len);
	ssize_t (*ch_write_attr)(const char *device, const char *channel,
				 bool ch_out, const char *attr,
				 const char *buf, size_t len);

	int (*open)(const char *device, size_t sample_size, uint32_t mask);
	int (*close)(const char *device);

	ssize_t (*read_data)(const char *device, char *buf, size_t bytes_count);

	int (*get_mask)(const char *device, uint32_t *mask);
};

struct tinyiiod * tinyiiod_create(const char *xml,
				  const struct tinyiiod_ops *ops);
void tinyiiod_destroy(struct tinyiiod *iiod);
int tinyiiod_read_command(struct tinyiiod *iiod);

#endif /* TINYIIOD_H */
