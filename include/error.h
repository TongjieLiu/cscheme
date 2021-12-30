/* error.h -- error handling

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

#ifndef __CSCM_ERROR_H__

#define __CSCM_ERROR_H__




#include <stddef.h>




#define CSCM_ERROR_NULL_PTR	"null pointer"




/* cscheme error */
void cscm_error_report(char *func, char *msg);
void cscm_syntax_error_report(char *filename, size_t line, char *msg);
void cscm_runtime_error_report(char *object_name, char *msg);


/* standard c library error */
void cscm_libc_fail(char *pos, char *name);


/* signal handling */
void cscm_sigabrt_handler(int signum);




#endif
