/* lambda.h -- scheme lambda expression(special form)

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

#ifndef __CSCM_LAMBDA_H__

#define __CSCM_LAMBDA_H__




#include <stddef.h>

#include "ef.h"
#include "ast.h"




#define CSCM_ERROR_LAMBDA_NO_PARAM	"formal parameter is not specified"
#define CSCM_ERROR_LAMBDA_BAD_PARAMS	"bad formal parameters"
#define CSCM_ERROR_LAMBDA_BAD_PARAM	"bad formal parameter"


#define CSCM_ERROR_LAMBDA_EMPTY_BODY	"empty body"




struct _CSCM_LAMBDA_EF_STATE {
	size_t n_params;
	char **params;

	CSCM_EF *body;
};


typedef struct _CSCM_LAMBDA_EF_STATE CSCM_LAMBDA_EF_STATE;




int cscm_is_lambda(CSCM_AST_NODE *exp);
CSCM_EF *cscm_analyze_lambda(CSCM_AST_NODE *exp);


void cscm_lambda_ef_free(CSCM_EF *ef);




#endif
