/* core.h -- syntactic analysis

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

#ifndef __CSCM_CORE_H__

#define __CSCM_CORE_H__




#include <stddef.h>

#include "object.h"
#include "ast.h"
#include "ef.h"




typedef int (*CSCM_SA_PREDICATE)(CSCM_AST_NODE *exp);
typedef CSCM_EF *(*CSCM_SA_ANALYZE)(CSCM_AST_NODE *exp);


struct _CSCM_SA_FUNCS {
	int flag_end;

	CSCM_SA_PREDICATE predicate;
	CSCM_SA_ANALYZE analyze;
};


typedef struct _CSCM_SA_FUNCS CSCM_SA_FUNCS;




struct _CSCM_COMBINATION_EF_STATE {
	CSCM_EF *proc_ef;

	size_t n_arg_efs;
	CSCM_EF **arg_efs;
};


typedef struct _CSCM_COMBINATION_EF_STATE CSCM_COMBINATION_EF_STATE;




#define CSCM_ERROR_ANALYZE_UNKNOWN_EXP_TYPE	"unknown expression type"


#define CSCM_ERROR_APPLY_NO_PROC		"procedure is not specified"


#define CSCM_ERROR_APPLY_N_ARGS			"incorrect number of arguments"




CSCM_OBJECT *cscm_eval(CSCM_AST_NODE *exp, CSCM_OBJECT *env);
CSCM_EF *cscm_analyze(CSCM_AST_NODE *exp);




CSCM_OBJECT *cscm_apply(CSCM_OBJECT *proc, \
		size_t n_args, CSCM_OBJECT **args);
int cscm_is_combination(CSCM_AST_NODE *exp);
CSCM_EF *cscm_analyze_combination(CSCM_AST_NODE *exp);


void cscm_combination_ef_free(CSCM_EF *ef);




#endif
