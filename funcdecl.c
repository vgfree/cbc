/*******************************************************************************
 * CbFuncDeclarationNode -- 'CbSyntree'-node, that contains information about a
 *                          function.
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
CbSyntree* cb_funcdecl_create(char* identifier, CbSyntree* body,
							  CbStrlist* params)
{
	CbFuncDeclarationNode* node = malloc(sizeof(CbFuncDeclarationNode));
	node->type	 = SNT_FUNC_DECL;
	node->sym_id = strdup(identifier);
	node->body	 = body;
	node->params = params;
	
	return (CbSyntree*) node;
}
