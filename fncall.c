/*******************************************************************************
 * fncall -- part of the abstrct-syntax-tree struct 'syntree'.
 ******************************************************************************/

#include <stdlib.h>
#include "fncall.h"
#include "errors.h"

// -----------------------------------------------------------------------------
// create a function-call-node
// -----------------------------------------------------------------------------
syntree* fncall_create(symbol* s, syntree* args)
{
	fncall* node = malloc(sizeof(fncall));
	if (!node)
	{
		yyerror(ERR_BADALLOC);
		exit(1);
	}
	
	node->type		= SNT_FUNC_CALL;
	node->sym		= s;
	node->table_sym	= NULL;
	node->args		= args;
	
	return (syntree*) node;
}

