/*******************************************************************************
 * symbol -- Implementation of a symbol-structure
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "symbol.h"


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
symbol_t* symbol_create()
{
	symbol_t* s	= (symbol_t*) malloc(sizeof(symbol_t));
	s->type		= SYM_TYPE_UNDEFINED;
	s->state	= SYM_STATE_INITIAL;
	s->id		= NULL;
	s->next		= NULL;
	s->previous	= NULL;
	
	return s;
}

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
void symbol_free(symbol_t* s)
{
	if (s->state > SYM_STATE_INITIAL)
		free(s->id);
	
	// destroy attributes
	symbol_settype(s, SYM_TYPE_UNDEFINED);
	
	free(s);
}

// -----------------------------------------------------------------------------
// set symbol-identifier
// -----------------------------------------------------------------------------
void symbol_setid(symbol_t* s, char* id)
{
	if (s->state == SYM_STATE_INITIAL)
	{
		s->id	= strdup(id);
		s->state= SYM_STATE_HASID;
	}
	// TODO: Error-handling
	else
	{
		fprintf(stderr, "Error: Cannot change symbol-identifer after beeing "\
						"already set\n");
		exit(EXIT_FAILURE);
	}
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
// set symbol-type
// -----------------------------------------------------------------------------
void symbol_settype(symbol_t* s, enum symbol_type_t type)
{
	if (s->type == type) // nothing changed -> nothing to do
		return;
	
	// free old attributes first, if necessary
	switch (s->type)
	{
		case SYM_TYPE_VARIABLE:
			value_free(s->value);
			break;
		
		case SYM_TYPE_FUNCTION:
			function_free(s->function);
			break;
	}
	
	// set new attributes
	switch (type)
	{
		case SYM_TYPE_VARIABLE:
			s->value = value_create();
			break;
		
		case SYM_TYPE_FUNCTION:
			s->function = function_create();
			break;
	}
	
	// finally, apply new symobl-type
	s->type = type;
}
