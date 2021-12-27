/* definition.h -- scheme definition(special form)

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

#ifndef __CSCM_DEFINITION_H__

#define __CSCM_DEFINITION_H__




#include "ef.h"
#include "ast.h"




struct _CSCM_DEFINITION_EF_STATE {
	char *var;
	CSCM_EF *val_ef;
};


typedef struct _CSCM_DEFINITION_EF_STATE CSCM_DEFINITION_EF_STATE;




#define CSCM_ERROR_DEFINITION_NO_VAR		"definition variable is not specified"
#define CSCM_ERROR_DEFINITION_BAD_VAR		"bad definition variable"


#define CSCM_ERROR_DEFINITION_NO_VAL		"definition value is not specified"
#define CSCM_ERROR_DEFINITION_BAD_VAL		"bad definition value"




#define CSCM_ERROR_DEFINITION_NO_PROC		\
	"procedure name is not specified in definition expression"

#define CSCM_ERROR_DEFINITION_EMPTY_BODY	\
	"empty body in definition expression"




int cscm_is_definition(CSCM_AST_NODE *exp);
CSCM_EF *cscm_analyze_definition(CSCM_AST_NODE *exp);


void cscm_definition_ef_free(CSCM_EF *ef);




#endif
