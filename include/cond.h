/* cond.h -- scheme cond expression(syntactic sugar)

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

#ifndef __CSCM_COND_H__

#define __CSCM_COND_H__




#include "ef.h"
#include "ast.h"




#define CSCM_ERROR_COND_NO_CLAUSE	"no clasue"
#define CSCM_ERROR_COND_BAD_CLAUSE	"bad clause"
#define CSCM_ERROR_COND_BAD_ELSE	"bad else clause"




int cscm_is_cond(CSCM_AST_NODE *exp);
CSCM_EF *cscm_analyze_cond(CSCM_AST_NODE *exp);




#endif
