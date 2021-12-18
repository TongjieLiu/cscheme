/* builtin.c -- cscheme standard library

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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "error.h"
#include "object.h"
#include "pair.h"
#include "bool.h"
#include "ast.h"
#include "str.h"
#include "symbol.h"
#include "num.h"
#include "core.h"
#include "ef.h"
#include "env.h"
#include "builtin.h"
#include "builtin_seq.h"




/* funcname: name of the caller function
 * rn: requested argument number
 * n: actual argument number
 * args: argument pointers */
void cscm_builtin_check_args(char *funcname,	\
			size_t rn, size_t n,	\
			CSCM_OBJECT **args)
{
	int i;


	if (funcname == NULL)
		cscm_error_report("cscm_builtin_check_args", \
				CSCM_ERROR_BUILTIN_FUNCNAME);


	if (n != rn)
		cscm_error_report(funcname, CSCM_ERROR_BUILTIN_N_ARGS);


	if (n >= 1 && args == NULL)
		cscm_error_report(funcname, CSCM_ERROR_NULL_PTR);


	for (i = 0; i < n; i++)
		if (args[i] == NULL)
			cscm_error_report(funcname, CSCM_ERROR_NULL_PTR);
}


/* funcname: name of the caller function
 * lbn: requested lower bound of argument number
 * n: actual argument number
 * args: argument pointers */
void cscm_builtin_check_lb_args(char *funcname,	\
			size_t lbn, size_t n,	\
			CSCM_OBJECT **args)
{
	int i;


	if (funcname == NULL)
		cscm_error_report("cscm_builtin_check_lb_args", \
				CSCM_ERROR_BUILTIN_FUNCNAME);


	if (n < lbn)
		cscm_error_report(funcname, CSCM_ERROR_BUILTIN_N_ARGS);


	if (n >= 1 && args == NULL)
		cscm_error_report(funcname, CSCM_ERROR_NULL_PTR);


	for (i = 0; i < n; i++)
		if (args[i] == NULL)
			cscm_error_report(funcname, CSCM_ERROR_NULL_PTR);
}


/* funcname: name of the caller function
 * lbn: requested lower bound of argument number
 * ubn: requested upper bound of argument number
 * n: actual argument number
 * args: argument pointers */
void cscm_builtin_check_interval_args(char *funcname,			\
			size_t lbn, size_t ubn, size_t n,	\
			CSCM_OBJECT **args)
{
	int i;


	if (funcname == NULL)
		cscm_error_report("cscm_builtin_check_interval_args", \
				CSCM_ERROR_BUILTIN_FUNCNAME);


	if (n < lbn || n > ubn)
		cscm_error_report(funcname, CSCM_ERROR_BUILTIN_N_ARGS);


	if (n >= 1 && args == NULL)
		cscm_error_report(funcname, CSCM_ERROR_NULL_PTR);


	for (i = 0; i < n; i++)
		if (args[i] == NULL)
			cscm_error_report(funcname, CSCM_ERROR_NULL_PTR);
}




/* do not accept 0 argument */
CSCM_OBJECT *cscm_builtin_proc_print(size_t n, CSCM_OBJECT **args)
{
	int i;


	cscm_builtin_check_lb_args("cscm_builtin_proc_print",	\
				1,				\
				n,				\
				args);


	cscm_object_print(args[0], stdout);


	for (i = 1; i < n; i++) {
		fputc(' ', stdout);
		cscm_object_print(args[i], stdout);
	}


	return NULL;
}


/* accept 0 argument */
CSCM_OBJECT *cscm_builtin_proc_printn(size_t n, CSCM_OBJECT **args)
{
	cscm_builtin_check_lb_args("cscm_builtin_proc_printn",	\
				0,				\
				n,				\
				args);


	if (n >= 1)
		cscm_builtin_proc_print(n, args);


	fputc('\n', stdout);


	return NULL;
}


CSCM_OBJECT *cscm_builtin_proc_display(size_t n, CSCM_OBJECT **args)
{
	cscm_builtin_check_args("cscm_builtin_proc_display",	\
				1,				\
				n,				\
				args);


	cscm_object_print(args[0], stdout);


	return NULL;
}


CSCM_OBJECT *cscm_builtin_proc_newline(size_t n, CSCM_OBJECT **args)
{
	cscm_builtin_check_args("cscm_builtin_proc_newline",	\
				0,				\
				n,				\
				args);


	fputc('\n', stdout);


	return NULL;
}


CSCM_OBJECT *cscm_builtin_proc_read(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *global_env;

	CSCM_OBJECT *list;
	CSCM_AST_NODE *list_ast;


	cscm_builtin_check_args("cscm_builtin_proc_newline",	\
				0,				\
				n,				\
				args);


	global_env = cscm_global_env_get();


	list_ast = cscm_list_ast_build(stdin, "-");

	list = cscm_ef_exec(cscm_analyze(list_ast), global_env);


	cscm_ast_free_tree(list_ast);

	return list;
}




CSCM_OBJECT *cscm_builtin_proc_set_car(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *pair, *obj;


	cscm_builtin_check_args("cscm_builtin_proc_set_car",	\
				2,				\
				n,				\
				args);


	pair = args[0];
	obj = args[1];


	if (pair->type != CSCM_OBJECT_TYPE_PAIR)
		cscm_error_report("cscm_builtin_proc_set_car", \
				CSCM_ERROR_OBJECT_TYPE);


	cscm_pair_set_car(pair, obj);


	return NULL;
}


CSCM_OBJECT *cscm_builtin_proc_set_cdr(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *pair, *obj;


	cscm_builtin_check_args("cscm_builtin_proc_set_cdr",	\
				2,				\
				n,				\
				args);


	pair = args[0];
	obj = args[1];


	if (pair->type != CSCM_OBJECT_TYPE_PAIR)
		cscm_error_report("cscm_builtin_proc_set_cdr", \
				CSCM_ERROR_OBJECT_TYPE);


	cscm_pair_set_cdr(pair, obj);


	return NULL;
}




CSCM_OBJECT *cscm_builtin_proc_cons(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *pair;


	cscm_builtin_check_args("cscm_builtin_proc_cons",	\
				2,				\
				n,				\
				args);


	pair = cscm_pair_create();

	cscm_pair_set(pair, args[0], args[1]);


	return pair;
}


CSCM_OBJECT *cscm_builtin_proc_car(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *pair;


	cscm_builtin_check_args("cscm_builtin_proc_car",	\
				1,				\
				n,				\
				args);


	pair = args[0];


	if (pair->type != CSCM_OBJECT_TYPE_PAIR)
		cscm_error_report("cscm_builtin_proc_car", \
				CSCM_ERROR_OBJECT_TYPE);


	return cscm_pair_get_car(pair);
}


CSCM_OBJECT *cscm_builtin_proc_cdr(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *pair;


	cscm_builtin_check_args("cscm_builtin_proc_cdr",	\
				1,				\
				n,				\
				args);


	pair = args[0];


	if (pair->type != CSCM_OBJECT_TYPE_PAIR)
		cscm_error_report("cscm_builtin_proc_cdr", \
				CSCM_ERROR_OBJECT_TYPE);


	return cscm_pair_get_cdr(pair);
}




CSCM_OBJECT *cscm_builtin_proc_list(size_t n, CSCM_OBJECT **args)
{
	cscm_builtin_check_lb_args("cscm_builtin_proc_list",	\
				0,				\
				n,				\
				args);


	if (n == 0)
		return CSCM_NIL;
	else
		return cscm_list_create(n, args);	
}




CSCM_OBJECT *cscm_builtin_proc_add(size_t n, CSCM_OBJECT **args)
{
	int i;

	int flag_long_ret;

	long l;
	double d;

	CSCM_OBJECT *ret;


	cscm_builtin_check_lb_args("cscm_builtin_proc_add",	\
				1,				\
				n,				\
				args);


	if (n == 1)
		return args[0];


	flag_long_ret = 1;
	for (i = 0; i < n; i++) {
		if (args[i]->type != CSCM_OBJECT_TYPE_NUM_LONG) {
			flag_long_ret = 0;
			break;
		}
	}


	l = d = 0;
	if (flag_long_ret) {
		for (i = 0; i < n; i++)
			l += cscm_num_long_get(args[i]);

		ret = cscm_num_long_create();
		cscm_num_long_set(ret, l);
	} else {
		for (i = 0; i < n; i++) {
			if (args[i]->type == CSCM_OBJECT_TYPE_NUM_LONG)
				d += cscm_num_long_get(args[i]);
			else if (args[i]->type == CSCM_OBJECT_TYPE_NUM_DOUBLE)
				d += cscm_num_double_get(args[i]);
			else
				cscm_error_report("cscm_builtin_proc_add", \
						CSCM_ERROR_OBJECT_TYPE);
		}

		ret = cscm_num_double_create();
		cscm_num_double_set(ret, d);
	}


	return ret;
}




CSCM_OBJECT *cscm_builtin_proc_subtract(size_t n, CSCM_OBJECT **args)
{
	int i;

	int flag_long_ret;

	long l;
	double d;

	CSCM_OBJECT *ret;


	cscm_builtin_check_lb_args("cscm_builtin_proc_subtract",	\
				1,					\
				n,					\
				args);


	if (n == 1) {
		if (args[0]->type == CSCM_OBJECT_TYPE_NUM_LONG) {
			l = cscm_num_long_get(args[0]);

			ret = cscm_num_long_create();
			cscm_num_long_set(ret, -l);
		} else if (args[0]->type == CSCM_OBJECT_TYPE_NUM_DOUBLE) {
			d = cscm_num_double_get(args[0]);

			ret = cscm_num_double_create();
			cscm_num_double_set(ret, -d);
		} else {
			cscm_error_report("cscm_builtin_proc_subtract", \
					CSCM_ERROR_OBJECT_TYPE);
		}


		return ret;
	}


	flag_long_ret = 1;
	for (i = 0; i < n; i++) {
		if (args[i]->type != CSCM_OBJECT_TYPE_NUM_LONG) {
			flag_long_ret = 0;
			break;
		}
	}


	if (flag_long_ret) {
		l = cscm_num_long_get(args[0]);


		for (i = 1; i < n; i++)
			l -= cscm_num_long_get(args[i]);


		ret = cscm_num_long_create();
		cscm_num_long_set(ret, l);
	} else {
		if (args[0]->type == CSCM_OBJECT_TYPE_NUM_LONG)
			d = cscm_num_long_get(args[0]);
		else if (args[0]->type == CSCM_OBJECT_TYPE_NUM_DOUBLE)
			d = cscm_num_double_get(args[0]);
		else
			cscm_error_report("cscm_builtin_proc_subtract", \
					CSCM_ERROR_OBJECT_TYPE);


		for (i = 1; i < n; i++) {
			if (args[i]->type == CSCM_OBJECT_TYPE_NUM_LONG)
				d -= cscm_num_long_get(args[i]);
			else if (args[i]->type == CSCM_OBJECT_TYPE_NUM_DOUBLE)
				d -= cscm_num_double_get(args[i]);
			else
				cscm_error_report("cscm_builtin_proc_subtract", \
						CSCM_ERROR_OBJECT_TYPE);
		}


		ret = cscm_num_double_create();
		cscm_num_double_set(ret, d);
	}


	return ret;
}




CSCM_OBJECT *cscm_builtin_proc_multiply(size_t n, CSCM_OBJECT **args)
{
	int i;

	int flag_long_ret;

	long l;
	double d;

	CSCM_OBJECT *ret;


	cscm_builtin_check_lb_args("cscm_builtin_proc_multiply",	\
				1,					\
				n,					\
				args);


	if (n == 1)
		return args[0];


	flag_long_ret = 1;
	for (i = 0; i < n; i++) {
		if (args[i]->type != CSCM_OBJECT_TYPE_NUM_LONG) {
			flag_long_ret = 0;
			break;
		}
	}


	l = d = 1;
	if (flag_long_ret) {
		for (i = 0; i < n; i++)
			l *= cscm_num_long_get(args[i]);

		ret = cscm_num_long_create();
		cscm_num_long_set(ret, l);
	} else {
		for (i = 0; i < n; i++) {
			if (args[i]->type == CSCM_OBJECT_TYPE_NUM_LONG)
				d *= cscm_num_long_get(args[i]);
			else if (args[i]->type == CSCM_OBJECT_TYPE_NUM_DOUBLE)
				d *= cscm_num_double_get(args[i]);
			else
				cscm_error_report("cscm_builtin_proc_multiply", \
						CSCM_ERROR_OBJECT_TYPE);
		}

		ret = cscm_num_double_create();
		cscm_num_double_set(ret, d);
	}


	return ret;
}




CSCM_OBJECT *cscm_builtin_proc_divide(size_t n, CSCM_OBJECT **args)
{
	int i;

	int flag_long_ret;

	long l;
	double d;

	ldiv_t q;

	CSCM_OBJECT *ret;


	cscm_builtin_check_lb_args("cscm_builtin_proc_divide",	\
				1,				\
				n,				\
				args);


	if (n == 1) {
		if (args[0]->type == CSCM_OBJECT_TYPE_NUM_LONG)
			d = cscm_num_long_get(args[0]);
		else if (args[0]->type == CSCM_OBJECT_TYPE_NUM_DOUBLE)
			d = cscm_num_double_get(args[0]);
		else
			cscm_error_report("cscm_builtin_proc_divide", \
					CSCM_ERROR_OBJECT_TYPE);


		ret = cscm_num_double_create();
		cscm_num_double_set(ret, 1 / d);

		return ret;
	}


	flag_long_ret = 1;
	for (i = 0; i < n; i++) {
		if (args[i]->type != CSCM_OBJECT_TYPE_NUM_LONG) {
			flag_long_ret = 0;
			break;
		}
	}


	if (flag_long_ret) {
		l = cscm_num_long_get(args[0]);

		for (i = 1; i < n; i++) {
			q = ldiv(l, cscm_num_long_get(args[i]));

			if (q.rem) {
				flag_long_ret = 0;
				break;
			} else {
				l = q.quot;
			}
		}
	}


	if (flag_long_ret) {
		ret = cscm_num_long_create();
		cscm_num_long_set(ret, l);
	} else {
		if (args[0]->type == CSCM_OBJECT_TYPE_NUM_LONG)
			d = cscm_num_long_get(args[0]);
		else if (args[0]->type == CSCM_OBJECT_TYPE_NUM_DOUBLE)
			d = cscm_num_double_get(args[0]);
		else
			cscm_error_report("cscm_builtin_proc_divide", \
					CSCM_ERROR_OBJECT_TYPE);


		for (i = 1; i < n; i++) {
			if (args[i]->type == CSCM_OBJECT_TYPE_NUM_LONG)
				d /= cscm_num_long_get(args[i]);
			else if (args[i]->type == CSCM_OBJECT_TYPE_NUM_DOUBLE)
				d /= cscm_num_double_get(args[i]);
			else
				cscm_error_report("cscm_builtin_proc_divide", \
						CSCM_ERROR_OBJECT_TYPE);
		}


		ret = cscm_num_double_create();
		cscm_num_double_set(ret, d);
	}


	return ret;
}




CSCM_OBJECT *cscm_builtin_proc_remainder(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *x, *y;
	CSCM_OBJECT *ret;


	cscm_builtin_check_args("cscm_builtin_proc_remainder",	\
				2,				\
				n,				\
				args);


	x = args[0];
	y = args[1];


	if (x->type != CSCM_OBJECT_TYPE_NUM_LONG \
		|| y->type != CSCM_OBJECT_TYPE_NUM_LONG)
		cscm_error_report("cscm_builtin_proc_remainder", \
				CSCM_ERROR_OBJECT_TYPE);


	ret = cscm_num_long_create();
	cscm_num_long_set(ret, \
			cscm_num_long_get(x) % cscm_num_long_get(y));


	return ret;
}




#define _CSCM_BUILTIN_NUMCMP_OP_E	0
#define _CSCM_BUILTIN_NUMCMP_OP_G	1
#define _CSCM_BUILTIN_NUMCMP_OP_GE	2
#define _CSCM_BUILTIN_NUMCMP_OP_L	3
#define _CSCM_BUILTIN_NUMCMP_OP_LE	4


CSCM_OBJECT *_cscm_builtin_numcmp(char *func, int op, size_t n, CSCM_OBJECT **args)
{
	int result;

	long lval_x, lval_y;
	double dval_x, dval_y;
	CSCM_OBJECT *x, *y;


	cscm_builtin_check_args(func,	\
				2,	\
				n,	\
				args);


	x = args[0];
	y = args[1];


	if (x->type == CSCM_OBJECT_TYPE_NUM_LONG
		&& y->type == CSCM_OBJECT_TYPE_NUM_LONG) {
		lval_x = cscm_num_long_get(x);
		lval_y = cscm_num_long_get(y);


		switch (op)
		{
			case _CSCM_BUILTIN_NUMCMP_OP_E:
				result = lval_x == lval_y;
				break;
			case _CSCM_BUILTIN_NUMCMP_OP_G:
				result = lval_x > lval_y;
				break;
			case _CSCM_BUILTIN_NUMCMP_OP_GE:
				result = lval_x >= lval_y;
				break;
			case _CSCM_BUILTIN_NUMCMP_OP_L:
				result = lval_x < lval_y;
				break;
			case _CSCM_BUILTIN_NUMCMP_OP_LE:
				result = lval_x <= lval_y;
				break;
			default:
				cscm_error_report("_cscm_builtin_numcmp", \
						CSCM_ERROR_BUILTIN_BAD_OP);
		}


		if (result)
			return CSCM_TRUE;
		else
			return CSCM_FALSE;

	}
	
	
	// cast both x and y to double if at least one of them is double
	if (x->type == CSCM_OBJECT_TYPE_NUM_LONG		\
		&& y->type == CSCM_OBJECT_TYPE_NUM_DOUBLE) {
		dval_x = cscm_num_long_get(x);
		dval_y = cscm_num_double_get(y);
	} else if (x->type == CSCM_OBJECT_TYPE_NUM_DOUBLE	\
		&& y->type == CSCM_OBJECT_TYPE_NUM_LONG) {
		dval_x = cscm_num_double_get(x);
		dval_y = cscm_num_long_get(y);
	} else if (x->type == CSCM_OBJECT_TYPE_NUM_DOUBLE	\
		&& y->type == CSCM_OBJECT_TYPE_NUM_DOUBLE) {
		dval_x = cscm_num_double_get(x);
		dval_y = cscm_num_double_get(y);
	} else {
		cscm_error_report(func, CSCM_ERROR_OBJECT_TYPE);
	}


	switch (op)
	{
		case _CSCM_BUILTIN_NUMCMP_OP_E:
			result = dval_x == dval_y;
			break;
		case _CSCM_BUILTIN_NUMCMP_OP_G:
			result = dval_x > dval_y;
			break;
		case _CSCM_BUILTIN_NUMCMP_OP_GE:
			result = dval_x >= dval_y;
			break;
		case _CSCM_BUILTIN_NUMCMP_OP_L:
			result = dval_x < dval_y;
			break;
		case _CSCM_BUILTIN_NUMCMP_OP_LE:
			result = dval_x <= dval_y;
			break;
		default:
			cscm_error_report("_cscm_builtin_numcmp", \
					CSCM_ERROR_BUILTIN_BAD_OP);
	}


	if (result)
		return CSCM_TRUE;
	else
		return CSCM_FALSE;
}


CSCM_OBJECT *cscm_builtin_proc_equal_num(size_t n, CSCM_OBJECT **args)
{
	_cscm_builtin_numcmp("cscm_builtin_proc_equal_num",	\
		_CSCM_BUILTIN_NUMCMP_OP_E,			\
		n,						\
		args);
}


/* test for equality of symbols and strings */
CSCM_OBJECT *cscm_builtin_proc_equal_ss(size_t n, CSCM_OBJECT **args)
{
	char *text_x, *text_y;
	CSCM_OBJECT *x, *y;


	cscm_builtin_check_args("cscm_builtin_proc_equal_ss",	\
				2,				\
				n,				\
				args);


	x = args[0];
	y = args[1];


	if (x->type == CSCM_OBJECT_TYPE_SYMBOL)
		text_x = cscm_symbol_get(x);
	else if (x->type == CSCM_OBJECT_TYPE_STRING)
		text_x = cscm_string_get(x);
	else
		cscm_error_report("cscm_builtin_proc_equal_ss", \
				CSCM_ERROR_OBJECT_TYPE);


	if (y->type == CSCM_OBJECT_TYPE_SYMBOL)
		text_y = cscm_symbol_get(y);
	else if (y->type == CSCM_OBJECT_TYPE_STRING)
		text_y = cscm_string_get(y);
	else
		cscm_error_report("cscm_builtin_proc_equal_ss", \
				CSCM_ERROR_OBJECT_TYPE);


	if (!strcmp(text_x, text_y))
		return CSCM_TRUE;
	else
		return CSCM_FALSE;

}




CSCM_OBJECT *cscm_builtin_proc_equal(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *x, *y;


	cscm_builtin_check_args("cscm_builtin_proc_equal",	\
				2,				\
				n,				\
				args);


	x = args[0];
	y = args[1];


	if ((x->type == CSCM_OBJECT_TYPE_NUM_LONG		\
		|| x->type == CSCM_OBJECT_TYPE_NUM_DOUBLE)	\
		&&						\
		(y->type == CSCM_OBJECT_TYPE_NUM_LONG		\
	 	|| y->type == CSCM_OBJECT_TYPE_NUM_DOUBLE))
		return cscm_builtin_proc_equal_num(n, args);		
	else if ((x->type == CSCM_OBJECT_TYPE_SYMBOL		\
		&& y->type == CSCM_OBJECT_TYPE_SYMBOL)		\
		||						\
		(x->type == CSCM_OBJECT_TYPE_STRING		\
	 	&& y->type == CSCM_OBJECT_TYPE_STRING))
		return cscm_builtin_proc_equal_ss(n, args);		
	else
		cscm_error_report("cscm_builtin_proc_equal", \
				CSCM_ERROR_OBJECT_TYPE);
}




CSCM_OBJECT *cscm_builtin_proc_greater_than(size_t n, CSCM_OBJECT **args)
{
	_cscm_builtin_numcmp("cscm_builtin_proc_greater_than",	\
		_CSCM_BUILTIN_NUMCMP_OP_G,			\
		n,						\
		args);
}


CSCM_OBJECT *cscm_builtin_proc_greater_equal(size_t n, CSCM_OBJECT **args)
{
	_cscm_builtin_numcmp("cscm_builtin_proc_greater_equal",	\
		_CSCM_BUILTIN_NUMCMP_OP_GE,			\
		n,						\
		args);
}


CSCM_OBJECT *cscm_builtin_proc_less_than(size_t n, CSCM_OBJECT **args)
{
	_cscm_builtin_numcmp("cscm_builtin_proc_less_than",	\
		_CSCM_BUILTIN_NUMCMP_OP_L,			\
		n,						\
		args);
}


CSCM_OBJECT *cscm_builtin_proc_less_equal(size_t n, CSCM_OBJECT **args)
{
	_cscm_builtin_numcmp("cscm_builtin_proc_less_equal",	\
		_CSCM_BUILTIN_NUMCMP_OP_LE,			\
		n,						\
		args);
}




CSCM_OBJECT *cscm_builtin_proc_is_string(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *x;


	cscm_builtin_check_args("cscm_builtin_proc_is_string",	\
				1,				\
				n,				\
				args);


	x = args[0];


	if (x->type == CSCM_OBJECT_TYPE_STRING)
		return CSCM_TRUE;
	else
		return CSCM_FALSE;
}


CSCM_OBJECT *cscm_builtin_proc_is_symbol(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *x;


	cscm_builtin_check_args("cscm_builtin_proc_is_symbol",	\
				1,				\
				n,				\
				args);


	x = args[0];


	if (x->type == CSCM_OBJECT_TYPE_SYMBOL)
		return CSCM_TRUE;
	else
		return CSCM_FALSE;
}


CSCM_OBJECT *cscm_builtin_proc_is_number(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *x;


	cscm_builtin_check_args("cscm_builtin_proc_is_number",	\
				1,				\
				n,				\
				args);


	x = args[0];


	if (x->type == CSCM_OBJECT_TYPE_NUM_LONG \
		|| x->type == CSCM_OBJECT_TYPE_NUM_DOUBLE)
		return CSCM_TRUE;
	else
		return CSCM_FALSE;
}


CSCM_OBJECT *cscm_builtin_proc_is_pair(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *x;


	cscm_builtin_check_args("cscm_builtin_proc_is_pair",	\
				1,				\
				n,				\
				args);


	x = args[0];


	if (x->type == CSCM_OBJECT_TYPE_PAIR)
		return CSCM_TRUE;
	else
		return CSCM_FALSE;
}


CSCM_OBJECT *cscm_builtin_proc_is_nil(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *x;


	cscm_builtin_check_args("cscm_builtin_proc_is_nil",	\
				1,				\
				n,				\
				args);


	x = args[0];


	if (x == CSCM_NIL)
		return CSCM_TRUE;
	else
		return CSCM_FALSE;
}




CSCM_BUILTIN_MODULE _cscm_builtin_module_list[] = {
	{0, "seq", cscm_builtin_module_func_seq},

	{1, NULL, NULL}
};


CSCM_OBJECT *cscm_builtin_proc_include(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *mod_name;

	CSCM_BUILTIN_MODULE *mod;


	cscm_builtin_check_args("cscm_builtin_proc_include",	\
				1,				\
				n,				\
				args);


	mod_name = args[0];


	if (mod_name->type != CSCM_OBJECT_TYPE_STRING)
		cscm_error_report("cscm_builtin_proc_include", \
				CSCM_ERROR_OBJECT_TYPE);


	for (mod = _cscm_builtin_module_list; !(mod->flag_last); mod++) {
		if (cscm_string_text_equal(mod_name, mod->mod_name)) {
			mod->f();
			return NULL;
		}
	}


	cscm_error_report("cscm_builtin_proc_include", \
			CSCM_ERROR_BUILTIN_BAD_MODULE);
}




CSCM_OBJECT *cscm_builtin_proc_max(size_t n, CSCM_OBJECT **args)
{
	int i;

	int flag_long;

	long max_long_number;
	double max_double_number;

	CSCM_OBJECT *number;
	long long_number;
	double double_number;

	CSCM_OBJECT *ret;


	cscm_builtin_check_lb_args("cscm_builtin_proc_max",	\
				1,				\
				n,				\
				args);


	number = args[0];

	if (number->type == CSCM_OBJECT_TYPE_NUM_LONG) {
		flag_long = 1;
		max_long_number = cscm_num_long_get(number);
	} else if (number->type == CSCM_OBJECT_TYPE_NUM_DOUBLE) {
		flag_long = 0;
		max_double_number = cscm_num_double_get(number);
	} else {
		cscm_error_report("cscm_builtin_proc_max", \
				CSCM_ERROR_OBJECT_TYPE);
	}


	for (i = 1; i < n; i++) {
		number = args[i];

		if (number->type == CSCM_OBJECT_TYPE_NUM_LONG) {
			if (flag_long) {
				long_number = cscm_num_long_get(number);

				if (max_long_number < long_number)
					max_long_number = long_number;
			} else {
				double_number = cscm_num_long_get(number);

				if (max_double_number < double_number)
					max_double_number = double_number;
			}
		} else if (number->type == CSCM_OBJECT_TYPE_NUM_DOUBLE) {
			double_number = cscm_num_double_get(number);

			if (flag_long) {
				flag_long = 0;
				max_double_number = max_long_number;
			}

			if (max_double_number < double_number)
				max_double_number = double_number;
		} else {
			cscm_error_report("cscm_builtin_proc_max", \
					CSCM_ERROR_OBJECT_TYPE);
		}
	}


	if (flag_long) {
		ret = cscm_num_long_create();
		cscm_num_long_set(ret, max_long_number);
	} else {
		ret = cscm_num_double_create();
		cscm_num_double_set(ret, max_double_number);
	}


	return ret;
}




CSCM_OBJECT *cscm_builtin_proc_min(size_t n, CSCM_OBJECT **args)
{
	int i;

	int flag_long;

	long min_long_number;
	double min_double_number;

	CSCM_OBJECT *number;
	long long_number;
	double double_number;

	CSCM_OBJECT *ret;


	cscm_builtin_check_lb_args("cscm_builtin_proc_min",	\
				1,				\
				n,				\
				args);


	number = args[0];

	if (number->type == CSCM_OBJECT_TYPE_NUM_LONG) {
		flag_long = 1;
		min_long_number = cscm_num_long_get(number);
	} else if (number->type == CSCM_OBJECT_TYPE_NUM_DOUBLE) {
		flag_long = 0;
		min_double_number = cscm_num_double_get(number);
	} else {
		cscm_error_report("cscm_builtin_proc_min", \
				CSCM_ERROR_OBJECT_TYPE);
	}


	for (i = 1; i < n; i++) {
		number = args[i];

		if (number->type == CSCM_OBJECT_TYPE_NUM_LONG) {
			if (flag_long) {
				long_number = cscm_num_long_get(number);

				if (min_long_number > long_number)
					min_long_number = long_number;
			} else {
				double_number = cscm_num_long_get(number);

				if (min_double_number > double_number)
					min_double_number = double_number;
			}
		} else if (number->type == CSCM_OBJECT_TYPE_NUM_DOUBLE) {
			double_number = cscm_num_double_get(number);

			if (flag_long) {
				flag_long = 0;
				min_double_number = min_long_number;
			}

			if (min_double_number > double_number)
				min_double_number = double_number;
		} else {
			cscm_error_report("cscm_builtin_proc_min", \
					CSCM_ERROR_OBJECT_TYPE);
		}
	}


	if (flag_long) {
		ret = cscm_num_long_create();
		cscm_num_long_set(ret, min_long_number);
	} else {
		ret = cscm_num_double_create();
		cscm_num_double_set(ret, min_double_number);
	}


	return ret;
}
