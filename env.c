/* env.c -- environment

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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "object.h"
#include "error.h"
#include "text.h"
#include "bool.h"
#include "pair.h"
#include "proc.h"
#include "gc.h"
#include "builtin.h"
#include "env.h"




/* There is only one copy of **UNASSIGNED** in cscheme */
CSCM_OBJECT _cscm_unassigned = {CSCM_OBJECT_TYPE_UNASSIGNED, NULL, 0};




CSCM_OBJECT *cscm_frame_create()
{
	CSCM_OBJECT *obj;
	CSCM_FRAME *frame;


	obj = cscm_object_create();


	obj->type = CSCM_OBJECT_TYPE_FRAME;


	frame = malloc(sizeof(CSCM_FRAME));
	if (frame == NULL)
		cscm_libc_fail("cscm_frame_create", "malloc");

	frame->n_bindings = 0;

	obj->value = frame;


	return obj;
}




void cscm_frame_init(CSCM_OBJECT *frame_obj, \
		size_t n, char **vars, CSCM_OBJECT **vals)
{
	int i;
	CSCM_FRAME *frame;


	if (frame_obj == NULL)
		cscm_error_report("cscm_frame_init", \
				CSCM_ERROR_NULL_PTR);
	else if (frame_obj->type != CSCM_OBJECT_TYPE_FRAME)
		cscm_error_report("cscm_frame_init", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (frame_obj->value == NULL)
		cscm_error_report("cscm_frame_init", \
				CSCM_ERROR_EMPTY_OBJECT);


	frame = (CSCM_FRAME *)frame_obj->value;
	if (frame->n_bindings != 0)
		cscm_error_report("cscm_frame_init", \
				CSCM_ERROR_FRAME_NOT_EMPTY);
	else if (n > CSCM_FRAME_MAX_SIZE)
		cscm_error_report("cscm_frame_init", \
				CSCM_ERROR_FRAME_TOO_MANY_VARS);
	else if (vars == NULL || n == 0)
		cscm_error_report("cscm_frame_init", \
				CSCM_ERROR_FRAME_NO_VAR);
	else if (vals == NULL)
		cscm_error_report("cscm_frame_init", \
				CSCM_ERROR_FRAME_NO_VAL);
	else if (!cscm_text_unique(n, vars))
		cscm_error_report("cscm_frame_init", \
				CSCM_ERROR_FRAME_NOT_UNIQUE);




	for (i = 0; i < n; i++) {
		if (vars[i] == NULL || *(vars[i]) == 0)
			cscm_error_report("cscm_frame_init", \
					CSCM_ERROR_FRAME_NO_VAR);
		else if (vals[i] == NULL)
			cscm_error_report("cscm_frame_init", \
					CSCM_ERROR_FRAME_NO_VAL);


		frame->vars[i] = cscm_text_cpy(vars[i]);


		frame->vals[i] = vals[i];
		cscm_gc_inc(vals[i]);
	}


	frame->n_bindings = n;
}




// add when not exists, set when exists.
void cscm_frame_add_var(CSCM_OBJECT *frame_obj, char *var, CSCM_OBJECT *val)
{
	int i;
	CSCM_FRAME *frame;


	if (frame_obj == NULL)
		cscm_error_report("cscm_frame_add_var", \
				CSCM_ERROR_NULL_PTR);
	else if (frame_obj->type != CSCM_OBJECT_TYPE_FRAME)
		cscm_error_report("cscm_frame_add_var", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (frame_obj->value == NULL)
		cscm_error_report("cscm_frame_add_var", \
				CSCM_ERROR_EMPTY_OBJECT);
	else if (var == NULL || *var == 0)
		cscm_error_report("cscm_frame_add_var", \
				CSCM_ERROR_FRAME_NO_VAR);
	else if (val == NULL)
		cscm_error_report("cscm_frame_add_var", \
				CSCM_ERROR_FRAME_NO_VAL);


	frame = (CSCM_FRAME *)frame_obj->value;


	for (i = 0; i < frame->n_bindings; i++) {
		if (!strcmp(var, frame->vars[i])) {
			cscm_gc_dec(frame->vals[i]);
			cscm_gc_free(frame->vals[i]);

			frame->vals[i] = val;
			cscm_gc_inc(val);

			return;
		}
	}


	if (frame->n_bindings >= CSCM_FRAME_MAX_SIZE)
		cscm_error_report("cscm_frame_add_var", \
				CSCM_ERROR_FRAME_FULL);


	frame->vars[frame->n_bindings] = cscm_text_cpy(var);


	frame->vals[frame->n_bindings] = val;
	cscm_gc_inc(val);


	frame->n_bindings++;
}




/*	Return value will not be a null pointer when the function has
 * found the specified variable, in other words all variables are bound
 * to objects. When the function returns a null pointer, it actually
 * means the specified variable is not existed in this frame. */
CSCM_OBJECT *cscm_frame_get_var(CSCM_OBJECT *frame_obj, char *var)
{
	int i;
	CSCM_FRAME *frame;


	if (frame_obj == NULL)
		cscm_error_report("cscm_frame_get_var", \
				CSCM_ERROR_NULL_PTR);
	else if (frame_obj->type != CSCM_OBJECT_TYPE_FRAME)
		cscm_error_report("cscm_frame_get_var", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (frame_obj->value == NULL)
		cscm_error_report("cscm_frame_get_var", \
				CSCM_ERROR_EMPTY_OBJECT);
	else if (var == NULL || *var == 0)
		cscm_error_report("cscm_frame_get_var", \
				CSCM_ERROR_FRAME_NO_VAR);


	frame = (CSCM_FRAME *)frame_obj->value;


	for (i = 0; i < frame->n_bindings; i++) {
		if (!strcmp(var, frame->vars[i])) {
			if (frame->vals[i] == CSCM_UNASSIGNED)
				cscm_runtime_error_report(var, \
						CSCM_ERROR_FRAME_UNASSIGNED);
			else if (frame->vals[i] == NULL)
				cscm_error_report("cscm_frame_get_var", \
						CSCM_ERROR_FRAME_EMPTY_BINDING);
			else
				return frame->vals[i];
		}
	}


	return NULL;
}




void cscm_frame_set_var(CSCM_OBJECT *frame_obj, char *var, CSCM_OBJECT *val)
{
	int i;
	CSCM_FRAME *frame;


	if (frame_obj == NULL)
		cscm_error_report("cscm_frame_set_var", \
				CSCM_ERROR_NULL_PTR);
	else if (frame_obj->type != CSCM_OBJECT_TYPE_FRAME)
		cscm_error_report("cscm_frame_set_var", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (frame_obj->value == NULL)
		cscm_error_report("cscm_frame_set_var", \
				CSCM_ERROR_EMPTY_OBJECT);
	else if (var == NULL || *var == 0)
		cscm_error_report("cscm_frame_set_var", \
				CSCM_ERROR_FRAME_NO_VAR);
	else if (val == NULL)
		cscm_error_report("cscm_frame_set_var", \
				CSCM_ERROR_FRAME_NO_VAL);


	frame = (CSCM_FRAME *)frame_obj->value;


	for (i = 0; i < frame->n_bindings; i++) {
		if (!strcmp(var, frame->vars[i])) {
			cscm_gc_dec(frame->vals[i]);
			cscm_gc_free(frame->vals[i]);

			frame->vals[i] = val;
			cscm_gc_inc(val);

			return;
		}
	}


	cscm_runtime_error_report(var, CSCM_ERROR_FRAME_UNBOUND);
}





CSCM_OBJECT *cscm_env_create()
{
	CSCM_OBJECT *obj;
	CSCM_ENV *env;


	obj = cscm_object_create();


	obj->type = CSCM_OBJECT_TYPE_ENV;


	env = malloc(sizeof(CSCM_ENV));
	if (env == NULL)
		cscm_libc_fail("cscm_env_create", "malloc");

	env->n_frames = 0;
	env->frames = NULL;

	obj->value = env;


	return obj;
}




CSCM_OBJECT *cscm_env_cpy_extend(CSCM_OBJECT *env_obj, CSCM_OBJECT *frame)
{
	int i;

	CSCM_ENV *env, *new_env;

	CSCM_OBJECT *new_env_obj;

	CSCM_OBJECT **frames_buf;


	if (env_obj == NULL)
		cscm_error_report("cscm_env_cpy_extend", \
				CSCM_ERROR_NULL_PTR);
	else if (env_obj->type != CSCM_OBJECT_TYPE_ENV)
		cscm_error_report("cscm_env_cpy_extend", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (env_obj->value == NULL)
		cscm_error_report("cscm_env_cpy_extend", \
				CSCM_ERROR_EMPTY_OBJECT);


	env = (CSCM_ENV *)env_obj->value;
	if (env->n_frames == 0)
		cscm_error_report("cscm_env_cpy_extend", \
				CSCM_ERROR_ENV_EMPTY);
	else if (frame->type != CSCM_OBJECT_TYPE_FRAME)
		cscm_error_report("cscm_env_cpy_extend", \
				CSCM_ERROR_OBJECT_TYPE);


	new_env_obj = cscm_env_create();
	new_env = (CSCM_ENV *)new_env_obj->value;


	new_env->n_frames = env->n_frames + 1;


	frames_buf = cscm_object_ptrs_create(env->n_frames + 1);


	for (i = 1; i < new_env->n_frames; i++) {
		frames_buf[i] = env->frames[i - 1];
		cscm_gc_inc(frames_buf[i]);
	}


	frames_buf[0] = frame;
	cscm_gc_inc(frame);


	new_env->frames = frames_buf;


	return new_env_obj;
}




CSCM_OBJECT *cscm_env_get_var(CSCM_OBJECT *env_obj, char *var)
{
	int i;
	CSCM_ENV *env;
	CSCM_OBJECT *val;


	if (env_obj == NULL)
		cscm_error_report("cscm_env_get_var", \
				CSCM_ERROR_NULL_PTR);
	else if (env_obj->type != CSCM_OBJECT_TYPE_ENV)
		cscm_error_report("cscm_env_get_var", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (env_obj->value == NULL)
		cscm_error_report("cscm_env_get_var", \
				CSCM_ERROR_EMPTY_OBJECT);
	else if (var == NULL)
		cscm_error_report("cscm_env_get_var", \
				CSCM_ERROR_ENV_NO_VAR);


	env = (CSCM_ENV *)env_obj->value;


	for (i = 0; i < env->n_frames; i++) {
		val = cscm_frame_get_var(env->frames[i], var);
		if (val)
			return val;
	}


	cscm_runtime_error_report(var, CSCM_ERROR_ENV_UNBOUND);
}




void cscm_env_set_var(CSCM_OBJECT *env_obj, char *var, CSCM_OBJECT *val)
{
	int i;
	CSCM_ENV *env;
	CSCM_OBJECT *old_val;


	if (env_obj == NULL)
		cscm_error_report("cscm_env_set_var", \
				CSCM_ERROR_NULL_PTR);
	else if (env_obj->type != CSCM_OBJECT_TYPE_ENV)
		cscm_error_report("cscm_env_set_var", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (env_obj->value == NULL)
		cscm_error_report("cscm_env_set_var", \
				CSCM_ERROR_EMPTY_OBJECT);
	else if (var == NULL)
		cscm_error_report("cscm_env_set_var", \
				CSCM_ERROR_ENV_NO_VAR);
	else if (val == NULL)
		cscm_error_report("cscm_env_set_var", \
				CSCM_ERROR_ENV_NO_VAL);


	env = (CSCM_ENV *)env_obj->value;


	for (i = 0; i < env->n_frames; i++) {
		old_val = cscm_frame_get_var(env->frames[i], var);
		if (old_val) {
			cscm_frame_set_var(env->frames[i], var, val);
			return;
		}
	}


	cscm_runtime_error_report(var, CSCM_ERROR_ENV_UNBOUND);
}




void cscm_env_add_var(CSCM_OBJECT *env_obj, char *var, CSCM_OBJECT *val)
{
	CSCM_ENV *env;


	if (env_obj == NULL)
		cscm_error_report("cscm_env_add_var", \
				CSCM_ERROR_NULL_PTR);
	else if (env_obj->type != CSCM_OBJECT_TYPE_ENV)
		cscm_error_report("cscm_env_add_var", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (env_obj->value == NULL)
		cscm_error_report("cscm_env_add_var", \
				CSCM_ERROR_EMPTY_OBJECT);
	else if (var == NULL)
		cscm_error_report("cscm_env_add_var", \
				CSCM_ERROR_ENV_NO_VAR);
	else if (val == NULL)
		cscm_error_report("cscm_env_add_var", \
				CSCM_ERROR_ENV_NO_VAL);


	env = (CSCM_ENV *)env_obj->value;
	if (env->n_frames == 0)
		cscm_error_report("cscm_env_add_var", \
				CSCM_ERROR_ENV_EMPTY);




	cscm_frame_add_var(env->frames[0], var, val);
}




CSCM_OBJECT *_cscm_global_env_create()
{
	CSCM_OBJECT *obj;
	CSCM_ENV *env;

	CSCM_OBJECT *frame;


	obj = cscm_env_create();
	env = (CSCM_ENV *)obj->value;


	env->frames = cscm_object_ptrs_create(1);


	frame = cscm_frame_create();

	env->frames[0] = frame;
	cscm_gc_inc(frame);


	env->n_frames = 1;


	return obj;
}


char *_cscm_env_builtin_names[] = {
	/* data */
	"#t", "#f", "nil",

	/* primitive procedures */
	"print", "printn", "display", "newline", "read",
	"set-car!", "set-cdr!",
	"cons", "car", "cdr", "cadr", "cdar", "cddr", "caar",
	"caaar", "cdaar", "cadar", "caadr", "cddar", "caddr", "cdadr", "cdddr",
	"list",
	"+", "-", "*", "/",
	"remainder",
	"=", "eq?", "equal?",
	">", ">=", "<", "<=",
	"string?", "symbol?", "number?", "pair?", "null?",
	"include",
	"max", "min",
	"apply", "not",
	"error",

	NULL
};




CSCM_OBJECT *_cscm_env_builtin_data[] = {
	CSCM_TRUE,
	CSCM_FALSE,
	CSCM_NIL,

	NULL
};




CSCM_PROC_PRIM_FUNC _cscm_env_builtin_pp_funcs[] = {
	cscm_builtin_proc_print,
	cscm_builtin_proc_printn,
	cscm_builtin_proc_display,
	cscm_builtin_proc_newline,
	cscm_builtin_proc_read,

	cscm_builtin_proc_set_car,
	cscm_builtin_proc_set_cdr,

	cscm_builtin_proc_cons,
	cscm_builtin_proc_car,
	cscm_builtin_proc_cdr,
	cscm_builtin_proc_cadr,
	cscm_builtin_proc_cdar,
	cscm_builtin_proc_cddr,
	cscm_builtin_proc_caar,
	cscm_builtin_proc_caaar,
	cscm_builtin_proc_cdaar,
	cscm_builtin_proc_cadar,
	cscm_builtin_proc_caadr,
	cscm_builtin_proc_cddar,
	cscm_builtin_proc_caddr,
	cscm_builtin_proc_cdadr,
	cscm_builtin_proc_cdddr,

	cscm_builtin_proc_list,

	cscm_builtin_proc_add,
	cscm_builtin_proc_subtract,
	cscm_builtin_proc_multiply,
	cscm_builtin_proc_divide,

	cscm_builtin_proc_remainder,

	cscm_builtin_proc_equal_num,
	cscm_builtin_proc_equal_ssb,
	cscm_builtin_proc_equal,

	cscm_builtin_proc_greater_than,
	cscm_builtin_proc_greater_equal,
	cscm_builtin_proc_less_than,
	cscm_builtin_proc_less_equal,

	cscm_builtin_proc_is_string,
	cscm_builtin_proc_is_symbol,
	cscm_builtin_proc_is_number,
	cscm_builtin_proc_is_pair,
	cscm_builtin_proc_is_nil,

	cscm_builtin_proc_include,

	cscm_builtin_proc_max,
	cscm_builtin_proc_min,

	cscm_builtin_proc_apply,
	cscm_builtin_proc_not,

	cscm_builtin_proc_error,

	NULL
};




CSCM_OBJECT *_cscm_global_env = NULL;


CSCM_OBJECT *cscm_global_env_setup()
{
	size_t index;

	CSCM_OBJECT *env;

	CSCM_OBJECT **data;
	CSCM_PROC_PRIM_FUNC *pp_func;
	CSCM_OBJECT *proc;


	if (_cscm_global_env)
		cscm_error_report("cscm_global_env_setup", \
				CSCM_ERROR_GLOBAL_ENV_EXISTED);


	env = _cscm_global_env_create();


	index = 0;


	data = _cscm_env_builtin_data;
	for (; *data; data++, index++) {
		cscm_env_add_var(env,				\
				_cscm_env_builtin_names[index],	\
				*data);
	}


	pp_func = _cscm_env_builtin_pp_funcs;
	for (; *pp_func; pp_func++, index++) {
		proc = cscm_proc_prim_create();
		cscm_proc_prim_set(proc, *pp_func);

		cscm_env_add_var(env,				\
				_cscm_env_builtin_names[index],	\
				proc);
	}


	_cscm_global_env = env;

	return env;
}


CSCM_OBJECT *cscm_global_env_get()
{
	if (_cscm_global_env == NULL)
		cscm_error_report("cscm_global_env_get", \
				CSCM_ERROR_GLOBAL_ENV_NOT_EXISTED);


	return _cscm_global_env;
}




void cscm_frame_print(CSCM_OBJECT *obj, FILE *stream)
{
	if (obj == NULL || stream == NULL)
		cscm_error_report("cscm_frame_print", \
				CSCM_ERROR_NULL_PTR);
	else if (obj->type != CSCM_OBJECT_TYPE_FRAME)
		cscm_error_report("cscm_frame_print", \
				CSCM_ERROR_OBJECT_TYPE);


	fprintf(stream, "<frame at 0x%p>", obj);
}


void cscm_frame_print_details(CSCM_OBJECT *obj, char *prefix)
{
	int i;
	CSCM_FRAME *frame;


	if (obj == NULL || prefix == NULL)
		cscm_error_report("cscm_frame_print_details", \
				CSCM_ERROR_NULL_PTR);
	else if (obj->type != CSCM_OBJECT_TYPE_FRAME)
		cscm_error_report("cscm_frame_print_details", \
				CSCM_ERROR_OBJECT_TYPE);


	frame = (CSCM_FRAME *)obj->value;
	for (i = 0; i < frame->n_bindings; i++) {
		printf("%s%-16s: ", prefix, frame->vars[i]);
		cscm_object_print(frame->vals[i], stdout);
		puts("");
	}
}


void cscm_env_print(CSCM_OBJECT *obj, FILE *stream)
{
	if (obj == NULL || stream == NULL)
		cscm_error_report("cscm_env_print", \
				CSCM_ERROR_NULL_PTR);
	else if (obj->type != CSCM_OBJECT_TYPE_ENV)
		cscm_error_report("cscm_env_print", \
				CSCM_ERROR_OBJECT_TYPE);


	fprintf(stream, "<env at 0x%p>", obj);
}


void cscm_env_print_details(CSCM_OBJECT *obj)
{
	int i;
	CSCM_ENV *env;


	if (obj == NULL)
		cscm_error_report("cscm_env_print_details", \
				CSCM_ERROR_NULL_PTR);
	else if (obj->type != CSCM_OBJECT_TYPE_ENV)
		cscm_error_report("cscm_env_print_details", \
				CSCM_ERROR_OBJECT_TYPE);


	env = (CSCM_ENV *)obj->value;
	for (i = 0; i < env->n_frames; i++) {
		if (i > 0)
			puts("");

		printf("FRAME %d:\n", i);
		cscm_frame_print_details(env->frames[i], "\t");
	}
}


void cscm_unassigned_print(CSCM_OBJECT *obj, FILE *stream)
{
	if (obj == NULL || stream == NULL)
		cscm_error_report("cscm_unassigned_print", \
				CSCM_ERROR_NULL_PTR);


	if (obj == CSCM_UNASSIGNED)
		fputs("**UNASSIGNED**", stdout);
	else if (obj->type == CSCM_OBJECT_TYPE_UNASSIGNED)
		cscm_error_report("cscm_unassigned_print", \
				CSCM_ERROR_UNASSIGNED_EXTRA_COPY);
	else
		cscm_error_report("cscm_unassigned_print", \
				CSCM_ERROR_OBJECT_TYPE);
}




void cscm_frame_free(CSCM_OBJECT *obj)
{
	int i;

	CSCM_FRAME *frame;


	if (obj == NULL)
		cscm_error_report("cscm_frame_free", \
				CSCM_ERROR_NULL_PTR);
	else if (obj->type != CSCM_OBJECT_TYPE_FRAME)
		cscm_error_report("cscm_frame_free", \
				CSCM_ERROR_OBJECT_TYPE);


	frame = (CSCM_FRAME *)obj->value;


	for (i = 0; i < frame->n_bindings; i++) {
		free(frame->vars[i]);

		cscm_gc_dec(frame->vals[i]);
		cscm_gc_free(frame->vals[i]);
	}


	free(frame);

	free(obj);
}


void cscm_env_free(CSCM_OBJECT *obj)
{
	int i;

	CSCM_ENV *env;


	if (obj == NULL)
		cscm_error_report("cscm_env_free", \
				CSCM_ERROR_NULL_PTR);
	else if (obj->type != CSCM_OBJECT_TYPE_ENV)
		cscm_error_report("cscm_env_free", \
				CSCM_ERROR_OBJECT_TYPE);


	env = (CSCM_ENV *)obj->value;


	for (i = 0; i < env->n_frames; i++) {
		cscm_gc_dec(env->frames[i]);
		cscm_gc_free(env->frames[i]);
	}


	free(env->frames);

	free(env);

	free(obj);
}




void cscm_unassigned_free(CSCM_OBJECT *obj)
{
	if (obj == NULL)
		cscm_error_report("cscm_unassigned_free", \
				CSCM_ERROR_NULL_PTR);


	if (obj == CSCM_UNASSIGNED)
		return; // since it is not stored on the heap
	else if (obj->type == CSCM_OBJECT_TYPE_UNASSIGNED)
		cscm_error_report("cscm_unassigned_free", \
				CSCM_ERROR_UNASSIGNED_EXTRA_COPY);
	else
		cscm_error_report("cscm_unassigned_free", \
				CSCM_ERROR_OBJECT_TYPE);
}
