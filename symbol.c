/* symbol.c -- scheme symbol

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

#include <stdlib.h>
#include <stdio.h>

#include "object.h"
#include "error.h"
#include "ef.h"
#include "ast.h"
#include "gc.h"
#include "text.h"
#include "symbol.h"




CSCM_OBJECT *cscm_symbol_create()
{
	CSCM_OBJECT *obj;


	obj = cscm_object_create();


	obj->type = CSCM_OBJECT_TYPE_SYMBOL;
	obj->value = NULL;


	return obj;
}




void cscm_symbol_set(CSCM_OBJECT *symbol, char *text)
{
	char *old_text;


	if (symbol == NULL)
		cscm_error_report("cscm_symbol_set", CSCM_ERROR_NULL_PTR);
	else if (symbol->type != CSCM_OBJECT_TYPE_SYMBOL)
		cscm_error_report("cscm_symbol_set", CSCM_ERROR_OBJECT_TYPE);
	else if (text == NULL)
		cscm_error_report("cscm_symbol_set", CSCM_ERROR_NULL_PTR);


	old_text = symbol->value;
	symbol->value = cscm_text_cpy(text);


	/*	The new text may be a substring of the old text,
	 * then there will be no new text after we have freed the
	 * old text.
	 * 	Therefore, we have to cscm_text_cpy() the new text
	 * first, and only after that we can safely free the old
	 * one. */
	if (old_text)
		free(old_text);
}




/*	Since we'll free the memory of old text, we should not call
 * this function with string literals and any substrings of the old
 * text as the new text to be set. */
void cscm_symbol_set_simple(CSCM_OBJECT *symbol, char *text)
{
	if (symbol == NULL)
		cscm_error_report("cscm_symbol_set_simple", \
				CSCM_ERROR_NULL_PTR);
	else if (symbol->type != CSCM_OBJECT_TYPE_SYMBOL)
		cscm_error_report("cscm_symbol_set_simple", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (text == NULL)
		cscm_error_report("cscm_symbol_set_simple", \
				CSCM_ERROR_NULL_PTR);


	if (symbol->value)
		free(symbol->value);

	symbol->value = text;
}




char *cscm_symbol_get(CSCM_OBJECT *symbol)
{
	if (symbol == NULL)
		cscm_error_report("cscm_symbol_get", \
				CSCM_ERROR_NULL_PTR);
	else if (symbol->type != CSCM_OBJECT_TYPE_SYMBOL)
		cscm_error_report("cscm_symbol_get", \
				CSCM_ERROR_OBJECT_TYPE);
	else if (symbol->value == NULL)
		cscm_error_report("cscm_symbol_get", \
				CSCM_ERROR_EMPTY_OBJECT);


	return (char *)symbol->value;
}




void cscm_symbol_print(CSCM_OBJECT *obj, FILE *stream)
{
	if (obj == NULL || stream == NULL)
		cscm_error_report("cscm_symbol_print", \
				CSCM_ERROR_NULL_PTR);


	if (obj->type == CSCM_OBJECT_TYPE_SYMBOL)
		fputs((char *)obj->value, stream);
	else
		cscm_error_report("cscm_symbol_print", \
				CSCM_ERROR_OBJECT_TYPE);
}




CSCM_OBJECT *_cscm_symbol_ef(void *state, CSCM_OBJECT *env)
{
	return (CSCM_OBJECT *)state;
}


CSCM_EF *cscm_analyze_symbol(CSCM_AST_NODE *exp)
{
	char *text;
	CSCM_OBJECT *symbol;


	/*	All scheme symbols should be in lower-case in order
	 * to simplify processes of comparison. */
	text = cscm_text_cpy_lowercase(exp->text);


	symbol = cscm_symbol_create();

	cscm_symbol_set_simple(symbol, text);


	/*	The execution function only has 1 copy of this
	 * object, therefore we have to keep it safe no matter
	 * how many times the execution function is being called. */
	cscm_gc_inc(symbol);


	return cscm_ef_construct(CSCM_EF_TYPE_SYMBOL,	\
				symbol,			\
				_cscm_symbol_ef);
}




void cscm_symbol_free(CSCM_OBJECT *obj)
{
	if (obj == NULL)
		cscm_error_report("cscm_symbol_free", \
				CSCM_ERROR_NULL_PTR);
	else if (obj->type != CSCM_OBJECT_TYPE_SYMBOL)
		cscm_error_report("cscm_symbol_free", \
				CSCM_ERROR_OBJECT_TYPE);


	if (obj->value)
		free(obj->value);

	free(obj);
}




void cscm_symbol_ef_free(CSCM_EF *ef)
{
	CSCM_OBJECT *symbol;


	if (ef == NULL)
		cscm_error_report("cscm_symbol_ef_free", \
				CSCM_ERROR_NULL_PTR);
	else if (ef->type != CSCM_EF_TYPE_SYMBOL)
		cscm_error_report("cscm_symbol_ef_free", \
				CSCM_ERROR_OBJECT_TYPE);


	symbol = (CSCM_OBJECT *)ef->state;

	cscm_gc_dec(symbol);
	cscm_gc_free(symbol);


	free(ef);
}
