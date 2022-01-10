/* definition.c -- scheme definition(special form)

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

#include <stdlib.h>

#include "error.h"
#include "ast.h"
#include "object.h"
#include "ef.h"
#include "env.h"
#include "text.h"
#include "core.h"
#include "lambda.h"
#include "num.h"
#include "str.h"
#include "var.h"
#include "definition.h"




int cscm_is_definition(CSCM_AST_NODE *exp)
{
	int i;

	CSCM_AST_NODE *head;

	CSCM_AST_NODE *var;
	CSCM_AST_NODE *proc_part;


	if (exp == NULL)
		cscm_error_report("cscm_is_definition", \
				CSCM_ERROR_NULL_PTR);
	else if (!cscm_ast_is_exp(exp))
		return 0;
	else if (cscm_ast_is_exp_empty(exp))
		return 0;


	head = cscm_ast_exp_index(exp, 0);
	if (!cscm_ast_is_symbol(head))
		return 0;
	else if (!cscm_ast_symbol_text_equal(head, "define"))
		return 0;


	if (exp->n_childs == 1)
		cscm_syntax_error_report(exp->filename,		\
				exp->line,			\
				CSCM_ERROR_DEFINITION_NO_VAR);


	var = cscm_ast_exp_index(exp, 1);
	if (cscm_ast_is_exp(var)) {	// define a new compound procedure
		if (cscm_ast_is_exp_empty(var))
			cscm_syntax_error_report(var->filename,	\
					var->line,		\
					CSCM_ERROR_DEFINITION_NO_PROC);

		for (i = 0; i < var->n_childs; i++) {
			proc_part = cscm_ast_exp_index(var, i);

			if (cscm_is_num_long(proc_part)			\
				|| cscm_is_num_double(proc_part)	\
				|| cscm_is_string(proc_part))
				cscm_syntax_error_report(var->filename,	\
						var->line,		\
						CSCM_ERROR_DEFINITION_BAD_VAR);
			else if (!cscm_is_var(proc_part))
				cscm_syntax_error_report(var->filename,	\
						var->line,		\
						CSCM_ERROR_DEFINITION_BAD_VAR);
		}


		if (exp->n_childs == 2) // do not support empty body
			cscm_syntax_error_report(exp->filename,		\
					exp->line,			\
					CSCM_ERROR_DEFINITION_EMPTY_BODY);
	} else {			// define a new variable
		if (cscm_is_num_long(var)		\
			|| cscm_is_num_double(var)	\
			|| cscm_is_string(var))
			cscm_syntax_error_report(var->filename,		\
					var->line,			\
					CSCM_ERROR_DEFINITION_BAD_VAR);
		else if (!cscm_is_var(var))
			cscm_syntax_error_report(var->filename,		\
					var->line,			\
					CSCM_ERROR_DEFINITION_BAD_VAR);


		if (exp->n_childs == 2)
			cscm_syntax_error_report(exp->filename,		\
					exp->line,			\
					CSCM_ERROR_DEFINITION_NO_VAL);
		else if (exp->n_childs > 3) // multiple value to set
			cscm_syntax_error_report(exp->filename,		\
					exp->line,			\
					CSCM_ERROR_DEFINITION_BAD_VAL);
	}


	return 1;
}




CSCM_DEFINITION_EF_STATE *_cscm_definition_ef_state_create()
{
	CSCM_DEFINITION_EF_STATE *state;


	state = malloc(sizeof(CSCM_DEFINITION_EF_STATE));
	if (state == NULL)
		cscm_libc_fail("_cscm_defintion_ef_state_create", \
				"malloc");


	state->var = NULL;
	state->val_ef = NULL;


	return state;
}


CSCM_OBJECT *_cscm_definition_ef(void *state, CSCM_OBJECT *env)
{
	CSCM_OBJECT *val;
	CSCM_DEFINITION_EF_STATE *s;


	s = (CSCM_DEFINITION_EF_STATE *)state;

	val = cscm_ef_exec(s->val_ef, env);

	cscm_env_add_var(env, s->var, val);


	return NULL;
}


CSCM_EF *cscm_analyze_definition(CSCM_AST_NODE *exp)
{
	int i;

	CSCM_AST_NODE *var, *val;
	CSCM_AST_NODE *proc, *head;

	CSCM_AST_NODE *lambda, *new_lambda_exp, *new_var;

	char *var_text;
	CSCM_EF *val_ef;
	CSCM_DEFINITION_EF_STATE *state;


	var = cscm_ast_exp_index(exp, 1);

	if (cscm_ast_is_exp(var)) {	// define a new compound procedure
		proc = cscm_ast_exp_index(var, 0);
		new_var = cscm_ast_symbol_create("<transformation>", 0);
		cscm_ast_symbol_set(new_var, proc->text);
		cscm_ast_exp_drop_first(var);

		lambda = cscm_ast_symbol_create("<transformation>", 0);
		cscm_ast_symbol_set(lambda, "lambda");

		new_lambda_exp = cscm_ast_exp_create("<transformation>", 0);
		cscm_ast_exp_append(new_lambda_exp, lambda);
		for (i = 1; i < exp->n_childs; i++)
			cscm_ast_exp_append(new_lambda_exp, \
					cscm_ast_exp_index(exp, i));

		head = cscm_ast_exp_index(exp, 0);
		cscm_ast_exp_empty(exp);
		cscm_ast_exp_append(exp, head);
		cscm_ast_exp_append(exp, new_var);
		cscm_ast_exp_append(exp, new_lambda_exp);

		/* Now, exp represents a lambda expression */
		val_ef = cscm_analyze_lambda(new_lambda_exp);
		var_text = cscm_text_cpy(new_var->text);
	} else {			// define a new variable
		var_text = cscm_text_cpy(var->text);

		val = cscm_ast_exp_index(exp, 2);
		val_ef = cscm_analyze(val);
	}


	state = _cscm_definition_ef_state_create();
	state->var = var_text;
	state->val_ef = val_ef;


	return cscm_ef_construct(CSCM_EF_TYPE_DEFINITION,	\
				state,				\
				exp,				\
				_cscm_definition_ef);
}




void cscm_definition_ef_free(CSCM_EF *ef)
{
	CSCM_DEFINITION_EF_STATE *state;


	if (ef == NULL)
		cscm_error_report("cscm_definition_ef_free", \
				CSCM_ERROR_NULL_PTR);
	else if (ef->type != CSCM_EF_TYPE_DEFINITION)
		cscm_error_report("cscm_definition_ef_free", \
				CSCM_ERROR_EF_TYPE);


	state = (CSCM_DEFINITION_EF_STATE *)ef->state;

	free(state->var);
	cscm_ef_free_tree(state->val_ef);

	free(state);


	free(ef);
}
