/* builtin_symbol.c -- cscheme standard library module: symbol

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
#include "object.h"
#include "num.h"
#include "symbol.h"
#include "env.h"
#include "proc.h"
#include "builtin.h"
#include "builtin_symbol.h"




/* (symbol number) -> symbol
 * (symbol symbol) -> symbol
 * (symbol string) -> symbol
 * */
CSCM_OBJECT *cscm_builtin_proc_symbol(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *obj;

	CSCM_OBJECT *symbol;
	char num_buf[CSCM_NUM_MAX_TEXT_LEN + 1];


	cscm_builtin_check_args("cscm_builtin_proc_symbol",	\
				1,				\
				n,				\
				args);


	obj = args[0];


	symbol = cscm_symbol_create();

	if (obj->type == CSCM_OBJECT_TYPE_NUM_LONG) {
		snprintf(num_buf, CSCM_NUM_MAX_TEXT_LEN, "%ld", \
			cscm_num_long_get(obj));

		cscm_symbol_set(symbol, num_buf);
	} else if (obj->type == CSCM_OBJECT_TYPE_NUM_DOUBLE) {
		snprintf(num_buf, CSCM_NUM_MAX_TEXT_LEN, "%f", \
			cscm_num_double_get(obj));

		cscm_symbol_set(symbol, num_buf);
	} else if (obj->type == CSCM_OBJECT_TYPE_SYMBOL \
		|| obj->type == CSCM_OBJECT_TYPE_STRING) {
		cscm_symbol_set(symbol, (char *)obj->value);
	} else {
		cscm_error_report("cscm_builtin_proc_symbol", \
				CSCM_ERROR_OBJECT_TYPE);
	}


	return symbol;
}




/* (symbol-append symbol1 [symbol2] [symbol3] ...) */
CSCM_OBJECT *cscm_builtin_proc_symbol_append(size_t n, CSCM_OBJECT **args)
{
	int i;
	char *dest, *src;
	CSCM_OBJECT *symbol;

	size_t len;
	char *text;


	cscm_builtin_check_lb_args("cscm_builtin_proc_symbol_append",	\
				1,					\
				n,					\
				args);


	len = 0;
	for (i = 0; i < n; i++) {
		symbol = args[i];
		
		if (symbol->type != CSCM_OBJECT_TYPE_SYMBOL)
			cscm_error_report("cscm_builtin_proc_symbol_append", \
					CSCM_ERROR_OBJECT_TYPE);

		len += strlen((char *)symbol->value);
	}


	len++; // terminating null byte


	text = malloc(len);
	if (text == NULL)
		cscm_libc_fail("cscm_builtin_proc_symbol_append", \
				"malloc");

	dest = text;
	for (i = 0; i < n; i++) {
		symbol = args[i];

		for (src = (char *)symbol->value; *src; src++, dest++)
			*dest = *src;
	}

	*dest = 0;


	symbol = cscm_symbol_create();
	cscm_symbol_set_simple(symbol, text);


	return symbol;
}




void cscm_builtin_module_func_symbol()
{
	CSCM_OBJECT *global_env;

	CSCM_OBJECT *proc;


	global_env = cscm_global_env_get();


	proc = cscm_proc_prim_create();
	cscm_proc_prim_set(proc, cscm_builtin_proc_symbol);
	cscm_env_add_var(global_env, "symbol", proc);

	proc = cscm_proc_prim_create();
	cscm_proc_prim_set(proc, cscm_builtin_proc_symbol_append);
	cscm_env_add_var(global_env, "symbol-append", proc);
}
