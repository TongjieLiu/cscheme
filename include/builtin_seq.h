/* builtin_seq.h -- cscheme standard library module: seq

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

#ifndef __CSCM_BUILTIN_SEQ_H__

#define __CSCM_BUILTIN_SEQ_H__




#include <stddef.h>




CSCM_OBJECT *cscm_builtin_proc_sort(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_length(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_list_ref(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_range(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_append(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_reverse(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_list_copy(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_map(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_for_each(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_filter(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_accumulate(size_t n, CSCM_OBJECT **args);
CSCM_OBJECT *cscm_builtin_proc_fold_left(size_t n, CSCM_OBJECT **args);




void cscm_builtin_module_func_seq();




#endif
