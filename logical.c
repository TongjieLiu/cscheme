/* logical.c -- scheme logical composition operations(special form)

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
#include "core.h"
#include "bool.h"
#include "gc.h"
#include "logical.h"




int cscm_is_ao(CSCM_AST_NODE *exp)
{
	CSCM_AST_NODE *head;


	if (exp == NULL)
		cscm_error_report("cscm_is_ao", \
				CSCM_ERROR_NULL_PTR);
	else if (!cscm_ast_is_exp(exp))
		return 0;
	else if (cscm_ast_is_exp_empty(exp))
		return 0;


	head = cscm_ast_exp_index(exp, 0);
	if (!cscm_ast_is_symbol(head))
		return 0;
	else if (!cscm_ast_symbol_text_equal(head, "and") \
		&& !cscm_ast_symbol_text_equal(head, "or"))
		return 0;


	if (exp->n_childs == 1) // do not support empty and/or expression
		cscm_syntax_error_report(exp->filename,		\
				exp->line,			\
				CSCM_ERROR_AO_INCOMPLETE);


	return 1;
}




CSCM_AO_EF_STATE *_cscm_ao_ef_state_create()
{
	CSCM_AO_EF_STATE *state;


	state = malloc(sizeof(CSCM_AO_EF_STATE));
	if (state == NULL)
		cscm_libc_fail("_cscm_ao_ef_state_create", \
				"malloc");


	state->n_clause_efs = 0;
	state->clause_efs = NULL;


	return state;
}


CSCM_OBJECT *_cscm_and_ef(void *state, CSCM_OBJECT *env)
{
	int i, end;
	CSCM_EF *ef;
	CSCM_OBJECT *result;

	CSCM_AO_EF_STATE *s;


	s = (CSCM_AO_EF_STATE *)state;

	end = s->n_clause_efs - 1;
	for (i = 0; i < end; i++) {
		ef = s->clause_efs[i];
		result = cscm_ef_exec(ef, env);

		if (result == CSCM_FALSE) {
			return CSCM_FALSE;
		} else {
			if (result)
				cscm_gc_free(result);
		}
	}


	ef = s->clause_efs[i];
	result = cscm_ef_exec(ef, env);

	return result; // result of the last clause
}


CSCM_OBJECT *_cscm_or_ef(void *state, CSCM_OBJECT *env)
{
	int i;
	CSCM_EF *ef;
	CSCM_OBJECT *result;

	CSCM_AO_EF_STATE *s;


	s = (CSCM_AO_EF_STATE *)state;

	for (i = 0; i < s->n_clause_efs; i++) {
		ef = s->clause_efs[i];
		result = cscm_ef_exec(ef, env);

		if (result != CSCM_FALSE)
			return result; // result of the first true clause
	}


	return CSCM_FALSE; // all clauses are false
}


CSCM_EF *cscm_analyze_ao(CSCM_AST_NODE *exp)
{
	int i;
	CSCM_EF *ef;
	CSCM_AST_NODE *clause;

	CSCM_AO_EF_STATE *state;
	CSCM_EF **clause_efs;

	CSCM_AST_NODE *type;


	state = _cscm_ao_ef_state_create();
	state->n_clause_efs = exp->n_childs - 1;

	clause_efs = cscm_ef_ptrs_create(state->n_clause_efs);
	state->clause_efs = clause_efs;


	for (i = 1; i < exp->n_childs; i++) {
		clause = cscm_ast_exp_index(exp, i);
		ef = cscm_analyze(clause);

		clause_efs[i - 1] = ef;
	}


	type = cscm_ast_exp_index(exp, 0);
	if (cscm_ast_symbol_text_equal(type, "and"))
		return cscm_ef_construct(CSCM_EF_TYPE_AO,	\
					state,			\
					exp,			\
					_cscm_and_ef);
	else
		return cscm_ef_construct(CSCM_EF_TYPE_AO,	\
					state,			\
					exp,			\
					_cscm_or_ef);
}




void cscm_ao_ef_free(CSCM_EF *ef)
{
	int i;

	CSCM_AO_EF_STATE *state;


	if (ef == NULL)
		cscm_error_report("cscm_ao_ef_free", \
				CSCM_ERROR_NULL_PTR);
	else if (ef->type != CSCM_EF_TYPE_AO)
		cscm_error_report("cscm_ao_ef_free", \
				CSCM_ERROR_EF_TYPE);


	state = (CSCM_AO_EF_STATE *)ef->state;

	for (i = 0; i < state->n_clause_efs; i++)
		cscm_ef_free_tree(state->clause_efs[i]);

	free(state->clause_efs);

	free(state);


	free(ef);
}
