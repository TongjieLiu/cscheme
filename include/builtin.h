/* builtin.h -- cscheme standard library

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

#ifndef __CSCM_BUILTIN_H__

#define __CSCM_BUILTIN_H__




#include <stddef.h>




typedef void (*CSCM_BUILTIN_MODULE_FUNC)();


struct _CSCM_BUILTIN_MODULE {
	int flag_last;

	char *mod_name;
	CSCM_BUILTIN_MODULE_FUNC f;
};


typedef struct _CSCM_BUILTIN_MODULE CSCM_BUILTIN_MODULE;




#define CSCM_ERROR_BUILTIN_N_ARGS	"incorrect number of arguments"
#define CSCM_ERROR_BUILTIN_FUNCNAME	"function name is not specified"




#define CSCM_ERROR_BUILTIN_BAD_OP	"bad op"




#define CSCM_ERROR_BUILTIN_RETURN_TYPE	"bad return type"




#define CSCM_ERROR_BUILTIN_BAD_PROC	"bad proc"
#define CSCM_ERROR_BUILTIN_BAD_PRED	"bad predicate"
#define CSCM_ERROR_BUILTIN_BAD_ACTION	"bad action"
#define CSCM_ERROR_BUILTIN_BAD_SEQ	"bad sequence"
#define CSCM_ERROR_BUILTIN_BAD_INITIAL	"bad initial"




#define CSCM_ERROR_BUILTIN_BAD_MODULE	"bad module"




void cscm_builtin_check_args(char *funcname,		\
			size_t rn, size_t n,		\
			CSCM_OBJECT **args);

void cscm_builtin_check_lb_args(char *funcname,		\
			size_t lbn, size_t n,		\
			CSCM_OBJECT **args);

void cscm_builtin_check_interval_args(char *funcname,		\
			size_t lbn, size_t ubn, size_t n,	\
			CSCM_OBJECT **args);




CSCM_OBJECT *cscm_builtin_proc_print(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_printn(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_display(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_newline(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_read(size_t n, CSCM_OBJECT **args);




CSCM_OBJECT *cscm_builtin_proc_set_car(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_set_cdr(size_t n, CSCM_OBJECT **args);




CSCM_OBJECT *cscm_builtin_proc_cons(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_car(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_cdr(size_t n, CSCM_OBJECT **args);




CSCM_OBJECT *cscm_builtin_proc_list(size_t n, CSCM_OBJECT **args);




CSCM_OBJECT *cscm_builtin_proc_add(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_subtract(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_multiply(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_divide(size_t n, CSCM_OBJECT **args);




CSCM_OBJECT *cscm_builtin_proc_remainder(size_t n, CSCM_OBJECT **args);




CSCM_OBJECT *cscm_builtin_proc_equal_num(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_equal_ssb(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_equal(size_t n, CSCM_OBJECT **args);




CSCM_OBJECT *cscm_builtin_proc_greater_than(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_greater_equal(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_less_than(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_less_equal(size_t n, CSCM_OBJECT **args);




CSCM_OBJECT *cscm_builtin_proc_is_string(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_is_symbol(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_is_number(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_is_pair(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_is_nil(size_t n, CSCM_OBJECT **args);




CSCM_OBJECT *cscm_builtin_proc_include(size_t n, CSCM_OBJECT **args);




CSCM_OBJECT *cscm_builtin_proc_max(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_min(size_t n, CSCM_OBJECT **args);




CSCM_OBJECT *cscm_builtin_proc_apply(size_t n, CSCM_OBJECT **args);




#endif
