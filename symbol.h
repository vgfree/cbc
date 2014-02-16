/*******************************************************************************
 * CbSymbol -- Implementation of a symbol-structure.
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

// type of the symbol
enum cb_symbol_type
{
	SYM_TYPE_UNDEFINED = 0,
	SYM_TYPE_VARIABLE,
	SYM_TYPE_FUNCTION
};

typedef struct CbSymbol CbSymbol;


// interface-functions
CbSymbol* cb_symbol_create_variable(char* identifier);
CbSymbol* cb_symbol_create_function(char* identifier, function_t* func_object);
void cb_symbol_free(CbSymbol* s);
void cb_symbol_connect(CbSymbol* s1, CbSymbol* s2);
const char* cb_symbol_get_id(const CbSymbol* s);
CbSymbol* cb_symbol_get_next(const CbSymbol* s);
CbSymbol* cb_symbol_get_previous(const CbSymbol* s);
const CbScope* cb_symbol_get_scope(const CbSymbol* s);
void cb_symbol_set_next(CbSymbol* s, const CbSymbol* next);
void cb_symbol_set_previous(CbSymbol* s, const CbSymbol* previous);
void cb_symbol_set_scope(CbSymbol* s, const CbScope* scope);
const CbValue* cb_symbol_variable_get_value(const CbSymbol* s);
void cb_symbol_variable_assign_value(CbSymbol* s, const CbValue* new_value);
function_t* cb_symbol_function_get_function(const CbSymbol* s);


#endif // SYMBOL_H
