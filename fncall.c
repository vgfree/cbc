/*******************************************************************************
 * fncall -- part of the abstrct-syntax-tree struct 'syntree'.
 ******************************************************************************/

#include <stdlib.h>
#include "fncall.h"
#include "errors.h"

// -----------------------------------------------------------------------------
// create a function-call-node
// -----------------------------------------------------------------------------
syntree* fncall_create(symbol* s)
{
	fncall* node = malloc(sizeof(fncall));
	if (!node)
	{
		yyerror(ERR_BADALLOC);
		exit(1);
	}
	
	node->type	= SNT_FUNC_CALL;
	node->sym	= s;
	
	return (syntree*) node;
}
