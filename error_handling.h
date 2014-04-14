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

typedef int CbErrorType;

// interface functions for flex & bison
void yyerror(void* call_context, const char* format, ...);

// interface functions
void cb_print_error(cb_error_type type, int line, const char* message, ...);
void cb_print_error_msg(const char* format, ...);
void cb_set_error_output(FILE* error_ouput);
void cb_error_set_code(CbErrorType code);
void cb_error_set();
void cb_error_set_msg(const char* message);
CbErrorType cb_error_get();
const char* cb_error_get_msg();
void cb_error_clear();
CbErrorType cb_error_get();


#endif // ERROR_HANDLING_H
