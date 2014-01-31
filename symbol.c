/*******************************************************************************
 * symbol -- Implementation of a symbol-structure
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "symbol.h"


// #############################################################################
// implementation-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor (internal)
// -----------------------------------------------------------------------------
symbol_t* symbol_create(char* identifier)
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
