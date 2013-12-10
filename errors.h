/*******************************************************************************
 * errors -- collection of error-output functions
 ******************************************************************************/

#ifndef ERRORS_H
#define ERRORS_H

#include <stdio.h>
#include <stdarg.h>

#define CBC_LOG_FILENAME	"cbc_output.log"
#define ERR_BADALLOC		"out of memory"

// error output format functions
void yyerror(const char* format, ...);

#endif // ERRORS_H
