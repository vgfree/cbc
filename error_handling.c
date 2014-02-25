/*******************************************************************************
 * error_handling -- Collection of error handling utilities.
 *                   This also includes the yyerror()-function for flex & bison.
 * 
 *    Used macros:
 *      - _CBC_USE_CUSTOM_YYERROR_MESSAGE:
 *            Enables custom parser and syntax error messages.
 *            If this is not set, standard bison error messages will be used.
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "error_handling.h"


// #############################################################################
// declarations
// #############################################################################

// Default error output stream
FILE* err_out = NULL;

static void cb_print_error_internal(FILE* output, const char* format,
									va_list* args);
static void cb_appropriately_set_default_error_output_internal();


// #############################################################################
// interface functions for flex & bison
// #############################################################################

// -----------------------------------------------------------------------------
// Basic function for reporting parser and lexer errors.
// This function is called as soon as an error occurs during parsing stage.
// -----------------------------------------------------------------------------
void yyerror(void* param, const char* format, ...)
{
	extern int yylineno;
	
#ifdef _CBC_USE_CUSTOM_YYERROR_MESSAGE
	#include "cbc_parse.h"	// this is necessary for yy_get_token_string() !
	
	cb_print_error(CB_ERR_SYNTAX, yylineno, "Unexpected token %s",
				   yy_get_token_string(&yychar));
#else
	// allocate base buffer
	size_t base_len = strlen(format) + 1;
	char* buffer = (char*) malloc(base_len);
	
	va_list arglist;
	va_start(arglist, format);
	
	size_t needed = vsnprintf(buffer, base_len, format, arglist);
	// if there were any additional arguments passed to the function,
	// the buffer needs to be increased.
	if (needed > base_len)
	{
		buffer = (char*) realloc(buffer, needed);
		vsprintf(buffer, format, arglist);
	}
	
	cb_print_error(CB_ERR_SYNTAX, yylineno, buffer);
	
	free(buffer);	// always free the allocated buffer
	
	va_end(arglist);
#endif // _CBC_USE_CUSTOM_YYERROR_MESSAGE
}


// #############################################################################
// interface functions
// #############################################################################

// -----------------------------------------------------------------------------
// Print error message and specific error information
// -----------------------------------------------------------------------------
void cb_print_error(enum cb_error_type type, int line, const char* format, ...)
{
	cb_appropriately_set_default_error_output_internal();
	
	switch (type)
	{
		case CB_ERR_RUNTIME:
			fprintf(err_out, "Runtime error: ");
			break;
		
		case CB_ERR_SYNTAX:
			fprintf(err_out, "Syntax error: ");
			break;
		
		default:
			fprintf(err_out, "Unknown error: ");
			break;
	}
	
	if (line > 0)
		fprintf(err_out, "Line %d: ", line);
	
	va_list arglist;
	va_start(arglist, format);
	cb_print_error_internal(err_out, format, &arglist);
	va_end(arglist);
}

// -----------------------------------------------------------------------------
// Print error message only
// -----------------------------------------------------------------------------
void cb_print_error_msg(const char* format, ...)
{
	cb_appropriately_set_default_error_output_internal();
	
	va_list arglist;
	fprintf(err_out, "Error: ");
	va_start(arglist, format);
	cb_print_error_internal(err_out, format, &arglist);
	va_end(arglist);
}

// -----------------------------------------------------------------------------
// Set the output stream for error messages
// -----------------------------------------------------------------------------
void cb_set_error_output(FILE* error_ouput)
{
	assert(error_ouput);
	
	err_out = error_ouput;
}


// #############################################################################
// internal functions
// #############################################################################

// -----------------------------------------------------------------------------
// Format and print message (internal)
// -----------------------------------------------------------------------------
static void cb_print_error_internal(FILE* output, const char* format,
									va_list* args)
{
	vfprintf(output, format, *args);	// print formatted message
	fprintf(output, "\n");
}

// -----------------------------------------------------------------------------
// set default error output stream
// -----------------------------------------------------------------------------
static void cb_appropriately_set_default_error_output_internal()
{
	// set default error output stream if necessary
	if (!err_out)
		err_out = stderr;
}
