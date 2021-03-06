/*******************************************************************************
 * error_handling -- Collection of error handling utilities.
 *                   This also includes the yyerror()-function for flex & bison.
 ******************************************************************************/

#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H


#include <stdio.h>
#include <stdbool.h>

typedef enum cb_error_type
{
    CB_ERR_UNKNOWN,
    CB_ERR_SYNTAX,
    CB_ERR_RUNTIME
} cb_error_type;

typedef enum cb_yyerror_call_context
{
    CB_YYERROR_CC_BISON = 0,
    CB_YYERROR_CC_FLEX
} cb_yyerror_call_context;

typedef enum cb_error_code
{
    CB_ERR_CODE_NOERROR = 0,    // 0 indicates "no error"
    CB_ERR_CODE_DIVISIONBYZERO, // Division by zero error
    
    CB_ERR_CODE_END             // End of enumerations (this is not an error!)
} CbErrorCode;

// Error code for custom error messages
extern const CbErrorCode CB_ERR_CODE_CUSTOMERROR;

// interface functions for flex & bison
void yyerror(void* call_context, const char* format, ...);

// interface functions
void cb_print_error(cb_error_type type, int line, const char* message, ...);
void cb_print_error_msg(const char* format, ...);
void cb_set_error_output(FILE* error_ouput);
void cb_error_set(CbErrorCode code);
void cb_error_set_msg(const char* message);
CbErrorCode cb_error_get();
const char* cb_error_get_custom_message();
const char* cb_error_get_message();
bool cb_error_is_set();
void cb_error_clear();
void cb_error_catch();
void cb_error_reset_catch();
bool cb_error_is_catched();
void cb_error_handling_initialize();
void cb_error_handling_finalize();
bool cb_error_handling_is_initialized();


#endif // ERROR_HANDLING_H
