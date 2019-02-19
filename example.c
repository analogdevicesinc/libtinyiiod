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

static void read_data(char *buf, size_t len)
{
	fread(buf, 1, len, stdin);
}

static void write_data(const char *buf, size_t len)
{
	fwrite(buf, 1, len, stdout);
}

static ssize_t read_attr(const char *device, const char *attr,
			 char *buf, size_t len, bool debug)
{
	if (!strcmp(device, "adc") || !strcmp(device, "0")) {
		if (debug) {
			if (!strcmp(attr, "direct_reg_access"))
				return (ssize_t) snprintf(buf, len, "0");
		} else {
			if (!strcmp(attr, "sample_rate"))
				return (ssize_t) snprintf(buf, len, "1000");
		}
	}

	return -ENOENT;
}

static ssize_t write_attr(const char *device, const char *attr,
			  const char *buf, size_t len, bool debug)
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

static const struct tinyiiod_ops ops = {
	.read = read_data,
	.write = write_data,

	.read_attr = read_attr,
	.write_attr = write_attr,
	.ch_read_attr = ch_read_attr,
	.ch_write_attr = ch_write_attr,
};

static const char * const xml =
	"<?xml version=\"1.0\" encoding=\"utf-8\"?><!DOCTYPE context [<!ELEMENT context "
	"(device)*><!ELEMENT device (channel | attribute | debug-attribute)*><!ELEMENT "
	"channel (scan-element?, attribute*)><!ELEMENT attribute EMPTY><!ELEMENT "
	"scan-element EMPTY><!ELEMENT debug-attribute EMPTY><!ATTLIST context name "
	"CDATA #REQUIRED description CDATA #IMPLIED><!ATTLIST device id CDATA "
	"#REQUIRED name CDATA #IMPLIED><!ATTLIST channel id CDATA #REQUIRED type "
	"(input|output) #REQUIRED name CDATA #IMPLIED><!ATTLIST scan-element index "
	"CDATA #REQUIRED format CDATA #REQUIRED scale CDATA #IMPLIED><!ATTLIST "
	"attribute name CDATA #REQUIRED filename CDATA #IMPLIED><!ATTLIST "
	"debug-attribute name CDATA #REQUIRED>]><context name=\"tiny\" "
	"description=\"Tiny IIOD\" >"
	"<device id=\"0\" name=\"adc\" >"
	"<channel id=\"voltage0\" type=\"input\" >"
	"<attribute name=\"scale\" /><attribute name=\"raw\" /></channel>"
	"<channel id=\"voltage1\" type=\"input\" >"
	"<attribute name=\"scale\" /><attribute name=\"raw\" /></channel>"
	"<attribute name=\"sample_rate\" />"
	"<debug-attribute name=\"direct_reg_access\" />"
	"</device></context>";

static bool stop;

static void set_handler(int signal_nb, void (*handler)(int))
{
	struct sigaction sig;
	sigaction(signal_nb, NULL, &sig);
	sig.sa_handler = handler;
	sigaction(signal_nb, &sig, NULL);
}

static void quit_all(int sig)
{
	stop = true;
}

int main(void)
{
	struct tinyiiod *iiod = tinyiiod_create(xml, &ops);

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
