/* if.c -- scheme if expression(special form)

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
#include "core.h"
#include "bool.h"
#include "tco.h"
#include "if.h"




int cscm_is_if(CSCM_AST_NODE *exp)
{
	CSCM_AST_NODE *head;


	if (exp == NULL)
		cscm_error_report("cscm_is_if", \
				CSCM_ERROR_NULL_PTR);
	else if (exp->type != CSCM_AST_NODE_TYPE_EXP)
		return 0;
	else if (cscm_ast_is_exp_empty(exp))
		return 0;


	head = cscm_ast_exp_index(exp, 0);
	if (!cscm_ast_is_symbol(head))
		return 0;
	else if (!cscm_ast_symbol_text_equal(head, "if"))
		return 0;


	if (exp->n_childs != 3 && exp->n_childs != 4)
		cscm_syntax_error_report(exp->filename,	\
				exp->line,		\
				CSCM_ERROR_IF_INCOMPLETE);


	return 1;
}




CSCM_IF_EF_STATE *_cscm_if_ef_state_create()
{
	CSCM_IF_EF_STATE *state;


	state = malloc(sizeof(CSCM_IF_EF_STATE));
	if (state == NULL)
		cscm_libc_fail("_cscm_if_ef_state_create", \
				"malloc");


	state->predicate_ef = NULL;
	state->consequent_ef = NULL;
	state->alternative_ef = NULL;


	return state;
}


CSCM_OBJECT *_cscm_if_ef(void *state, CSCM_OBJECT *env)
{
	CSCM_IF_EF_STATE *s;
	int flag_tco_allow;

	CSCM_OBJECT *result;

	CSCM_EF *ef;
	CSCM_OBJECT *ret;


	s = (CSCM_IF_EF_STATE *)state;


	flag_tco_allow = cscm_tco_get_flag(CSCM_TCO_FLAG_ALLOW);


	cscm_tco_unset_flag(CSCM_TCO_FLAG_ALLOW);
	result = cscm_ef_exec(s->predicate_ef, env);


	if (flag_tco_allow) // restore the original value of the flag
		cscm_tco_set_flag(CSCM_TCO_FLAG_ALLOW);

	if (result == CSCM_FALSE) {
		if (s->alternative_ef) {
			ef = s->alternative_ef;

			if (ef->type != CSCM_EF_TYPE_COMBINATION \
				&& ef->type != CSCM_EF_TYPE_IF)
				cscm_tco_unset_flag(CSCM_TCO_FLAG_ALLOW);

			ret = cscm_ef_exec(ef, env);
		} else { // an if expression without the alternative expression
			ret = NULL;
		}
	} else {
		ef = s->consequent_ef;

		if (ef->type != CSCM_EF_TYPE_COMBINATION \
			&& ef->type != CSCM_EF_TYPE_IF)
			cscm_tco_unset_flag(CSCM_TCO_FLAG_ALLOW);

		ret = cscm_ef_exec(ef, env);
	}


	return ret;
}


CSCM_EF *cscm_analyze_if(CSCM_AST_NODE *exp)
{
	CSCM_IF_EF_STATE *state;

	CSCM_AST_NODE *predicate, *consequent, *alternative;


	predicate = cscm_ast_exp_index(exp, 1);
	consequent = cscm_ast_exp_index(exp, 2);

	state = _cscm_if_ef_state_create();

	state->predicate_ef = cscm_analyze(predicate);
	state->consequent_ef = cscm_analyze(consequent);


	if (exp->n_childs == 4) {
		alternative = cscm_ast_exp_index(exp, 3);
		state->alternative_ef = cscm_analyze(alternative);
	} else { // an if expression without the alternative expression
		state->alternative_ef = NULL;
	}


	return cscm_ef_construct(CSCM_EF_TYPE_IF,	\
			state,				\
			_cscm_if_ef);
}




void cscm_if_ef_free(CSCM_EF *ef)
{
	CSCM_IF_EF_STATE *state;


	if (ef == NULL)
		cscm_error_report("cscm_if_ef_free", \
				CSCM_ERROR_NULL_PTR);
	else if (ef->type != CSCM_EF_TYPE_IF)
		cscm_error_report("cscm_if_ef_free", \
				CSCM_ERROR_EF_TYPE);


	state = (CSCM_IF_EF_STATE *)ef->state;


	cscm_ef_free_tree(state->predicate_ef);

	cscm_ef_free_tree(state->consequent_ef);

	if (state->alternative_ef)
		cscm_ef_free_tree(state->alternative_ef);


	free(state);


	free(ef);
}
