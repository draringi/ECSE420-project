%{
#include "parser.tab.h"

%}

%option outfile="common/parser.yy.c" header-file="common/parser.yy.h"
%option warn nodefault
%option yylineno

DIGIT [0-9]
INT {DIGIT}+
%%
"x"		return tTIMES;
","		return tCOMMA;
"("		return tOPEN_BRACKET;
")"		return tCLOSE_BRACKET;
{INT}		{
			yylval.intconst = atoi(yytext);
			return tINT;
		}
[ \t\n]+	/* Whitespace, Ignore */;
.		yyerror();
