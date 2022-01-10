/* let.c -- scheme let expression(syntactic sugar)

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

#include <stddef.h>

#include "error.h"
#include "ast.h"
#include "ef.h"
#include "core.h"
#include "symbol.h"
#include "num.h"
#include "str.h"
#include "var.h"
#include "let.h"




int cscm_is_let(CSCM_AST_NODE *exp)
{
	int i;

	CSCM_AST_NODE *head;

	CSCM_AST_NODE *var, *binding, *bindings;


	if (exp == NULL)
		cscm_error_report("cscm_is_let", \
				CSCM_ERROR_NULL_PTR);
	else if (!cscm_ast_is_exp(exp))
		return 0;
	else if (cscm_ast_is_exp_empty(exp))
		return 0;


	head = cscm_ast_exp_index(exp, 0);
	if (!cscm_ast_is_symbol(head))
		return 0;
	else if (!cscm_ast_symbol_text_equal(head, "let"))
		return 0;


	if (exp->n_childs == 1)
		cscm_syntax_error_report(exp->filename,	\
				exp->line,		\
				CSCM_ERROR_LET_NO_BINDING);

	bindings = cscm_ast_exp_index(exp, 1);
	if (!cscm_ast_is_exp(bindings))
		cscm_syntax_error_report(bindings->filename,	\
				bindings->line,			\
				CSCM_ERROR_LET_BAD_BINDINGS);
	else if (cscm_ast_is_exp_empty(bindings)) // do not support no binding
		cscm_syntax_error_report(bindings->filename,	\
				bindings->line,			\
				CSCM_ERROR_LET_NO_BINDING);

	if (exp->n_childs == 2) // do not support empty body
		cscm_syntax_error_report(exp->filename,	\
				exp->line,			\
				CSCM_ERROR_LET_EMPTY_BODY);


	for (i = 0; i < bindings->n_childs; i++) {
		binding = cscm_ast_exp_index(bindings, i);

		if (!cscm_ast_is_exp(binding))
			cscm_syntax_error_report(binding->filename,	\
					binding->line,			\
					CSCM_ERROR_LET_BAD_BINDING);
		else if (binding->n_childs != 2)
			cscm_syntax_error_report(binding->filename,	\
					binding->line,			\
					CSCM_ERROR_LET_BAD_BINDING);

		var = cscm_ast_exp_index(binding, 0);
		if (cscm_is_num_long(var)		\
			|| cscm_is_num_double(var)	\
			|| cscm_is_string(var))
			cscm_syntax_error_report(var->filename,		\
					var->line,			\
					CSCM_ERROR_LET_BAD_BINDING);
		else if (!cscm_is_var(var))
			cscm_syntax_error_report(var->filename,		\
					var->line,			\
					CSCM_ERROR_LET_BAD_BINDING);
	}


	return 1;
}




CSCM_EF *cscm_analyze_let(CSCM_AST_NODE *exp)
{
	int i;

	CSCM_AST_NODE *binding, *bindings;
	CSCM_AST_NODE *param, *arg;

	CSCM_AST_NODE *proc, *lambda, *params;
	CSCM_AST_NODE *combination, *args;


	cscm_ast_free_symbol(cscm_ast_exp_index(exp, 0)); // symbol: "let"


	params = cscm_ast_exp_create("<transformation>", 0);
	args = cscm_ast_exp_create("<transformation>", 0);

	bindings = cscm_ast_exp_index(exp, 1);
	for (i = 0; i < bindings->n_childs; i++) {
		binding = cscm_ast_exp_index(bindings, i);

		param = cscm_ast_exp_index(binding, 0);
		arg = cscm_ast_exp_index(binding, 1);

		cscm_ast_exp_append(params, param);
		cscm_ast_exp_append(args, arg);


		cscm_ast_free_exp(binding);
	}

	cscm_ast_free_exp(bindings);


	/* constructing the new lambda expression */
	proc = cscm_ast_exp_create("<transformation>", 0);

	lambda = cscm_ast_symbol_create("<transformation>", 0);
	cscm_ast_symbol_set(lambda, "lambda");

	cscm_ast_exp_append(proc, lambda);
	cscm_ast_exp_append(proc, params);

	for (i = 2; i < exp->n_childs; i++) // append body
		cscm_ast_exp_append(proc, cscm_ast_exp_index(exp, i));


	/* constructing the new combination */
	cscm_ast_exp_insert_first(args, proc);
	combination = args;


	/*	The reason of moving combination to exp is that
	 * we'll free the original ast tree after analyzing the
	 * entire script and this recursive process will try to
	 * free only some parts of the newly created combination
	 * but not all if we don't move. */
	cscm_ast_exp_mv(combination, exp);


	return cscm_analyze_combination(exp);
}
