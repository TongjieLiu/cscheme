/* cond.c -- scheme cond expression(syntactic sugar)

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

#include "error.h"
#include "ast.h"
#include "ef.h"
#include "if.h"
#include "cond.h"




int cscm_is_cond(CSCM_AST_NODE *exp)
{
	int i;

	CSCM_AST_NODE *head;

	CSCM_AST_NODE *clause;
	CSCM_AST_NODE *predicate;


	if (exp == NULL)
		cscm_error_report("cscm_is_cond", \
				CSCM_ERROR_NULL_PTR);
	else if (!cscm_ast_is_exp(exp))
		return 0;
	else if (cscm_ast_is_exp_empty(exp))
		return 0;


	head = cscm_ast_exp_index(exp, 0);
	if (!cscm_ast_is_symbol(head))
		return 0;
	else if (!cscm_ast_symbol_text_equal(head, "cond"))
		return 0;


	if (exp->n_childs == 1)
		cscm_syntax_error_report(exp->filename,	\
				exp->line,		\
				CSCM_ERROR_COND_NO_CLAUSE);


	for (i = 1; i < exp->n_childs; i++) {
		clause = cscm_ast_exp_index(exp, i);

		if (clause->n_childs != 2)
			cscm_syntax_error_report(clause->filename,	\
					clause->line,			\
					CSCM_ERROR_COND_BAD_CLAUSE);

		predicate = cscm_ast_exp_index(clause, 0);

		/* check whether the else clause is the last or not */
		if (cscm_ast_is_symbol(predicate)                        \
			&& cscm_ast_symbol_text_equal(predicate, "else") \
			&& i < (exp->n_childs - 1))
			cscm_syntax_error_report(predicate->filename,	\
						predicate->line,	\
						CSCM_ERROR_COND_BAD_ELSE);
	}


	return 1;
}




CSCM_AST_NODE *_cscm_cond_expand(CSCM_AST_NODE *clauses, \
				size_t index, size_t last)
{
	CSCM_AST_NODE *clause;
	CSCM_AST_NODE *predicate, *consequent;

	CSCM_AST_NODE *if_exp;
	CSCM_AST_NODE *if_symbol;


	clause = cscm_ast_exp_index(clauses, index);

	predicate = cscm_ast_exp_index(clause, 0);
	consequent = cscm_ast_exp_index(clause, 1);

	cscm_ast_free_exp(clause);


	if (cscm_ast_is_symbol(predicate) \
		&& cscm_ast_symbol_text_equal(predicate, "else")) {
		return consequent;
	} else {
		if_exp = cscm_ast_exp_create("<transformation>", 0);

		if_symbol = cscm_ast_symbol_create("<transformation>", 0);
		cscm_ast_symbol_set(if_symbol, "if");

		cscm_ast_exp_append(if_exp, if_symbol);
		cscm_ast_exp_append(if_exp, predicate);
		cscm_ast_exp_append(if_exp, consequent);


		if (index < last)
			cscm_ast_exp_append(if_exp, \
				_cscm_cond_expand(clauses, index + 1, last));


		return if_exp;
	}
}


CSCM_EF *cscm_analyze_cond(CSCM_AST_NODE *exp)
{
	CSCM_AST_NODE *if_exp;


	cscm_ast_exp_drop_first(exp);

	if_exp = _cscm_cond_expand(exp, 0, exp->n_childs - 1);

	cscm_ast_exp_mv(if_exp, exp);


	return cscm_analyze_if(exp);
}
