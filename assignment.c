/* assignment.c -- scheme assignment(special form)

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

#include "error.h"
#include "ast.h"
#include "object.h"
#include "ef.h"
#include "env.h"
#include "text.h"
#include "core.h"
#include "symbol.h"
#include "num.h"
#include "str.h"
#include "var.h"
#include "assignment.h"




int cscm_is_assignment(CSCM_AST_NODE *exp)
{
	CSCM_AST_NODE *head;
	CSCM_AST_NODE *var;


	if (exp == NULL)
		cscm_error_report("cscm_is_assignment", \
				CSCM_ERROR_NULL_PTR);
	else if (!cscm_ast_is_exp(exp))
		return 0;
	else if (cscm_ast_is_exp_empty(exp))
		return 0;


	head = cscm_ast_exp_index(exp, 0);
	if (!cscm_ast_is_symbol(head))
		return 0;
	else if (!cscm_ast_symbol_text_equal(head, "set!"))
		return 0;


	if (exp->n_childs == 1)
		cscm_syntax_error_report(exp->filename,		\
				exp->line,			\
				CSCM_ERROR_ASSIGNMENT_NO_VAR);


	var = cscm_ast_exp_index(exp, 1);
	if (cscm_is_symbol(var)			\
		|| cscm_is_num_long(var)	\
		|| cscm_is_num_double(var)	\
		|| cscm_is_string(var))
		cscm_syntax_error_report(var->filename,		\
				var->line,			\
				CSCM_ERROR_ASSIGNMENT_BAD_VAR);
	else if (!cscm_is_var(var))
		cscm_syntax_error_report(var->filename,		\
				var->line,			\
				CSCM_ERROR_ASSIGNMENT_BAD_VAR);


	if (exp->n_childs == 2)
		cscm_syntax_error_report(exp->filename,		\
				exp->line,			\
				CSCM_ERROR_ASSIGNMENT_NO_VAL);
	else if (exp->n_childs > 3) // multiple value to set
		cscm_syntax_error_report(exp->filename,		\
				exp->line,			\
				CSCM_ERROR_ASSIGNMENT_BAD_VAL);


	return 1;
}




CSCM_ASSIGNMENT_EF_STATE *_cscm_assignment_ef_state_create()
{
	CSCM_ASSIGNMENT_EF_STATE *state;


	state = malloc(sizeof(CSCM_ASSIGNMENT_EF_STATE));
	if (state == NULL)
		cscm_libc_fail("_cscm_assignment_ef_state_create", \
				"malloc");


	state->var = NULL;
	state->val_ef = NULL;


	return state;
}


CSCM_OBJECT *_cscm_assignment_ef(void *state, CSCM_OBJECT *env)
{
	CSCM_OBJECT *val;
	CSCM_ASSIGNMENT_EF_STATE *s;


	s = (CSCM_ASSIGNMENT_EF_STATE *)state;

	val = cscm_ef_exec(s->val_ef, env);

	cscm_env_set_var(env, s->var, val);


	return NULL;
}


CSCM_EF *cscm_analyze_assignment(CSCM_AST_NODE *exp)
{
	CSCM_AST_NODE *var, *val;

	char *var_text;
	CSCM_EF *val_ef;
	CSCM_ASSIGNMENT_EF_STATE *state;


	var = cscm_ast_exp_index(exp, 1);
	val = cscm_ast_exp_index(exp, 2);


	var_text = cscm_text_cpy(var->text);
	val_ef = cscm_analyze(val);


	state = _cscm_assignment_ef_state_create();
	state->var = var_text;
	state->val_ef = val_ef;


	return cscm_ef_construct(CSCM_EF_TYPE_ASSIGNMENT,	\
				state,				\
				_cscm_assignment_ef);
}




void cscm_assignment_ef_free(CSCM_EF *ef)
{
	CSCM_ASSIGNMENT_EF_STATE *state;


	if (ef == NULL)
		cscm_error_report("cscm_assignment_ef_free", \
				CSCM_ERROR_NULL_PTR);
	else if (ef->type != CSCM_EF_TYPE_ASSIGNMENT)
		cscm_error_report("cscm_assignment_ef_free", \
				CSCM_ERROR_EF_TYPE);


	state = (CSCM_ASSIGNMENT_EF_STATE *)ef->state;

	free(state->var);
	cscm_ef_free_tree(state->val_ef);

	free(state);


	free(ef);
}
