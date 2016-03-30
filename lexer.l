%{
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

#include <string.h>

#define fprintf(f, format, msg) printf(format, msg)
%}

%option noyywrap reentrant bison-bridge nounistd nounput noinput never-interactive

WORD (([[:alpha:]]+,)|(iio:))?(-|_|\.|[[:alnum:]])+

%s WANT_DEVICE
%s WANT_CHN_OR_ATTR
%s WANT_CHN
%s WANT_ATTR
%s WANT_VALUE
%%

<INITIAL>VERSION|version {
	return VERSION;
}

<INITIAL>PRINT|print {
	return PRINT;
}

<INITIAL>EXIT|exit|QUIT|quit {
	return EXIT;
}

<INITIAL>HELP|help {
	return HELP;
}

<INITIAL>TIMEOUT|timeout {
	return TIMEOUT;
}

<INITIAL>OPEN|open {
	BEGIN(WANT_DEVICE);
	return OPEN;
}

<INITIAL>CLOSE|close {
	BEGIN(WANT_DEVICE);
	return CLOSE;
}

<INITIAL>READ|read {
	BEGIN(WANT_DEVICE);
	return READ;
}

<INITIAL>READBUF|readbuf {
	BEGIN(WANT_DEVICE);
	return READBUF;
}

<INITIAL>WRITEBUF|writebuf {
	BEGIN(WANT_DEVICE);
	return WRITEBUF;
}

<INITIAL>WRITE|write {
	BEGIN(WANT_DEVICE);
	return WRITE;
}

<INITIAL>SETTRIG|settrig {
	BEGIN(WANT_DEVICE);
	return SETTRIG;
}

<INITIAL>GETTRIG|gettrig {
	BEGIN(WANT_DEVICE);
	return GETTRIG;
}

<INITIAL>SET|set {
	BEGIN(WANT_DEVICE);
	return SET;
}

<WANT_DEVICE>{WORD} {
	yylval->word = strdup(yytext);
	BEGIN(WANT_CHN_OR_ATTR);
	return DEVICE;
}

<WANT_CHN_OR_ATTR>BUFFERS_COUNT|buffers_count {
	BEGIN(WANT_VALUE);
	return BUFFERS_COUNT;
}

<WANT_CHN_OR_ATTR>DEBUG|debug {
	BEGIN(WANT_ATTR);
	return DEBUG_ATTR;
}

<WANT_CHN_OR_ATTR>INPUT|input|OUTPUT|output {
	yylval->value = yytext[0] == 'o' || yytext[0] == 'O';
	BEGIN(WANT_CHN);
	return IN_OUT;
}

<WANT_CHN>{WORD} {
	yylval->word = strdup(yytext);
	BEGIN(WANT_ATTR);
	return CHANNEL;
}

<WANT_VALUE>{WORD} {
	yylval->value = strtol(yytext, NULL, 10);
	return VALUE;
}

CYCLIC|cyclic {
	return CYCLIC;
}

{WORD} {
	yylval->word = strdup(yytext);
	return WORD;
}

[ \t]+ {
	return SPACE;
}

[ \t]*\r?\n {
	BEGIN(INITIAL);
	return END;
}

. {
	BEGIN(INITIAL);
}
