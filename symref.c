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

// -----------------------------------------------------------------------------
// this function replaces the 'sym'-member in the passed symref-struct with the
// "actual" reference to the symbol in the symbol-table, if it exists.
// furthermore, the old dummy symbol will be freed since it isn't necessary
// anymore.
// if there is no such a symbol in the symbol-table, everything stays as it is.
// -----------------------------------------------------------------------------
void symref_setsymbolfromtable(symref* node)
{
	symbol* sym = node->sym;
	// check if symbol is already defined
	if (sym->type != SYM_UNDEFINED)
		return;	// symbol is "defined", that means it is contained in the
				// symbol-table -> nothing has to be done.
	
	symbol* dummy	= sym;	// remember dummy-symbol
	sym				= symtab_lookup(gl_symtab, sym->identifier);
	// if there is no such a symbol -> error
	if (!sym)
	{
		yyerror("undefined symbol: %s", dummy->identifier);
		exit(1);
	}
	// replace dummy-symbol with the actual symbol in the symbol-table
	node->sym = sym;
	// free dummy-symbol
	symbol_free(dummy);
}
