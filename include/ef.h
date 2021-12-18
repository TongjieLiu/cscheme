/* ef.h -- execution function

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

#ifndef __CSCM_EF_H__

#define __CSCM_EF_H__




#include <stddef.h>

#include "cscheme.h"
#include "object.h"




#ifdef __CSCM_CSCHEME_DEBUG__
	#define __CSCM_EF_DEBUG__
#endif




#define CSCM_EF_TYPE_NUM_LONG		0
#define CSCM_EF_TYPE_NUM_DOUBLE		1
#define CSCM_EF_TYPE_SYMBOL		2
#define CSCM_EF_TYPE_STRING		3
#define CSCM_EF_TYPE_VAR		4
#define CSCM_EF_TYPE_ASSIGNMENT		5
#define CSCM_EF_TYPE_DEFINITION		6
#define CSCM_EF_TYPE_LAMBDA		7
#define CSCM_EF_TYPE_IF			8
#define CSCM_EF_TYPE_SEQ		9
#define CSCM_EF_TYPE_AO			10
#define CSCM_EF_TYPE_NOT		11
#define CSCM_EF_TYPE_COMBINATION	12
#define CSCM_EF_TYPE_NONE		13




typedef CSCM_OBJECT *(*CSCM_EF_FUNC)(void *state, CSCM_OBJECT *env);


struct _CSCM_EF {
	int type;

	void *state;
	CSCM_EF_FUNC f;
};


typedef struct _CSCM_EF CSCM_EF;




typedef void (*CSCM_EF_FREE_FUNC)(CSCM_EF *ef);




CSCM_OBJECT *cscm_ef_exec(CSCM_EF *ef, CSCM_OBJECT *env);


CSCM_EF *cscm_ef_construct(int type, void *state, CSCM_EF_FUNC f);


CSCM_EF **cscm_ef_ptrs_create(size_t n);


void cscm_ef_free_tree(CSCM_EF *ef);




#define CSCM_ERROR_EF_TYPE	"incorrect execution function type"


#define CSCM_ERROR_EF_BAD_FUNC	"bad function"


#define CSCM_ERROR_EF_ZERO_PTR	"requesting zero pointer"




#endif
