	/* DEFINITIONS ---------------------------------------------------------- */


%option yylineno

%{

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
// include syntree.h BEFORE any other parser-specific header-files!
// otherwise the build would fail due to unknown type-names.
#include "syntree.h"
// same applies for symtab.h
#include "symtab.h"
#include "cbc_parse.h"
#include "errors.h"

%}


%%	/* RULES ---------------------------------------------------------------- */


			/* numbers */
[0-9]+		{
				yylval.value = atoi(yytext);
				return NUMBER;
			}
			
			/* identifier */
[_a-zA-Z][_a-zA-Z0-9]*	{
				// check if symbol is declared
				symbol* s = symtab_lookup(gl_symtab, yytext);
				if (!s)
				{
					yyerror("undefined symbol: %s", yytext);
					exit(1);
				}
				
				yylval.sym = s;
				return IDENTIFIER;
			}
			
			/* operators */
[-+*/,()]	{ return *yytext; }
			
			/* end of file */
<<EOF>>		{ return ENDOFFILE; }
			
			/* comments */
"//".*		{ ; }	// ignore
			
			/* whitespaces */
[ \t\n]+	{ ; }	// ignore
			
			/* anything else */
.			{ yyerror("unknown character `%c'.", *yytext); }


%%	/* ROUTINES ------------------------------------------------------------- */


int yywrap(void)
{
	return 1;
}
