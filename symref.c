/*******************************************************************************
 * symref_t -- 'syntree_t'-node, that references a symbol in the symbol-table.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "symref.h"
#include "symtab.h"
#include "syntree.h"


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
CbSyntree* cb_symref_create(char* identifier)
{
	CbSymref* node	= malloc(sizeof(CbSymref));
	node->type		= SNT_SYMREF;
	node->line_no	= 0;
	node->sym_id	= strdup(identifier);
	node->table_sym	= NULL;
	
	return (CbSyntree*) node;
}

// -----------------------------------------------------------------------------
// this function expects a symbol with the given identifier to exist in the
// global symbol-table.
// if there is no such a symbol, an error will be raised.
// -----------------------------------------------------------------------------
void cb_symref_set_symbol_from_table(CbSymref* node, CbSymtab* symtab)
{
	CbSymbol* table_sym = node->table_sym;
	
	// get symbol-reference
	CbSymbol* dummy = cb_symtab_lookup(symtab, node->sym_id);
	
	// if there is no such a symbol -> error
	if (!dummy)
	{
		fprintf(stderr, "Error: Undefined symbol: %s\n", node->sym_id);
		exit(EXIT_FAILURE);
	}
	
	// symbol was found -> store reference
	node->table_sym = dummy;
}
