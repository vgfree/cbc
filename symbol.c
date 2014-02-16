/*******************************************************************************
 * CbSymbol -- Implementation of a symbol-structure.
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
struct CbSymbol
{
	enum cb_symbol_type type;	// type
	char* id;					// identifier
	struct CbSymbol* next;		// reference to the next symbol (in case of a list)
	struct CbSymbol* previous;	// reference to the previous symbol (in case of a list)
	const scope_t* scope;		// scope, in which the symbol is valid
	
	union
	{
		CbValue* value;			// symbol-value, is used if the symbol is a variable
		function_t* function;	// function, is used if the symbol is a function
	};
};


// #############################################################################
// implementation-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor (internal)
// -----------------------------------------------------------------------------
static CbSymbol* symbol_create(char* identifier)
{
	CbSymbol* s	= (CbSymbol*) malloc(sizeof(CbSymbol));
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
CbSymbol* cb_symbol_create_variable(char* identifier)
{
	CbSymbol* s	= symbol_create(identifier);
	s->type		= SYM_TYPE_VARIABLE;
	s->value	= cb_value_create();
	
	return s;
}

// -----------------------------------------------------------------------------
// constructor (function)
// -----------------------------------------------------------------------------
CbSymbol* cb_symbol_create_function(char* identifier, function_t* func_object)
{
	CbSymbol* s	= symbol_create(identifier);
	s->type		= SYM_TYPE_FUNCTION;
	s->function	= func_object;
	
	return s;
}

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
void cb_symbol_free(CbSymbol* s)
{
	free(s->id);
	
	switch (s->type)
	{
		case SYM_TYPE_VARIABLE:
			cb_value_free(s->value);
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
void cb_symbol_connect(CbSymbol* s1, CbSymbol* s2)
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
const char* cb_symbol_get_id(const CbSymbol* s)
{
	return s->id;
}

// -----------------------------------------------------------------------------
// get next-item
// -----------------------------------------------------------------------------
CbSymbol* cb_symbol_get_next(const CbSymbol* s)
{
	return s->next;
}

// -----------------------------------------------------------------------------
// get previous-item
// -----------------------------------------------------------------------------
CbSymbol* cb_symbol_get_previous(const CbSymbol* s)
{
	return s->previous;
}

// -----------------------------------------------------------------------------
// get scope
// -----------------------------------------------------------------------------
const scope_t* cb_symbol_get_scope(const CbSymbol* s)
{
	return s->scope;
}

// -----------------------------------------------------------------------------
// set next-item
// -----------------------------------------------------------------------------
void cb_symbol_set_next(CbSymbol* s, const CbSymbol* next)
{
	s->next = (CbSymbol*) next;
}

// -----------------------------------------------------------------------------
// set previous-item
// -----------------------------------------------------------------------------
void cb_symbol_set_previous(CbSymbol* s, const CbSymbol* previous)
{
	s->previous = (CbSymbol*) previous;
}

// -----------------------------------------------------------------------------
// set scope
// -----------------------------------------------------------------------------
void cb_symbol_set_scope(CbSymbol* s, const scope_t* scope)
{
	s->scope = scope;
}

// -----------------------------------------------------------------------------
// get value-object of an variable-symbol (variables only!)
// -----------------------------------------------------------------------------
const CbValue* cb_symbol_variable_get_value(const CbSymbol* s)
{
	assert(s->type == SYM_TYPE_VARIABLE);
	
	return s->value;
}

// -----------------------------------------------------------------------------
// assign new value to the symbol-value (variables only!)
// -----------------------------------------------------------------------------
void cb_symbol_variable_assign_value(CbSymbol* s, const CbValue* new_value)
{
	assert(s->type == SYM_TYPE_VARIABLE);
	
	cb_value_assign(new_value, s->value);
}

// -----------------------------------------------------------------------------
// get function-object of an function-symbol (functions only!)
// -----------------------------------------------------------------------------
function_t* cb_symbol_function_get_function(const CbSymbol* s)
{
	assert(s->type == SYM_TYPE_FUNCTION);
	
	return s->function;
}
