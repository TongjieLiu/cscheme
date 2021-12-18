/* if.h -- scheme if expression(special form)

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

#ifndef __CSCM_IF_H__

#define __CSCM_IF_H__




#include "ef.h"
#include "ast.h"




struct _CSCM_IF_EF_STATE {
	CSCM_EF *predicate_ef;
	CSCM_EF *consequent_ef;
	CSCM_EF *alternative_ef;
};


typedef struct _CSCM_IF_EF_STATE CSCM_IF_EF_STATE;




#define CSCM_ERROR_IF_INCOMPLETE	"incomplete if expression"




int cscm_is_if(CSCM_AST_NODE *exp);
CSCM_EF *cscm_analyze_if(CSCM_AST_NODE *exp);


void cscm_if_ef_free(CSCM_EF *ef);




#endif
