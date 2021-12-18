/* str.h -- scheme string

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

#ifndef __CSCM_STR_H__

#define __CSCM_STR_H__




#include "object.h"
#include "ef.h"
#include "ast.h"




CSCM_OBJECT *cscm_string_create();


void cscm_string_set(CSCM_OBJECT *string, char *text);


char *cscm_string_get(CSCM_OBJECT *string);
int cscm_string_text_equal(CSCM_OBJECT *string, char* text);




void cscm_string_print(CSCM_OBJECT *obj, FILE *stream);




int cscm_is_string(CSCM_AST_NODE *exp);


CSCM_EF *cscm_analyze_string(CSCM_AST_NODE *exp);




void cscm_string_free(CSCM_OBJECT *obj);




void cscm_string_ef_free(CSCM_EF *ef);




#endif
