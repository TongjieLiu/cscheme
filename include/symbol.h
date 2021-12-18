/* symbol.h -- scheme symbol

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

#ifndef __CSCM_SYMBOL_H__

#define __CSCM_SYMBOL_H__




#include "object.h"
#include "ef.h"
#include "ast.h"




CSCM_OBJECT *cscm_symbol_create();


void cscm_symbol_set(CSCM_OBJECT *symbol, char *text);


char *cscm_symbol_get(CSCM_OBJECT *symbol);




void cscm_symbol_print(CSCM_OBJECT *obj, FILE *stream);




int cscm_is_symbol(CSCM_AST_NODE *exp);


CSCM_EF *cscm_analyze_symbol(CSCM_AST_NODE *exp);




void cscm_symbol_free(CSCM_OBJECT *obj);




void cscm_symbol_ef_free(CSCM_EF *ef);




#endif
