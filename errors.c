/*******************************************************************************
 * errors -- collection of error-output functions
 ******************************************************************************/

#include "errors.h"

extern int yylineno;

// -----------------------------------------------------------------------------
// write error to file (internal)
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
	FILE* log_file = fopen(CBC_LOG_FILENAME, "a+");
	fyyerror(log_file, format, &arglist);
	fclose(log_file);
#endif // _CBC_NOLOG
	va_end(arglist);
}
