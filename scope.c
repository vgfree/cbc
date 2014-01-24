/*******************************************************************************
 * scope_t -- Stores information about the current codeblock execution-scope
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "scope.h"


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
scope_t* scope_create(char* context, int level)
{
	scope_t* scope	= malloc(sizeof(scope_t));
	scope->context	= strdup(context);
	scope->level	= level;
	
	return scope;
}

// -----------------------------------------------------------------------------
// destructur
// -----------------------------------------------------------------------------
void scope_free(scope_t* scope)
{
	free(scope->context);
	free(scope);
}
