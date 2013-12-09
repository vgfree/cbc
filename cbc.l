	/* DEFINITIONS ---------------------------------------------------------- */
%{

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
// include syntree.h BEFORE any other parser-specific header-files!
// otherwise the build would fail due to unknown type-names.
#include "syntree.h"
#include "cbc_parse.h"

#define CBC_LOG_FILENAME "cbc_output.log"

// error output format functions
void fyyerror(FILE* fd, const char* format, va_list* args);
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
// write error to file
// -----------------------------------------------------------------------------
void fyyerror(FILE* fd, const char* format, va_list* args)
{
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
	FILE* log_file = fopen(CBC_LOG_FILENAME, "a+");
	fyyerror(log_file, format, &arglist);
	fclose(log_file);
#endif // _CBC_NOLOG
	va_end(arglist);
}
