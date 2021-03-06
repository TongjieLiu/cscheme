/* core.c -- syntactic analysis

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
#include "quasiquote.h"
#include "gc.h"
#include "env.h"
#include "pair.h"
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
	{0, cscm_is_string, cscm_analyze_string},
	{0, cscm_is_var, cscm_analyze_var},

	{0, cscm_is_quote, cscm_analyze_quote},
	{0, cscm_is_quasiquote, cscm_analyze_quasiquote},
	{0, cscm_is_assignment, cscm_analyze_assignment},
	{0, cscm_is_definition, cscm_analyze_definition},
	{0, cscm_is_lambda, cscm_analyze_lambda},
	{0, cscm_is_if, cscm_analyze_if},
	{0, cscm_is_cond, cscm_analyze_cond},
	{0, cscm_is_begin, cscm_analyze_begin},
	{0, cscm_is_let, cscm_analyze_let},
	{0, cscm_is_ao, cscm_analyze_ao}, // and/or
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




CSCM_OBJECT *cscm_apply(CSCM_OBJECT *proc, \
			size_t n_args, CSCM_OBJECT **args)
{
	int i;

	CSCM_OBJECT *ret;

	int flag_tco_allow;
	CSCM_PROC_PRIM_FUNC f;

	CSCM_EF *body_ef;
	CSCM_OBJECT *env;
	size_t n_params;
	char **params;

	int flag_dtn;
	size_t n_required_args;
	CSCM_OBJECT **arguments;

	CSCM_OBJECT *frame;

	CSCM_AST_NODE *exp;


	if (proc == NULL)
		cscm_error_report("cscm_apply", \
				CSCM_ERROR_APPLY_NO_PROC);
	else if (n_args >= 1 && args == NULL)
		cscm_error_report("cscm_apply", \
				CSCM_ERROR_NULL_PTR);


	if (proc->type == CSCM_OBJECT_TYPE_PROC_PRIM) {
		flag_tco_allow = cscm_tco_get_flag(CSCM_TCO_FLAG_ALLOW);
		cscm_tco_unset_flag(CSCM_TCO_FLAG_ALLOW);


		f = cscm_proc_prim_get_f(proc);
		ret = f(n_args, args);


		if (ret) // try to save it from freeing arguments
			cscm_gc_inc(ret);

		for (i = 0; i < n_args; i++)
			cscm_gc_free(args[i]);

		if (ret)
			cscm_gc_dec(ret);


		if (flag_tco_allow) // restore the original value of the flag
			cscm_tco_set_flag(CSCM_TCO_FLAG_ALLOW);
	} else if (proc->type == CSCM_OBJECT_TYPE_PROC_COMP) {
		body_ef = cscm_proc_comp_get_body(proc);
		env = cscm_proc_comp_get_env(proc);
		n_params = cscm_proc_comp_get_n_params(proc);
		params = cscm_proc_comp_get_params(proc);
		flag_dtn = cscm_proc_comp_get_flag_dtn(proc);


		if (flag_dtn) { // at least 1 formal parameter
			n_required_args = n_params - 1;

			if (n_args < n_required_args) {
				cscm_error_report("cscm_apply", \
						CSCM_ERROR_APPLY_N_ARGS);
			} else if (n_args == n_required_args) {
				arguments = cscm_object_ptrs_create(n_params);

				for (i = 0; i < n_required_args; i++)
					arguments[i] = args[i];

				arguments[i] = CSCM_NIL;
			} else {
				args[n_params - 1] =			\
						cscm_list_create(	\
						n_args - n_params + 1,	\
						&args[n_params - 1]);

				arguments = args;
			}
		} else {
			if (n_args != n_params)
				cscm_error_report("cscm_apply", \
						CSCM_ERROR_APPLY_N_ARGS);

			arguments = args;
		}


		frame = cscm_frame_create();

		if (n_params > 0)
			cscm_frame_init(frame,		\
					n_params,	\
					params,		\
					arguments);

		if (arguments != args)
			free(arguments);

		env = cscm_env_cpy_extend(env, frame);
		cscm_gc_inc(env);


		if (!cscm_tco_get_flag(CSCM_TCO_FLAG_ALLOW)) {
			cscm_tco_set_flag(CSCM_TCO_FLAG_ALLOW);
		} else {
			/* get current exp as the next exp */
			exp = cscm_ef_backtrace_pop();
			cscm_ef_backtrace_push(exp);

			cscm_tco_state_save(env, body_ef, exp);
			return NULL;
		}

		ret = cscm_ef_exec(body_ef, env);


		while (cscm_tco_get_flag(CSCM_TCO_FLAG_STATE_SAVED)) {
			cscm_gc_dec(env);
			cscm_gc_free(env);

			cscm_tco_state_get(&env, &body_ef, &exp);

			/* replace current exp with next exp*/
			cscm_ef_backtrace_pop();
			cscm_ef_backtrace_push(exp);

			cscm_tco_unset_flag(CSCM_TCO_FLAG_STATE_SAVED);

			ret = cscm_ef_exec(body_ef, env);
		}

		cscm_tco_unset_flag(CSCM_TCO_FLAG_ALLOW);


		if (ret) {
			cscm_gc_inc(ret); // try to save it from freeing env
			cscm_gc_dec(env);
			cscm_gc_free(env);
			cscm_gc_dec(ret);
		} else {
			cscm_gc_dec(env);
			cscm_gc_free(env);
		}
	} else {
		cscm_error_report("cscm_apply", CSCM_ERROR_OBJECT_TYPE);
	}


	cscm_gc_free(proc);


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


	state->proc_ef = NULL;

	state->n_arg_efs = 0;
	state->arg_efs = NULL;


	return state;
}


CSCM_OBJECT *_cscm_combination_ef(void *state, CSCM_OBJECT *env)
{
	int i;

	CSCM_COMBINATION_EF_STATE *s;
	int flag_tco_allow;

	CSCM_OBJECT *proc, **args;

	CSCM_OBJECT *ret;


	s = (CSCM_COMBINATION_EF_STATE *)state;


	flag_tco_allow = cscm_tco_get_flag(CSCM_TCO_FLAG_ALLOW);
	cscm_tco_unset_flag(CSCM_TCO_FLAG_ALLOW);


	proc = cscm_ef_exec(s->proc_ef, env);


	if (s->n_arg_efs == 0) {
		if (flag_tco_allow) // restore the original value of the flag
			cscm_tco_set_flag(CSCM_TCO_FLAG_ALLOW);


		ret = cscm_apply(proc, 0, NULL);
	} else {
		args = cscm_object_ptrs_create(s->n_arg_efs);
		for (i = 0; i < s->n_arg_efs; i++)
			args[i] = cscm_ef_exec(s->arg_efs[i], env);


		if (flag_tco_allow) // restore the original value of the flag
			cscm_tco_set_flag(CSCM_TCO_FLAG_ALLOW);


		ret = cscm_apply(proc, s->n_arg_efs, args);

		free(args);
	}


	return ret;
}




CSCM_EF *cscm_analyze_combination(CSCM_AST_NODE *exp)
{
	int i;

	CSCM_COMBINATION_EF_STATE *state;

	CSCM_EF *proc_ef, **arg_efs;


	state = _cscm_combination_ef_state_create();


	proc_ef = cscm_analyze(cscm_ast_exp_index(exp, 0));
	state->proc_ef = proc_ef;


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
				exp,				\
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


	cscm_ef_free_tree(state->proc_ef);

	for (i = 0; i < state->n_arg_efs; i++)
		cscm_ef_free_tree(state->arg_efs[i]);

	if (state->n_arg_efs)
		free(state->arg_efs);


	free(state);


	free(ef);
}
