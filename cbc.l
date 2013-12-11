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
			
			/* keywords */
"declare"	{ return DECLARE; }
"if"		{ return IF; }
"then"		{ return THEN; }
"else"		{ return ELSE; }
"endif"		{ return ENDIF; }
"while"		{ return WHILE; }
"do"		{ return DO; }
"end"		{ return END; }
			
			/* identifier */
[_a-zA-Z][_a-zA-Z0-9]*	{
				// check if symbol is declared
				symbol* s = symtab_lookup(gl_symtab, yytext);
				if (!s)
					// create dummy-symbol
					s = symbol_create(SYM_UNDEFINED, yytext);
				
				yylval.sym = s;
				return IDENTIFIER;
			}
			
			/* single character operators */
[-+*/,()]	{ return *yytext; }
			
			/* assignment operator */
":="		{ return ASSIGN; }
			
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
