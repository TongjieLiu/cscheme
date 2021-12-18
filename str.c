/* str.c -- scheme string

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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "object.h"
#include "error.h"
#include "text.h"
#include "ast.h"
#include "ef.h"
#include "gc.h"
#include "str.h"




CSCM_OBJECT *cscm_string_create()
{
	CSCM_OBJECT *obj;


	obj = cscm_object_create();


	obj->type = CSCM_OBJECT_TYPE_STRING;
	obj->value = NULL;


	return obj;
}




void cscm_string_set(CSCM_OBJECT *string, char *text)
{
	char *old_text;


	if (string == NULL)
		cscm_error_report("cscm_string_set", \
				CSCM_ERROR_NULL_PTR);
	else if (string->type != CSCM_OBJECT_TYPE_STRING)
		cscm_error_report("cscm_string_set", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (text == NULL)
		cscm_error_report("cscm_string_set", \
				CSCM_ERROR_NULL_PTR);


	old_text = string->value;
	string->value = cscm_text_cpy_ec(text); // handle escape characters


	/*	The new text may be a substring of the old text,
	 * then there will be no new text after we have freed the
	 * old text.
	 * 	Therefore, we have to cscm_text_cpy_ec() the new
	 * text first, and only after that we can safely free the
	 * old one. */
	if (old_text)
		free(old_text);
}




char *cscm_string_get(CSCM_OBJECT *string)
{
	if (string == NULL)
		cscm_error_report("cscm_string_get", \
				CSCM_ERROR_NULL_PTR);
	else if (string->type != CSCM_OBJECT_TYPE_STRING)
		cscm_error_report("cscm_string_get", \
				CSCM_ERROR_OBJECT_TYPE);


	return (char *)string->value;
}


int cscm_string_text_equal(CSCM_OBJECT *string, char* text)
{
	if (string == NULL)
		cscm_error_report("cscm_string_text_equal", \
				CSCM_ERROR_NULL_PTR);
	else if (string->type != CSCM_OBJECT_TYPE_STRING)
		cscm_error_report("cscm_string_text_equal", \
				CSCM_ERROR_OBJECT_TYPE);


	return !strcmp((char *)string->value, text);
}




void cscm_string_print(CSCM_OBJECT *obj, FILE *stream)
{
	if (obj == NULL || stream == NULL)
		cscm_error_report("cscm_string_print", \
				CSCM_ERROR_NULL_PTR);


	if (obj->type == CSCM_OBJECT_TYPE_STRING)
		fputs((char *)obj->value, stream);
	else
		cscm_error_report("cscm_string_print", \
				CSCM_ERROR_OBJECT_TYPE);
}




int cscm_is_string(CSCM_AST_NODE *exp)
{
	char *string;


	if (exp == NULL)
		cscm_error_report("cscm_is_string", \
				CSCM_ERROR_NULL_PTR);
	else if (!cscm_ast_is_symbol(exp))
		return 0;
	else if (cscm_ast_is_symbol_empty(exp))
		cscm_error_report("cscm_is_string", \
				CSCM_ERROR_AST_EMPTY_SYMBOL);


	string = cscm_text_strip_all_squotes(exp->text);

	if (cscm_text_is_dquoted(string)) {
		if (string != exp->text)
			cscm_ast_symbol_set(exp, string);

		return 1;
	} else {
		return 0;
	}
}




CSCM_OBJECT *_cscm_string_ef(void *state, CSCM_OBJECT *env)
{
	return (CSCM_OBJECT *)state;
}


CSCM_EF *cscm_analyze_string(CSCM_AST_NODE *exp)
{
	char *text;
	CSCM_OBJECT *string;


	string = cscm_string_create();


	text = cscm_text_cpy_strip_dquotes(exp->text);

	cscm_string_set(string, text);

	free(text);


	/*	The execution function only has 1 copy of this
	 * object, therefore we have to keep it safe no matter
	 * how many times the execution function is being called. */
	cscm_gc_inc(string);


	return cscm_ef_construct(CSCM_EF_TYPE_STRING,	\
				string,			\
				_cscm_string_ef);
}




void cscm_string_free(CSCM_OBJECT *obj)
{
	if (obj == NULL)
		cscm_error_report("cscm_string_free", \
				CSCM_ERROR_NULL_PTR);
	else if (obj->type != CSCM_OBJECT_TYPE_STRING)
		cscm_error_report("cscm_string_free", \
				CSCM_ERROR_OBJECT_TYPE);


	free(obj->value);

	free(obj);
}




void cscm_string_ef_free(CSCM_EF *ef)
{
	CSCM_OBJECT *string;


	if (ef == NULL)
		cscm_error_report("cscm_string_ef_free", \
				CSCM_ERROR_NULL_PTR);
	else if (ef->type != CSCM_EF_TYPE_STRING)
		cscm_error_report("cscm_string_ef_free", \
				CSCM_ERROR_OBJECT_TYPE);


	string = (CSCM_OBJECT *)ef->state;

	cscm_gc_dec(string);
	cscm_gc_free(string);

	free(ef);
}
