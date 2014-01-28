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

// -----------------------------------------------------------------------------
// compare scopes
// -----------------------------------------------------------------------------
bool scope_equals(const scope_t* scope1, const scope_t* scope2)
{
	if (scope1 == NULL || scope2 == NULL)
		return scope1 == scope2;
	else
		return (scope1->level == scope2->level &&
				strcmp(scope1->context, scope2->context) == 0);
}
