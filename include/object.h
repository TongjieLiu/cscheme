/* object.h -- basic object framework

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

#ifndef __CSCM_OBJECT_H__

#define __CSCM_OBJECT_H__




#include <stddef.h>
#include <stdio.h>




#define CSCM_OBJECT_TYPE_NUM_LONG	0
#define CSCM_OBJECT_TYPE_NUM_DOUBLE	1
#define CSCM_OBJECT_TYPE_SYMBOL		2
#define CSCM_OBJECT_TYPE_STRING		3
#define CSCM_OBJECT_TYPE_PAIR		4
#define CSCM_OBJECT_TYPE_PROC_PRIM	5
#define CSCM_OBJECT_TYPE_PROC_COMP	6
#define CSCM_OBJECT_TYPE_ENV		7
#define CSCM_OBJECT_TYPE_FRAME		8
#define CSCM_OBJECT_TYPE_NIL		9
#define CSCM_OBJECT_TYPE_BOOL_TRUE	10
#define CSCM_OBJECT_TYPE_BOOL_FALSE	11
#define CSCM_OBJECT_TYPE_UNASSIGNED	12
#define CSCM_OBJECT_TYPE_NONE		13




struct _CSCM_OBJECT {
	int type;
	void *value;

	size_t ref_count;	// gc
};


typedef struct _CSCM_OBJECT CSCM_OBJECT;




typedef void (*CSCM_OBJECT_PRINT_FUNC)(CSCM_OBJECT *obj, FILE *stream);




typedef void (*CSCM_OBJECT_FREE_FUNC)(CSCM_OBJECT *obj);




CSCM_OBJECT *cscm_object_create();


CSCM_OBJECT **cscm_object_ptrs_create(size_t n);




void cscm_object_print(CSCM_OBJECT *obj, FILE *stream);




void cscm_object_free(CSCM_OBJECT *obj);




#define CSCM_ERROR_OBJECT_TYPE		"incorrect object type"
#define CSCM_ERROR_EMPTY_OBJECT		"empty object(object->value = NULL)"


#define CSCM_ERROR_OBJECT_ZERO_PTR	"requesting zero pointer"


#define CSCM_ERROR_NIL_EXTRA_COPY	"unauthorized copy of the \"nil\""




#endif
