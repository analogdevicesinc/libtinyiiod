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

#include "tinyiiod-private.h"
#include "parser.h"

#include <errno.h>
#include <stdio.h>

void yyerror(yyscan_t scanner, const char *msg);
%}

%code requires {
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void *yyscan_t;
#endif

int yylex();
int yylex_init_extra(void *d, yyscan_t *scanner);
int yylex_destroy(yyscan_t yyscanner);

void * yyget_extra(yyscan_t scanner);

#define ECHO do { } while (0)

#define YY_INPUT(buf,result,max_size) do { } while (0)

struct yy_buffer_state;
typedef struct yy_buffer_state *YY_BUFFER_STATE;
#define YY_TYPEDEF_YY_BUFFER_STATE

YY_BUFFER_STATE yy_scan_string(const char *yy_str, yyscan_t yyscanner);
void yy_delete_buffer(YY_BUFFER_STATE b, yyscan_t yyscanner);
}

%define api.pure
%lex-param { yyscan_t scanner }
%parse-param { yyscan_t scanner }

%union {
	char *word;
	long value;
}

%token SPACE
%token END

%token VERSION
%token EXIT
%token HELP
%token OPEN
%token CLOSE
%token PRINT
%token READ
%token READBUF
%token WRITEBUF
%token WRITE
%token SETTRIG
%token GETTRIG
%token TIMEOUT
%token DEBUG_ATTR
%token CYCLIC
%token SET
%token BUFFERS_COUNT

%token <word> WORD
%token <word> DEVICE
%token <word> CHANNEL
%token <value> VALUE;
%token <value> IN_OUT

%destructor { free($$); } <word>

%start Line
%%

Line:
	END {
		YYACCEPT;
	}
	| VERSION END {
		struct tinyiiod *iiod = yyget_extra(scanner);
		char buf[128];
		snprintf(buf, sizeof(buf), "%u.%u.%-7.7s\n",
				TINYIIOD_VERSION_MAJOR,
				TINYIIOD_VERSION_MINOR,
				TINYIIOD_VERSION_GIT);
		tinyiiod_write_string(iiod, buf);
		YYACCEPT;
	}
	| PRINT END {
		struct tinyiiod *iiod = yyget_extra(scanner);
		tinyiiod_write_xml(iiod);
		tinyiiod_write_char(iiod, '\n');
		YYACCEPT;
	}
	| READ SPACE DEVICE SPACE WORD END {
		struct tinyiiod *iiod = yyget_extra(scanner);

		tinyiiod_do_read_attr(iiod, $3, NULL, false, $5);
		free($3);
		free($5);
		YYACCEPT;
	}
	| READ SPACE DEVICE SPACE IN_OUT SPACE CHANNEL SPACE WORD END {
		struct tinyiiod *iiod = yyget_extra(scanner);

		tinyiiod_do_read_attr(iiod, $3, $7, $5, $9);
		free($3);
		free($7);
		free($9);
		YYACCEPT;
	}
	| WRITE SPACE DEVICE SPACE WORD SPACE WORD END {
		struct tinyiiod *iiod = yyget_extra(scanner);

		tinyiiod_do_write_attr(iiod, $3, NULL, false, $5, (size_t) atoi($7));
		free($3);
		free($5);
		free($7);
		YYACCEPT;
	}
	| WRITE SPACE DEVICE SPACE IN_OUT SPACE CHANNEL SPACE WORD SPACE WORD END {
		struct tinyiiod *iiod = yyget_extra(scanner);

		tinyiiod_do_write_attr(iiod, $3, $7, $5, $9, (size_t) atoi($11));
		free($3);
		free($7);
		free($9);
		YYACCEPT;
	}
	| error END {
		yyclearin;
		yyerrok;
		YYACCEPT;
	}
	;

%%

void yyerror(yyscan_t scanner, const char *msg)
{
	struct tinyiiod *iiod = yyget_extra(scanner);

	tinyiiod_write_value(iiod, -EINVAL);
}
