	/* DEFINITIONS ---------------------------------------------------------- */


%option yylineno

%{

#include <stdio.h>
#include <stdarg.h>
#include "cbc_parse.h"
#include "error_handling.h"

%}


%%	/* RULES ---------------------------------------------------------------- */


			/* numbers */
[0-9]+		{
				yylval.val = atoi(yytext);
				return NUMBER;
			}
			
			/* keywords */
"if"		{ return IF; }
"then"		{ return THEN; }
"else"		{ return ELSE; }
"endif"		{ return ENDIF; }
"while"		{ return WHILE; }
"do"		{ return DO; }
"end"		{ return END; }
"function"	{ return FUNCTION; }
"print"		{ return PRINT; }

			/* logical gates */
"and"		{ return AND; }
"or"		{ return OR; }
"not"		{ return NOT; }
			
			/* boolean values */
"True"		{
				yylval.boolval = true;
				return BOOLEAN;
			}
"False"		{
				yylval.boolval = false;
				return BOOLEAN;
			}
			
			/* identifiers */
[_a-zA-Z][_a-zA-Z0-9]* {
				yylval.id = strdup(yytext);
				return IDENTIFIER;
			}
			
			/* string literals */
\'([^']|'')*\' {
				// string literals are always in one line only.
				// the apostrophe can be escaped by writing a double apostrophe.
				char* str	= (char*) malloc(yyleng - 1);
				*str		= '\0';	// terminate string
				
				// omit first and last char, which are both an apostrophe
				yytext[yyleng - 1]	= '\0';
				strcpy(str, (yytext + 1));
				
				// replace all occourences of a double apostrophe with a single
				// one.
				// ATTENTION:	this causes a source and destination overlap in
				//				strcat!
				char* p		= str;
				char* found	= NULL;
				while (found = strstr(p, "''"))
				{
					*found	= '\0';
					p		= found + 1;
					strcat(str, p);
				}
				
				yylval.str = str;
				return STRING;
			}
			
			/* single character operators */
[-+*/,()|]	{ return *yytext; }
			
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
.			{
				yyerror((void*) CB_YYERROR_CC_FLEX,
						"Unexpected character `%c'.", *yytext);
			}


%%	/* ROUTINES ------------------------------------------------------------- */


int yywrap(void)
{
	return 1;
}
