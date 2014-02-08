/*******************************************************************************
 * builtin -- Collection of functions to register builtin and pre-defined
 *  		  symbols like functions and variables/constants.
 ******************************************************************************/

#ifndef BUILTIN_H
#define BUILTIN_H


#include "value.h"
#include "symtab_if.h"
#include "stack.h"

// function-pointer to a generic builtin function
typedef value_t* (*func_ref_t) (stack_t*);


// interface functions
int register_builtin_func(symtab_t* symtab, char* identifier,
						  func_ref_t func, int expected_param_count);
int register_builtin_all(symtab_t* symtab);


#endif // BUILTIN_H
