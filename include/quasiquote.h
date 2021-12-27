/* quasiquote.h -- scheme quasiquote expression(special form)

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

#ifndef __CSCM_QUASIQUOTE_H__

#define __CSCM_QUASIQUOTE_H__




#include <stddef.h>

#include "ef.h"
#include "ast.h"




struct _CSCM_QUASIQUOTE_EF_STATE {
	size_t n_efs;
	CSCM_EF **efs;
};


typedef struct _CSCM_QUASIQUOTE_EF_STATE CSCM_QUASIQUOTE_EF_STATE;




#define CSCM_ERROR_QUASIQUOTE_N_CLAUSES	\
	"quasiquote expression only accept 1 clause"

#define CSCM_ERROR_UNQUOTE_N_CLAUSES	\
	"unquote expression only accept 1 clause"




int cscm_is_quasiquote(CSCM_AST_NODE *exp);


CSCM_EF *cscm_analyze_quasiquote(CSCM_AST_NODE *exp);




void cscm_quasiquote_ef_free(CSCM_EF *ef);




#endif
