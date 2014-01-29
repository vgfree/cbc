	/* DEFINITIONS ---------------------------------------------------------- */


%option yylineno

%{

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "value.h"
#include "syntree.h"
#include "strlist.h"
#include "cbc_parse.h"

#ifndef _CBC_NOLOG
#define _CBC_LOG_FILENAME "cbc.log"
#endif // _CBC_NOLOG

void yyerror(const char* format, ...);

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
.			{ yyerror("unexpected character `%c'.", *yytext); }


%%	/* ROUTINES ------------------------------------------------------------- */


int yywrap(void)
{
	return 1;
}

// -----------------------------------------------------------------------------
// write error message to file (internal)
// -----------------------------------------------------------------------------
void fyyerror(FILE* fd, const char* format, va_list* args)
{
	fprintf(fd, "%d: error: ", yylineno);
	vfprintf(fd, format, *args);
	fprintf(fd, "\n");
}

// -----------------------------------------------------------------------------
// general error output
// -----------------------------------------------------------------------------
void yyerror(const char* format, ...)
{
	va_list arglist;
	va_start(arglist, format);
#ifdef _CBC_DEBUG
	// actual output to stderr
	fyyerror(stderr, format, &arglist);
#endif // _CBC_DEBUG
#ifndef _CBC_NOLOG
	// log to file
	FILE* log_file = fopen(_CBC_LOG_FILENAME, "a+");
	fyyerror(log_file, format, &arglist);
	fclose(log_file);
#endif // _CBC_NOLOG
	va_end(arglist);
}
