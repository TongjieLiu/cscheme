/* ef.c -- execution function

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

#include <stdlib.h>
#include <stdio.h>

#include "error.h"
#include "object.h"
#include "env.h"
#include "num.h"
#include "symbol.h"
#include "str.h"
#include "var.h"
#include "assignment.h"
#include "definition.h"
#include "lambda.h"
#include "if.h"
#include "begin.h"
#include "logical.h"
#include "core.h"
#include "quote.h"
#include "quasiquote.h"
#include "ast.h"
#include "ef.h"




size_t _cscm_ef_total_count = 0;


void _cscm_ef_inc_total_count()
{
	_cscm_ef_total_count++;

	if ((_cscm_ef_total_count % 10) == 0)
		printf("*** EF DEBUG INFO *** "				\
			"total execution-function count: %lu\n",	\
			(unsigned long)_cscm_ef_total_count);
}


void _cscm_ef_dec_total_count()
{
	_cscm_ef_total_count--;

	if ((_cscm_ef_total_count % 10) == 0)
		printf("*** EF DEBUG INFO *** "				\
			"total execution-function count: %lu\n",	\
			(unsigned long)_cscm_ef_total_count);
}




CSCM_OBJECT *cscm_ef_exec(CSCM_EF *ef, CSCM_OBJECT *env)
{
	CSCM_OBJECT *ret;


	if (ef == NULL)
		cscm_error_report("cscm_ef_exec", \
				CSCM_ERROR_NULL_PTR);
	else if (env->type != CSCM_OBJECT_TYPE_ENV)
		cscm_error_report("cscm_ef_exec", \
				CSCM_ERROR_OBJECT_TYPE);

	if (ef->exp)
		cscm_ef_backtrace_push(ef->exp);

	ret =  ef->f(ef->state, env);

	if (ef->exp)
		cscm_ef_backtrace_pop();


	return ret;
}




CSCM_EF *cscm_ef_construct(int type, \
			void *state, CSCM_AST_NODE *exp, CSCM_EF_FUNC f)
{
	CSCM_EF *ef;


	if (f == NULL)
		cscm_error_report("cscm_ef_construct", \
				CSCM_ERROR_EF_BAD_FUNC);


	ef = malloc(sizeof(CSCM_EF));
	if (ef == NULL)
		cscm_libc_fail("cscm_ef_construct", "malloc");


	ef->type = type;

	ef->state = state;
	ef->exp = exp;
	ef->f = f;

	#ifdef __CSCM_EF_DEBUG__
		_cscm_ef_inc_total_count();
	#endif


	return ef;
}




CSCM_EF **cscm_ef_ptrs_create(size_t n)
{
	size_t size;
	CSCM_EF **efs;


	if (n == 0)
		cscm_error_report("cscm_ef_ptrs_create", \
				CSCM_ERROR_EF_ZERO_PTR);


	size = n * sizeof(CSCM_EF *);
	efs = malloc(size);
	if (efs == NULL)
		cscm_libc_fail("cscm_ef_ptrs_create", "malloc");


	return efs;
}




CSCM_EF_FREE_FUNC _cscm_ef_free_func_list[] = {
	cscm_num_ef_free,
	cscm_num_ef_free,
	cscm_symbol_ef_free,
	cscm_string_ef_free,
	cscm_var_ef_free,

	cscm_quote_ef_free,
	cscm_quasiquote_ef_free,
	cscm_assignment_ef_free,
	cscm_definition_ef_free,
	cscm_lambda_ef_free,
	cscm_if_ef_free,
	cscm_seq_ef_free,
	cscm_ao_ef_free,
	cscm_combination_ef_free
};


void cscm_ef_free_tree(CSCM_EF *ef)
{
	CSCM_EF_FREE_FUNC ef_free;


	if (ef == NULL)
		cscm_error_report("cscm_ef_free_tree", \
				CSCM_ERROR_NULL_PTR);
	else if (ef->type < 0 || ef->type >= CSCM_EF_TYPE_NONE)
		cscm_error_report("cscm_ef_free_tree", \
				CSCM_ERROR_EF_TYPE);


	ef_free = _cscm_ef_free_func_list[ef->type];
	ef_free(ef);

	#ifdef __CSCM_EF_DEBUG__
		_cscm_ef_dec_total_count();
	#endif
}




size_t _cscm_ef_backtrace_count = 0;
CSCM_AST_NODE *_cscm_ef_backtrace_stack[CSCM_EF_BACKTRACE_MAX_N];


void cscm_ef_backtrace_push(CSCM_AST_NODE *exp)
{
	if (exp == NULL)
		cscm_error_report("cscm_ef_backtrace_push", \
				CSCM_ERROR_NULL_PTR);
	else if (_cscm_ef_backtrace_count >= CSCM_EF_BACKTRACE_MAX_N)
		cscm_error_report("cscm_ef_backtrace_push", \
				CSCM_ERROR_EF_BACKTRACE_FULL_STACK);


	_cscm_ef_backtrace_stack[_cscm_ef_backtrace_count] = exp;
	_cscm_ef_backtrace_count++;
}


CSCM_AST_NODE *cscm_ef_backtrace_pop()
{
	if (_cscm_ef_backtrace_count == 0)
		cscm_error_report("cscm_ef_backtrace_pop", \
				CSCM_ERROR_EF_BACKTRACE_EMPTY_STACK);


	_cscm_ef_backtrace_count--;
	return _cscm_ef_backtrace_stack[_cscm_ef_backtrace_count];
}


int cscm_ef_backtrace_is_empty()
{
	return _cscm_ef_backtrace_count == 0 ? 1 : 0;
}
