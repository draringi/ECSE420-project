%{
#include <stdio.h>
#include <stdlib.h>
#include "parser.yy.h"
void code_errormsg(char*);
void yyerror() { code_errormsg("syntax error"); }
void code_errormsg(char* reason) { fprintf (stderr, "Invalid: %s near %s on line %d\n", reason, yytext, yylineno); exit(EXIT_FAILURE); }
%}

%union {
	int intconst;
}

%token tTIMES tCOMMA tOPEN_BRACKET tCLOSE_BRACKET
%token <intconst> tINT

%start file

%%

file: matrix_def
	| matrix_def entry_list

matrix_def: tINT tTIMES tINT { define_matrix($1, $3);  }

entry_list: 	entry
	|	entry_list entry

entry: tOPEN_BRACKET tINT tCOMMA tINT tCLOSE_BRACKET { add_entry($2, $4); }
