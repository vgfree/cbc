/*******************************************************************************
 * symbol -- Implementation of a symbol-structure
 * 
 *  	A symbol can either be a variable or a function.
 *  	Every symbol contains a symbol-identifier, the type and the value of
 *  	the symbol.
 ******************************************************************************/

#ifndef SYMBOL_H
#define SYMBOL_H


#include "value.h"
#include "function.h"
#include "scope.h"

// enum symbol_type_t -- type of the symbol
enum symbol_type_t
{
	SYM_TYPE_UNDEFINED = 0,
	SYM_TYPE_VARIABLE,
	SYM_TYPE_FUNCTION
};

typedef struct symbol_t symbol_t;


// interface-functions
symbol_t* symbol_create_variable(char* identifier);
symbol_t* symbol_create_function(char* identifier, function_t* func_object);
void symbol_free(symbol_t* s);
void symbol_connect(symbol_t* s1, symbol_t* s2);
const char* symbol_get_id(const symbol_t* s);
symbol_t* symbol_get_next(const symbol_t* s);
symbol_t* symbol_get_previous(const symbol_t* s);
const scope_t* symbol_get_scope(const symbol_t* s);
void symbol_set_next(symbol_t* s, const symbol_t* next);
void symbol_set_previous(symbol_t* s, const symbol_t* previous);
void symbol_set_scope(symbol_t* s, const scope_t* scope);
const CbValue* symbol_variable_get_value(const symbol_t* s);
void symbol_variable_assign_value(symbol_t* s, const CbValue* new_value);
function_t* symbol_function_get_function(const symbol_t* s);


#endif // SYMBOL_H
