/* proc.h -- scheme procedure

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

#ifndef __CSCM_PROC_H__

#define __CSCM_PROC_H__




#include <stddef.h>

#include "object.h"
#include "core.h"




typedef CSCM_OBJECT *(*CSCM_PROC_PRIM_FUNC)(size_t n, CSCM_OBJECT **args);


struct _CSCM_PROC_PRIM {
	CSCM_PROC_PRIM_FUNC f;
};


typedef struct _CSCM_PROC_PRIM CSCM_PROC_PRIM;




struct _CSCM_PROC_COMP {
	int flag_dtn; // dotted-tail notation

	size_t n_params;
	char **params; // formal parameters

	CSCM_EF *body;

	CSCM_OBJECT *env;
};


typedef struct _CSCM_PROC_COMP CSCM_PROC_COMP;




#define CSCM_ERROR_PROC_PRIM_BAD_FUNC	"bad function"



#define CSCM_ERROR_PROC_COMP_BAD_PARAMS	"bad formal parameters"
#define CSCM_ERROR_PROC_COMP_BAD_BODY	"bad body execution function"
#define CSCM_ERROR_PROC_COMP_BAD_ENV	"bad environment"




#define CSCM_ERROR_PROC_COMP_NOT_INIT	"only support initialization"




CSCM_OBJECT *cscm_proc_prim_create();


void cscm_proc_prim_set(CSCM_OBJECT *proc_obj, CSCM_PROC_PRIM_FUNC f);


CSCM_PROC_PRIM_FUNC cscm_proc_prim_get_f(CSCM_OBJECT *proc_obj);




CSCM_OBJECT *cscm_proc_comp_create();


void cscm_proc_comp_set(CSCM_OBJECT *proc_obj,	\
		int flag_dtn,			\
		size_t n_params, char **params,	\
		CSCM_EF *body,			\
		CSCM_OBJECT *env);


size_t cscm_proc_comp_get_flag_dtn(CSCM_OBJECT *proc_obj);
size_t cscm_proc_comp_get_n_params(CSCM_OBJECT *proc_obj);
char **cscm_proc_comp_get_params(CSCM_OBJECT *proc_obj);
CSCM_EF *cscm_proc_comp_get_body(CSCM_OBJECT *proc_obj);
CSCM_OBJECT *cscm_proc_comp_get_env(CSCM_OBJECT *proc_obj);




void cscm_proc_print(CSCM_OBJECT *obj, FILE *stream);




void cscm_proc_prim_free(CSCM_OBJECT *obj);
void cscm_proc_comp_free(CSCM_OBJECT *obj);




#endif
