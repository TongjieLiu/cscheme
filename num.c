/* num.c -- scheme number: long and double

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
#include "ast.h"
#include "ef.h"
#include "text.h"
#include "gc.h"
#include "num.h"




CSCM_OBJECT *cscm_num_long_create()
{
	long *l;
	CSCM_OBJECT *obj;


	obj = cscm_object_create();


	l = malloc(sizeof(long));
	if (l == NULL)
		cscm_libc_fail("cscm_num_long_create", "malloc");


	*l = 0;


	obj->type = CSCM_OBJECT_TYPE_NUM_LONG;
	obj->value = l;


	return obj;
}


CSCM_OBJECT *cscm_num_double_create()
{
	double *d;
	CSCM_OBJECT *obj;


	obj = cscm_object_create();


	d = malloc(sizeof(double));
	if (d == NULL)
		cscm_libc_fail("cscm_num_double_create", "malloc");


	*d = 0.0;


	obj->type = CSCM_OBJECT_TYPE_NUM_DOUBLE;
	obj->value = d;


	return obj;
}




void cscm_num_long_set(CSCM_OBJECT *num, long val)
{
	long *l;


	if (num == NULL)
		cscm_error_report("cscm_num_long_set", \
				CSCM_ERROR_NULL_PTR);
	else if (num->type != CSCM_OBJECT_TYPE_NUM_LONG)
		cscm_error_report("cscm_num_long_set", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (num->value == NULL)
		cscm_error_report("cscm_num_long_set", \
				CSCM_ERROR_EMPTY_OBJECT);


	l = (long *)num->value;


	*l = val;
}


void cscm_num_double_set(CSCM_OBJECT *num, double val)
{
	double *d;


	if (num == NULL)
		cscm_error_report("cscm_num_double_set", \
				CSCM_ERROR_NULL_PTR);
	else if (num->type != CSCM_OBJECT_TYPE_NUM_DOUBLE)
		cscm_error_report("cscm_num_double_set", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (num->value == NULL)
		cscm_error_report("cscm_num_double_set", \
				CSCM_ERROR_EMPTY_OBJECT);


	d = (double *)num->value;


	*d = val;
}




long cscm_num_long_get(CSCM_OBJECT *num)
{
	long *l;


	if (num == NULL)
		cscm_error_report("cscm_num_long_get", \
				CSCM_ERROR_NULL_PTR);
	else if (num->type != CSCM_OBJECT_TYPE_NUM_LONG)
		cscm_error_report("cscm_num_long_get", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (num->value == NULL)
		cscm_error_report("cscm_num_long_get", \
				CSCM_ERROR_EMPTY_OBJECT);


	l = (long *)num->value;


	return *l;
}


double cscm_num_double_get(CSCM_OBJECT *num)
{
	double *d;


	if (num == NULL)
		cscm_error_report("cscm_num_double_get", \
				CSCM_ERROR_NULL_PTR);
	else if (num->type != CSCM_OBJECT_TYPE_NUM_DOUBLE)
		cscm_error_report("cscm_num_double_get", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (num->value == NULL)
		cscm_error_report("cscm_num_double_get", \
				CSCM_ERROR_EMPTY_OBJECT);


	d = (double *)num->value;


	return *d;
}




void cscm_num_print(CSCM_OBJECT *obj, FILE *stream)
{
	long *l;
	double *d;


	if (obj == NULL || stream == NULL) {
		cscm_error_report("cscm_num_print", \
				CSCM_ERROR_NULL_PTR);
	} else if (obj->type == CSCM_OBJECT_TYPE_NUM_LONG) {
		l = (long *)obj->value;
		printf("%ld", *l);
	} else if (obj->type == CSCM_OBJECT_TYPE_NUM_DOUBLE) {
		d = (double *)obj->value;
		printf("%f", *d);
	} else {
		cscm_error_report("cscm_num_print", \
				CSCM_ERROR_OBJECT_TYPE);
	}
}




int cscm_is_num_long(CSCM_AST_NODE *exp)
{
	if (exp == NULL)
		cscm_error_report("cscm_is_num_long", \
				CSCM_ERROR_NULL_PTR);
	else if (!cscm_ast_is_symbol(exp))
		return 0;
	else if (cscm_ast_is_symbol_empty(exp))
		cscm_error_report("cscm_is_num_long", \
				CSCM_ERROR_AST_EMPTY_SYMBOL);


	if (cscm_text_is_integer(exp->text))
		return 1;
	else
		return 0;
}


int cscm_is_num_double(CSCM_AST_NODE *exp)
{
	if (exp == NULL)
		cscm_error_report("cscm_is_num_double", \
				CSCM_ERROR_NULL_PTR);
	else if (!cscm_ast_is_symbol(exp))
		return 0;
	else if (cscm_ast_is_symbol_empty(exp))
		cscm_error_report("cscm_is_num_double", \
				CSCM_ERROR_AST_EMPTY_SYMBOL);


	if (cscm_text_is_fpn(exp->text))
		return 1;
	else
		return 0;
}




CSCM_OBJECT *_cscm_num_long_ef(void *state, CSCM_OBJECT *env)
{
	return (CSCM_OBJECT *)state;
}


CSCM_EF *cscm_analyze_num_long(CSCM_AST_NODE *exp)
{
	long number;
	CSCM_OBJECT *number_obj;


	number = atol(exp->text);


	number_obj = cscm_num_long_create();
	cscm_num_long_set(number_obj, number);


	/*	The execution function only has 1 copy of this
	 * object, therefore we have to keep it safe no matter
	 * how many times the execution function will be called. */
	cscm_gc_inc(number_obj);


	return cscm_ef_construct(CSCM_EF_TYPE_NUM_LONG,		\
				number_obj,			\
				NULL,				\
				_cscm_num_long_ef);
}




CSCM_OBJECT *_cscm_num_double_ef(void *state, CSCM_OBJECT *env)
{
	return (CSCM_OBJECT *)state;
}


CSCM_EF *cscm_analyze_num_double(CSCM_AST_NODE *exp)
{
	double number;
	CSCM_OBJECT *number_obj;


	number = atof(exp->text);


	number_obj = cscm_num_double_create();
	cscm_num_double_set(number_obj, number);


	/*	The execution function only has 1 copy of this
	 * object, therefore we have to keep it safe no matter
	 * how many times the execution function will be called. */
	cscm_gc_inc(number_obj);


	return cscm_ef_construct(CSCM_EF_TYPE_NUM_DOUBLE,	\
				number_obj,			\
				NULL,				\
				_cscm_num_double_ef);
}




void cscm_num_ef_free(CSCM_EF *ef)
{
	CSCM_OBJECT *num;


	if (ef == NULL)
		cscm_error_report("cscm_num_ef_free", \
				CSCM_ERROR_NULL_PTR);
	else if (ef->type != CSCM_EF_TYPE_NUM_LONG \
		&& ef->type != CSCM_EF_TYPE_NUM_DOUBLE)
		cscm_error_report("cscm_num_ef_free", \
				CSCM_ERROR_EF_TYPE);


	num = (CSCM_OBJECT *)ef->state;

	cscm_gc_dec(num);
	cscm_gc_free(num);

	free(ef);
}




void cscm_num_free(CSCM_OBJECT *obj)
{
	if (obj == NULL)
		cscm_error_report("cscm_num_free", \
				CSCM_ERROR_NULL_PTR);
	else if (obj->type != CSCM_EF_TYPE_NUM_LONG \
		&& obj->type != CSCM_EF_TYPE_NUM_DOUBLE)
		cscm_error_report("cscm_num_free", \
				CSCM_ERROR_OBJECT_TYPE);


	free(obj->value); // pointed to memories allocated when created

	free(obj);
}
