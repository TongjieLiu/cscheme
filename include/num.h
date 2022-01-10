/* num.h -- scheme number: long and double

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

#ifndef __CSCM_NUM_H__

#define __CSCM_NUM_H__




#include "object.h"
#include "ef.h"
#include "ast.h"




#define CSCM_NUM_MAX_TEXT_LEN		25




CSCM_OBJECT *cscm_num_long_create();
CSCM_OBJECT *cscm_num_double_create();


void cscm_num_long_set(CSCM_OBJECT *num, long val);
void cscm_num_double_set(CSCM_OBJECT *num, double val);


long cscm_num_long_get(CSCM_OBJECT *num);
double cscm_num_double_get(CSCM_OBJECT *num);




void cscm_num_print(CSCM_OBJECT *obj, FILE *stream);




int cscm_is_num_long(CSCM_AST_NODE *exp);
int cscm_is_num_double(CSCM_AST_NODE *exp);


CSCM_EF *cscm_analyze_num_long(CSCM_AST_NODE *exp);
CSCM_EF *cscm_analyze_num_double(CSCM_AST_NODE *exp);




void cscm_num_ef_free(CSCM_EF *ef);




void cscm_num_free(CSCM_OBJECT *obj);




#endif
