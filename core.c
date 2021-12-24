/* core.c -- syntactic analysis

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
#include "object.h"
#include "ast.h"
#include "ef.h"
#include "num.h"
#include "symbol.h"
#include "str.h"
#include "var.h"
#include "assignment.h"
#include "definition.h"
#include "begin.h"
#include "let.h"
#include "lambda.h"
#include "if.h"
#include "cond.h"
#include "logical.h"
#include "proc.h"
#include "quote.h"
#include "gc.h"
#include "env.h"
#include "tco.h"
#include "core.h"




CSCM_OBJECT *cscm_eval(CSCM_AST_NODE *exp, CSCM_OBJECT *env)
{
	CSCM_EF *ef;

	CSCM_OBJECT *ret;


	if (exp == NULL || env == NULL)
		cscm_error_report("cscm_eval", CSCM_ERROR_NULL_PTR);


	#ifdef __CSCM_GC_DEBUG__
		cscm_gc_show_total_object_count("ANALYZE");
	#endif
	ef = cscm_analyze(exp);


	#ifdef __CSCM_GC_DEBUG__
		cscm_gc_show_total_object_count("FREE-AST");
	#endif
	cscm_ast_free_tree(exp);


	#ifdef __CSCM_GC_DEBUG__
		cscm_gc_show_total_object_count("EXECUTE");
	#endif
	ret = cscm_ef_exec(ef, env);
	if (ret)
		cscm_gc_inc(ret); // try to save it from freeing all efs


	#ifdef __CSCM_GC_DEBUG__
		cscm_gc_show_total_object_count("FREE-ALL-EFS");
	#endif
	cscm_ef_free_tree(ef);


	return ret;
}




CSCM_SA_FUNCS _cscm_sa_func_list[] = {
	{0, cscm_is_num_long, cscm_analyze_num_long},
	{0, cscm_is_num_double, cscm_analyze_num_double},
	{0, cscm_is_symbol, cscm_analyze_symbol},
	{0, cscm_is_string, cscm_analyze_string},
	{0, cscm_is_var, cscm_analyze_var},


	{0, cscm_is_assignment, cscm_analyze_assignment},
	{0, cscm_is_definition, cscm_analyze_definition},
	{0, cscm_is_lambda, cscm_analyze_lambda},
	{0, cscm_is_if, cscm_analyze_if},
	{0, cscm_is_cond, cscm_analyze_cond},
	{0, cscm_is_begin, cscm_analyze_begin},
	{0, cscm_is_let, cscm_analyze_let},
	{0, cscm_is_ao, cscm_analyze_ao}, // and/or
	{0, cscm_is_not, cscm_analyze_not},
	{0, cscm_is_quote, cscm_analyze_quote},
	{0, cscm_is_combination, cscm_analyze_combination},


	{1, NULL, NULL}
};


CSCM_EF *cscm_analyze(CSCM_AST_NODE *exp)
{
	CSCM_SA_FUNCS *p;


	if (exp == NULL)
		cscm_error_report("cscm_analyze", CSCM_ERROR_NULL_PTR);


	for (p = _cscm_sa_func_list; !(p->flag_end); p++)
		if (p->predicate(exp))
			return p->analyze(exp);


	cscm_syntax_error_report(exp->filename,	\
				exp->line,	\
				CSCM_ERROR_ANALYZE_UNKNOWN_EXP_TYPE);
}




CSCM_OBJECT *cscm_apply(CSCM_OBJECT *operator, \
			size_t n_args, CSCM_OBJECT **args)
{
	int i;

	CSCM_OBJECT *ret;

	CSCM_PROC_PRIM_FUNC f;

	CSCM_EF *body_ef;
	CSCM_OBJECT *env;
	size_t n_params;
	char **params;

	CSCM_OBJECT *frame;


	if (operator == NULL)
		cscm_error_report("cscm_apply", \
				CSCM_ERROR_APPLY_NO_OPERATOR);
	else if (n_args >= 1 && args == NULL)
		cscm_error_report("cscm_apply", \
				CSCM_ERROR_NULL_PTR);


	if (operator->type == CSCM_OBJECT_TYPE_PROC_PRIM) {
		f = cscm_proc_prim_get_f(operator);

		ret = f(n_args, args);


		for (i = 0; i < n_args; i++)
			cscm_gc_free(args[i]);
	} else if (operator->type == CSCM_OBJECT_TYPE_PROC_COMP) {
		body_ef = cscm_proc_comp_get_body(operator);
		env = cscm_proc_comp_get_env(operator);
		n_params = cscm_proc_comp_get_n_params(operator);
		params = cscm_proc_comp_get_params(operator);


		if (n_params != n_args)
			cscm_error_report("cscm_apply", \
					CSCM_ERROR_APPLY_N_ARGS);


		frame = cscm_frame_create();

		if (n_args > 0)
			cscm_frame_init(frame,		\
					n_params,	\
					params,		\
					args);

		env = cscm_env_cpy_extend(env, frame);


		if (!cscm_tco_get_flag(CSCM_TCO_FLAG_ALLOW)) {
			cscm_tco_set_flag(CSCM_TCO_FLAG_ALLOW);
		} else {
			cscm_tco_state_save(env, body_ef);
			return NULL;
		}

		ret = cscm_ef_exec(body_ef, env);


		while (cscm_tco_get_flag(CSCM_TCO_FLAG_STATE_SAVED)) {
			cscm_gc_free(env);

			env = cscm_tco_state_get_new_env();
			body_ef = cscm_tco_state_get_new_body_ef();

			cscm_tco_unset_flag(CSCM_TCO_FLAG_STATE_SAVED);

			ret = cscm_ef_exec(body_ef, env);
		}

		cscm_tco_unset_flag(CSCM_TCO_FLAG_ALLOW);


		if (ret) {
			cscm_gc_inc(ret); // try to save it from freeing env
			cscm_gc_free(env);
			cscm_gc_dec(ret);
		} else {
			cscm_gc_free(env);
		}
	} else {
		cscm_error_report("cscm_apply", CSCM_ERROR_OBJECT_TYPE);
	}


	cscm_gc_free(operator);


	return ret;
}




int cscm_is_combination(CSCM_AST_NODE *exp)
{
	if (exp == NULL)
		cscm_error_report("cscm_is_combination", \
				CSCM_ERROR_NULL_PTR);
	else if (!cscm_ast_is_exp(exp))
		return 0;
	else if (cscm_ast_is_exp_empty(exp))
		return 0;
	else
		return 1;
}




CSCM_COMBINATION_EF_STATE *_cscm_combination_ef_state_create()
{
	CSCM_COMBINATION_EF_STATE *state;


	state = malloc(sizeof(CSCM_COMBINATION_EF_STATE));
	if (state == NULL)
		cscm_libc_fail("cscm_combination_ef_state_create", \
				"malloc");


	state->operator_ef = NULL;

	state->n_arg_efs = 0;
	state->arg_efs = NULL;


	return state;
}


CSCM_OBJECT *_cscm_combination_ef(void *state, CSCM_OBJECT *env)
{
	int i;

	CSCM_COMBINATION_EF_STATE *s;
	int flag_tco_allow;

	CSCM_OBJECT *operator, **args;

	CSCM_OBJECT *ret;


	s = (CSCM_COMBINATION_EF_STATE *)state;


	flag_tco_allow = cscm_tco_get_flag(CSCM_TCO_FLAG_ALLOW);
	cscm_tco_unset_flag(CSCM_TCO_FLAG_ALLOW);


	operator = cscm_ef_exec(s->operator_ef, env);


	if (s->n_arg_efs == 0) {
		if (flag_tco_allow) // restore the original value of the flag
			cscm_tco_set_flag(CSCM_TCO_FLAG_ALLOW);


		ret = cscm_apply(operator, 0, NULL);
	} else {
		args = cscm_object_ptrs_create(s->n_arg_efs);
		for (i = 0; i < s->n_arg_efs; i++)
			args[i] = cscm_ef_exec(s->arg_efs[i], env);


		if (flag_tco_allow) // restore the original value of the flag
			cscm_tco_set_flag(CSCM_TCO_FLAG_ALLOW);


		ret = cscm_apply(operator, s->n_arg_efs, args);
	}


	return ret;
}




CSCM_EF *cscm_analyze_combination(CSCM_AST_NODE *exp)
{
	int i;

	CSCM_COMBINATION_EF_STATE *state;

	CSCM_EF *operator_ef, **arg_efs;


	state = _cscm_combination_ef_state_create();


	operator_ef = cscm_analyze(cscm_ast_exp_index(exp, 0));
	state->operator_ef = operator_ef;


	if (exp->n_childs == 1) {	// no argument in the combination
		state->n_arg_efs = 0;
		state->arg_efs = NULL;
	} else {			// there are arguments here
		arg_efs = cscm_ef_ptrs_create(exp->n_childs - 1);

		for (i = 1; i < exp->n_childs; i++)
			arg_efs[i - 1] = cscm_analyze( \
					cscm_ast_exp_index(exp, i));



		state->n_arg_efs = exp->n_childs - 1;
		state->arg_efs = arg_efs;
	}

	return cscm_ef_construct(CSCM_EF_TYPE_COMBINATION,	\
				state,				\
				_cscm_combination_ef);
}




void cscm_combination_ef_free(CSCM_EF *ef)
{
	int i;

	CSCM_COMBINATION_EF_STATE *state;


	if (ef == NULL)
		cscm_error_report("cscm_combination_ef_free", \
				CSCM_ERROR_NULL_PTR);
	else if (ef->type != CSCM_EF_TYPE_COMBINATION)
		cscm_error_report("cscm_combination_ef_free", \
				CSCM_ERROR_EF_TYPE);


	state = (CSCM_COMBINATION_EF_STATE *)ef->state;


	cscm_ef_free_tree(state->operator_ef);

	for (i = 0; i < state->n_arg_efs; i++)
		cscm_ef_free_tree(state->arg_efs[i]);

	free(state->arg_efs);


	free(state);


	free(ef);
}
