/* lambda.c -- scheme lambda expression(special form)

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
#include "begin.h"
#include "text.h"
#include "proc.h"
#include "num.h"
#include "str.h"
#include "var.h"
#include "lambda.h"




int cscm_is_lambda(CSCM_AST_NODE *exp)
{
	int i;

	CSCM_AST_NODE *head;

	CSCM_AST_NODE *param, *params;


	if (exp == NULL)
		cscm_error_report("cscm_is_lambda", \
				CSCM_ERROR_NULL_PTR);
	else if (!cscm_ast_is_exp(exp))
		return 0;
	else if (cscm_ast_is_exp_empty(exp))
		return 0;


	head = cscm_ast_exp_index(exp, 0);
	if (!cscm_ast_is_symbol(head))
		return 0;
	else if (!cscm_ast_symbol_text_equal(head, "lambda"))
		return 0;


	if (exp->n_childs == 1)
		cscm_syntax_error_report(exp->filename,		\
				exp->line,			\
				CSCM_ERROR_LAMBDA_NO_PARAM);

	params = cscm_ast_exp_index(exp, 1);
	if (!cscm_ast_is_exp(params))
		cscm_syntax_error_report(params->filename,	\
				params->line,			\
				CSCM_ERROR_LAMBDA_BAD_PARAMS);

	if (exp->n_childs == 2) // do not support empty body
		cscm_syntax_error_report(exp->filename,		\
				exp->line,			\
				CSCM_ERROR_LAMBDA_EMPTY_BODY);


	for (i = 0; i < params->n_childs; i++) { // support no formal parameter
		param = cscm_ast_exp_index(params, i);

		if (cscm_is_num_long(param)		\
			|| cscm_is_num_double(param)	\
			|| cscm_is_string(param))
			cscm_syntax_error_report(param->filename,	\
					param->line,			\
					CSCM_ERROR_LAMBDA_BAD_PARAM);
		else if (!cscm_is_var(param))
			cscm_syntax_error_report(param->filename,	\
					param->line,			\
					CSCM_ERROR_LAMBDA_BAD_PARAM);
	}


	return 1;
}


CSCM_LAMBDA_EF_STATE *_cscm_lambda_ef_state_create()
{
	CSCM_LAMBDA_EF_STATE *state;


	state = malloc(sizeof(CSCM_LAMBDA_EF_STATE));
	if (state == NULL)
		cscm_libc_fail("_cscm_lambda_ef_state_create", "malloc");


	state->n_params = 0;
	state->params = NULL;

	state->body = NULL;


	return state;
}


CSCM_OBJECT *_cscm_lambda_ef(void *state, CSCM_OBJECT *env)
{
	CSCM_OBJECT *proc;

	CSCM_LAMBDA_EF_STATE *s;


	proc = cscm_proc_comp_create();

	s = (CSCM_LAMBDA_EF_STATE *)state;
	cscm_proc_comp_set(proc,	\
			s->n_params,	\
			s->params,	\
			s->body,	\
			env);


	return proc;
}


CSCM_EF *cscm_analyze_lambda(CSCM_AST_NODE *exp)
{
	int i;
	CSCM_AST_NODE *param, *params;

	CSCM_LAMBDA_EF_STATE *state;


	state = _cscm_lambda_ef_state_create();


	params = cscm_ast_exp_index(exp, 1);

	state->n_params = params->n_childs; 

	state->params = malloc(params->n_childs * sizeof(char *));
	if (state->params == NULL)
		cscm_libc_fail("cscm_analyze_lambda", "malloc");

	for (i = 0; i < params->n_childs; i++) {
		param = cscm_ast_exp_index(params, i);
		state->params[i] = cscm_text_cpy(param->text);
	}

	cscm_ast_exp_drop_first(exp); // drop "lambda"
	cscm_ast_exp_drop_first(exp); // drop formal parameters
	state->body = cscm_analyze_seq(exp);


	return cscm_ef_construct(CSCM_EF_TYPE_LAMBDA,	\
				state,			\
				_cscm_lambda_ef);
}




void cscm_lambda_ef_free(CSCM_EF *ef)
{
	int i;
	CSCM_LAMBDA_EF_STATE *state;


	if (ef == NULL)
		cscm_error_report("cscm_lambda_ef_free", \
				CSCM_ERROR_NULL_PTR);
	else if (ef->type != CSCM_EF_TYPE_LAMBDA)
		cscm_error_report("cscm_lambda_ef_free", \
				CSCM_ERROR_EF_TYPE);


	state = (CSCM_LAMBDA_EF_STATE *)ef->state;

	for (i = 0; i < state->n_params; i++)
		free(state->params[i]);

	free(state->params);

	cscm_ef_free_tree(state->body);


	free(state);

	free(ef);
}
