	/* DEFINITIONS ---------------------------------------------------------- */
%{

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "cbc_parse.h"

#define CBC_LOG_FILENAME "cbc_output.log"

// error format function
void yyerror(const char* format, ...);

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

// -----------------------------------------------------------------------------
// andvanced error output
// -----------------------------------------------------------------------------
void yyerror(const char* format, ...)
{
	va_list arglist;
	va_start(arglist, format);

	// add line feed to message
	char* message	= (char*) malloc(strlen(format) + 2);
	*message		= '\0';
	strcpy(message, format);
	strcat(message, "\n");
#ifdef _CBC_DEBUG
	// actual print
	vprintf(message, arglist);
#endif // _CBC_DEBUG
#ifndef _CBC_NOLOG
	// log to file
	FILE* log_file = fopen(CBC_LOG_FILENAME, "a+");
	vfprintf(log_file, message, arglist);
	fclose(log_file);
#endif // _CBC_NOLOG
	// free memory
	free(message);

	va_end(arglist);
}
