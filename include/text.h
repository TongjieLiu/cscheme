/* text.h -- string manipulation

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

#ifndef __CSCM_TEXT_H__

#define __CSCM_TEXT_H__




#include <stddef.h>




#define CSCM_ERROR_TEXT_EMPTY_TEXTS	"empty texts"

#define CSCM_ERROR_TEXT_NON_DQUOTED	"not double quoted"

#define CSCM_ERROR_TEXT_BACKSLASH	"unexpected backslash(\\)"
#define CSCM_ERROR_TEXT_EC		"unsupported escape character"




char *cscm_text_cpy(char *text);
char *cscm_text_cpy_ec(char *text);
char **cscm_text_ptrs_cpy(size_t n, char **texts);




int cscm_text_unique(size_t n, char **texts);




int cscm_text_is_dquoted(char *text);
char *cscm_text_cpy_strip_dquotes(char *text);




int cscm_text_is_squoted(char *text);
char *cscm_text_cpy_add_squote(char *text);
char *cscm_text_strip_squote(char *text);
char *cscm_text_strip_all_squotes(char *text);




char *cscm_text_cpy_lowercase(char *text);




int cscm_text_is_integer(char *text);
int cscm_text_is_fpn(char *text);




#endif
