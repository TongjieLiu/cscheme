/* assignment.h -- scheme assignment(special form)

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

#ifndef __CSCM_ASSIGNMENT_H__

#define __CSCM_ASSIGNMENT_H__




#include "ef.h"
#include "ast.h"




struct _CSCM_ASSIGNMENT_EF_STATE {
	char *var;
	CSCM_EF *val_ef;
};


typedef struct _CSCM_ASSIGNMENT_EF_STATE CSCM_ASSIGNMENT_EF_STATE;




#define CSCM_ERROR_ASSIGNMENT_NO_VAR	"variable is not specified"
#define CSCM_ERROR_ASSIGNMENT_BAD_VAR	"bad variable"


#define CSCM_ERROR_ASSIGNMENT_NO_VAL	"value is not specified"
#define CSCM_ERROR_ASSIGNMENT_BAD_VAL	"bad value"




int cscm_is_assignment(CSCM_AST_NODE *exp);
CSCM_EF *cscm_analyze_assignment(CSCM_AST_NODE *exp);


void cscm_assignment_ef_free(CSCM_EF *ef);




#endif
