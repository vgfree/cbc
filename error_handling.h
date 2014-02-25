/*******************************************************************************
 * error_handling -- Error handling utilities.
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


// interface functions for flex & bison
void yyerror(void* param, const char* format, ...);

// interface functions
void cb_print_error(enum cb_error_type type, int line, const char* message, ...);
void cb_print_error_msg(const char* format, ...);
void cb_set_error_output(FILE* error_ouput);


#endif // ERROR_HANDLING_H
