/* error.c -- error handling

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

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "error.h"




void cscm_error_report(char *func, char *msg)
{
	fprintf(stderr, "%s(): %s\n", func, msg);
	exit(1);
}




void cscm_syntax_error_report(char *filename, size_t line, char *msg)
{
	fprintf(stderr, "%s:%lu: %s\n", filename, (unsigned long)line, msg);
	exit(1);
}




void cscm_runtime_error_report(char *object_name, char *msg)
{
	fprintf(stderr, "\"%s\": %s\n", object_name, msg);
	exit(1);
}




void cscm_libc_fail(char *pos, char *name)
{
	fprintf(stderr, "%s(): %s(): %s\n", pos, name, strerror(errno));
	exit(1);
}
