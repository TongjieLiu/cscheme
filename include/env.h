/* env.h -- environment

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

#ifndef __CSCM_ENV_H__

#define __CSCM_ENV_H__




#include <stddef.h>

#include "object.h"




#define CSCM_FRAME_MAX_SIZE	512




struct _CSCM_FRAME {
	size_t n_bindings;

	char *vars[CSCM_FRAME_MAX_SIZE];
	CSCM_OBJECT *vals[CSCM_FRAME_MAX_SIZE];
};

typedef struct _CSCM_FRAME CSCM_FRAME;


struct _CSCM_ENV {
	size_t n_frames;
	CSCM_OBJECT **frames;
};

typedef struct _CSCM_ENV CSCM_ENV;




#define CSCM_ERROR_FRAME_TOO_MANY_VARS	"too many variables"


#define CSCM_ERROR_FRAME_NOT_EMPTY	"frame is not empty"
#define CSCM_ERROR_FRAME_FULL		"frame is full"


#define CSCM_ERROR_FRAME_NO_VAR		"variable name is not specified"
#define CSCM_ERROR_FRAME_NO_VAL		"value is not specified"


#define CSCM_ERROR_FRAME_UNASSIGNED	"scheme special value *unassigned*"
#define CSCM_ERROR_FRAME_UNBOUND	"unbound variable"


#define CSCM_ERROR_FRAME_NOT_UNIQUE	"specified variables have duplications"


#define CSCM_ERROR_FRAME_EMPTY_BINDING	"empty binding"




#define CSCM_ERROR_ENV_UNBOUND		"unbound variable"


#define CSCM_ERROR_ENV_NO_VAR		"variable name is not specified"
#define CSCM_ERROR_ENV_NO_VAL		"value is not specified"


#define CSCM_ERROR_ENV_EMPTY		"empty environment"




#define CSCM_ERROR_UNASSIGNED_EXTRA_COPY \
	"unauthorized copy of the \"**UNASSIGNED**\""




#define CSCM_ERROR_GLOBAL_ENV_EXISTED \
	"the global environment had already been created"

#define CSCM_ERROR_GLOBAL_ENV_NOT_EXISTED \
	"the global environment had not been created"




extern CSCM_OBJECT _cscm_unassigned;

#define CSCM_UNASSIGNED	(&_cscm_unassigned)




CSCM_OBJECT *cscm_frame_create();


void cscm_frame_init(CSCM_OBJECT *frame_obj, size_t n, char **vars, CSCM_OBJECT **vals);


void cscm_frame_add_var(CSCM_OBJECT *frame_obj, char *var, CSCM_OBJECT *val);
CSCM_OBJECT *cscm_frame_get_var(CSCM_OBJECT *frame_obj, char *var);
void cscm_frame_set_var(CSCM_OBJECT *frame_obj, char *var, CSCM_OBJECT *val);




CSCM_OBJECT *cscm_env_create();


CSCM_OBJECT *cscm_env_cpy_extend(CSCM_OBJECT *env_obj, CSCM_OBJECT *frame);


CSCM_OBJECT *cscm_env_get_var(CSCM_OBJECT *env_obj, char *var);
void cscm_env_set_var(CSCM_OBJECT *env_obj, char *var, CSCM_OBJECT *val);
void cscm_env_add_var(CSCM_OBJECT *env_obj, char *var, CSCM_OBJECT *val);




CSCM_OBJECT *cscm_global_env_setup();
CSCM_OBJECT *cscm_global_env_get();




void cscm_frame_print(CSCM_OBJECT *obj, FILE *stream);
void cscm_frame_print_details(CSCM_OBJECT *obj, char *prefix);
void cscm_env_print(CSCM_OBJECT *obj, FILE *stream);
void cscm_env_print_details(CSCM_OBJECT *obj);
void cscm_unassigned_print(CSCM_OBJECT *obj, FILE *stream);




void cscm_frame_free(CSCM_OBJECT *obj);
void cscm_env_free(CSCM_OBJECT *obj);
void cscm_unassigned_free(CSCM_OBJECT *obj);




#endif
