/* tco.c -- tail-call optimization

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
#include "ef.h"
#include "object.h"
#include "ast.h"
#include "tco.h"




unsigned char _cscm_tco_flag = 0;


void cscm_tco_set_flag(unsigned char flag)
{
	if (flag != CSCM_TCO_FLAG_ALLOW \
		&& flag != CSCM_TCO_FLAG_STATE_SAVED)
		cscm_error_report("cscm_tco_set_flag", \
				CSCM_ERROR_TCO_FLAG_TYPE);


	_cscm_tco_flag |= flag;
}


void cscm_tco_unset_flag(unsigned char flag)
{
	unsigned char mask;


	if (flag != CSCM_TCO_FLAG_ALLOW \
		&& flag != CSCM_TCO_FLAG_STATE_SAVED)
		cscm_error_report("cscm_tco_unset_flag", \
				CSCM_ERROR_TCO_FLAG_TYPE);


	mask = ~flag;
	_cscm_tco_flag &= mask;
}




int cscm_tco_get_flag(unsigned char flag)
{
	unsigned char result;


	if (flag != CSCM_TCO_FLAG_ALLOW \
		&& flag != CSCM_TCO_FLAG_STATE_SAVED)
		cscm_error_report("cscm_tco_get_flag", \
				CSCM_ERROR_TCO_FLAG_TYPE);


	result = _cscm_tco_flag & flag;
	return result ? 1 : 0;
}




CSCM_OBJECT *_cscm_tco_state_new_env = NULL;
CSCM_EF *_cscm_tco_state_new_body_ef = NULL;
CSCM_AST_NODE *_cscm_tco_state_new_exp = NULL;


void cscm_tco_state_save(CSCM_OBJECT *new_env,	\
			CSCM_EF *new_body_ef,	\
			CSCM_AST_NODE *new_exp)
{
	if (new_env == NULL || new_body_ef == NULL || new_exp == NULL)
		cscm_error_report("cscm_tco_state_save", \
				CSCM_ERROR_NULL_PTR);
	else if (!cscm_tco_get_flag(CSCM_TCO_FLAG_ALLOW))
		cscm_error_report("cscm_tco_state_save", \
				CSCM_ERROR_TCO_NOT_ALLOW);
	else if (cscm_tco_get_flag(CSCM_TCO_FLAG_STATE_SAVED))
		cscm_error_report("cscm_tco_state_save", \
				CSCM_ERROR_TCO_STATE_SAVED);


	_cscm_tco_state_new_env = new_env;
	_cscm_tco_state_new_body_ef = new_body_ef;
	_cscm_tco_state_new_exp = new_exp;

	cscm_tco_set_flag(CSCM_TCO_FLAG_STATE_SAVED);
}




void cscm_tco_state_get(CSCM_OBJECT **new_env_ptr,	\
			CSCM_EF **new_body_ef_ptr,	\
			CSCM_AST_NODE **new_exp_ptr)
{
	if (new_env_ptr == NULL			\
		|| new_body_ef_ptr == NULL	\
		|| new_exp_ptr == NULL)
		cscm_error_report("cscm_tco_state_get", \
				CSCM_ERROR_NULL_PTR);
	else if (!cscm_tco_get_flag(CSCM_TCO_FLAG_STATE_SAVED))
		cscm_error_report("cscm_tco_state_get", \
				CSCM_ERROR_TCO_STATE_NOT_SAVED);


	*new_env_ptr = _cscm_tco_state_new_env;
	*new_body_ef_ptr = _cscm_tco_state_new_body_ef;
	*new_exp_ptr = _cscm_tco_state_new_exp;
}
