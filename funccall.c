/*******************************************************************************
 * CbFuncCallNode -- 'CbSyntree'-node, that creates a function-call.
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
CbSyntree* cb_funccall_create(char* identifier, CbStrlist* args)
{
	CbFuncCallNode* node = malloc(sizeof(CbFuncCallNode));
	node->type			 = SNT_FUNC_CALL;
	node->line_no		 = 0;
	node->sym_id		 = strdup(identifier);
	node->table_sym		 = NULL;
	node->args			 = args;
	
	return (CbSyntree*) node;
}
