/*******************************************************************************
 * funcdecl_t -- 'syntree_t'-node, that contains information about a function.
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "funcdecl.h"
#include "symbol.h"
#include "symtab.h"
#include "syntree.h"


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
syntree_t* funcdecl_create(	char* identifier, syntree_t* body,
							strlist_t* params)
{
	funcdecl_t* node= malloc(sizeof(funcdecl_t));
	node->type		= SNT_FUNC_DECL;
	node->sym_id	= strdup(identifier);
	node->body		= body;
	node->params	= params;
	
	return (syntree_t*) node;
}
