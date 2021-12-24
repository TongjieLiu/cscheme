/* begin.c -- scheme begin expression(special form)

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
#include "gc.h"
#include "core.h"
#include "tco.h"
#include "begin.h"




int cscm_is_begin(CSCM_AST_NODE *exp)
{
	CSCM_AST_NODE *head;


	if (exp == NULL)
		cscm_error_report("cscm_is_begin", \
				CSCM_ERROR_NULL_PTR);
	else if (!cscm_ast_is_exp(exp))
		return 0;
	else if (cscm_ast_is_exp_empty(exp))
		return 0;


	head = cscm_ast_exp_index(exp, 0);
	if (!cscm_ast_is_symbol(head))
		return 0;
	else if (!cscm_ast_symbol_text_equal(head, "begin"))
		return 0;


	if (exp->n_childs < 2) // do not support empty begin expression
		cscm_syntax_error_report(exp->filename,		\
					exp->line,		\
					CSCM_ERROR_BEGIN_EMPTY);
	else
		return 1;
}




CSCM_EF *cscm_analyze_begin(CSCM_AST_NODE *exp)
{
	cscm_ast_exp_drop_first(exp);

	return cscm_analyze_seq(exp);
}




CSCM_SEQ_EF_STATE *_cscm_seq_ef_state_create()
{
	CSCM_SEQ_EF_STATE *state;


	state = malloc(sizeof(CSCM_SEQ_EF_STATE));
	if (state == NULL)
		cscm_libc_fail("cscm_seq_ef_state_create", "malloc");


	state->n_clause_efs = 0;
	state->clause_efs = NULL;


	return state;
}


CSCM_OBJECT *_cscm_seq_ef(void *state, CSCM_OBJECT *env)
{
	int i, end;

	CSCM_SEQ_EF_STATE *s;
	int flag_tco_allow;

	CSCM_EF *last_clause_ef;
	CSCM_OBJECT *ret;


	s = (CSCM_SEQ_EF_STATE *)state;


	flag_tco_allow = cscm_tco_get_flag(CSCM_TCO_FLAG_ALLOW);
	cscm_tco_unset_flag(CSCM_TCO_FLAG_ALLOW);


	end = s->n_clause_efs - 1;
	for (i = 0; i < end; i++) {
		ret = cscm_ef_exec(s->clause_efs[i], env);

		if (ret)
			cscm_gc_free(ret);
	}


	last_clause_ef = s->clause_efs[i];


	if (flag_tco_allow) // restore the original value of the flag
		cscm_tco_set_flag(CSCM_TCO_FLAG_ALLOW);

	if (last_clause_ef->type != CSCM_EF_TYPE_COMBINATION \
		&& last_clause_ef->type != CSCM_EF_TYPE_IF)
		cscm_tco_unset_flag(CSCM_TCO_FLAG_ALLOW);


	ret = cscm_ef_exec(last_clause_ef, env);


	return ret; // return the value of the last clause in the sequence
}


CSCM_EF *cscm_analyze_seq(CSCM_AST_NODE *exp)
{
	int i;
	CSCM_EF **clause_efs;
	CSCM_SEQ_EF_STATE *state;


	if (exp == NULL)
		cscm_error_report("cscm_analyze_seq", \
				CSCM_ERROR_NULL_PTR);
	else if (!cscm_ast_is_exp(exp))
		cscm_error_report("cscm_analyze_seq", \
				CSCM_ERROR_AST_NODE_TYPE);
	else if (cscm_ast_is_exp_empty(exp)) // do not support empty sequence
		cscm_syntax_error_report(exp->filename,	\
					exp->line,		\
					CSCM_ERROR_SEQ_EMPTY);


	clause_efs = cscm_ef_ptrs_create(exp->n_childs);


	for (i = 0; i < exp->n_childs; i++)
		clause_efs[i] = cscm_analyze(cscm_ast_exp_index(exp, i));


	state = _cscm_seq_ef_state_create();


	state->n_clause_efs = exp->n_childs;
	state->clause_efs = clause_efs;


	return cscm_ef_construct(CSCM_EF_TYPE_SEQ,	\
				state,			\
				_cscm_seq_ef);
}




void cscm_seq_ef_free(CSCM_EF *ef)
{
	int i;
	CSCM_SEQ_EF_STATE *state;


	if (ef == NULL)
		cscm_error_report("cscm_seq_ef_free", \
				CSCM_ERROR_NULL_PTR);
	else if (ef->type != CSCM_EF_TYPE_SEQ)
		cscm_error_report("cscm_seq_ef_free", \
				CSCM_ERROR_EF_TYPE);


	state = (CSCM_SEQ_EF_STATE *)ef->state;


	for (i = 0; i < state->n_clause_efs; i++)
		cscm_ef_free_tree(state->clause_efs[i]);


	free(state->clause_efs);

	free(state);

	free(ef);
}
