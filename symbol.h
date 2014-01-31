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

// symbol_t struct
typedef struct symbol_t
{
	enum symbol_type_t type;	// type
	char* id;					// identifier
	struct symbol_t* next;		// reference to the next symbol (in case of a list)
	struct symbol_t* previous;	// reference to the previous symbol (in case of a list)
	const scope_t* scope;		// scope, in which the symbol is valid
	
	union
	{
		value_t* value;			// symbol-value, is used if the symbol is a variable
		function_t* function;	// function, is used if the symbol is a function
	};
} symbol_t;


// interface-functions
symbol_t* symbol_create_variable(char* identifier);
symbol_t* symbol_create_function(char* identifier, function_t* func_object);
void symbol_free(symbol_t* s);
void symbol_connect(symbol_t* s1, symbol_t* s2);


#endif // SYMBOL_H
