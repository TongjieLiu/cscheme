/* ast.h -- Abstract Syntax Tree

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

#ifndef __CSCM_AST_H__

#define __CSCM_AST_H__




#include <stddef.h>
#include <stdio.h>




#define CSCM_AST_NODE_TYPE_EXP		0
#define CSCM_AST_NODE_TYPE_SYMBOL	1




#define CSCM_AST_TEXT_MAX_LEN		256

#define CSCM_AST_EXP_PAGE_MAX_N		16
#define CSCM_AST_EXP_PAGE_SIZE		128




struct _CSCM_AST_NODE {
	int type;


	char *filename;	// source program path
	size_t line;	// source program line number


	char *text;					// symbol


	size_t n_childs;				// expression

	/* child nodes are divided into pages */
	struct _CSCM_AST_NODE ***pages;			// expression
};


typedef struct _CSCM_AST_NODE CSCM_AST_NODE;




int cscm_ast_is_symbol(CSCM_AST_NODE *node);
int cscm_ast_is_exp(CSCM_AST_NODE *node);


int cscm_ast_is_symbol_empty(CSCM_AST_NODE *node);
int cscm_ast_is_exp_empty(CSCM_AST_NODE *node);




CSCM_AST_NODE **cscm_ast_node_ptrs_create(size_t n);


CSCM_AST_NODE *cscm_ast_exp_create(char *filename, size_t line);
CSCM_AST_NODE *cscm_ast_symbol_create(char *filename, size_t line);




int cscm_ast_symbol_text_equal(CSCM_AST_NODE *symbol, char *text);


CSCM_AST_NODE *cscm_ast_exp_index(CSCM_AST_NODE *exp, size_t index);




void cscm_ast_symbol_set(CSCM_AST_NODE *symbol, char *text);
void cscm_ast_symbol_set_simple(CSCM_AST_NODE *symbol, char *text);


void cscm_ast_exp_append(CSCM_AST_NODE *exp, CSCM_AST_NODE *new_child);
void cscm_ast_exp_drop_first(CSCM_AST_NODE *exp);
void cscm_ast_exp_insert_first(CSCM_AST_NODE *exp, CSCM_AST_NODE *new_child);


void cscm_ast_exp_empty(CSCM_AST_NODE *exp);
void cscm_ast_exp_mv(CSCM_AST_NODE *src, CSCM_AST_NODE *dest);




void cscm_ast_free_symbol(CSCM_AST_NODE *symbol);
void cscm_ast_free_exp(CSCM_AST_NODE *exp);
void cscm_ast_free_tree(CSCM_AST_NODE *node);




CSCM_AST_NODE *cscm_ast_build(FILE *script, char *filename);
CSCM_AST_NODE *cscm_list_ast_build(FILE *file, char *filename);




#define CSCM_ERROR_AST_NODE_TYPE		"Incorrect ast node type"
#define CSCM_ERROR_AST_BAD_INDEX		"bad index"
#define CSCM_ERROR_AST_NO_CTEXT			"no text to compare"
#define CSCM_ERROR_AST_NO_STEXT			"no text to set"


#define CSCM_ERROR_AST_EMPTY_SYMBOL		"empty CSCM_AST_NODE(symbol)"
#define CSCM_ERROR_AST_EMPTY_EXP		"empty CSCM_AST_NODE(exp)"
#define CSCM_ERROR_AST_FULL_EXP			"CSCM_AST_NODE(exp) is full"


#define CSCM_ERROR_AST_EOF			"unexpected EOF"
#define CSCM_ERROR_AST_LC			"unexpected line continuation"
#define CSCM_ERROR_AST_SQUOTE			"unexpected SINGLE-QUOTE(\')"
#define CSCM_ERROR_AST_DQUOTE			"unexpected DOUBLE-QUOTE(\")"
#define CSCM_ERROR_AST_CP			"unexpected \")\""
#define CSCM_ERROR_AST_OP			"unexpected \"(\""



#define CSCM_ERROR_LIST_AST_N_EXPS		"more than 1 expression is parsed"




#endif
