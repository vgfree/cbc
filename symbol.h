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

// enum symbol_type_t -- type of the symbol
enum symbol_type_t
{
	SYM_TYPE_UNDEFINED = 0,
	SYM_TYPE_VARIABLE,
	SYM_TYPE_FUNCTION
};

// enum symbol_state_t -- state of the symbol
enum symbol_state_t
{
	SYM_STATE_INITIAL = 0,
	SYM_STATE_HASID
};

// symbol_t struct
typedef struct cb_symbol_t
{
	enum symbol_type_t type;		// type
	enum symbol_state_t state;		// current state
	char* id;						// identifier
	struct cb_symbol_t* next;		// reference to the next symbol (in case of a list)
	struct cb_symbol_t* previous;	// reference to the previous symbol (in case of a list)
	
	union
	{
		value_t* value;				// symbol-value, is used if the symbol is a variable
		function_t* function;		// function, is used if the symbol is a function
	};
} symbol_t;


// interface-functions
symbol_t* symbol_create();
void symbol_free(symbol_t* s);
void symbol_setid(symbol_t* s, char* id);
void symbol_connect(symbol_t* s1, symbol_t* s2);
void symbol_settype(symbol_t* s, enum symbol_type_t type);


#endif // SYMBOL_H
