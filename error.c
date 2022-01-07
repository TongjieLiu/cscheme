/* error.c -- error handling

   Copyright (C) 2021 Tongjie Liu <tongjieandliu@gmail.com>.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.*/

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "error.h"
#include "ef.h"
#include "ast.h"




void cscm_error_report(char *func, char *msg)
{
	int i;
	CSCM_AST_NODE *exp;


	fprintf(stderr, "%s(): %s\n", func, msg);

	if (!cscm_ef_backtrace_is_empty())
		puts("\n-------------------- BACKTRACE --------------------");

	for (i = 0; !cscm_ef_backtrace_is_empty(); i++) {
		exp = cscm_ef_backtrace_pop();

		printf("[%d] %s:%lu ",			\
			i,				\
			exp->filename,			\
			(unsigned long)exp->line);
		cscm_ast_print_tree(exp);
		puts("");
	}

	exit(1);
}




void cscm_syntax_error_report(char *filename, size_t line, char *msg)
{
	fprintf(stderr, "%s:%lu: %s\n", filename, (unsigned long)line, msg);
	exit(1);
}




void cscm_runtime_error_report(char *object_name, char *msg)
{
	int i;
	CSCM_AST_NODE *exp;


	fprintf(stderr, "\"%s\": %s\n", object_name, msg);

	if (!cscm_ef_backtrace_is_empty())
		puts("\n-------------------- BACKTRACE --------------------");

	for (i = 0; !cscm_ef_backtrace_is_empty(); i++) {
		exp = cscm_ef_backtrace_pop();

		printf("[%d] %s:%lu ",			\
			i,				\
			exp->filename,			\
			(unsigned long)exp->line);
		cscm_ast_print_tree(exp);
		puts("");
	}


	exit(1);
}




void cscm_libc_fail(char *pos, char *name)
{
	int i;
	CSCM_AST_NODE *exp;


	fprintf(stderr, "%s(): %s(): %s\n", pos, name, strerror(errno));

	if (!cscm_ef_backtrace_is_empty())
		puts("\n-------------------- BACKTRACE --------------------");

	for (i = 0; !cscm_ef_backtrace_is_empty(); i++) {
		exp = cscm_ef_backtrace_pop();

		printf("[%d] %s:%lu ",			\
			i,				\
			exp->filename,			\
			(unsigned long)exp->line);
		cscm_ast_print_tree(exp);
		puts("");
	}

	exit(1);
}




void cscm_sigabrt_handler(int signum)
{
	int i;
	CSCM_AST_NODE *exp;


	puts("cscheme received a SIGABRT signal");

	if (!cscm_ef_backtrace_is_empty())
		puts("\n-------------------- BACKTRACE --------------------");

	for (i = 0; !cscm_ef_backtrace_is_empty(); i++) {
		exp = cscm_ef_backtrace_pop();

		printf("[%d] %s:%lu ",			\
			i,				\
			exp->filename,			\
			(unsigned long)exp->line);
		cscm_ast_print_tree(exp);
		puts("");
	}

	exit(1);
}
