/* ast.c -- Abstract Syntax Tree

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
#include <stdio.h>

#include "error.h"
#include "text.h"
#include "ast.h"




int cscm_ast_is_symbol(CSCM_AST_NODE *node)
{
	if (node == NULL)
		return 0;


	if (node->type == CSCM_AST_NODE_TYPE_SYMBOL)
		return 1;
	else
		return 0;
}


int cscm_ast_is_exp(CSCM_AST_NODE *node)
{
	if (node == NULL)
		return 0;


	if (node->type == CSCM_AST_NODE_TYPE_EXP)
		return 1;
	else
		return 0;
}




int cscm_ast_is_symbol_empty(CSCM_AST_NODE *node)
{
	if (!cscm_ast_is_symbol(node))
		cscm_error_report("cscm_ast_is_symbol_empty", \
				CSCM_ERROR_AST_NODE_TYPE);


	if (node->text == NULL) {
		return 1;
	} else if (node->text[0] == 0) {
		free(node->text);
		node->text = NULL;

		return 1;
	} else {
		return 0;
	}
}


int cscm_ast_is_exp_empty(CSCM_AST_NODE *node)
{
	if (!cscm_ast_is_exp(node))
		cscm_error_report("cscm_ast_is_exp_empty", \
				CSCM_ERROR_AST_NODE_TYPE);


	if (node->n_childs == 0)
		return 1;
	else
		return 0;
}




CSCM_AST_NODE **cscm_ast_node_ptrs_create(size_t n)
{
	size_t size;
	CSCM_AST_NODE **nodes;


	size = n * sizeof(CSCM_AST_NODE *);
	nodes = malloc(size);
	if (nodes == NULL)
		cscm_libc_fail("cscm_ast_node_ptrs_create", "malloc");


	return nodes;
}




CSCM_AST_NODE *cscm_ast_symbol_create(char *filename, size_t line)
{
	CSCM_AST_NODE *symbol;


	symbol = malloc(sizeof(CSCM_AST_NODE));
	if (symbol == NULL)
		cscm_libc_fail("cscm_ast_symbol_create", "malloc");


	symbol->type = CSCM_AST_NODE_TYPE_SYMBOL;
	symbol->filename = filename;
	symbol->line = line;

	symbol->text = NULL;

	symbol->n_childs = 0;
	symbol->pages = NULL;


	return symbol;
}


CSCM_AST_NODE *cscm_ast_exp_create(char *filename, size_t line)
{
	CSCM_AST_NODE *exp;


	exp = malloc(sizeof(CSCM_AST_NODE));
	if (exp == NULL)
		cscm_libc_fail("cscm_ast_exp_create", "malloc");


	exp->type = CSCM_AST_NODE_TYPE_EXP;
	exp->filename = filename;
	exp->line = line;

	exp->text = NULL;

	exp->n_childs = 0;
	exp->pages = (CSCM_AST_NODE ***)cscm_ast_node_ptrs_create( \
						CSCM_AST_EXP_PAGE_MAX_N);


	return exp;

}




int cscm_ast_symbol_text_equal(CSCM_AST_NODE *symbol, char *text)
{
	if (symbol == NULL)
		cscm_error_report("cscm_ast_symbol_text_equal", \
				CSCM_ERROR_NULL_PTR);
	else if (symbol->type != CSCM_AST_NODE_TYPE_SYMBOL)
		cscm_error_report("cscm_ast_symbol_text_equal", \
				CSCM_ERROR_AST_NODE_TYPE);
	else if (text == NULL || *text == 0)	
		cscm_error_report("cscm_ast_symbol_text_equal", \
				CSCM_ERROR_AST_NO_CTEXT);
	else if (symbol->text == NULL)
		cscm_error_report("cscm_ast_symbol_text_equal", \
				CSCM_ERROR_AST_EMPTY_SYMBOL);


	return !strcmp(symbol->text, text);
}


CSCM_AST_NODE *cscm_ast_exp_index(CSCM_AST_NODE *exp, size_t index)
{
	size_t page_n, page_index;
	div_t q;

	CSCM_AST_NODE **page;


	if (exp == NULL)
		cscm_error_report("cscm_ast_exp_index", \
				CSCM_ERROR_NULL_PTR);
	else if (exp->type != CSCM_AST_NODE_TYPE_EXP)
		cscm_error_report("cscm_ast_exp_index", \
				CSCM_ERROR_AST_NODE_TYPE);
	else if (index >= exp->n_childs)
		cscm_error_report("cscm_ast_exp_index", \
				CSCM_ERROR_AST_BAD_INDEX);


	q = div(index, CSCM_AST_EXP_PAGE_SIZE);
	page_n = q.quot;
	page_index = q.rem;


	page = exp->pages[page_n];


	return page[page_index];
}




/* no symbol has empty text, except newly created ones */
void cscm_ast_symbol_set(CSCM_AST_NODE *symbol, char *text)
{
	char *old_text;


	if (symbol == NULL)
		cscm_error_report("cscm_ast_symbol_set", \
				CSCM_ERROR_NULL_PTR);
	else if (symbol->type != CSCM_AST_NODE_TYPE_SYMBOL)
		cscm_error_report("cscm_ast_symbol_set", \
				CSCM_ERROR_AST_NODE_TYPE);
	else if (text == NULL || *text == 0)	
		cscm_error_report("cscm_ast_symbol_set", \
				CSCM_ERROR_AST_NO_STEXT);


	old_text = symbol->text;
	symbol->text = cscm_text_cpy(text);


	/*	The new text may be a substring of the old text,
	 * then there will be no new text after we have freed the
	 * old text.
	 * 	Therefore, we have to cscm_text_cpy() the new text
	 * first, and only after that we can safely free the old
	 * one. */
	if (old_text)
		free(old_text);
}


/*	Since we'll free the memory of old text, we should not call
 * this function with string literals and any substrings of the old
 * text as the new text to be set. */
void cscm_ast_symbol_set_simple(CSCM_AST_NODE *symbol, char *text)
{
	if (symbol == NULL)
		cscm_error_report("cscm_ast_symbol_set_simple", \
				CSCM_ERROR_NULL_PTR);
	else if (symbol->type != CSCM_AST_NODE_TYPE_SYMBOL)
		cscm_error_report("cscm_ast_symbol_set_simple", \
				CSCM_ERROR_AST_NODE_TYPE);
	else if (text == NULL || *text == 0)	
		cscm_error_report("cscm_ast_symbol_set_simple", \
				CSCM_ERROR_AST_NO_STEXT);


	if (symbol->text)
		free(symbol->text);


	symbol->text = text;
}


/* add a new child node after all existed ones */
void cscm_ast_exp_append(CSCM_AST_NODE *exp, CSCM_AST_NODE *new_child)
{
	size_t index_new_child;

	size_t page_n, page_index;
	div_t q;

	CSCM_AST_NODE **page;


	if (exp == NULL || new_child == NULL)
		cscm_error_report("cscm_ast_exp_append", \
				CSCM_ERROR_NULL_PTR);
	else if (exp->type != CSCM_AST_NODE_TYPE_EXP)
		cscm_error_report("cscm_ast_exp_append", \
				CSCM_ERROR_AST_NODE_TYPE);


	exp->n_childs++;
	index_new_child = exp->n_childs - 1;


	q = div((int)index_new_child, CSCM_AST_EXP_PAGE_SIZE);
	page_n = q.quot;
	page_index = q.rem;


	if (page_n >= CSCM_AST_EXP_PAGE_MAX_N)
		cscm_error_report("cscm_ast_exp_append", \
				CSCM_ERROR_AST_FULL_EXP);


	if (page_index == 0)
		exp->pages[page_n] = cscm_ast_node_ptrs_create( \
						CSCM_AST_EXP_PAGE_SIZE);


	page = exp->pages[page_n];
	page[page_index] = new_child;
}


/* delete the first child node */
void cscm_ast_exp_drop_first(CSCM_AST_NODE *exp)
{
	int i, j, end;

	size_t index_last_child;

	size_t page_n, page_index;
	div_t q;

	CSCM_AST_NODE **page, **next_page;


	if (exp == NULL)
		cscm_error_report("cscm_ast_exp_drop_first", \
				CSCM_ERROR_NULL_PTR);
	else if (exp->type != CSCM_AST_NODE_TYPE_EXP)
		cscm_error_report("cscm_ast_exp_drop_first", \
				CSCM_ERROR_AST_NODE_TYPE);
	else if (exp->n_childs == 0)
		cscm_error_report("cscm_ast_exp_drop_first", \
				CSCM_ERROR_AST_EMPTY_EXP);


	index_last_child = exp->n_childs - 1;

	q = div(index_last_child, CSCM_AST_EXP_PAGE_SIZE);
	page_n = q.quot;
	page_index = q.rem;


	page = exp->pages[0];

	cscm_ast_free_tree(page[0]);

	exp->n_childs--;

	if (exp->n_childs == 0) {
		free(page);
		return;
	}


	if (page_n == 0) // if there is only one page
		end = page_index;
	else
		end = CSCM_AST_EXP_PAGE_SIZE - 1;


	for (j = 1; j <= end; j++)
		page[j - 1] = page[j];


	for (i = 0; i < page_n; i++) {
		page = exp->pages[i];
		next_page = exp->pages[i + 1];

		page[CSCM_AST_EXP_PAGE_SIZE - 1] = next_page[0];

		for (j = 1; j < CSCM_AST_EXP_PAGE_SIZE; j++)
			next_page[j - 1] = next_page[j];

	}


	// the last page is now empty after dropping the first child
	if (page_index == 0)
		free(exp->pages[page_n]);
}


/* insert a new child node before all existed ones */
void cscm_ast_exp_insert_first(CSCM_AST_NODE *exp, CSCM_AST_NODE *new_child)
{
	size_t index_last_child;

	int i, j;

	size_t page_n, page_index;
	div_t q;

	CSCM_AST_NODE **page, **next_page;


	if (exp == NULL || new_child == NULL)
		cscm_error_report("cscm_ast_exp_insert_first", \
				CSCM_ERROR_NULL_PTR);
	else if (exp->type != CSCM_AST_NODE_TYPE_EXP)
		cscm_error_report("cscm_ast_exp_insert_first", \
				CSCM_ERROR_AST_NODE_TYPE);


	if (exp->n_childs == 0) {
		cscm_ast_exp_append(exp, new_child);
		return;
	}


	index_last_child = exp->n_childs - 1;

	q = div(index_last_child, CSCM_AST_EXP_PAGE_SIZE);
	page_n = q.quot;
	page_index = q.rem;


	page = exp->pages[page_n];


	// now the last page is full, and we have to allocate a new one
	if (page_index == (CSCM_AST_EXP_PAGE_SIZE - 1)) {
		if ((page_n + 1) == CSCM_AST_EXP_PAGE_MAX_N) {
			cscm_error_report("cscm_ast_exp_insert_first", \
					CSCM_ERROR_AST_FULL_EXP);
		} else {
			exp->pages[page_n + 1] = cscm_ast_node_ptrs_create( \
						CSCM_AST_EXP_PAGE_SIZE);

			next_page = exp->pages[page_n + 1];

			next_page[0] = page[CSCM_AST_EXP_PAGE_SIZE - 1];
		}
	}


	for (j = page_index; j >= 0; j--)
		page[j + 1] = page[j];


	for (i = page_n - 1; i >= 0; i--) {
		page = exp->pages[i];
		next_page = exp->pages[i + 1];

		next_page[0] = page[CSCM_AST_EXP_PAGE_SIZE - 1];

		for (j = CSCM_AST_EXP_PAGE_SIZE - 2; j >= 0; j--)
			page[j + 1] = page[j];
	}


	page[0] = new_child;


	exp->n_childs++;
}




void cscm_ast_exp_empty(CSCM_AST_NODE *exp)
{
	int i;

	size_t index_last_child;
	size_t last_page_n;

	div_t q;


	if (exp == NULL)
		cscm_error_report("cscm_ast_exp_empty", \
				CSCM_ERROR_NULL_PTR);
	else if (exp->type != CSCM_AST_NODE_TYPE_EXP)
		cscm_error_report("cscm_ast_exp_empty", \
				CSCM_ERROR_AST_NODE_TYPE);
	else if (exp->n_childs == 0)
		return;


	index_last_child = exp->n_childs - 1;

	q = div(index_last_child, CSCM_AST_EXP_PAGE_SIZE);
	last_page_n = q.quot;

	for (i = 0; i <= last_page_n; i++)
		free(exp->pages[i]);


	exp->n_childs = 0;
}




void cscm_ast_exp_mv(CSCM_AST_NODE *src, CSCM_AST_NODE *dest)
{
	if (src == NULL || dest == NULL)
		cscm_error_report("cscm_ast_exp_mv", \
				CSCM_ERROR_NULL_PTR);
	else if (src->type != CSCM_AST_NODE_TYPE_EXP \
		|| dest->type != CSCM_AST_NODE_TYPE_EXP)
		cscm_error_report("cscm_ast_exp_mv", \
				CSCM_ERROR_AST_NODE_TYPE);


	cscm_ast_exp_empty(dest);

	free(dest->pages);

	dest->n_childs = src->n_childs;
	dest->pages = src->pages;

	free(src);
}




void cscm_ast_free_symbol(CSCM_AST_NODE *symbol)
{
	if (!cscm_ast_is_symbol(symbol))
		cscm_error_report("cscm_ast_free_symbol", \
				CSCM_ERROR_AST_NODE_TYPE);


	if (symbol->text)
		free(symbol->text);


	free(symbol);
}


void cscm_ast_free_exp(CSCM_AST_NODE *exp)
{
	int i;

	div_t q;
	size_t index_last_child;


	if (!cscm_ast_is_exp(exp))
		cscm_error_report("cscm_ast_free_exp", \
				CSCM_ERROR_AST_NODE_TYPE);


	if (exp->n_childs) {
		index_last_child = exp->n_childs - 1;

		q = div(index_last_child, CSCM_AST_EXP_PAGE_SIZE);

		for (i = 0; i <= q.quot; i++)
			free(exp->pages[i]);

		free(exp->pages);
	}


	free(exp);
}


void cscm_ast_free_tree(CSCM_AST_NODE *node)
{
	int i;


	if (cscm_ast_is_symbol(node)) {
		cscm_ast_free_symbol(node);
	} else if (cscm_ast_is_exp(node)) {
		for (i = 0; i < node->n_childs; i++)
			cscm_ast_free_tree(cscm_ast_exp_index(node, i));

		cscm_ast_free_exp(node);
	} else {
		cscm_error_report("cscm_ast_free_tree", \
				CSCM_ERROR_AST_NODE_TYPE);
	}
}




#define CSCM_AST_READ_AHEAD		1
#define CSCM_AST_NOT_READ_AHEAD		0


/* the terminating NULL byte of string is added */
#define CSCM_AST_BUF_TEXT_SIZE	(CSCM_AST_TEXT_MAX_LEN + 1)
char text_buf[CSCM_AST_BUF_TEXT_SIZE];


// support symbolic list transformation: '(a b c) => (list 'a 'b 'c)
size_t _do_cscm_ast_build(FILE *file,	\
		CSCM_AST_NODE *exp,	\
		size_t level,		\
		int flag_read_ahead) // true: whole file; false: first expression
{
	int i, end;

	int c;
	size_t line;

	size_t text_len;
	char *next_char;

	CSCM_AST_NODE *new_subexp, *new_symbol;


	line = exp->line;
	while (1) {
		c = fgetc(file);


		if (c == EOF) {
			if (level != 0)
				cscm_syntax_error_report(	\
						exp->filename,	\
						line,		\
						CSCM_ERROR_AST_EOF);
			else
				return line;
		} else if (c == ' ' || c == '\t' || c == '\n') {
			do {
				if (c == '\n')
					line++;
				c = fgetc(file);
			} while (c == ' ' || c == '\t' || c == '\n');


			if (c == EOF) {
				if (level != 0)
					cscm_syntax_error_report(	\
							exp->filename,	\
							line,		\
							CSCM_ERROR_AST_EOF);
				else
					return line;
			} else {
				ungetc(c, file);
			}
		} else if (c == ';') { // scheme comment
			do {
				c = fgetc(file);
			} while (c != '\n' && c != EOF);


			if (c == EOF) {
				if (level != 0)
					cscm_syntax_error_report(	\
							exp->filename,	\
							line,		\
							CSCM_ERROR_AST_EOF);
				else
					return line;
			} else {
				line++;
			}
		} else if (c == '\'') {
			new_symbol = cscm_ast_symbol_create(		\
							exp->filename,	\
							line);
			cscm_ast_symbol_set(new_symbol, "quote");

			new_subexp = cscm_ast_exp_create(		\
							exp->filename,	\
							line);


			cscm_ast_exp_append(new_subexp, new_symbol);
			line = _do_cscm_ast_build(file,			\
						new_subexp,		\
						0,			\
						CSCM_AST_NOT_READ_AHEAD);


			cscm_ast_exp_append(exp, new_subexp);


			/* parsing of the first list is complete */
			if (!flag_read_ahead && level == 0)
				return line;
		} else if (c == '`') {
			new_symbol = cscm_ast_symbol_create(		\
							exp->filename,	\
							line);
			cscm_ast_symbol_set(new_symbol, "quasiquote");

			new_subexp = cscm_ast_exp_create(		\
							exp->filename,	\
							line);


			cscm_ast_exp_append(new_subexp, new_symbol);
			line = _do_cscm_ast_build(file,			\
						new_subexp,		\
						0,			\
						CSCM_AST_NOT_READ_AHEAD);


			cscm_ast_exp_append(exp, new_subexp);


			/* parsing of the first list is complete */
			if (!flag_read_ahead && level == 0)
				return line;
		} else if (c == ',') {
			new_symbol = cscm_ast_symbol_create(		\
							exp->filename,	\
							line);
			cscm_ast_symbol_set(new_symbol, "unquote");

			new_subexp = cscm_ast_exp_create(		\
							exp->filename,	\
							line);


			cscm_ast_exp_append(new_subexp, new_symbol);
			line = _do_cscm_ast_build(file,			\
						new_subexp,		\
						0,			\
						CSCM_AST_NOT_READ_AHEAD);


			cscm_ast_exp_append(exp, new_subexp);


			/* parsing of the first list is complete */
			if (!flag_read_ahead && level == 0)
				return line;
		} else if (c == '(') {
			new_subexp = cscm_ast_exp_create(exp->filename, \
							line);


			line = _do_cscm_ast_build(file,		\
						new_subexp,	\
						level + 1,	\
						flag_read_ahead);

			cscm_ast_exp_append(exp, new_subexp);

			/* parsing of the first list is complete */
			if (!flag_read_ahead && level == 0)
				return line;
		} else if (c == ')') {
			/*	A ")" that may end the AST construct process
			 * before the end-of-file. */
			if (level == 0)
				cscm_syntax_error_report(		\
						exp->filename,		\
						line,			\
						CSCM_ERROR_AST_CP);


			return line;
		} else if (c == '"') { // scheme string
			*text_buf = '"';

			next_char = text_buf + 1;
			text_len = 1;


			while(1) {
				c = fgetc(file);

				if (c == '\n') {
					line++;

					*next_char = '\n';
					next_char ++;

					text_len++;
				} else if (c == EOF) {
					cscm_syntax_error_report(	\
							exp->filename,	\
							line,		\
							CSCM_ERROR_AST_EOF);
				} else if (c == '"') {
					*next_char = '"';
					next_char++;

					text_len++;
					break;
				} else {
					*next_char = c;
					next_char++;

					text_len++;
				}
			}


			new_symbol = cscm_ast_symbol_create(exp->filename, \
							line);


			text_buf[text_len] = 0;
			cscm_ast_symbol_set(new_symbol, text_buf);


			cscm_ast_exp_append(exp, new_symbol);

			/* parsing of the first list is complete */
			if (!flag_read_ahead && level == 0)
				return line;
		} else { // symbol, not subexp
			text_len = 0;
			next_char = text_buf;


			do {
				*next_char = c;

				next_char++;
				text_len++;


				c = fgetc(file);

				if (c == '\'')
					cscm_syntax_error_report(	\
							exp->filename,	\
							line,		\
							CSCM_ERROR_AST_SQUOTE);
				else if (c == '"')
					cscm_syntax_error_report(	\
							exp->filename,	\
							line,		\
							CSCM_ERROR_AST_DQUOTE);
			} while (c != ' ' && c != '\t' && c != '\n'	\
				&& c != '(' && c != ')' && c != EOF	\
				&& c != '\'' && c != '"');


			if (c == '(')
				cscm_syntax_error_report(	\
						exp->filename,	\
						line,		\
						CSCM_ERROR_AST_OP);
			else if (c == '\'')
				cscm_syntax_error_report(	\
						exp->filename,	\
						line,		\
						CSCM_ERROR_AST_SQUOTE);
			else if (c == '"')
				cscm_syntax_error_report(	\
						exp->filename,	\
						line,		\
						CSCM_ERROR_AST_DQUOTE);
			else if (c != EOF)
				ungetc(c, file);


			new_symbol = cscm_ast_symbol_create(exp->filename, \
							line);


			text_buf[text_len] = 0;
			cscm_ast_symbol_set(new_symbol, text_buf);


			cscm_ast_exp_append(exp, new_symbol);

			/* parsing of the first list is complete */
			if (!flag_read_ahead && level == 0)
				return line;


			if (c == EOF) {
				if (level != 0)
					cscm_syntax_error_report(	\
							exp->filename,	\
							line,		\
							CSCM_ERROR_AST_EOF);
				else
					return line;
			}
		}
	}
}




CSCM_AST_NODE *cscm_ast_build(FILE *script, char *filename)
{
	CSCM_AST_NODE *exp;
	CSCM_AST_NODE *symbol_begin;


	exp = cscm_ast_exp_create(filename, 1);
	_do_cscm_ast_build(script, exp, 0, CSCM_AST_READ_AHEAD);


	// wrap a begin expression outside the entire script
	symbol_begin = cscm_ast_symbol_create(filename, 1);
	cscm_ast_symbol_set(symbol_begin, "begin");

	cscm_ast_exp_insert_first(exp, symbol_begin);


	return exp;
}


/* only read and parse the first list in the file */
CSCM_AST_NODE *cscm_list_ast_build(FILE *file, char *filename)
{
	CSCM_AST_NODE *exp, *ret;


	exp = cscm_ast_exp_create(filename, 1);
	_do_cscm_ast_build(file, exp, 0, CSCM_AST_NOT_READ_AHEAD);


	if (exp->n_childs != 1)
		cscm_error_report("cscm_list_ast_build", \
				CSCM_ERROR_LIST_AST_N_EXPS);

	ret = cscm_ast_exp_index(exp, 0);

	cscm_ast_free_exp(exp);


	return ret;
}




void cscm_ast_print_tree(CSCM_AST_NODE *node)
{
	int i;
	CSCM_AST_NODE *child;


	if (node == NULL)
		cscm_error_report("cscm_ast_print_tree", \
				CSCM_ERROR_NULL_PTR);


	if (cscm_ast_is_symbol(node)) {
		if (cscm_ast_is_symbol_empty(node))
			cscm_error_report("cscm_ast_print_tree", \
					CSCM_ERROR_AST_EMPTY_SYMBOL);
		else
			fputs(node->text, stdout);
	} else if (cscm_ast_is_exp(node)) {
		if (cscm_ast_is_exp_empty(node)) {
			fputs("()", stdout);
		} else {
			fputc('(', stdout);

			child = cscm_ast_exp_index(node, 0);
			cscm_ast_print_tree(child);

			for (i = 1; i < node->n_childs; i++) {
				fputc(' ', stdout);
				child = cscm_ast_exp_index(node, i);
				cscm_ast_print_tree(child);
			}

			fputc(')', stdout);
		}
	} else {
		cscm_error_report("cscm_ast_print_tree", \
				CSCM_ERROR_AST_NODE_TYPE);
	}
}
