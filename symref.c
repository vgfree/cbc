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
	
	node->type		= SNT_SYMREF;
	node->sym		= s;
	node->table_sym	= NULL;
	
	return (syntree*) node;
}

// -----------------------------------------------------------------------------
// this function expects a symbol with the given identifier to exist in the
// global symbol-table.
// if there is no such a symbol, an error will be raised.
// -----------------------------------------------------------------------------
void symref_setsymbolfromtable(symref* node)
{
	symbol* table_sym = node->table_sym;
	
	// check if reference to the instance in the symbol-table exists
	if (table_sym)
		return;	// reference to symbol in the global symbol-table is already
				// available -> nothing has to be done.
	
	// get symbol-reference
	symbol* dummy = symtab_lookup(gl_symtab, node->sym->identifier);
	
	// if there is no such a symbol -> error
	if (!dummy)
	{
		yyerror("undefined symbol: %s", node->sym->identifier);
		exit(1);
	}
	// symbol was found -> store reference
	node->table_sym = dummy;
}
