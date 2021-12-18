/* object.c -- basic object framework

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

#include "object.h"
#include "error.h"
#include "proc.h"
#include "symbol.h"
#include "str.h"
#include "bool.h"
#include "env.h"
#include "num.h"
#include "gc.h"
#include "pair.h"




CSCM_OBJECT *cscm_object_create()
{
	CSCM_OBJECT *obj;


	obj = malloc(sizeof(CSCM_OBJECT));
	if (obj == NULL)
		cscm_libc_fail("cscm_object_create", "malloc");


	obj->type = CSCM_OBJECT_TYPE_NONE;
	obj->value = NULL;
	obj->ref_count = 0;


	#ifdef __CSCM_GC_DEBUG__
		cscm_gc_inc_total_object_count();
	#endif


	return obj;
}


CSCM_OBJECT **cscm_object_ptrs_create(size_t n)
{
	size_t size;
	CSCM_OBJECT **ptrs;


	if (n == 0)
		cscm_error_report("cscm_object_ptrs_create", \
				CSCM_ERROR_OBJECT_ZERO_PTR);


	size = n * sizeof(CSCM_OBJECT *);
	ptrs = malloc(size);
	if (ptrs == NULL)
		cscm_libc_fail("cscm_object_ptrs_create", "malloc");


	return ptrs;
}




CSCM_OBJECT_PRINT_FUNC _cscm_object_print_func_list[] = {
	cscm_num_print,
	cscm_num_print,
	cscm_symbol_print,
	cscm_string_print,
	cscm_list_print,
	cscm_proc_print,
	cscm_proc_print,
	cscm_env_print,
	cscm_frame_print,
	cscm_nil_print,
	cscm_bool_print,
	cscm_bool_print,
	cscm_unassigned_print
};


void cscm_object_print(CSCM_OBJECT *obj, FILE *stream)
{
	CSCM_OBJECT_PRINT_FUNC pf;


	if (obj == NULL || stream == NULL)
		cscm_error_report("cscm_object_print", \
				CSCM_ERROR_NULL_PTR);
	else if (obj->type < 0 || obj->type >= CSCM_OBJECT_TYPE_NONE)
		cscm_error_report("cscm_object_print", \
				CSCM_ERROR_OBJECT_TYPE);


	pf = _cscm_object_print_func_list[obj->type];
	pf(obj, stream);
}




CSCM_OBJECT_FREE_FUNC _cscm_object_free_func_list[] = {
	cscm_num_free,
	cscm_num_free,
	cscm_symbol_free,
	cscm_string_free,
	cscm_pair_free,
	cscm_proc_prim_free,
	cscm_proc_comp_free,
	cscm_env_free,
	cscm_frame_free,
	cscm_nil_free,
	cscm_bool_free,
	cscm_bool_free,
	cscm_unassigned_free
};


void cscm_object_free(CSCM_OBJECT *obj)
{
	CSCM_OBJECT_FREE_FUNC ff;


	if (obj == NULL)
		cscm_error_report("cscm_object_free", \
				CSCM_ERROR_NULL_PTR);
	else if (obj->type < 0 || obj->type >= CSCM_OBJECT_TYPE_NONE)
		cscm_error_report("cscm_object_free", \
				CSCM_ERROR_OBJECT_TYPE);


	ff = _cscm_object_free_func_list[obj->type];
	ff(obj);
}
