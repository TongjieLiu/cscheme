/* pair.h -- scheme pair and list

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

#ifndef __CSCM_PAIR_H__

#define __CSCM_PAIR_H__




#include <stddef.h>
#include <stdio.h>

#include "object.h"
#include "ast.h"




struct _CSCM_PAIR {
	void *car;
	void *cdr;
};


typedef struct _CSCM_PAIR CSCM_PAIR;




CSCM_OBJECT *cscm_pair_create();


void cscm_pair_set(CSCM_OBJECT *pair_obj, void *car, void *cdr);
void cscm_pair_set_car(CSCM_OBJECT *pair_obj, void *car);
void cscm_pair_set_cdr(CSCM_OBJECT *pair_obj, void *cdr);


void *cscm_pair_get_car(CSCM_OBJECT *pair_obj);
void *cscm_pair_get_cdr(CSCM_OBJECT *pair_obj);




/* constructor for sequence represented in list structure */
CSCM_OBJECT *cscm_list_create(size_t n, CSCM_OBJECT **objs);
CSCM_OBJECT *cscm_list_range(long first, long last, long step);


/* selectors for sequence represented in list structure */
CSCM_OBJECT *cscm_list_cpy(CSCM_OBJECT *list);
CSCM_OBJECT *cscm_list_reverse(CSCM_OBJECT *list);
CSCM_OBJECT *cscm_list_append(CSCM_OBJECT *x, CSCM_OBJECT *y);
size_t cscm_list_get_len(CSCM_OBJECT *list);
CSCM_OBJECT *cscm_list_index(CSCM_OBJECT *list, size_t index);
CSCM_OBJECT **cscm_list_to_object_ptrs(CSCM_OBJECT *list);




void cscm_list_print(CSCM_OBJECT *obj, FILE *stream); // general list structure
void cscm_nil_print(CSCM_OBJECT *obj, FILE *stream);




void cscm_nil_free(CSCM_OBJECT *obj);
void cscm_pair_free(CSCM_OBJECT *obj);




extern CSCM_OBJECT _cscm_nil;

#define CSCM_NIL	(&_cscm_nil)




#define CSCM_ERROR_LIST_NOT_SEQ		"not a sequence"


#define CSCM_ERROR_LIST_NO_OBJ		"object is not specified"
#define CSCM_ERROR_LIST_INDEX		"incorrect index"
#define CSCM_ERROR_LIST_RANGE		"incorrect range"




#endif
