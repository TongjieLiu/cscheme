/* bool.c -- scheme true and false

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

#include <stddef.h>
#include <stdio.h>

#include "error.h"
#include "object.h"
#include "bool.h"




/* there is only one copy of true in cscheme */
CSCM_OBJECT _cscm_bool_true = {CSCM_OBJECT_TYPE_BOOL_TRUE, (void *)0, 0};

/* there is only one copy of false in cscheme */
CSCM_OBJECT _cscm_bool_false = {CSCM_OBJECT_TYPE_BOOL_FALSE, (void *)0, 0};




void cscm_bool_print(CSCM_OBJECT *obj, FILE *stream)
{
	if (obj == NULL || stream == NULL)
		cscm_error_report("cscm_bool_print", \
				CSCM_ERROR_NULL_PTR);


	if (obj == CSCM_TRUE)
		fputs("#t", stream);
	else if (obj == CSCM_FALSE)
		fputs("#f", stream);
	else if (obj->type == CSCM_OBJECT_TYPE_BOOL_TRUE
		|| obj->type == CSCM_OBJECT_TYPE_BOOL_FALSE)
		cscm_error_report("cscm_bool_print", \
				CSCM_ERROR_BOOL_EXTRA_COPY);
	else
		cscm_error_report("cscm_bool_print", \
				CSCM_ERROR_OBJECT_TYPE);
}




void cscm_bool_free(CSCM_OBJECT *obj)
{
	if (obj == NULL)
		cscm_error_report("cscm_bool_free", \
				CSCM_ERROR_NULL_PTR);


	if (obj == CSCM_TRUE)
		return; // since it is not stored on the heap
	else if (obj == CSCM_FALSE)
		return; // since it is not stored on the heap
	else if (obj->type == CSCM_OBJECT_TYPE_BOOL_TRUE
		|| obj->type == CSCM_OBJECT_TYPE_BOOL_FALSE)
		cscm_error_report("cscm_bool_free", \
				CSCM_ERROR_BOOL_EXTRA_COPY);
	else
		cscm_error_report("cscm_bool_free", \
				CSCM_ERROR_OBJECT_TYPE);
}
