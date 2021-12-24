/* tco.c -- tail-call optimization

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
#include "ef.h"
#include "object.h"
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


void cscm_tco_state_save(CSCM_OBJECT *new_env, CSCM_EF *new_body_ef)
{
	if (new_env == NULL || new_body_ef == NULL)
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

	cscm_tco_set_flag(CSCM_TCO_FLAG_STATE_SAVED);
}




CSCM_OBJECT *cscm_tco_state_get_new_env()
{
	if (!cscm_tco_get_flag(CSCM_TCO_FLAG_STATE_SAVED))
		cscm_error_report("cscm_tco_state_get_new_env", \
				CSCM_ERROR_TCO_STATE_NOT_SAVED);


	return _cscm_tco_state_new_env;
}


CSCM_EF *cscm_tco_state_get_new_body_ef()
{
	if (!cscm_tco_get_flag(CSCM_TCO_FLAG_STATE_SAVED))
		cscm_error_report("cscm_tco_state_get_new_body_ef", \
				CSCM_ERROR_TCO_STATE_NOT_SAVED);


	return _cscm_tco_state_new_body_ef;
}
