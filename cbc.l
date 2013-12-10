	/* DEFINITIONS ---------------------------------------------------------- */


%option yylineno

%{

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
// include syntree.h BEFORE any other parser-specific header-files!
// otherwise the build would fail due to unknown type-names.
#include "syntree.h"
#include "cbc_parse.h"
#include "errors.h"

%}


%%	/* RULES ---------------------------------------------------------------- */


			/* numbers */
[0-9]+		{
				yylval.value = atoi(yytext);
				return NUMBER;
			}
			
			/* operators */
[-+*/,()]	{ return *yytext; }

			/* whitespaces */
[ \t\n]+	{ ; }	// ignore

			/* anything else */
.			{ yyerror("unknown character `%c'.", *yytext); }


%%	/* ROUTINES ------------------------------------------------------------- */


int yywrap(void)
{
	return 1;
}
