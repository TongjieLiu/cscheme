/* debug.c -- cscheme debug mode

   Copyright (C) 2022 Tongjie Liu <tongjieandliu@gmail.com>.

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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "error.h"
#include "ef.h"
#include "ast.h"
#include "env.h"
#include "object.h"
#include "text.h"
#include "debug.h"




int cscm_debug_mode = 0;




size_t _cscm_debug_next = 0;




size_t _cscm_debug_cmd_count = 0;
char **_cscm_debug_cmd_vector = NULL;


void _cscm_debug_parse_cmd()
{
	int i;

	int c;
	char *option;
	int flag_space;

	size_t vector_size;
	size_t option_buf_size;

	size_t cmd_count;
	char **cmd_vector;


	if (_cscm_debug_cmd_vector == NULL) {
		vector_size = CSCM_DEBUG_MAX_CMD_COUNT * sizeof(char *);

		_cscm_debug_cmd_vector = malloc(vector_size);
		if (_cscm_debug_cmd_vector == NULL)
			cscm_libc_fail("_cscm_debug_parse_cmd", "malloc");


		option_buf_size = CSCM_DEBUG_MAX_OPTION_LEN + 1;
		for (i = 0; i < CSCM_DEBUG_MAX_CMD_COUNT; i++) {
			_cscm_debug_cmd_vector[i] = malloc(option_buf_size);
			if (_cscm_debug_cmd_vector[i] == NULL)
				cscm_libc_fail("_cscm_debug_parse_cmd", \
						"malloc");
		}
	}


	i = 0;
	flag_space = 1; // in case of the first character is a space
	cmd_count = 0;
	cmd_vector = _cscm_debug_cmd_vector;
	while (cmd_count < CSCM_DEBUG_MAX_CMD_COUNT)
	{
		c = fgetc(stdin);


		if (c == EOF) {
			exit(0);
		} else if (c == ' ' || c == '\t') {
			if (!flag_space) {
				option = cmd_vector[cmd_count++];
				option[i] = 0;

				flag_space = 1;
				i = 0;
			} else {
				continue;
			}
		} else if (c == '\n') {
			if (!flag_space) {
				option = cmd_vector[cmd_count++];
				option[i] = 0;
			}

			_cscm_debug_cmd_count = cmd_count;

			return;
		} else {
			flag_space = 0;

			option = cmd_vector[cmd_count];
			option[i++] = (char)c;

			if (i == CSCM_DEBUG_MAX_OPTION_LEN)
				cscm_error_report(			\
					"_cscm_debug_parse_cmd",	\
					CSCM_ERROR_DEBUG_OPTION_TOO_LONG);
		}
	}


	cscm_error_report("_cscm_debug_parse_cmd", \
			CSCM_ERROR_DEBUG_CMD_TOO_LONG);
}




int _cscm_debug_cmd_handler_help(CSCM_OBJECT *env)
{
	if (_cscm_debug_cmd_count != 1)
		cscm_error_report("cscm_debug_cmd_handler_help", \
				CSCM_ERROR_DEBUG_OPTION_N);


	puts("help -- show command documentations");
	puts("print variable -- print variable value");
	puts("next [times] -- continue with next execution function");
	puts("env -- print current environment");
	puts("frame [index]-- print current/specified frame");
	puts("backtrace -- print execution function stack");


	return CSCM_DEBUG_CMD_RET_CONTINUE;
}




int _cscm_debug_cmd_handler_print(CSCM_OBJECT *env)
{
	CSCM_OBJECT *obj;
	if (_cscm_debug_cmd_count != 2)
		cscm_error_report("cscm_debug_cmd_handler_print", \
				CSCM_ERROR_DEBUG_OPTION_N);


	obj = cscm_env_get_var(env, _cscm_debug_cmd_vector[1]);

	cscm_object_print(obj, stdout);
	puts("");


	return CSCM_DEBUG_CMD_RET_CONTINUE;
}




int _cscm_debug_cmd_handler_next(CSCM_OBJECT *env)
{
	long times;
	char *option_times;


	if (_cscm_debug_cmd_count == 1) {
		_cscm_debug_next = 0;
		return CSCM_DEBUG_CMD_RET_EXIT;
	} else if (_cscm_debug_cmd_count > 2) {
		cscm_error_report("cscm_debug_cmd_handler_next", \
				CSCM_ERROR_DEBUG_OPTION_N);
	}


	option_times = _cscm_debug_cmd_vector[1];
	if (!cscm_text_is_integer(option_times))
		cscm_error_report("cscm_debug_cmd_handler_next", \
				CSCM_ERROR_DEBUG_CMD_NEXT_NOT_TIMES);

	times = atol(option_times);
	if (times <= 0)
		cscm_error_report("cscm_debug_cmd_handler_next", \
				CSCM_ERROR_DEBUG_CMD_NEXT_NEG_TIMES);
	_cscm_debug_next = times - 1;


	return CSCM_DEBUG_CMD_RET_EXIT;
}




int _cscm_debug_cmd_handler_env(CSCM_OBJECT *env)
{
	if (_cscm_debug_cmd_count != 1)
		cscm_error_report("cscm_debug_cmd_handler_env", \
				CSCM_ERROR_DEBUG_OPTION_N);


	cscm_env_print_details(env);


	return CSCM_DEBUG_CMD_RET_CONTINUE;
}




int _cscm_debug_cmd_handler_frame(CSCM_OBJECT *env)
{
	long index;
	char *option_index;

	CSCM_ENV *real_env;


	if (_cscm_debug_cmd_count == 1) {
		index = 0;
	} else if (_cscm_debug_cmd_count > 2) {
		cscm_error_report("cscm_debug_cmd_handler_frame", \
				CSCM_ERROR_DEBUG_OPTION_N);
	} else {
		option_index = _cscm_debug_cmd_vector[1];
		if (!cscm_text_is_integer(option_index))
			cscm_error_report("cscm_debug_cmd_handler_frame", \
					CSCM_ERROR_DEBUG_CMD_FRAME_INDEX);

		index = atol(option_index);
	}


	real_env = (CSCM_ENV *)env->value;

	if (index < 0 || index >= real_env->n_frames)
		cscm_error_report("cscm_debug_cmd_handler_frame", \
				CSCM_ERROR_DEBUG_CMD_FRAME_INDEX);

	cscm_frame_print_details(real_env->frames[index], "");


	return CSCM_DEBUG_CMD_RET_CONTINUE;
}




int _cscm_debug_cmd_handler_backtrace(CSCM_OBJECT *env)
{
	int i;
	CSCM_AST_NODE *exp;

	if (_cscm_debug_cmd_count != 1)
		cscm_error_report("cscm_debug_cmd_handler_backtrace", \
				CSCM_ERROR_DEBUG_OPTION_N);


	cscm_ef_backtrace_backup();

	for (i = 0; !cscm_ef_backtrace_is_empty(); i++) {
		exp = cscm_ef_backtrace_pop();

		printf("[%d] %s:%lu ",		\
			i,			\
			exp->filename,		\
			(unsigned long)exp->line);
		cscm_ast_print_tree(exp);
		puts("");
	}

	cscm_ef_backtrace_restore();


	return CSCM_DEBUG_CMD_RET_CONTINUE;
}




CSCM_DEBUG_CMD _cscm_debug_cmd_list[] = {
	{0, "help", _cscm_debug_cmd_handler_help},
	{0, "print", _cscm_debug_cmd_handler_print},
	{0, "next", _cscm_debug_cmd_handler_next},
	{0, "env", _cscm_debug_cmd_handler_env},
	{0, "frame", _cscm_debug_cmd_handler_frame},
	{0, "backtrace", _cscm_debug_cmd_handler_backtrace},
	{1, NULL, NULL}
};


void cscm_debug_shell_start(CSCM_EF *ef, CSCM_OBJECT *env)
{
	int ret;
	CSCM_AST_NODE *exp;
	CSCM_DEBUG_CMD *cmd;


	exp = ef->exp;
	printf("NEXT: %s:%lu: ",	\
		exp->filename,		\
		(unsigned long)exp->line);
	cscm_ast_print_tree(exp);
	puts("");


	if(_cscm_debug_next) {
		_cscm_debug_next--;
		return;
	}


	ret = CSCM_DEBUG_CMD_RET_CONTINUE;
	while (ret != CSCM_DEBUG_CMD_RET_EXIT)
	{
		fputs(CSCM_DEBUG_SHELL_PROMPT, stdout);
		fflush(stdout);

		_cscm_debug_parse_cmd();


		if (_cscm_debug_cmd_count == 0)
			continue;


		for (cmd = _cscm_debug_cmd_list; !(cmd->flag_last); cmd++) {
			if (!strcmp(cmd->name, _cscm_debug_cmd_vector[0])) {
				ret = cmd->handler(env);
				break;
			}
		}


		if (cmd->flag_last)
			cscm_error_report("cscm_debug_shell_start", \
					CSCM_ERROR_DEBUG_UNKNOWN_CMD);
	}
}
