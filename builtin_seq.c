/* builtin_seq.c -- cscheme standard library module: seq

   Copyright (C) 2021-2022 Tongjie Liu <tongjieandliu@gmail.com>.

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

#include "error.h"
#include "object.h"
#include "pair.h"
#include "core.h"
#include "num.h"
#include "bool.h"
#include "env.h"
#include "proc.h"
#include "gc.h"
#include "builtin.h"
#include "builtin_seq.h"




CSCM_OBJECT *_cscm_builtin_proc_sort_cmp_proc;


int _cscm_builtin_proc_sort_cmp(const void *a, const void *b)
{
	CSCM_OBJECT *proc;
	CSCM_OBJECT *args[2];

	CSCM_OBJECT *result;


	args[0] = *(CSCM_OBJECT **)a;
	args[1] = *(CSCM_OBJECT **)b;

	proc = _cscm_builtin_proc_sort_cmp_proc;


	/*	cscm_apply below will try to free proc, but when
	 * cscm_builtin_proc_sort returns, the outside cscm_apply
	 * will try to free it the second time */
	cscm_gc_inc(proc);
	result = cscm_apply(proc, 2, args);
	cscm_gc_dec(proc);

	if (result->type != CSCM_OBJECT_TYPE_NUM_LONG)
		cscm_error_report("cscm_builtin_proc_sort_cmp", \
				CSCM_ERROR_BUILTIN_RETURN_TYPE);


	return (int)cscm_num_long_get(result);
}


/* (sort cmp-proc seq) */
CSCM_OBJECT *cscm_builtin_proc_sort(size_t n, CSCM_OBJECT **args)
{
	size_t len;

	CSCM_OBJECT *cmp_proc, *seq;

	CSCM_OBJECT **objs;

	CSCM_OBJECT *ret;


	cscm_builtin_check_args("cscm_builtin_proc_sort",	\
				2,				\
				n,				\
				args);


	cmp_proc = args[0];
	seq = args[1];


	if ((cmp_proc->type != CSCM_OBJECT_TYPE_PROC_PRIM)
		&& (cmp_proc->type != CSCM_OBJECT_TYPE_PROC_COMP))
		cscm_error_report("cscm_builtin_proc_sort", \
				CSCM_ERROR_BUILTIN_BAD_PROC);
	else if (seq->type != CSCM_OBJECT_TYPE_PAIR \
		&& seq != CSCM_NIL)
		cscm_error_report("cscm_builtin_proc_sort", \
				CSCM_ERROR_BUILTIN_BAD_SEQ);


	if (seq == CSCM_NIL)
		return CSCM_NIL;


	len = cscm_list_get_len(seq);
	objs = cscm_list_to_object_ptrs(seq);
	_cscm_builtin_proc_sort_cmp_proc = cmp_proc;

	qsort(objs,				\
		len,				\
		sizeof(CSCM_OBJECT *),		\
		_cscm_builtin_proc_sort_cmp);

	ret = cscm_list_create(len, objs);


	free(objs);

	return ret;
}




/* (length seq) */
CSCM_OBJECT *cscm_builtin_proc_length(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *seq;

	CSCM_OBJECT *ret;


	cscm_builtin_check_args("cscm_builtin_proc_length",	\
				1,				\
				n,				\
				args);


	seq = args[0];

	if (seq->type != CSCM_OBJECT_TYPE_PAIR \
		&& seq != CSCM_NIL)
		cscm_error_report("cscm_builtin_proc_length", \
				CSCM_ERROR_BUILTIN_BAD_SEQ);


	ret = cscm_num_long_create();
	cscm_num_long_set(ret, (long)cscm_list_get_len(seq));


	return ret;
}




/* (list-ref seq index) */
CSCM_OBJECT *cscm_builtin_proc_list_ref(size_t n, CSCM_OBJECT **args)
{
	long index_number;

	CSCM_OBJECT *index, *seq;


	cscm_builtin_check_args("cscm_builtin_proc_list_ref",	\
				2,				\
				n,				\
				args);


	seq = args[0];
	index = args[1];


	if (index->type != CSCM_OBJECT_TYPE_NUM_LONG)
		cscm_error_report("cscm_builtin_proc_list_ref", \
				CSCM_ERROR_LIST_INDEX);
	else if (seq->type != CSCM_OBJECT_TYPE_PAIR) // do not support nil
		cscm_error_report("cscm_builtin_proc_list_ref", \
				CSCM_ERROR_BUILTIN_BAD_SEQ);


	index_number = cscm_num_long_get(index);
	if (index_number < 0)
		cscm_error_report("cscm_builtin_proc_list_ref", \
				CSCM_ERROR_LIST_INDEX);

	return cscm_list_index(seq, (size_t)index_number);
}




/* (range first-number last-number [step-number]) */
CSCM_OBJECT *cscm_builtin_proc_range(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *first, *last, *step;

	long first_number, last_number, step_number;


	cscm_builtin_check_interval_args("cscm_builtin_proc_range",	\
				2,					\
				3,					\
				n,					\
				args);


	first = args[0];
	last = args[1];

	if (first->type != CSCM_OBJECT_TYPE_NUM_LONG)
		cscm_error_report("cscm_builtin_proc_range", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (last->type != CSCM_OBJECT_TYPE_NUM_LONG)
		cscm_error_report("cscm_builtin_proc_range", \
				CSCM_ERROR_OBJECT_TYPE);

	first_number = cscm_num_long_get(first);
	last_number= cscm_num_long_get(last);


	if (n == 3) {
		step = args[2];

		if (step->type != CSCM_OBJECT_TYPE_NUM_LONG)
			cscm_error_report("cscm_builtin_proc_range", \
					CSCM_ERROR_OBJECT_TYPE);

		step_number= cscm_num_long_get(step);
	} else {
		step_number = 1;
	}


	return cscm_list_range(first_number, last_number, step_number);
}




/* (append x y) */
CSCM_OBJECT *cscm_builtin_proc_append(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *x, *y;


	cscm_builtin_check_args("cscm_builtin_proc_append",	\
				2,				\
				n,				\
				args);


	x = args[0];
	y = args[1];


	if ((x->type != CSCM_OBJECT_TYPE_PAIR		\
		&& x != CSCM_NIL)			\
		||					\
		(y->type != CSCM_OBJECT_TYPE_PAIR	\
		&& y != CSCM_NIL))
		cscm_error_report("cscm_builtin_proc_append", \
				CSCM_ERROR_BUILTIN_BAD_SEQ);


	return cscm_list_append(x, y);
}




/* (reverse seq) */
CSCM_OBJECT *cscm_builtin_proc_reverse(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *seq;


	cscm_builtin_check_args("cscm_builtin_proc_reverse",	\
				1,				\
				n,				\
				args);


	seq = args[0];


	if (seq->type != CSCM_OBJECT_TYPE_PAIR \
		&& seq != CSCM_NIL)
		cscm_error_report("cscm_builtin_proc_reverse", \
				CSCM_ERROR_OBJECT_TYPE);


	return cscm_list_reverse(seq);
}




/* (list-copy seq) */
CSCM_OBJECT *cscm_builtin_proc_list_copy(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *seq;


	cscm_builtin_check_args("cscm_builtin_proc_copy",	\
				1,				\
				n,				\
				args);


	seq = args[0];


	if (seq->type != CSCM_OBJECT_TYPE_PAIR \
		&& seq != CSCM_NIL)
		cscm_error_report("cscm_builtin_proc_list_copy", \
				CSCM_ERROR_BUILTIN_BAD_SEQ);


	return cscm_list_cpy(seq);
}




/* (map proc seq) */
CSCM_OBJECT *cscm_builtin_proc_map(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *proc, *seq;

	CSCM_OBJECT *src, *dest;
	CSCM_OBJECT *new_pair;
	CSCM_OBJECT *ret;

	CSCM_OBJECT *proc_args[1];
	CSCM_OBJECT *item_result;


	cscm_builtin_check_args("cscm_builtin_proc_map",	\
				2,				\
				n,				\
				args);


	proc = args[0];
	seq = args[1];


	if (proc->type != CSCM_OBJECT_TYPE_PROC_PRIM \
		&& proc->type != CSCM_OBJECT_TYPE_PROC_COMP)
		cscm_error_report("cscm_builtin_proc_map", \
				CSCM_ERROR_BUILTIN_BAD_PROC);
	else if (seq->type != CSCM_OBJECT_TYPE_PAIR \
		&& seq != CSCM_NIL)
		cscm_error_report("cscm_builtin_proc_map", \
				CSCM_ERROR_BUILTIN_BAD_SEQ);


	if (seq == CSCM_NIL)
		return CSCM_NIL;


	/*	cscm_apply below will try to free proc every
	 * iteration in the loop */
	cscm_gc_inc(proc);


	new_pair = cscm_pair_create();
	ret = new_pair;

	src = seq;

	do {
		dest = new_pair;

		new_pair = cscm_pair_create();

		proc_args[0] = cscm_pair_get_car(src);
		item_result = cscm_apply(proc, 1, proc_args);

		cscm_pair_set(dest, item_result, new_pair);


		src = cscm_pair_get_cdr(src);

		if (src == NULL)
			cscm_error_report("cscm_builtin_proc_map", \
					CSCM_ERROR_LIST_NOT_SEQ);
		else if (src->type != CSCM_OBJECT_TYPE_PAIR \
			&& src != CSCM_NIL)
			cscm_error_report("cscm_builtin_proc_map", \
					CSCM_ERROR_LIST_NOT_SEQ);
	} while (src != CSCM_NIL);


	/* this automatically freed the new_pair */
	cscm_pair_set_cdr(dest, CSCM_NIL);


	cscm_gc_dec(proc);
	return ret;
}




/* (for-each action seq) */
CSCM_OBJECT *cscm_builtin_proc_for_each(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *action, *seq;

	CSCM_OBJECT *pair;

	CSCM_OBJECT *action_args[1];


	cscm_builtin_check_args("cscm_builtin_proc_for_each",	\
				2,				\
				n,				\
				args);


	action = args[0];
	seq = args[1];


	if (action->type != CSCM_OBJECT_TYPE_PROC_PRIM \
		&& action->type != CSCM_OBJECT_TYPE_PROC_COMP)
		cscm_error_report("cscm_builtin_proc_for_each", \
				CSCM_ERROR_BUILTIN_BAD_ACTION);
	else if (seq->type != CSCM_OBJECT_TYPE_PAIR \
		&& seq != CSCM_NIL)
		cscm_error_report("cscm_builtin_proc_for_each", \
				CSCM_ERROR_BUILTIN_BAD_SEQ);


	if (seq == CSCM_NIL)
		return CSCM_TRUE;


	/*	cscm_apply below will try to free action every
	 * iteration in the loop */
	cscm_gc_inc(action);


	pair = seq;

	do {
		action_args[0] = cscm_pair_get_car(pair);
		cscm_apply(action, 1, action_args);


		pair = cscm_pair_get_cdr(pair);

		if (pair == NULL)
			cscm_error_report("cscm_builtin_proc_for_each", \
					CSCM_ERROR_LIST_NOT_SEQ);
		else if (pair->type != CSCM_OBJECT_TYPE_PAIR \
			&& pair != CSCM_NIL)
			cscm_error_report("cscm_builtin_proc_for_each", \
					CSCM_ERROR_LIST_NOT_SEQ);
	} while (pair != CSCM_NIL);


	cscm_gc_dec(action);
	return CSCM_TRUE;
}




/* (filter pred seq) */
CSCM_OBJECT *cscm_builtin_proc_filter(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *pred, *seq;

	CSCM_OBJECT *src, *dest;
	CSCM_OBJECT *new_pair;
	CSCM_OBJECT *ret;

	CSCM_OBJECT *pred_args[1];
	CSCM_OBJECT *pred_result;


	cscm_builtin_check_args("cscm_builtin_proc_filter",	\
				2,				\
				n,				\
				args);


	pred = args[0];
	seq = args[1];


	if (pred->type != CSCM_OBJECT_TYPE_PROC_PRIM \
		&& pred->type != CSCM_OBJECT_TYPE_PROC_COMP)
		cscm_error_report("cscm_builtin_proc_filter", \
				CSCM_ERROR_BUILTIN_BAD_PRED);
	else if (seq->type != CSCM_OBJECT_TYPE_PAIR \
		&& seq != CSCM_NIL)
		cscm_error_report("cscm_builtin_proc_filter", \
				CSCM_ERROR_BUILTIN_BAD_SEQ);


	if (seq == CSCM_NIL)
		return CSCM_NIL;


	/*	cscm_apply below will try to free pred every
	 * iteration in the loop */
	cscm_gc_inc(pred);


	new_pair = cscm_pair_create();
	ret = new_pair;

	src = seq;

	do {
		pred_args[0] = cscm_pair_get_car(src);
		pred_result = cscm_apply(pred, 1, pred_args);

		if (pred_result != CSCM_FALSE) {
			dest = new_pair;
			new_pair = cscm_pair_create();
			cscm_pair_set(dest, cscm_pair_get_car(src), new_pair);
		}


		src = cscm_pair_get_cdr(src);

		if (src == NULL)
			cscm_error_report("cscm_builtin_proc_filter", \
					CSCM_ERROR_LIST_NOT_SEQ);
		else if (src->type != CSCM_OBJECT_TYPE_PAIR \
			&& src != CSCM_NIL)
			cscm_error_report("cscm_builtin_proc_filter", \
					CSCM_ERROR_LIST_NOT_SEQ);
	} while (src != CSCM_NIL);


	/* this automatically freed the new_pair */
	cscm_pair_set_cdr(dest, CSCM_NIL);


	cscm_gc_dec(pred);
	return ret;
}




CSCM_OBJECT *_do_cscm_builtin_proc_accumulate(CSCM_OBJECT *proc,	\
					CSCM_OBJECT *initial,		\
					CSCM_OBJECT *rest_seq)
{
	CSCM_OBJECT *proc_args[2];

	CSCM_OBJECT *ret;


	if (rest_seq == NULL)
		cscm_error_report("_do_cscm_builtin_proc_accumulate", \
				CSCM_ERROR_LIST_NOT_SEQ);
	else if (rest_seq->type != CSCM_OBJECT_TYPE_PAIR \
		&& rest_seq != CSCM_NIL)
		cscm_error_report("_do_cscm_builtin_proc_accumulate", \
				CSCM_ERROR_LIST_NOT_SEQ);


	if (rest_seq == CSCM_NIL) {
		ret = initial;
	} else {
		proc_args[0] = cscm_pair_get_car(rest_seq);
		proc_args[1] = _do_cscm_builtin_proc_accumulate(	\
						proc,			\
						initial,		\
						cscm_pair_get_cdr(rest_seq));


		/*	cscm_apply below will try to free proc, but we
	 	* we need it every time we call _do_cscm_builtin_proc_
		* accumulate */
		cscm_gc_inc(proc);
		ret = cscm_apply(proc, 2, proc_args);
		cscm_gc_dec(proc);
	}


	return ret;
}


/* (accumulate proc initial seq)
 * 	proc: (proc current-item last-result) */
CSCM_OBJECT *cscm_builtin_proc_accumulate(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *proc, *initial, *seq;

	CSCM_OBJECT *ret;


	cscm_builtin_check_args("cscm_builtin_proc_accumulate",	\
				3,				\
				n,				\
				args);


	proc = args[0];
	initial = args[1];
	seq = args[2];


	if (proc->type != CSCM_OBJECT_TYPE_PROC_PRIM \
		&& proc->type != CSCM_OBJECT_TYPE_PROC_COMP)
		cscm_error_report("cscm_builtin_proc_accumulate", \
				CSCM_ERROR_BUILTIN_BAD_PROC);
	else if (seq->type != CSCM_OBJECT_TYPE_PAIR \
		&& seq != CSCM_NIL)
		cscm_error_report("cscm_builtin_proc_accumulate", \
				CSCM_ERROR_BUILTIN_BAD_SEQ);


	ret = _do_cscm_builtin_proc_accumulate(proc, initial, seq);


	return ret;
}




/* (fold-left proc initial seq)
 * 	proc: (proc last-result current-item) */
CSCM_OBJECT *cscm_builtin_proc_fold_left(size_t n, CSCM_OBJECT **args)
{
	CSCM_OBJECT *proc, *initial, *seq;

	CSCM_OBJECT *pair;

	CSCM_OBJECT *proc_args[1];
	CSCM_OBJECT *last_result;


	cscm_builtin_check_args("cscm_builtin_proc_fold_left",	\
				3,				\
				n,				\
				args);


	proc = args[0];
	initial = args[1];
	seq = args[2];


	if (proc->type != CSCM_OBJECT_TYPE_PROC_PRIM \
		&& proc->type != CSCM_OBJECT_TYPE_PROC_COMP)
		cscm_error_report("cscm_builtin_proc_fold_left", \
				CSCM_ERROR_BUILTIN_BAD_PROC);
	else if (seq->type != CSCM_OBJECT_TYPE_PAIR \
		&& seq != CSCM_NIL)
		cscm_error_report("cscm_builtin_proc_fold_left", \
				CSCM_ERROR_BUILTIN_BAD_SEQ);


	if (seq == CSCM_NIL)
		return initial;


	/*	cscm_apply below will try to free proc every
	 * iteration in the loop */
	cscm_gc_inc(proc);


	pair = seq;
	last_result = initial;

	do {
		proc_args[0] = last_result;
		proc_args[1] = cscm_pair_get_car(pair);

		last_result = cscm_apply(proc, 2, proc_args);


		pair = cscm_pair_get_cdr(pair);

		if (pair == NULL)
			cscm_error_report("cscm_builtin_proc_fold_left", \
					CSCM_ERROR_LIST_NOT_SEQ);
		else if (pair->type != CSCM_OBJECT_TYPE_PAIR \
			&& pair != CSCM_NIL)
			cscm_error_report("cscm_builtin_proc_fold_left", \
					CSCM_ERROR_LIST_NOT_SEQ);
	} while (pair != CSCM_NIL);


	cscm_gc_dec(proc);
	return last_result;
}




void cscm_builtin_module_func_seq()
{
	CSCM_OBJECT *global_env;

	CSCM_OBJECT *proc;


	global_env = cscm_global_env_get();


	proc = cscm_proc_prim_create();
	cscm_proc_prim_set(proc, cscm_builtin_proc_sort);
	cscm_env_add_var(global_env, "sort", proc);

	proc = cscm_proc_prim_create();
	cscm_proc_prim_set(proc, cscm_builtin_proc_length);
	cscm_env_add_var(global_env, "length", proc);

	proc = cscm_proc_prim_create();
	cscm_proc_prim_set(proc, cscm_builtin_proc_list_ref);
	cscm_env_add_var(global_env, "list-ref", proc);

	proc = cscm_proc_prim_create();
	cscm_proc_prim_set(proc, cscm_builtin_proc_range);
	cscm_env_add_var(global_env, "range", proc);

	proc = cscm_proc_prim_create();
	cscm_proc_prim_set(proc, cscm_builtin_proc_append);
	cscm_env_add_var(global_env, "append", proc);

	proc = cscm_proc_prim_create();
	cscm_proc_prim_set(proc, cscm_builtin_proc_reverse);
	cscm_env_add_var(global_env, "reverse", proc);

	proc = cscm_proc_prim_create();
	cscm_proc_prim_set(proc, cscm_builtin_proc_list_copy);
	cscm_env_add_var(global_env, "list-copy", proc);

	proc = cscm_proc_prim_create();
	cscm_proc_prim_set(proc, cscm_builtin_proc_map);
	cscm_env_add_var(global_env, "map", proc);

	proc = cscm_proc_prim_create();
	cscm_proc_prim_set(proc, cscm_builtin_proc_for_each);
	cscm_env_add_var(global_env, "for-each", proc);

	proc = cscm_proc_prim_create();
	cscm_proc_prim_set(proc, cscm_builtin_proc_filter);
	cscm_env_add_var(global_env, "filter", proc);

	proc = cscm_proc_prim_create();
	cscm_proc_prim_set(proc, cscm_builtin_proc_accumulate);
	cscm_env_add_var(global_env, "accumulate", proc);

	proc = cscm_proc_prim_create();
	cscm_proc_prim_set(proc, cscm_builtin_proc_fold_left);
	cscm_env_add_var(global_env, "fold-left", proc);
}
