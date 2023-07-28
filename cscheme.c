/* cscheme.c -- the entry point

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

#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "object.h"
#include "error.h"
#include "ast.h"
#include "core.h"
#include "env.h"
#include "pair.h"
#include "num.h"
#include "symbol.h"
#include "ef.h"
#include "gc.h"
#include "debug.h"
#include "text.h"
#include "cscheme.h"




#define CSCM_COPYRIGHT_MSG \
"cscheme  Copyright (C) 2021  Tongjie Liu\n"                             \
"This program comes with ABSOLUTELY NO WARRANTY. This is free soft-\n"   \
"ware and you are welcome to redistribute it. Program license: GPLv3,\n" \
"for details see <http://gnu.org/licenses/gpl.html>."


#define CSCM_USAGE_MSG \
"cscheme [options] file ...\n"					\
"options: -h\n"							\
"         --docs\n"						\
"         --debug\n"						\
"file: SCRIPT\n"						\
"      -(STDIN)\n"						\
"\nThere can be arguments for the script after \"file\"."


void cscm_print_usage()
{
	puts(CSCM_COPYRIGHT_MSG);
	puts("");
	puts(CSCM_USAGE_MSG);

	exit(0);
}




void cscm_print_basic_docs()
{
	puts("============ Basic Primitive Procedures ============");

	puts("(print object1 [object2] [object3] ...)");
	puts("(printn [object1] [object2] [object3] ...)");
	puts("(display object)");
	puts("(newline)");

	puts("");

	puts("(read) -> sequence");

	puts("");

	puts("(set-car! pair new-car)");
	puts("(set-cdr! pair new-cdr)");

	puts("");

	puts("(cons new-car new-cdr) -> sequence");
	puts("(car pair) -> object");
	puts("(cdr pair) -> object");
	puts("(cadr pair) -> object");
	puts("(cdar pair) -> object");
	puts("(cddr pair) -> object");
	puts("(caar pair) -> object");
	puts("(caaar pair) -> object");
	puts("(cdaar pair) -> object");
	puts("(cadar pair) -> object");
	puts("(caadr pair) -> object");
	puts("(cddar pair) -> object");
	puts("(caddr pair) -> object");
	puts("(cdadr pair) -> object");
	puts("(cdddr pair) -> object");

	puts("");

	puts("(list [object1] [object2] [object3] ...) -> sequence");

	puts("");

	puts("(+ number1 [number2] [number3] ...) -> long/double number");
	puts("(- number1 [number2] [number3] ...) -> long/double number");
	puts("(* number1 [number2] [number3] ...) -> long/double number");
	puts("(/ number1 [number2] [number3] ...) -> long/double number");

	puts("");

	puts("(remainder dividend divisor) -> long-number");

	puts("");

	puts("(= number1 number2) -> #t/#f");
	puts("(eq? symbol1 symbol2) -> #t/#f");
	puts("(eq? string1 string2) -> #t/#f");
	puts("(eq? bool1 bool2) -> #t/#f");
	puts("(equal? number1 number2) -> #t/#f");
	puts("(equal? symbol1 symbol2) -> #t/#f");
	puts("(equal? string1 string2) -> #t/#f");
	puts("(equal? bool1 bool2) -> #t/#f");

	puts("");

	puts("(> number1 number2) -> #t/#f");
	puts("(>= number1 number2) #t/#f");
	puts("(< number1 number2) -> #t/#f");
	puts("(<= number1 number2) -> #t/#f");

	puts("");

	puts("(string? object) -> #t/#f");
	puts("(symbol? object) -> #t/#f");
	puts("(number? object) -> #t/#f");
	puts("(pair? object) -> #t/#f");
	puts("(null? object) -> #t/#f");

	puts("");

	puts("(include module-name)");
	puts("	module-name: \"seq\"");

	puts("");

	puts("(max number1 [number2] [number3] ...) -> number");
	puts("(min number1 [number2] [number3] ...) -> number");

	puts("");

	puts("(apply proc argument-list) -> object");
	puts("(not object) -> #t/#f");
}




void cscm_print_seq_docs()
{
	puts("======================== seq ==========================");
	puts("(sort cmp-proc seq) -> new-sequence\n");

	puts("(length seq) -> long-number\n");

	puts("(list-ref seq index) -> object\n");

	puts("(range first-number last-number [step-number]) -> sequence\n");

	puts("(append seq-x seq-y) -> new-sequence\n");

	puts("(reverse seq) -> new-sequence\n");

	puts("(list-copy seq) -> new-sequence\n");

	puts("(map proc seq) -> new-sequence");
	puts("	proc: (proc current-item) -> item\n");

	puts("(for-each action seq)\n");

	puts("(filter pred seq) -> new-sequence\n");

	puts("(accumulate proc initial seq) -> object");
	puts("	proc: (proc current-item last-result)\n");

	puts("(fold-left proc initial seq) -> object");
	puts("	proc: (proc last-result current-item)");
}




void cscm_print_symbol_docs()
{
	puts("======================= symbol ========================");
	puts("(symbol number) -> symbol");
	puts("(symbol symbol) -> symbol");
	puts("(symbol string) -> symbol\n");

	puts("(symbol-append symbol1 [symbol2] [symbol3] ...)\n");
}




#define CSCM_DOCS_MSG \
"	cscheme is consistent with Structure and Interpretation of Computer\n"  \
"Programs, 2nd edition in function prototypes of all primitive procedures\n"    \
"shared between cscheme standard library and the Wizard Book.\n"                \
"	All module names used here are valid operands for the \"include pri-\n" \
"mitive procedure\". Note that the \"basic\" module is loaded by default.\n"


void cscm_print_docs()
{
	puts(CSCM_DOCS_MSG);

	cscm_print_basic_docs();

	puts("\n");

	cscm_print_seq_docs();

	puts("\n");

	cscm_print_symbol_docs();
}




/* Module Tests */
#define CSCM_TEST_AST_MOD_PREFIX_MAX_LEN	80

// the terminating NULL byte of string is added
char cscm_test_ast_mod_prefix_buf[CSCM_TEST_AST_MOD_PREFIX_MAX_LEN + 1];


void _do_cscm_test_ast_mod(CSCM_AST_NODE *node, size_t prefix_len)
{
	int i;

	size_t new_prefix_len;


	if (prefix_len > CSCM_TEST_AST_MOD_PREFIX_MAX_LEN) {
		cscm_error_report("_do_cscm_test_ast_mod", \
				CSCM_ERROR_CSCHEME_TEST_AST_MOD_EOL);
	} else {
		memset(cscm_test_ast_mod_prefix_buf, ' ', prefix_len);
		cscm_test_ast_mod_prefix_buf[prefix_len] = 0;
	}


	fputs(cscm_test_ast_mod_prefix_buf, stdout);


	if (cscm_ast_is_symbol(node)) {
		printf("line %lu: %s\n",			\
				(unsigned long)node->line,	\
				node->text);
	} else if (cscm_ast_is_exp(node)) {
		printf("line %lu: %lu subexps:\n",		\
				(unsigned long)node->line,	\
				(unsigned long)node->n_childs);
	} else {
		cscm_error_report("_do_cscm_test_ast_mod", \
				CSCM_ERROR_AST_NODE_TYPE);
	}


	new_prefix_len = prefix_len + 4;

	for (i = 0; i < node->n_childs; i++)
		_do_cscm_test_ast_mod(cscm_ast_exp_index(node, i), \
				new_prefix_len);
}


void cscm_test_ast_mod(CSCM_AST_NODE *exp)
{
	puts("ast.c is in testing ...\n");

	_do_cscm_test_ast_mod(exp, 0);
}




int main(int argc, char *argv[])
{
	int i;

	FILE *script;
	char *script_name;
	int flag_read_stdin;

	char *option;
	CSCM_OBJECT *option_obj;
	CSCM_OBJECT **option_objs;
	CSCM_OBJECT *internal_argc, *internal_argv;

	struct sigaction sigaction_abrt;

	CSCM_AST_NODE *exp;

	CSCM_OBJECT *global_env;

	CSCM_OBJECT *result;


	#ifdef __CSCM_GC_DEBUG__
		cscm_gc_show_total_object_count("HANDLE-CLI-OPTIONS");
	#endif

	flag_read_stdin = 0;
	if (argc == 1 || !strcmp(argv[1], "-")) {
		if (argc > 2)
			cscm_error_report("main", \
					CSCM_ERROR_CSCHEME_ARGC);

		script = stdin;
		script_name = "-";

		flag_read_stdin = 1;


		internal_argc = cscm_num_long_create();
		cscm_num_long_set(internal_argc, 1);

		option_obj = cscm_symbol_create();
		cscm_symbol_set(option_obj, "-");

		internal_argv = cscm_pair_create();
		cscm_pair_set(internal_argv, option_obj, CSCM_NIL);
	} else if (!strcmp(argv[1], "-h")) {
		if (argc > 2)
			cscm_error_report("main", \
					CSCM_ERROR_CSCHEME_ARGC);

		cscm_print_usage();

		return 0;
	} else if (!strcmp(argv[1], "--docs")) {
		if (argc > 2)
			cscm_error_report("main", \
					CSCM_ERROR_CSCHEME_ARGC);

		cscm_print_docs();

		return 0;
	} else if (!strcmp(argv[1], "--debug")) {
		if (argc < 3)
			cscm_error_report("main", \
					CSCM_ERROR_CSCHEME_ARGC);


		cscm_debug_mode = 1;


		script = fopen(argv[2], "r");
		if (script == NULL)
			cscm_libc_fail("main", "fopen");
	
		script_name = argv[2];


		internal_argc = cscm_num_long_create();
		cscm_num_long_set(internal_argc, argc - 2);

		option_objs = cscm_object_ptrs_create(argc - 2);
		for (i = 2; i < argc; i++) {
			option = argv[i];

			if (cscm_text_is_integer(option)) {
				option_obj = cscm_num_long_create();
				cscm_num_long_set(option_obj, atol(option));
			} else if (cscm_text_is_fpn(option)) {
				option_obj = cscm_num_double_create();
				cscm_num_double_set(option_obj, atof(option));
			} else {
				option_obj = cscm_symbol_create();
				cscm_symbol_set(option_obj, option);
			}

			option_objs[i - 2] = option_obj;
		}

		internal_argv = cscm_list_create(argc - 2, option_objs);

		free(option_objs);
	} else {
		script = fopen(argv[1], "r");
		if (script == NULL)
			cscm_libc_fail("main", "fopen");
	
		script_name = argv[1];


		internal_argc = cscm_num_long_create();
		cscm_num_long_set(internal_argc, argc - 1);

		option_objs = cscm_object_ptrs_create(argc - 1);
		for (i = 1; i < argc; i++) {
			option = argv[i];

			if (cscm_text_is_integer(option)) {
				option_obj = cscm_num_long_create();
				cscm_num_long_set(option_obj, atol(option));
			} else if (cscm_text_is_fpn(option)) {
				option_obj = cscm_num_double_create();
				cscm_num_double_set(option_obj, atof(option));
			} else {
				option_obj = cscm_symbol_create();
				cscm_symbol_set(option_obj, option);
			}

			option_objs[i - 1] = option_obj;
		}

		internal_argv = cscm_list_create(argc - 1, option_objs);

		free(option_objs);
	}


	sigaction_abrt.sa_handler = cscm_sigabrt_handler;
	sigemptyset(&sigaction_abrt.sa_mask);
	sigaction_abrt.sa_flags = 0;
	sigaction(SIGABRT, &sigaction_abrt, NULL);


	#ifdef __CSCM_GC_DEBUG__
		cscm_gc_show_total_object_count("AST");
	#endif
	exp = cscm_ast_build(script, script_name);

	if (!flag_read_stdin)
		fclose(script);


	#ifdef __CSCM_CSCHEME_DEBUG__
		#ifdef __CSCM_GC_DEBUG__
			cscm_gc_show_total_object_count("TEST-AST");
		#endif
		cscm_test_ast_mod(exp);
	#endif


	#ifdef __CSCM_GC_DEBUG__
		cscm_gc_show_total_object_count("GLOBAL-ENV");
	#endif
	global_env = cscm_global_env_setup();
	cscm_gc_inc(global_env);

	cscm_env_add_var(global_env, "argc", internal_argc);
	cscm_env_add_var(global_env, "argv", internal_argv);


	#ifdef __CSCM_GC_DEBUG__
		cscm_gc_show_total_object_count("EVAL");
	#endif
	result = cscm_eval(exp, global_env);

	#ifdef __CSCM_CSCHEME_DEBUG__
		fputs("*** CSCHEME DEBUG INFO *** final result: ", stdout);

		if (result)
			cscm_object_print(result, stdout);

		puts("");
	#endif


	#ifdef __CSCM_GC_DEBUG__
		cscm_gc_show_total_object_count("FREE-AST");
	#endif
	cscm_ast_free_tree(exp);


	if (result) {
		cscm_gc_dec(result);
		cscm_gc_free(result);
	}


	cscm_gc_dec(global_env);
	cscm_gc_free(global_env);


	return 0;
}
