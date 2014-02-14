/*******************************************************************************
 * cblib --	Collection of builtin functions.
 ******************************************************************************/

#ifndef CBLIB_H
#define CBLIB_H


#include "value.h"
#include "stack.h"

// builtin functions
value_t* bif_writeln(stack_t* arg_stack);
value_t* bif_mod(stack_t* arg_stack);
value_t* bif_valtype(stack_t* arg_stack);
value_t* bif_str(stack_t* arg_stack);
value_t* bif_eval(stack_t* arg_stack);
#ifdef _CBC_PLAT_WNDS
value_t* bif_meld(stack_t* arg_stack);
#endif // _CBC_PLAT_WNDS


#endif // CBLIB_H
