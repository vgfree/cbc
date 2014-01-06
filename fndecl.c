/*******************************************************************************
 * fndecl -- part of the abstrct-syntax-tree struct 'syntree'.
 ******************************************************************************/

#include <stdlib.h>
#include "fndecl.h"
#include "errors.h"

// -----------------------------------------------------------------------------
// create a function-declaration-node
// -----------------------------------------------------------------------------
syntree* fndecl_create(symbol* s, syntree* func_body, syntree* func_params)
{
	fndecl* node = malloc(sizeof(fndecl));
	if (!node)
	{
		yyerror(ERR_BADALLOC);
		exit(1);
	}
	
	symbol_settype(s, SYM_FUNCTION);
	s->func->body	= func_body;
	s->func->params	= func_params;
	
	node->type	= SNT_FUNC_DECL;
	node->sym	= s;
	
	return (syntree*) node;
}
