/*******************************************************************************
 * symref -- part of the abstrct-syntax-tree struct 'syntree'.
 ******************************************************************************/

#include <stdlib.h>
#include "symref.h"
#include "errors.h"

// -----------------------------------------------------------------------------
// create a syntax-tree symbol-reference-node
// -----------------------------------------------------------------------------
syntree* symref_create(symbol* s)
{
	symref* node = malloc(sizeof(symref));
	if (!node)
	{
		yyerror(ERR_BADALLOC);
		exit(1);
	}
	
	node->type	= SNT_SYMREF;
	node->sym	= s;
	
	return (syntree*) node;
}
