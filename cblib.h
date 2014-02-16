/*******************************************************************************
 * cblib --	Collection of builtin functions.
 ******************************************************************************/

#ifndef CBLIB_H
#define CBLIB_H


#include "value.h"
#include "stack.h"

// builtin functions
CbValue* bif_writeln(stack_t* arg_stack);
CbValue* bif_mod(stack_t* arg_stack);
CbValue* bif_valtype(stack_t* arg_stack);
CbValue* bif_str(stack_t* arg_stack);
CbValue* bif_eval(stack_t* arg_stack);
#ifdef _CBC_PLAT_WNDS
CbValue* bif_meld(stack_t* arg_stack);
#endif // _CBC_PLAT_WNDS


#endif // CBLIB_H
