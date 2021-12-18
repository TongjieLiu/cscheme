/* pair.c -- scheme pair and list

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
#include "gc.h"
#include "ast.h"
#include "num.h"
#include "str.h"
#include "symbol.h"
#include "pair.h"




/* there is only one copy of nil in cscheme */
CSCM_OBJECT _cscm_nil = {CSCM_OBJECT_TYPE_NIL, NULL, 0};




CSCM_OBJECT *cscm_pair_create()
{
	CSCM_OBJECT *obj;
	CSCM_PAIR *pair;


	pair = malloc(sizeof(CSCM_PAIR));
	if (pair == NULL)
		cscm_libc_fail("cscm_pair_create", "malloc");

	pair->car = NULL;
	pair->cdr = NULL;


	obj = cscm_object_create();

	obj->type = CSCM_OBJECT_TYPE_PAIR;
	obj->value = pair;


	return obj;
}




void cscm_pair_set(CSCM_OBJECT *pair_obj, void *car, void *cdr)
{
	CSCM_PAIR *pair;


	if (pair_obj == NULL || pair_obj->value == NULL)
		cscm_error_report("cscm_pair_set", CSCM_ERROR_NULL_PTR);
	else if (pair_obj->type != CSCM_OBJECT_TYPE_PAIR)
		cscm_error_report("cscm_pair_set", CSCM_ERROR_OBJECT_TYPE);
	else if (car == NULL || cdr == NULL)
		cscm_error_report("cscm_pair_set", CSCM_ERROR_NULL_PTR);


	pair = (CSCM_PAIR *)pair_obj->value;


	pair->car = car;
	pair->cdr = cdr;
}


void cscm_pair_set_car(CSCM_OBJECT *pair_obj, void *car)
{
	CSCM_PAIR *pair;


	if (pair_obj == NULL || pair_obj->value == NULL)
		cscm_error_report("cscm_pair_set_car", CSCM_ERROR_NULL_PTR);
	else if (pair_obj->type != CSCM_OBJECT_TYPE_PAIR)
		cscm_error_report("cscm_pair_set_car", CSCM_ERROR_OBJECT_TYPE);
	else if (car == NULL)
		cscm_error_report("cscm_pair_set_car", CSCM_ERROR_NULL_PTR);


	pair = (CSCM_PAIR *)pair_obj->value;


	if (pair->car) {
		cscm_gc_dec(pair->car);
		cscm_gc_free(pair->car);
	}


	cscm_gc_inc(car);
	pair->car = car;
}


void cscm_pair_set_cdr(CSCM_OBJECT *pair_obj, void *cdr)
{
	CSCM_PAIR *pair;


	if (pair_obj == NULL || pair_obj->value == NULL)
		cscm_error_report("cscm_pair_set_cdr", CSCM_ERROR_NULL_PTR);
	else if (pair_obj->type != CSCM_OBJECT_TYPE_PAIR)
		cscm_error_report("cscm_pair_set_cdr", CSCM_ERROR_OBJECT_TYPE);
	else if (cdr == NULL)
		cscm_error_report("cscm_pair_set_cdr", CSCM_ERROR_NULL_PTR);


	pair = (CSCM_PAIR *)pair_obj->value;


	if (pair->cdr) {
		cscm_gc_dec(pair->cdr);
		cscm_gc_free(pair->cdr);
	}


	cscm_gc_inc(cdr);
	pair->cdr = cdr;
}




void *cscm_pair_get_car(CSCM_OBJECT *pair_obj)
{
	CSCM_PAIR *pair;


	if (pair_obj == NULL || pair_obj->value == NULL)
		cscm_error_report("cscm_pair_get_car", CSCM_ERROR_NULL_PTR);
	else if (pair_obj->type != CSCM_OBJECT_TYPE_PAIR)
		cscm_error_report("cscm_pair_get_car", CSCM_ERROR_OBJECT_TYPE);


	pair = (CSCM_PAIR *)pair_obj->value;


	return pair->car;
}


void *cscm_pair_get_cdr(CSCM_OBJECT *pair_obj)
{
	CSCM_PAIR *pair;


	if (pair_obj == NULL || pair_obj->value == NULL)
		cscm_error_report("cscm_pair_get_cdr", CSCM_ERROR_NULL_PTR);
	else if (pair_obj->type != CSCM_OBJECT_TYPE_PAIR)
		cscm_error_report("cscm_pair_get_cdr", CSCM_ERROR_OBJECT_TYPE);


	pair = (CSCM_PAIR *)pair_obj->value;


	return pair->cdr;
}




/* sequence represented in list structure */
CSCM_OBJECT *cscm_list_create(size_t n, CSCM_OBJECT **objs)
{
	int i;

	CSCM_OBJECT *ret;
	CSCM_OBJECT **pairs;


	if (n == 0 || objs == NULL)
		cscm_error_report("cscm_list_create", \
				CSCM_ERROR_LIST_NO_OBJ);


	for (i = 0; i < n; i++)
		if (objs[i] == NULL)
			cscm_error_report("cscm_list_create", \
					CSCM_ERROR_NULL_PTR);


	pairs = cscm_object_ptrs_create(n);


	for (i = 0; i < n; i++) {
		pairs[i] = cscm_pair_create();
		cscm_pair_set_car(pairs[i], objs[i]);
	}


	for (i = 1; i < n; i++)
		cscm_pair_set_cdr(pairs[i - 1], pairs[i]);

	cscm_pair_set_cdr(pairs[n - 1], CSCM_NIL);


	ret = pairs[0];
	free(pairs);


	return ret;
}




/* sequence represented in list structure */
CSCM_OBJECT *cscm_list_range(long first, long last, long step)
{
	long abs_diff, abs_step;

	CSCM_OBJECT *number;

	CSCM_OBJECT *pair, *new_pair;
	CSCM_OBJECT *ret;


	if (step > 0 && first > last)
		cscm_error_report("cscm_list_range", \
				CSCM_ERROR_LIST_RANGE);
	else if (step < 0 && first < last)
		cscm_error_report("cscm_list_range", \
				CSCM_ERROR_LIST_RANGE);
	else if (step == 0)
		cscm_error_report("cscm_list_range", \
				CSCM_ERROR_LIST_RANGE);


	abs_diff = labs(last - first);
	abs_step = labs(step);

	if (abs_diff % abs_step)
		cscm_error_report("cscm_list_range", \
				CSCM_ERROR_LIST_RANGE);


	new_pair = cscm_pair_create();
	ret = new_pair;

	while (first != last) {
		pair = new_pair;

		number = cscm_num_long_create();
		cscm_num_long_set(number, first);

		new_pair = cscm_pair_create();

		cscm_pair_set(pair, number, new_pair);


		first += step;
	}


	number = cscm_num_long_create();
	cscm_num_long_set(number, last);

	cscm_pair_set(new_pair, number, CSCM_NIL);


	return ret;
}




/* sequence represented in list structure */
CSCM_OBJECT *cscm_list_cpy(CSCM_OBJECT *list)
{
	CSCM_OBJECT *src, *dest;
	CSCM_OBJECT *new_pair;

	CSCM_OBJECT *ret;


	if (list == NULL)
		cscm_error_report("cscm_list_cpy", \
				CSCM_ERROR_NULL_PTR);
	else if (list->type != CSCM_OBJECT_TYPE_PAIR)
		cscm_error_report("cscm_list_cpy", \
				CSCM_ERROR_OBJECT_TYPE);


	new_pair = cscm_pair_create();
	ret = new_pair;

	src = list;


	do {
		dest = new_pair;

		new_pair = cscm_pair_create();

		cscm_pair_set(dest, cscm_pair_get_car(src), new_pair);


		src = cscm_pair_get_cdr(src);

		if (src == NULL)
			cscm_error_report("cscm_list_cpy", \
					CSCM_ERROR_LIST_NOT_SEQ);
		else if (src->type != CSCM_OBJECT_TYPE_PAIR \
			&& src != CSCM_NIL)
			cscm_error_report("cscm_list_cpy", \
					CSCM_ERROR_LIST_NOT_SEQ);
	} while (src != CSCM_NIL);


	/* automatically freed the new_pair */
	cscm_pair_set_cdr(dest, CSCM_NIL);


	return ret;
}




/* sequence represented in list structure */
CSCM_OBJECT *cscm_list_reverse(CSCM_OBJECT *list)
{
	CSCM_OBJECT *src, *dest;
	CSCM_OBJECT *last_pair;


	if (list == NULL)
		cscm_error_report("cscm_list_reverse", \
				CSCM_ERROR_NULL_PTR);
	else if (list->type != CSCM_OBJECT_TYPE_PAIR)
		cscm_error_report("cscm_list_reverse", \
				CSCM_ERROR_OBJECT_TYPE);


	src = list;
	last_pair = CSCM_NIL;


	do {
		dest = cscm_pair_create();

		cscm_pair_set(dest, cscm_pair_get_car(src), last_pair);


		last_pair = dest;

		src = cscm_pair_get_cdr(src);

		if (src == NULL)
			cscm_error_report("cscm_list_reverse", \
					CSCM_ERROR_LIST_NOT_SEQ);
		else if (src->type != CSCM_OBJECT_TYPE_PAIR \
			&& src != CSCM_NIL)
			cscm_error_report("cscm_list_reverse", \
					CSCM_ERROR_LIST_NOT_SEQ);
	} while (src != CSCM_NIL);


	return dest;
}




/* sequence represented in list structure */
CSCM_OBJECT *cscm_list_append(CSCM_OBJECT *x, CSCM_OBJECT *y)
{
	CSCM_OBJECT *src, *dest;
	CSCM_OBJECT *new_pair;

	CSCM_OBJECT *ret;


	if (x == NULL || y == NULL)
		cscm_error_report("cscm_list_append", \
				CSCM_ERROR_NULL_PTR);
	else if (x->type != CSCM_OBJECT_TYPE_PAIR \
		|| y->type != CSCM_OBJECT_TYPE_PAIR)
		cscm_error_report("cscm_list_append", \
				CSCM_ERROR_OBJECT_TYPE);


	new_pair = cscm_pair_create();
	ret = new_pair;

	src = x;


	do {
		dest = new_pair;

		new_pair = cscm_pair_create();

		cscm_pair_set(dest, cscm_pair_get_car(src), new_pair);


		src = cscm_pair_get_cdr(src);

		if (src == NULL)
			cscm_error_report("cscm_list_append", \
					CSCM_ERROR_LIST_NOT_SEQ);
		else if (src->type != CSCM_OBJECT_TYPE_PAIR \
			&& src != CSCM_NIL)
			cscm_error_report("cscm_list_append", \
					CSCM_ERROR_LIST_NOT_SEQ);
	} while (src != CSCM_NIL);


	src = y;


	do {
		dest = new_pair;

		new_pair = cscm_pair_create();

		cscm_pair_set(dest, cscm_pair_get_car(src), new_pair);


		src = cscm_pair_get_cdr(src);

		if (src == NULL)
			cscm_error_report("cscm_list_append", \
					CSCM_ERROR_LIST_NOT_SEQ);
		else if (src->type != CSCM_OBJECT_TYPE_PAIR \
			&& src != CSCM_NIL)
			cscm_error_report("cscm_list_append", \
					CSCM_ERROR_LIST_NOT_SEQ);
	} while (src != CSCM_NIL);


	/* automatically freed the new_pair */
	cscm_pair_set_cdr(dest, CSCM_NIL);


	return ret;
}




/* sequence represented in list structure */
size_t cscm_list_get_len(CSCM_OBJECT *list)
{
	size_t len;

	CSCM_OBJECT *pair;


	if (list == NULL)
		cscm_error_report("cscm_list_get_len", \
				CSCM_ERROR_NULL_PTR);
	else if (list->type != CSCM_OBJECT_TYPE_PAIR)
		cscm_error_report("cscm_list_get_len", \
				CSCM_ERROR_OBJECT_TYPE);


	len = 0;
	pair = list;

	do {
		len++;


		pair = cscm_pair_get_cdr(pair);

		if (pair == NULL)
			cscm_error_report("cscm_list_get_len", \
					CSCM_ERROR_LIST_NOT_SEQ);
		else if (pair->type != CSCM_OBJECT_TYPE_PAIR \
			&& pair != CSCM_NIL)
			cscm_error_report("cscm_list_get_len", \
					CSCM_ERROR_LIST_NOT_SEQ);
	} while (pair != CSCM_NIL);


	return len;
}




/* sequence represented in list structure */
CSCM_OBJECT* cscm_list_index(CSCM_OBJECT *list, size_t index)
{
	size_t pos;

	CSCM_OBJECT *pair;


	if (list == NULL)
		cscm_error_report("cscm_list_index", \
				CSCM_ERROR_NULL_PTR);
	else if (list->type != CSCM_OBJECT_TYPE_PAIR)
		cscm_error_report("cscm_list_index", \
				CSCM_ERROR_OBJECT_TYPE);


	pos = 0;
	pair = list;

	do {
		if (pos == index)
			return cscm_pair_get_car(list);


		pair = cscm_pair_get_cdr(pair);

		if (pair->type != CSCM_OBJECT_TYPE_PAIR \
			&& pair != CSCM_NIL)
			cscm_error_report("cscm_list_index", \
					CSCM_ERROR_LIST_NOT_SEQ);


		pos++;
	} while (pair != CSCM_NIL);


	cscm_error_report("cscm_list_index", \
			CSCM_ERROR_LIST_INDEX);
}




/* sequence represented in list structure */
CSCM_OBJECT **cscm_list_to_object_ptrs(CSCM_OBJECT *list)
{
	int i;
	CSCM_OBJECT *pair;

	size_t n;
	CSCM_OBJECT **objs;


	if (list == NULL)
		cscm_error_report("cscm_list_to_object_ptrs", \
				CSCM_ERROR_NULL_PTR);
	else if (list->type != CSCM_OBJECT_TYPE_PAIR)
		cscm_error_report("cscm_list_to_object_ptrs", \
				CSCM_ERROR_OBJECT_TYPE);


	n = cscm_list_get_len(list); // only sequences'll get passed

	if (n == 0)
		return NULL;

	objs = cscm_object_ptrs_create(n);


	for (i = 0, pair = list; i < n; pair = cscm_pair_get_cdr(pair))
		objs[i] = cscm_pair_get_car(pair);


	return objs;
}




/* general list structure */
void cscm_list_print(CSCM_OBJECT *obj, FILE *stream)
{
	CSCM_OBJECT *cdr;


	if (obj == NULL || stream == NULL)
		cscm_error_report("cscm_list_print", \
				CSCM_ERROR_NULL_PTR);
	else if (obj->type != CSCM_OBJECT_TYPE_PAIR)
		cscm_error_report("cscm_list_print", \
				CSCM_ERROR_OBJECT_TYPE);

	fputc('(', stream);

	cscm_object_print(cscm_pair_get_car(obj), stream);
	cdr = cscm_pair_get_cdr(obj);

	// cscheme does not support null pointers in pairs
	if (cdr == NULL)
		cscm_error_report("cscm_list_print", \
				CSCM_ERROR_NULL_PTR);


	while(cdr->type == CSCM_OBJECT_TYPE_PAIR)
	{
		fputc(' ', stream);

		cscm_object_print(cscm_pair_get_car(cdr), stream);

		cdr = cscm_pair_get_cdr(cdr);

		// cscheme does not support null pointers in pairs
		if (cdr == NULL)
		cscm_error_report("cscm_list_print", \
				CSCM_ERROR_NULL_PTR);

	}


	if (cdr == CSCM_NIL) {
		fputc(')', stream);
	} else {
		fputs(" . ", stream);
		cscm_object_print(cdr, stream);
		fputc(')', stream);
	}
}




void cscm_nil_print(CSCM_OBJECT *obj, FILE *stream)
{
	if (obj == NULL || stream == NULL)
		cscm_error_report("cscm_nil_print", \
				CSCM_ERROR_NULL_PTR);


	if (obj == CSCM_NIL)
		fputs("nil", stream);
	else if (obj->type == CSCM_OBJECT_TYPE_NIL)
		cscm_error_report("cscm_nil_print", \
				CSCM_ERROR_NIL_EXTRA_COPY);
	else
		cscm_error_report("cscm_nil_print", \
				CSCM_ERROR_OBJECT_TYPE);

}




void cscm_nil_free(CSCM_OBJECT *obj)
{
	if (obj == NULL)
		cscm_error_report("cscm_nil_free", \
				CSCM_ERROR_NULL_PTR);


	if (obj == CSCM_NIL)
		return; // since it is not stored on the heap
	else if (obj->type == CSCM_OBJECT_TYPE_NIL)
		cscm_error_report("cscm_nil_free", \
				CSCM_ERROR_NIL_EXTRA_COPY);
	else
		cscm_error_report("cscm_nil_free", \
				CSCM_ERROR_OBJECT_TYPE);
}




void cscm_pair_free(CSCM_OBJECT *obj)
{
	CSCM_OBJECT *car, *cdr;


	if (obj == NULL)
		cscm_error_report("cscm_pair_free", \
				CSCM_ERROR_NULL_PTR);
	else if (obj->type != CSCM_OBJECT_TYPE_PAIR)
		cscm_error_report("cscmn_pair_free", \
				CSCM_ERROR_OBJECT_TYPE);


	car = cscm_pair_get_car(obj);
	cscm_gc_dec(car);
	cscm_gc_free(car);


	cdr = cscm_pair_get_cdr(obj);
	cscm_gc_dec(cdr);
	cscm_gc_free(cdr);


	free(obj->value);
	free(obj);
}
