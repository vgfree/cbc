/*******************************************************************************
 * error_handling -- Collection of error handling utilities.
 *                   This also includes the yyerror()-function for flex & bison.
 ******************************************************************************/

#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H


#include <stdio.h>

typedef enum cb_error_type
{
	CB_ERR_UNKNOWN,
	CB_ERR_SYNTAX,
	CB_ERR_RUNTIME
} cb_error_type;

typedef enum cb_error_code
{
	CB_ERR_CODE_NOERROR
} cb_error_code;

typedef enum cb_yyerror_call_context
{
	CB_YYERROR_CC_BISON = 0,
	CB_YYERROR_CC_FLEX
} cb_yyerror_call_context;


// interface functions for flex & bison
void yyerror(void* call_context, const char* format, ...);

// interface functions
void cb_print_error(enum cb_error_type type, int line, const char* message, ...);
void cb_print_error_msg(const char* format, ...);
void cb_set_error_output(FILE* error_ouput);


#endif // ERROR_HANDLING_H
