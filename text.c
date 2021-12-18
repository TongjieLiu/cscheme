/* text.c -- string manipulation

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

#include <string.h>
#include <stdlib.h>

#include "error.h"
#include "text.h"




char *cscm_text_cpy(char *text)
{
	char *new_text;


	if (text == NULL)
		cscm_error_report("cscm_text_cpy", \
				CSCM_ERROR_NULL_PTR);


	new_text = malloc(strlen(text) + 1);
	if (new_text == NULL)
		cscm_libc_fail("cscm_text_cpy", "malloc");


	strcpy(new_text, text);


	return new_text;
}




/*	Another version of function cscm_text_cpy() but
 * can handle escape characters */
char *cscm_text_cpy_ec(char *text)
{
	size_t size;

	char *new_text;

	char *src, *dest, *next;


	if (text == NULL)
		cscm_error_report("cscm_text_cpy_ec", \
				CSCM_ERROR_NULL_PTR);


	for (src = text, size = 0; *src; src++) {
		if (*src == '\\') {
			next = src + 1;

			if (*next == 'n')
				src++;
			else if (*next == 't')
				src++;
			else if (*next == '"')
				src++;
			else if (*next == '\\')
				src++;
			else if (*next == 0)
				cscm_error_report("cscm_text_cpy_ec", \
						CSCM_ERROR_TEXT_BACKSLASH);
			else
				cscm_error_report("cscm_text_cpy_ec", \
						CSCM_ERROR_TEXT_EC);
		}

		size++;
	}


	new_text = malloc(size + 1);
	if (new_text == NULL)
		cscm_libc_fail("cscm_text_cpy_ec", "malloc");


	for (src = text, dest = new_text; *src != 0; src++, dest++) {
		if (*src == '\\') {
			next = src + 1;

			if (*next == 'n') {
				src++;
				*dest = '\n';
			} else if (*next == 't') {
				src++;
				*dest = '\t';
			} else if (*next == '"') {
				src++;
				*dest = '"';
			} else if (*next == '\\') {
				src++;
				*dest = '\\';
			}
		} else {
			*dest = *src;
		}
	}


	*dest = 0;


	return new_text;

}




char **cscm_text_ptrs_cpy(size_t n, char **texts)
{
	size_t size;
	char **new_texts;


	if (n == 0)
		cscm_error_report("cscm_text_ptrs_cpy", \
				CSCM_ERROR_TEXT_EMPTY_TEXTS);
	else if (texts == NULL)
		cscm_error_report("cscm_text_ptrs_cpy", \
				CSCM_ERROR_NULL_PTR);


	size = n * sizeof(char *);


	new_texts = malloc(size);
	if (new_texts == NULL)
		cscm_libc_fail("cscm_text_ptrs_cpy", "malloc");


	memcpy(new_texts, texts, size);


	return new_texts;
}




int _cscm_text_unique_strcmp(const void *a, const void *b)
{
	return strcmp(*(char **)a, *(char **)b);
}


int cscm_text_unique(size_t n, char **texts)
{
	int i;
	char **texts_cpy;


	if (n == 0)
		cscm_error_report("cscm_text_unique", \
				CSCM_ERROR_TEXT_EMPTY_TEXTS);
	else if (n == 1)
		return 1;
	else if (texts == NULL)
		cscm_error_report("cscm_text_unique", \
				CSCM_ERROR_NULL_PTR);


	texts_cpy = cscm_text_ptrs_cpy(n, texts);
	qsort(texts_cpy, n, sizeof(char *), _cscm_text_unique_strcmp);


	for (i = 1; i < n; i++) {
		if (!strcmp(texts_cpy[i - 1], texts_cpy[i])) {
			free(texts_cpy);
			return 0;
		}
	}


	free(texts_cpy);
	return 1;
}




int cscm_text_is_dquoted(char *text)
{
	char *p;


	if (text == NULL)
	       cscm_error_report("cscm_text_is_dquoted", \
				CSCM_ERROR_NULL_PTR);


	if (*text == '"')
		p = text + 1;
	else
		return 0;


	for (; *p != 0; p++) {
		if (*p == '\\' && *(p + 1) == '"') {	// escaped "
			p++;
		} else if (*p == '"') {
			if (*(p + 1) == 0)		// closing "
				return 1;
			else				// unescaped "
				return 0;
		}
	}


	return 0;
}




char *cscm_text_cpy_strip_dquotes(char *text)
{
	char *src, *dest;
	char *new_text;


	if (!cscm_text_is_dquoted(text))
		cscm_error_report("cscm_text_cpy_strip_dquotes", \
				CSCM_ERROR_TEXT_NON_DQUOTED);


	new_text = malloc(strlen(text) + 1 - 2);
	if (new_text == NULL)
		cscm_libc_fail("cscm_text_cpy_strip_dquotes", "malloc");


	src = text + 1;
	dest = new_text;
	for (; *src != '"'; src++, dest++)
		*dest = *src;


	*dest = 0;


	return new_text;
}




int cscm_text_is_squoted(char *text)
{
	if (text == NULL)
		cscm_error_report("cscm_text_is_squoted", \
				CSCM_ERROR_NULL_PTR);
	else if (*text == '\'')
		return 1;
	else
		return 0;
}


char *cscm_text_cpy_add_squote(char *text)
{
	char *ret;


	if (text == NULL)
		cscm_error_report("cscm_text_cpy_add_squote", \
				CSCM_ERROR_NULL_PTR);


	ret = malloc(strlen(text) + 2);
	if (ret == NULL)
		cscm_libc_fail("cscm_text_cpy_add_squote", \
				"malloc");


	*ret = '\'';
	strcpy(ret + 1, text);


	return ret;
}


char *cscm_text_strip_squote(char *text)
{
	if (text == NULL)
		cscm_error_report("cscm_text_strip_squote", \
				CSCM_ERROR_NULL_PTR);
	else if (*text == '\'')
		return (text + 1);
	else
		return text;
}


char *cscm_text_strip_all_squotes(char *text)
{
	char *p;


	if (text == NULL)
		cscm_error_report("cscm_text_strip_all_squotes", \
				CSCM_ERROR_NULL_PTR);


	for (p = text; *p == '\''; p++);


	return p;
}




char *cscm_text_cpy_lowercase(char *text)
{
	char *p;
	char *text_cpy;


	if (text == NULL)
		cscm_error_report("cscm_text_cpy_lowercase", \
				CSCM_ERROR_NULL_PTR);


	text_cpy = cscm_text_cpy(text);
	for (p = text_cpy; *p != 0; p++)
		if (*p >= 'A' && *p <= 'Z')
			*p = *p - 'A' + 'a';


	return text_cpy;
}




// Criteria: ((+ | -) [0-9]) | [0-9]) [0-9]*
int cscm_text_is_integer(char *text)
{
	char *p;


	if (text == NULL)
		cscm_error_report("cscm_text_is_integer", \
				CSCM_ERROR_NULL_PTR);
	
		
	if ((text[0] == '+' || text[0] == '-') \
	    && (text[1] >= '0' && text[1] <= '9'))
		p = &text[2];
	else if (text[0] >= '0' && text[0] <= '9')
		p = &text[1];
	else
		return 0;


	for (; *p != 0; p++) // [0-9]*
		if (*p < '0' || *p > '9')
			return 0;


	return 1;
}


// "fpn" stands for "floating-point number"
// Criteria: ((+ | -) [0-9]) | [0-9]) [0-9]* \. [0-9] [0-9]*
int cscm_text_is_fpn(char *text)
{
	char *p;


	if (text == NULL)
		cscm_error_report("cscm_text_is_fpn", \
				CSCM_ERROR_NULL_PTR);
	
		
	if ((text[0] == '+' || text[0] == '-') \
		&& (text[1] >= '0' && text[1] <= '9'))
		p = &text[2];
	else if (text[0] >= '0' && text[0] <= '9')
		p = &text[1];
	else
		return 0;


	for (; *p != 0 && *p != '.'; p++)
		if (*p < '0' || *p > '9')
			return 0;


	if (*p == 0)
		return 0;
	else // *p == '.'
		p++;


	if (*p >= '0' && *p <= '9')
		p++;
	else
		return 0;


	for (; *p != 0; p++)
		if (*p < '0' || *p > '9')
			return 0;


	return 1;
}
