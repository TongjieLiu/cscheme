/* var.c -- scheme variable

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

#include "stdlib.h"

#include "error.h"
#include "ast.h"
#include "object.h"
#include "ef.h"
#include "text.h"
#include "env.h"
#include "var.h"




int cscm_is_var(CSCM_AST_NODE *exp)
{
	if (exp == NULL)
		cscm_error_report("cscm_is_var", \
				CSCM_ERROR_NULL_PTR);
	else if (!cscm_ast_is_symbol(exp))
		return 0;
	else if (cscm_ast_is_symbol_empty(exp))
		cscm_error_report("cscm_is_var", \
				CSCM_ERROR_AST_EMPTY_SYMBOL);
	else
		return 1;
}




CSCM_OBJECT *_cscm_var_ef(void *state, CSCM_OBJECT *env)
{
	CSCM_OBJECT *ret;


	if (state == NULL)
		ret = CSCM_UNASSIGNED;
	else
		ret = cscm_env_get_var(env, (char *)state);


	return ret;
}


CSCM_EF *cscm_analyze_var(CSCM_AST_NODE *exp)
{
	char *name;


	if (cscm_ast_symbol_text_equal(exp, "**UNASSIGNED**"))
		name = NULL;
	else
		name = cscm_text_cpy(exp->text);


	return cscm_ef_construct(CSCM_EF_TYPE_VAR,	\
				name,			\
				_cscm_var_ef);
}




void cscm_var_ef_free(CSCM_EF *ef)
{
	if (ef == NULL)
		cscm_error_report("cscm_var_ef_free", \
				CSCM_ERROR_NULL_PTR);
	else if (ef->type != CSCM_EF_TYPE_VAR)
		cscm_error_report("cscm_var_ef_free", \
				CSCM_ERROR_EF_TYPE);


	if (ef->state)
		free(ef->state);


	free(ef);
}
