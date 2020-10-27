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

#include "tinyiiod.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

static ssize_t read_data(char *buf, size_t len)
{
	return fread(buf, 1, len, stdin);
}

static ssize_t write_data(const char *buf, size_t len)
{
	return fwrite(buf, 1, len, stdout);
}

static ssize_t read_attr(const char *device, const char *attr,
			 char *buf, size_t len, enum iio_attr_type type)
{
	if (!strcmp(device, "adc") || !strcmp(device, "0")) {
		switch (type) {
		case IIO_ATTR_TYPE_DEVICE:
			return (ssize_t) snprintf(buf, len, "1000");
		case IIO_ATTR_TYPE_DEBUG:
			return (ssize_t) snprintf(buf, len, "0");
		case IIO_ATTR_TYPE_BUFFER:
			return (ssize_t) snprintf(buf, len, "8");
		default:
			return -ENOENT;
		}
	}

	return -ENOENT;
}

static ssize_t write_attr(const char *device, const char *attr,
			  const char *buf, size_t len, enum iio_attr_type type)
{
	return -ENOSYS;
}

static ssize_t ch_read_attr(const char *device, const char *channel,
			    bool ch_out, const char *attr, char *buf, size_t len)
{
	if (!strcmp(device, "adc") || !strcmp(device, "0")) {
		if (ch_out)
			return -ENOENT;

		if (!strcmp(channel, "voltage0")) {
			if (!strcmp(attr, "scale"))
				return (ssize_t) snprintf(buf, len, "0.033");
			else if (!strcmp(attr, "raw"))
				return (ssize_t) snprintf(buf, len, "256");
		} else if (!strcmp(channel, "voltage1")) {
			if (!strcmp(attr, "scale"))
				return (ssize_t) snprintf(buf, len, "0.033");
			else if (!strcmp(attr, "raw"))
				return (ssize_t) snprintf(buf, len, "128");
		}
	}

	return -ENOENT;
}

static ssize_t ch_write_attr(const char *device, const char *channel,
			     bool ch_out, const char *attr, const char *buf, size_t len)
{
	return -ENOSYS;
}

static const char * const xml =
	"<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE context [<!ELEMENT context "
	"(device)*><!ELEMENT device (channel | attribute | debug-attribute | buffer-attribute)*><!ELEMENT "
	"channel (scan-element?, attribute*)><!ELEMENT attribute EMPTY><!ELEMENT "
	"scan-element EMPTY><!ELEMENT debug-attribute EMPTY><!ELEMENT buffer-attribute EMPTY><!ATTLIST context name "
	"CDATA #REQUIRED description CDATA #IMPLIED><!ATTLIST device id CDATA "
	"#REQUIRED name CDATA #IMPLIED><!ATTLIST channel id CDATA #REQUIRED type "
	"(input|output) #REQUIRED name CDATA #IMPLIED><!ATTLIST scan-element index "
	"CDATA #REQUIRED format CDATA #REQUIRED scale CDATA #IMPLIED><!ATTLIST "
	"attribute name CDATA #REQUIRED filename CDATA #IMPLIED><!ATTLIST "
	"debug-attribute name CDATA #REQUIRED><!ATTLIST buffer-attribute name "
	"CDATA #REQUIRED value CDATA #IMPLIED>]><context name=\"tiny\" "
	"description=\"Tiny IIOD\" >"
	"<device id=\"0\" name=\"adc\" >"
	"<channel id=\"voltage0\" type=\"input\" >"
	"<attribute name=\"scale\" /><attribute name=\"raw\" /></channel>"
	"<channel id=\"voltage1\" type=\"input\" >"
	"<attribute name=\"scale\" /><attribute name=\"raw\" /></channel>"
	"<attribute name=\"sample_rate\" />"
	"<debug-attribute name=\"direct_reg_access\" />"
	"<buffer-attribute name=\"length_align_bytes\" />"
	"</device></context>";

static ssize_t get_xml(char **outxml)
{

	*outxml = calloc(1, strlen(xml) + 1);
	if (!(*outxml))
		return -ENOMEM;
	memcpy(*outxml, xml, strlen(xml));

	return 0;
}

static struct tinyiiod_ops ops = {
	.read = read_data,
	.write = write_data,

	.read_attr = read_attr,
	.write_attr = write_attr,
	.ch_read_attr = ch_read_attr,
	.ch_write_attr = ch_write_attr,
	.get_xml = get_xml,
};

static bool stop;

static void set_handler(int32_t signal_nb, void (*handler)(int32_t))
{
	struct sigaction sig;
	sigaction(signal_nb, NULL, &sig);
	sig.sa_handler = handler;
	sigaction(signal_nb, &sig, NULL);
}

static void quit_all(int32_t sig)
{
	stop = true;
}

int32_t main(void)
{
	struct tinyiiod *iiod = tinyiiod_create(&ops);

	set_handler(SIGHUP, &quit_all);
	set_handler(SIGPIPE, &quit_all);
	set_handler(SIGINT, &quit_all);
	set_handler(SIGSEGV, &quit_all);
	set_handler(SIGTERM, &quit_all);

	while (!stop)
		tinyiiod_read_command(iiod);

	tinyiiod_destroy(iiod);

	return 0;
}
