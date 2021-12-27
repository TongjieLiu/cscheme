/* quasiquote.c -- scheme quasiquote expression(special form)

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
#include <stdlib.h>

#include "error.h"
#include "ast.h"
#include "core.h"
#include "ef.h"
#include "object.h"
#include "symbol.h"
#include "num.h"
#include "str.h"
#include "pair.h"
#include "quasiquote.h"




int cscm_is_quasiquote(CSCM_AST_NODE *exp)
{
	CSCM_AST_NODE *head;


	if (exp == NULL)
		cscm_error_report("cscm_is_quasiquote", \
				CSCM_ERROR_NULL_PTR);
	else if (!cscm_ast_is_exp(exp))
		return 0;
	else if (cscm_ast_is_exp_empty(exp))
		return 0;


	head = cscm_ast_exp_index(exp, 0);
	if (!cscm_ast_is_symbol(head))
		return 0;
	else if (!cscm_ast_symbol_text_equal(head, "quasiquote"))
		return 0;


	if (exp->n_childs != 2)
		cscm_syntax_error_report(exp->filename,		\
					exp->line,		\
					CSCM_ERROR_QUASIQUOTE_N_CLAUSES);
	else
		return 1;
}




CSCM_QUASIQUOTE_EF_STATE *_cscm_quasiquote_ef_state_create()
{
	CSCM_QUASIQUOTE_EF_STATE *state;


	state = malloc(sizeof(CSCM_QUASIQUOTE_EF_STATE));
	if (state == NULL)
		cscm_libc_fail("cscm_quasiquote_ef_state_create", \
				"malloc");


	state->n_efs = 0;
	state->efs = NULL;


	return state;
}


CSCM_OBJECT *_cscm_quasiquote_ef(void *state, CSCM_OBJECT *env)
{
	int i;

	CSCM_QUASIQUOTE_EF_STATE *s;

	CSCM_OBJECT **objs;
	CSCM_OBJECT *ret;


	s = (CSCM_QUASIQUOTE_EF_STATE *)state;


	if (s->n_efs == 0) {
		ret = CSCM_NIL;
	} else {
		objs = cscm_object_ptrs_create(s->n_efs);

		for (i = 0; i < s->n_efs; i++)
			objs[i] = cscm_ef_exec(s->efs[i], env);

		ret = cscm_list_create(s->n_efs, objs);

		free(objs);
	}


	return ret;
}


int _cscm_quasiquote_is_unquote(CSCM_AST_NODE *exp)
{
	CSCM_AST_NODE *head;


	if (cscm_ast_is_exp_empty(exp))
		return 0;


	head = cscm_ast_exp_index(exp, 0);
	if (!cscm_ast_is_symbol(head))
		return 0;
	else if (!cscm_ast_symbol_text_equal(head, "unquote"))
		return 0;


	if (exp->n_childs != 2)
		cscm_syntax_error_report(exp->filename,		\
					exp->line,		\
					CSCM_ERROR_UNQUOTE_N_CLAUSES);
	else
		return 1;
}


CSCM_EF *_do_cscm_analyze_quasiquote(CSCM_AST_NODE *node)
{
	int i;
	CSCM_AST_NODE *clause, *child;

	CSCM_QUASIQUOTE_EF_STATE *state;


	if (node == NULL) {
		cscm_error_report("_do_cscm_analyze_quasiquote", \
				CSCM_ERROR_NULL_PTR);
	} else if (cscm_ast_is_symbol(node)) {
		if (cscm_is_num_long(node))
			return cscm_analyze_num_long(node);
		else if (cscm_is_num_double(node))
			return cscm_analyze_num_double(node);
		else if (cscm_is_string(node))
			return cscm_analyze_string(node);
		else
			return cscm_analyze_symbol(node);
	} else if (cscm_ast_is_exp(node)) {
		if (_cscm_quasiquote_is_unquote(node)) {
			clause = cscm_ast_exp_index(node, 1);

			return cscm_analyze(clause);
		} else {
			state = _cscm_quasiquote_ef_state_create();

			state->n_efs = node->n_childs;

			if (node->n_childs == 0) {
				state->efs = NULL;
			} else {
				state->efs = cscm_ef_ptrs_create(node->n_childs);

				for (i = 0; i < node->n_childs; i++) {
					child = cscm_ast_exp_index(node, i);
					state->efs[i] = \
					    _do_cscm_analyze_quasiquote(child);
				}
			}


			return cscm_ef_construct(CSCM_EF_TYPE_QUASIQUOTE, \
						state,                    \
						_cscm_quasiquote_ef);
		}
	} else {
		cscm_error_report("_do_cscm_analyze_quasiquote", \
				CSCM_ERROR_AST_NODE_TYPE);
	}
}


CSCM_EF *cscm_analyze_quasiquote(CSCM_AST_NODE *exp)
{
	CSCM_AST_NODE *clause;


	clause = cscm_ast_exp_index(exp, 1);
	return _do_cscm_analyze_quasiquote(clause);
}




void cscm_quasiquote_ef_free(CSCM_EF *ef)
{
	int i;

	CSCM_QUASIQUOTE_EF_STATE *state;


	if (ef == NULL)
		cscm_error_report("cscm_quasiquote_ef_free", \
				CSCM_ERROR_NULL_PTR);
	else if (ef->type != CSCM_EF_TYPE_QUASIQUOTE)
		cscm_error_report("cscm_quasiquote_ef_free", \
				CSCM_ERROR_EF_TYPE);


	state = (CSCM_QUASIQUOTE_EF_STATE *)ef->state;


	for (i = 0; i < state->n_efs; i++)
		cscm_ef_free_tree(state->efs[i]);

	if (state->efs)
		free(state->efs);

	free(state);


	free(ef);
}
