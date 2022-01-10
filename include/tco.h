/* tco.h -- tail-call optimization

   Copyright (C) 2021-2022 Tongjie Liu <tongjieandliu@gmail.com>.

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

#ifndef __CSCM_TCO_H__

#define __CSCM_TCO_H__




#include "ast.h"




#define CSCM_TCO_FLAG_ALLOW		((unsigned char)1)
#define CSCM_TCO_FLAG_STATE_SAVED	((unsigned char)2)




#define CSCM_ERROR_TCO_FLAG_TYPE	"unsupported flag type"


#define CSCM_ERROR_TCO_NOT_ALLOW	"not allow"
#define CSCM_ERROR_TCO_STATE_SAVED	"the state has already been saved"
#define CSCM_ERROR_TCO_STATE_NOT_SAVED	"the state has not been saved"




void cscm_tco_set_flag(unsigned char flag);
void cscm_tco_unset_flag(unsigned char flag);


int cscm_tco_get_flag(unsigned char flag);




void cscm_tco_state_save(CSCM_OBJECT *new_env,	\
			CSCM_EF *new_body_ef,	\
			CSCM_AST_NODE *new_exp);


void cscm_tco_state_get(CSCM_OBJECT **new_env_ptr,	\
			CSCM_EF **new_body_ef_ptr,	\
			CSCM_AST_NODE **new_exp_ptr);




#endif
