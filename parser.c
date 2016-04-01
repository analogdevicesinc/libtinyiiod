#include "tinyiiod-private.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

static void remove_eol(char *str)
{
	char *ptr;

	ptr = strrchr(str, '\n');
	if (ptr)
		*ptr = '\0';

	ptr = strrchr(str, '\r');
	if (ptr)
		*ptr = '\0';
}

static int parse_rw_string(struct tinyiiod *iiod, char *str, bool write)
{
	char *device, *channel, *attr, *ptr;
	bool is_channel = false, output = false;
	long bytes;

	ptr = strchr(str, ' ');
	if (!ptr)
		return -EINVAL;

	*ptr = '\0';
	device = str;
	str = ptr + 1;

	if (!strncmp(str, "INPUT ", sizeof("INPUT ") - 1)) {
		is_channel = true;
		output = false;
		str += sizeof("INPUT ") - 1;
	} else if (!strncmp(str, "OUTPUT ", sizeof("OUTPUT ") - 1)) {
		is_channel = true;
		output = true;
		str += sizeof("OUTPUT ") - 1;
	} else {
		is_channel = false;
		output = false;
	}

	if (is_channel) {
		ptr = strchr(str, ' ');
		if (!ptr)
			return -EINVAL;

		*ptr = '\0';
		channel = str;
		str = ptr + 1;
	} else {
		channel = NULL;
	}

	ptr = strchr(str, ' ');
	if (!!ptr ^ write)
		return -EINVAL;

	attr = str;

	if (write) {
		*ptr = '\0';
		str = ptr + 1;
	} else {
		tinyiiod_do_read_attr(iiod, device, channel, output, attr);
		return 0;
	}

	bytes = strtol(str, &ptr, 10);
	if (str == ptr || bytes < 0)
		return -EINVAL;

	tinyiiod_do_write_attr(iiod, device, channel,
			output, attr, (size_t) bytes);
	return 0;
}

int tinyiiod_parse_string(struct tinyiiod *iiod, char *str)
{
	remove_eol(str);

	if (str[0] == '\0')
		return 0;

	if (!strncmp(str, "VERSION", sizeof("VERSION"))) {
		char buf[32];

		snprintf(buf, sizeof(buf), "%u.%u.%-7.7s\n",
				TINYIIOD_VERSION_MAJOR,
				TINYIIOD_VERSION_MINOR,
				TINYIIOD_VERSION_GIT);
		tinyiiod_write_string(iiod, buf);
		return 0;
	}

	if (!strncmp(str, "PRINT", sizeof("PRINT"))) {
		tinyiiod_write_xml(iiod);
		return 0;
	}

	if (!strncmp(str, "READ ", sizeof("READ ") - 1))
		return parse_rw_string(iiod, str + sizeof("READ ") - 1, false);

	if (!strncmp(str, "WRITE ", sizeof("WRITE ") -1))
		return parse_rw_string(iiod, str + sizeof("WRITE ") - 1, true);

	return -EINVAL;
}
