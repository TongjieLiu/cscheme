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
#include "text.h"
#include "ef.h"
#include "ast.h"
#include "gc.h"
#include "pair.h"
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




char *cscm_symbol_get(CSCM_OBJECT *symbol)
{
	if (symbol == NULL)
		cscm_error_report("cscm_symbol_get", CSCM_ERROR_NULL_PTR);
	else if (symbol->type != CSCM_OBJECT_TYPE_SYMBOL)
		cscm_error_report("cscm_symbol_get", CSCM_ERROR_OBJECT_TYPE);


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




int cscm_is_symbol(CSCM_AST_NODE *exp)
{
	if (exp == NULL)
		cscm_error_report("cscm_is_symbol", \
				CSCM_ERROR_NULL_PTR);
	else if (!cscm_ast_is_symbol(exp))
		return 0;
	else if (cscm_ast_is_symbol_empty(exp))
		cscm_error_report("cscm_is_symbol", \
				CSCM_ERROR_AST_EMPTY_SYMBOL);
	else if (cscm_text_is_squoted(exp->text)		\
		&& !cscm_text_is_integer(			\
			cscm_text_strip_all_squotes(exp->text))	\
		&& !cscm_text_is_fpn(				\
			cscm_text_strip_all_squotes(exp->text))	\
		&& !cscm_text_is_dquoted(			\
			cscm_text_strip_all_squotes(exp->text)))

		return 1;
	else
		return 0;
}




CSCM_OBJECT *_cscm_symbol_text_to_slist(char *text)
{
	CSCM_OBJECT *symbol;
	CSCM_OBJECT *elems[2];


	if (cscm_text_is_squoted(text)) {
		elems[0] = cscm_symbol_create();
		cscm_symbol_set(elems[0], "quote");

		elems[1] = _cscm_symbol_text_to_slist( \
				cscm_text_strip_squote(text));

		return cscm_list_create(2, elems);
	} else {
		symbol = cscm_symbol_create();
		cscm_symbol_set(symbol, text);

		return symbol;
	}
}




CSCM_OBJECT *_cscm_symbol_ef(void *state, CSCM_OBJECT *env)
{
	char *text;

	CSCM_OBJECT *symbol, *list;


	text = (char *)state;

	if (cscm_text_is_squoted(text)) {
		list = _cscm_symbol_text_to_slist(text);

		return list;
	} else {
		symbol = cscm_symbol_create();
		cscm_symbol_set(symbol, text);

		return symbol;
	}
}


CSCM_EF *cscm_analyze_symbol(CSCM_AST_NODE *exp)
{
	char *text;


	text = cscm_text_strip_squote(exp->text);

	/*	All scheme symbols should be in lower-case in order
	 * to simplify processes of comparison. */
	text = cscm_text_cpy_lowercase(text);


	return cscm_ef_construct(CSCM_EF_TYPE_SYMBOL,	\
				text,			\
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


	free(obj->value);

	free(obj);
}




void cscm_symbol_ef_free(CSCM_EF *ef)
{
	if (ef == NULL)
		cscm_error_report("cscm_symbol_ef_free", \
				CSCM_ERROR_NULL_PTR);
	else if (ef->type != CSCM_EF_TYPE_SYMBOL)
		cscm_error_report("cscm_symbol_ef_free", \
				CSCM_ERROR_OBJECT_TYPE);


	free(ef->state);

	free(ef);
}
