/*******************************************************************************
 * funccall_t -- 'syntree_t'-node, that creates a function-call.
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "funccall.h"
#include "syntree.h"


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
CbSyntree* funccall_create(char* identifier, CbStrlist* args)
{
	funccall_t* node= malloc(sizeof(funccall_t));
	node->type		= SNT_FUNC_CALL;
	node->sym_id	= strdup(identifier);
	node->table_sym	= NULL;
	node->args		= args;
	
	return (CbSyntree*) node;
}
