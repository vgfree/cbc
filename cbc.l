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
				yylval.id = strdup(yytext);
				return IDENTIFIER;
			}
			
			/* single character operators */
[-+*/,()]	{ return *yytext; }
			
			/* comparison operators */
"="			{ yylval.cmp = CMP_EQ; return COMPARE; }
"<>"		{ yylval.cmp = CMP_NE; return COMPARE; }
">="		{ yylval.cmp = CMP_GE; return COMPARE; }
"<="		{ yylval.cmp = CMP_LE; return COMPARE; }
">"			{ yylval.cmp = CMP_GT; return COMPARE; }
"<"			{ yylval.cmp = CMP_LT; return COMPARE; }
			
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
