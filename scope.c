/*******************************************************************************
 * CbScope -- Stores information about the current codeblock execution-scope.
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
CbScope* cb_scope_create(char* context, int level)
{
	CbScope* scope = malloc(sizeof(CbScope));
	scope->context = strdup(context);
	scope->level   = level;
	
	return scope;
}

// -----------------------------------------------------------------------------
// destructur
// -----------------------------------------------------------------------------
void cb_scope_free(CbScope* scope)
{
	free(scope->context);
	free(scope);
}

// -----------------------------------------------------------------------------
// compare scopes
// -----------------------------------------------------------------------------
bool cb_scope_equals(const CbScope* scope1, const CbScope* scope2)
{
	if (scope1 == NULL || scope2 == NULL)
		return false;	// if either of the arguments is NULL
						// -> scopes are not equal!
	else
		return (scope1->level == scope2->level &&
				strcmp(scope1->context, scope2->context) == 0);
}
