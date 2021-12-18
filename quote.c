/* quote.c -- scheme quote expression(syntactic sugar)

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
#include "text.h"
#include "core.h"
#include "ef.h"
#include "quote.h"




int cscm_is_quote(CSCM_AST_NODE *exp)
{
	CSCM_AST_NODE *head;


	if (exp == NULL)
		cscm_error_report("cscm_is_quote", \
				CSCM_ERROR_NULL_PTR);
	else if (!cscm_ast_is_exp(exp))
		return 0;
	else if (cscm_ast_is_exp_empty(exp))
		return 0;


	head = cscm_ast_exp_index(exp, 0);
	if (!cscm_ast_is_symbol(head))
		return 0;
	else if (!cscm_ast_symbol_text_equal(head, "quote"))
		return 0;


	if (exp->n_childs != 2)
		cscm_syntax_error_report(exp->filename,		\
					exp->line,		\
					CSCM_ERROR_QUOTE_N_CLAUSES);
	else
		return 1;
}




void _cscm_quote_clause_to_slist(CSCM_AST_NODE *node)
{
	int i;

	CSCM_AST_NODE *head;


	if (cscm_ast_is_symbol(node)) {
		cscm_ast_symbol_set_simple(node, \
				cscm_text_cpy_add_squote(node->text));
	} else if (cscm_ast_is_exp(node)) {
		for (i = 0; i < node->n_childs; i++)
			_cscm_quote_clause_to_slist( \
					cscm_ast_exp_index(node, i));


		head = cscm_ast_symbol_create("transformation", 0);
		cscm_ast_symbol_set(head, "list");

		cscm_ast_exp_insert_first(node, head);
	} else {
		cscm_error_report("_cscm_quote_clause_to_slist", \
				CSCM_ERROR_AST_NODE_TYPE);
	}
}


CSCM_EF *cscm_analyze_quote(CSCM_AST_NODE *exp)
{
	CSCM_AST_NODE *clause;


	clause = cscm_ast_exp_index(exp, 1);

	_cscm_quote_clause_to_slist(clause);


	return cscm_analyze(clause);
}
