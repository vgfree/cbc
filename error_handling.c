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

// Global error flag
CbErrorCode cb_global_error = 0;
char* cb_global_error_msg = NULL;

bool error_handling_initialized;

// Default error output stream
FILE* err_out = NULL;

static void cb_print_error_internal(FILE* output, cb_error_type type, int line,
									const char* format, va_list* args);
static void cb_print_error_msg_internal(FILE* output, const char* format,
										va_list* args);
static void cb_appropriately_set_default_error_output_internal();


// #############################################################################
// interface functions for flex & bison
// #############################################################################

// -----------------------------------------------------------------------------
// Basic function for reporting parser and lexer errors.
// This function is called as soon as an error occurs during parsing stage.
// -----------------------------------------------------------------------------
void yyerror(void* call_context, const char* format, ...)
{
	extern int yylineno;
	
	switch ((cb_yyerror_call_context) call_context)
	{
		case CB_YYERROR_CC_FLEX:
		{
			cb_appropriately_set_default_error_output_internal();
			va_list arglist;
			va_start(arglist, format);
			cb_print_error_internal(err_out, CB_ERR_SYNTAX, yylineno, format,
									&arglist);
			va_end(arglist);
			break;
		}
		
		case CB_YYERROR_CC_BISON:
		default:			// treat unknown call context as a call from bison
		{
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
			break;
		}
	}
}


// #############################################################################
// interface functions
// #############################################################################

// -----------------------------------------------------------------------------
// Print error message and specific error information
// -----------------------------------------------------------------------------
void cb_print_error(cb_error_type type, int line, const char* format, ...)
{
	cb_appropriately_set_default_error_output_internal();
	
	va_list arglist;
	va_start(arglist, format);
	cb_print_error_internal(err_out, type, line, format, &arglist);
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
	cb_print_error_msg_internal(err_out, format, &arglist);
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

// -----------------------------------------------------------------------------
// set global error
// -----------------------------------------------------------------------------
void cb_error_set_code(CbErrorCode code)
{
	assert(cb_error_handling_is_initialized());
	
	cb_global_error = code;
}

// -----------------------------------------------------------------------------
// set global error (default)
// -----------------------------------------------------------------------------
void cb_error_set()
{
	assert(cb_error_handling_is_initialized());
	
	cb_error_set_code(1);
}

// -----------------------------------------------------------------------------
// set global error and print error message
// -----------------------------------------------------------------------------
void cb_error_set_msg(const char* message)
{
	assert(cb_error_handling_is_initialized());
	
	if (cb_global_error_msg != NULL)
		free(cb_global_error_msg);			// free old error message
	
	cb_error_set();
	cb_global_error_msg = strdup(message);	// assign copy of error message
}

// -----------------------------------------------------------------------------
// Get global error flag
// -----------------------------------------------------------------------------
CbErrorCode cb_error_get()
{
	assert(cb_error_handling_is_initialized());
	
	return cb_global_error;
}

// -----------------------------------------------------------------------------
// Get global error message
// -----------------------------------------------------------------------------
const char* cb_error_get_msg()
{
	assert(cb_error_handling_is_initialized());
	
	return cb_global_error_msg;
}

// -----------------------------------------------------------------------------
// Clear global error flag
// -----------------------------------------------------------------------------
void cb_error_clear()
{
	cb_global_error = 0;
}

// -----------------------------------------------------------------------------
// Initialize error handling
// -----------------------------------------------------------------------------
void cb_error_handling_initialize()
{
	if (!cb_error_handling_is_initialized())
	{
		error_handling_initialized = true;
		cb_global_error_msg		   = NULL;
		cb_error_clear();
	}
}

// -----------------------------------------------------------------------------
// Finalize error handling
// -----------------------------------------------------------------------------
void cb_error_handling_finalize()
{
	if (cb_error_handling_is_initialized())
	{
		error_handling_initialized = false;
		if (cb_global_error_msg != NULL)
			free(cb_global_error_msg);
		
		cb_error_clear();
	}
}

// -----------------------------------------------------------------------------
// Check if error handling is initialized
// -----------------------------------------------------------------------------
bool cb_error_handling_is_initialized()
{
	return error_handling_initialized;
}


// #############################################################################
// internal functions
// #############################################################################

// -----------------------------------------------------------------------------
// Format and print message including additional information (internal)
// -----------------------------------------------------------------------------
static void cb_print_error_internal(FILE* output, cb_error_type type, int line,
									const char* format, va_list* args)
{
	switch (type)
	{
		case CB_ERR_RUNTIME:
			fprintf(output, "Runtime error: ");
			break;
		
		case CB_ERR_SYNTAX:
			fprintf(output, "Syntax error: ");
			break;
		
		default:
			fprintf(output, "Unknown error: ");
			break;
	}
	
	if (line > 0)
		fprintf(output, "Line %d: ", line);
	
	cb_print_error_msg_internal(output, format, args);
}

// -----------------------------------------------------------------------------
// Format and print message (internal)
// -----------------------------------------------------------------------------
static void cb_print_error_msg_internal(FILE* output, const char* format,
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
