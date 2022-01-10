/* proc.c -- scheme procedure

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
#include <stdio.h>

#include "error.h"
#include "object.h"
#include "text.h"
#include "gc.h"
#include "proc.h"




CSCM_OBJECT *cscm_proc_prim_create()
{
	CSCM_PROC_PRIM *proc;
	CSCM_OBJECT *obj;


	obj = cscm_object_create();


	obj->type = CSCM_OBJECT_TYPE_PROC_PRIM;


	proc = malloc(sizeof(CSCM_PROC_PRIM));
	if (proc == NULL)
		cscm_libc_fail("cscm_proc_prim_create", "malloc");

	proc->f = NULL;

	obj->value = proc;


	return obj;
}




void cscm_proc_prim_set(CSCM_OBJECT *proc_obj, CSCM_PROC_PRIM_FUNC f)
{
	CSCM_PROC_PRIM *proc;


	if (proc_obj == NULL)
		cscm_error_report("cscm_proc_prim_set", \
				CSCM_ERROR_NULL_PTR);
	else if (proc_obj->type != CSCM_OBJECT_TYPE_PROC_PRIM)
		cscm_error_report("cscm_proc_prim_set", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (proc_obj->value == NULL)
		cscm_error_report("cscm_proc_prim_set", \
				CSCM_ERROR_EMPTY_OBJECT);
	else if (f == NULL)
		cscm_error_report("cscm_proc_prim_set", \
				CSCM_ERROR_PROC_PRIM_BAD_FUNC);


	proc = (CSCM_PROC_PRIM *)proc_obj->value;


	proc->f = f;
}




CSCM_PROC_PRIM_FUNC cscm_proc_prim_get_f(CSCM_OBJECT *proc_obj)
{
	CSCM_PROC_PRIM *proc;


	if (proc_obj == NULL)
		cscm_error_report("cscm_proc_prim_get_f", \
				CSCM_ERROR_NULL_PTR);
	else if (proc_obj->type != CSCM_OBJECT_TYPE_PROC_PRIM)
		cscm_error_report("cscm_proc_prim_get_f", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (proc_obj->value == NULL)
		cscm_error_report("cscm_proc_prim_get_f", \
				CSCM_ERROR_EMPTY_OBJECT);


	proc = (CSCM_PROC_PRIM *)proc_obj->value;


	return proc->f;
}




CSCM_OBJECT *cscm_proc_comp_create()
{
	CSCM_PROC_COMP *proc;
	CSCM_OBJECT *obj;


	obj = cscm_object_create();


	obj->type = CSCM_OBJECT_TYPE_PROC_COMP;


	proc = malloc(sizeof(CSCM_PROC_COMP));
	if (proc == NULL)
		cscm_libc_fail("cscm_proc_prim_create", "malloc");


	proc->n_params = 0;
	proc->params = NULL;
	proc->body = NULL;
	proc->env = NULL;

	obj->value = proc;


	return obj;
}




void cscm_proc_comp_set(CSCM_OBJECT *proc_obj,	\
		int flag_dtn,			\
		size_t n_params, char **params,	\
		CSCM_EF *body,			\
		CSCM_OBJECT *env)
{
	CSCM_PROC_COMP *proc;


	if (proc_obj == NULL)
		cscm_error_report("cscm_proc_comp_set", \
				CSCM_ERROR_NULL_PTR);
	else if (proc_obj->type != CSCM_OBJECT_TYPE_PROC_COMP)
		cscm_error_report("cscm_proc_comp_set", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (proc_obj->value == NULL)
		cscm_error_report("cscm_proc_comp_set", \
				CSCM_ERROR_EMPTY_OBJECT);
	else if (n_params > 0 && params == NULL)
		cscm_error_report("cscm_proc_comp_set", \
				CSCM_ERROR_PROC_COMP_BAD_PARAMS);
	else if (body == NULL)
		cscm_error_report("cscm_proc_comp_set", \
				CSCM_ERROR_PROC_COMP_BAD_BODY);
	else if (env == NULL)
		cscm_error_report("cscm_proc_comp_set", \
				CSCM_ERROR_PROC_COMP_BAD_ENV);


	proc = (CSCM_PROC_COMP *)proc_obj->value;
	if (proc->body != NULL && proc->env != NULL)
		cscm_error_report("cscm_proc_comp_set", \
				CSCM_ERROR_PROC_COMP_NOT_INIT);


	proc->flag_dtn = flag_dtn;

	proc->n_params = n_params;
	proc->params = params;

	proc->body = body;

	proc->env = env;
	cscm_gc_inc(env);
}




size_t cscm_proc_comp_get_flag_dtn(CSCM_OBJECT *proc_obj)
{
	CSCM_PROC_COMP *proc;


	if (proc_obj == NULL)
		cscm_error_report("cscm_proc_comp_get_flag_dtn", \
				CSCM_ERROR_NULL_PTR);
	else if (proc_obj->type != CSCM_OBJECT_TYPE_PROC_COMP)
		cscm_error_report("cscm_proc_comp_get_flag_dtn", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (proc_obj->value == NULL)
		cscm_error_report("cscm_proc_comp_get_flag_dtn", \
				CSCM_ERROR_EMPTY_OBJECT);


	proc = (CSCM_PROC_COMP *)proc_obj->value;


	return proc->flag_dtn;
}


size_t cscm_proc_comp_get_n_params(CSCM_OBJECT *proc_obj)
{
	CSCM_PROC_COMP *proc;


	if (proc_obj == NULL)
		cscm_error_report("cscm_proc_comp_get_n_params", \
				CSCM_ERROR_NULL_PTR);
	else if (proc_obj->type != CSCM_OBJECT_TYPE_PROC_COMP)
		cscm_error_report("cscm_proc_comp_get_n_params", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (proc_obj->value == NULL)
		cscm_error_report("cscm_proc_comp_get_n_params", \
				CSCM_ERROR_EMPTY_OBJECT);


	proc = (CSCM_PROC_COMP *)proc_obj->value;


	return proc->n_params;
}


char **cscm_proc_comp_get_params(CSCM_OBJECT *proc_obj)
{
	CSCM_PROC_COMP *proc;


	if (proc_obj == NULL)
		cscm_error_report("cscm_proc_comp_get_params", \
				CSCM_ERROR_NULL_PTR);
	else if (proc_obj->type != CSCM_OBJECT_TYPE_PROC_COMP)
		cscm_error_report("cscm_proc_comp_get_params", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (proc_obj->value == NULL)
		cscm_error_report("cscm_proc_comp_get_params", \
				CSCM_ERROR_EMPTY_OBJECT);


	proc = (CSCM_PROC_COMP *)proc_obj->value;


	return proc->params;
}


CSCM_EF *cscm_proc_comp_get_body(CSCM_OBJECT *proc_obj)
{
	CSCM_PROC_COMP *proc;


	if (proc_obj == NULL)
		cscm_error_report("cscm_proc_comp_get_body", \
				CSCM_ERROR_NULL_PTR);
	else if (proc_obj->type != CSCM_OBJECT_TYPE_PROC_COMP)
		cscm_error_report("cscm_proc_comp_get_body", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (proc_obj->value == NULL)
		cscm_error_report("cscm_proc_comp_get_body", \
				CSCM_ERROR_EMPTY_OBJECT);


	proc = (CSCM_PROC_COMP *)proc_obj->value;


	return proc->body;
}


CSCM_OBJECT *cscm_proc_comp_get_env(CSCM_OBJECT *proc_obj)
{
	CSCM_PROC_COMP *proc;


	if (proc_obj == NULL)
		cscm_error_report("cscm_proc_comp_get_env", \
				CSCM_ERROR_NULL_PTR);
	else if (proc_obj->type != CSCM_OBJECT_TYPE_PROC_COMP)
		cscm_error_report("cscm_proc_comp_get_env", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (proc_obj->value == NULL)
		cscm_error_report("cscm_proc_comp_get_env", \
				CSCM_ERROR_EMPTY_OBJECT);


	proc = (CSCM_PROC_COMP *)proc_obj->value;


	return proc->env;
}




void cscm_proc_print(CSCM_OBJECT *obj, FILE *stream)
{
	if (obj == NULL || stream == NULL)
		cscm_error_report("cscm_proc_print", \
				CSCM_ERROR_NULL_PTR);


	if (obj->type == CSCM_OBJECT_TYPE_PROC_PRIM)
		printf("<pproc at %p>", obj);
	else if (obj->type == CSCM_OBJECT_TYPE_PROC_COMP)
		printf("<cproc at %p>", obj);
	else
		cscm_error_report("cscm_proc_print", \
				CSCM_ERROR_OBJECT_TYPE);
}




void cscm_proc_prim_free(CSCM_OBJECT *obj)
{
	if (obj == NULL)
		cscm_error_report("cscm_proc_prim_free", \
				CSCM_ERROR_NULL_PTR);
	else if (obj->type != CSCM_OBJECT_TYPE_PROC_PRIM)
		cscm_error_report("cscm_proc_prim_free", \
				CSCM_ERROR_OBJECT_TYPE);


	free(obj->value);

	free(obj);
}




void cscm_proc_comp_free(CSCM_OBJECT *obj)
{
	CSCM_PROC_COMP *proc;


	if (obj == NULL)
		cscm_error_report("cscm_proc_comp_free", \
				CSCM_ERROR_NULL_PTR);
	else if (obj->type != CSCM_OBJECT_TYPE_PROC_COMP)
		cscm_error_report("cscm_proc_comp_free", \
				CSCM_ERROR_OBJECT_TYPE);


	proc = (CSCM_PROC_COMP *)obj->value;


	/*	The params and body field of a compound procedure are
	 * sharing memories with other compound procedures created by
	 * the same lambda expression, but the env field are not.
	 * 	More importantly, params and body are not scheme objects,
	 * therefore they don't have reference counts used in keeping
	 * themselves from being freed. */
	cscm_gc_dec(proc->env);
	cscm_gc_free(proc->env);

	free(proc);

	free(obj);
}
