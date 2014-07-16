/*******************************************************************************
 * builtin -- Collection of functions to register builtin and pre-defined
 *            symbols like functions and variables/constants.
 ******************************************************************************/

#ifndef BUILTIN_H
#define BUILTIN_H


#include "value.h"
#include "symtab_if.h"
#include "stack.h"

// function-pointer to a generic builtin function
typedef CbValue* (*CbBuiltinFunctionRef) (CbStack*);


// interface functions
int register_builtin_func(CbSymtab* symtab, char* identifier,
                          CbBuiltinFunctionRef func, int expected_param_count);
int register_builtin_all(CbSymtab* symtab);


#endif // BUILTIN_H
