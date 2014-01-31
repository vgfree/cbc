/*******************************************************************************
 * symbol -- Implementation of a symbol-structure
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "symbol.h"


// #############################################################################
// declarations
// #############################################################################

// symbol_t struct
struct symbol_t
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
};


// #############################################################################
// implementation-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor (internal)
// -----------------------------------------------------------------------------
static symbol_t* symbol_create(char* identifier)
{
	symbol_t* s	= (symbol_t*) malloc(sizeof(symbol_t));
	s->type		= SYM_TYPE_UNDEFINED;
	s->id		= strdup(identifier);
	s->next		= NULL;
	s->previous	= NULL;
	s->scope	= NULL;
	
	return s;
}


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor (variable)
// -----------------------------------------------------------------------------
symbol_t* symbol_create_variable(char* identifier)
{
	symbol_t* s	= symbol_create(identifier);
	s->type		= SYM_TYPE_VARIABLE;
	s->value	= value_create();
	
	return s;
}

// -----------------------------------------------------------------------------
// constructor (function)
// -----------------------------------------------------------------------------
symbol_t* symbol_create_function(char* identifier, function_t* func_object)
{
	symbol_t* s	= symbol_create(identifier);
	s->type		= SYM_TYPE_FUNCTION;
	s->function	= func_object;
	
	return s;
}

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
void symbol_free(symbol_t* s)
{
	free(s->id);
	
	switch (s->type)
	{
		case SYM_TYPE_VARIABLE:
			value_free(s->value);
			break;
		
		case SYM_TYPE_FUNCTION:
			function_free(s->function);
			break;
	}
	
	free(s);
}

// -----------------------------------------------------------------------------
// connect two symbols with each other
// -----------------------------------------------------------------------------
void symbol_connect(symbol_t* s1, symbol_t* s2)
{
	assert(s1);
	assert(s2);
	assert(s1->next		== NULL);
	assert(s2->previous	== NULL);
	
	s1->next	= s2;
	s2->previous= s1;
}

// -----------------------------------------------------------------------------
// get identifier
// -----------------------------------------------------------------------------
const char* symbol_get_id(const symbol_t* s)
{
	return s->id;
}

// -----------------------------------------------------------------------------
// get next-item
// -----------------------------------------------------------------------------
symbol_t* symbol_get_next(const symbol_t* s)
{
	return s->next;
}

// -----------------------------------------------------------------------------
// get previous-item
// -----------------------------------------------------------------------------
symbol_t* symbol_get_previous(const symbol_t* s)
{
	return s->previous;
}

// -----------------------------------------------------------------------------
// get scope
// -----------------------------------------------------------------------------
const scope_t* symbol_get_scope(const symbol_t* s)
{
	return s->scope;
}

// -----------------------------------------------------------------------------
// set next-item
// -----------------------------------------------------------------------------
void symbol_set_next(symbol_t* s, const symbol_t* next)
{
	s->next = (symbol_t*) next;
}

// -----------------------------------------------------------------------------
// set previous-item
// -----------------------------------------------------------------------------
void symbol_set_previous(symbol_t* s, const symbol_t* previous)
{
	s->previous = (symbol_t*) previous;
}

// -----------------------------------------------------------------------------
// set scope
// -----------------------------------------------------------------------------
void symbol_set_scope(symbol_t* s, const scope_t* scope)
{
	s->scope = scope;
}

// -----------------------------------------------------------------------------
// get value-object of an variable-symbol (variables only!)
// -----------------------------------------------------------------------------
const value_t* symbol_variable_get_value(const symbol_t* s)
{
	assert(s->type == SYM_TYPE_VARIABLE);
	
	return s->value;
}

// -----------------------------------------------------------------------------
// assign new value to the symbol-value (variables only!)
// -----------------------------------------------------------------------------
void symbol_variable_assign_value(symbol_t* s, const value_t* new_value)
{
	assert(s->type == SYM_TYPE_VARIABLE);
	
	value_assign(new_value, s->value);
}

// -----------------------------------------------------------------------------
// get function-object of an function-symbol (functions only!)
// -----------------------------------------------------------------------------
function_t* symbol_function_get_function(const symbol_t* s)
{
	assert(s->type == SYM_TYPE_FUNCTION);
	
	return s->function;
}
