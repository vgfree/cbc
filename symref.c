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
syntree_t* symref_create(char* identifier)
{
	symref_t* node	= malloc(sizeof(symref_t));
	node->type		= SNT_SYMREF;
	node->sym_id	= strdup(identifier);
	node->table_sym	= NULL;
	
	return (syntree_t*) node;
}

// -----------------------------------------------------------------------------
// this function expects a symbol with the given identifier to exist in the
// global symbol-table.
// if there is no such a symbol, an error will be raised.
// -----------------------------------------------------------------------------
void symref_setsymbolfromtable(symref_t* node, symtab_t* symtab)
{
	symbol_t* table_sym = node->table_sym;
	
	// get symbol-reference
	symbol_t* dummy = symtab_lookup(symtab, node->sym_id);
	
	// if there is no such a symbol -> error
	if (!dummy)
	{
		fprintf(stderr, "Error: Undefined symbol: %s\n", node->sym_id);
		exit(1);
	}
	
	// symbol was found -> store reference
	node->table_sym = dummy;
}
