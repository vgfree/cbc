/*******************************************************************************
 * CbSymref -- CbSyntree-node, that references a symbol in the symbol-table.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "symref.h"
#include "symtab.h"
#include "syntree.h"
#include "error_handling.h"


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
CbSyntree* cb_symref_create(char* identifier)
{
    CbSymref* node  = malloc(sizeof(CbSymref));
    node->type      = SNT_SYMREF;
    node->line_no   = 0;
    node->sym_id    = strdup(identifier);
    node->table_sym = NULL;
    
    return (CbSyntree*) node;
}

// -----------------------------------------------------------------------------
// this function expects a symbol with the given identifier to exist in the
// global symbol-table.
// if there is no such a symbol, an error will be raised.
// -----------------------------------------------------------------------------
int cb_symref_set_symbol_from_table(CbSymref* node, CbSymtab* symtab)
{
    CbSymbol* table_sym = node->table_sym;
    
    // get symbol-reference
    CbSymbol* dummy = cb_symtab_lookup(symtab, node->sym_id, false);
    
    if (!dummy) // if there is no such a symbol -> error
    {
        cb_print_error(CB_ERR_RUNTIME, node->line_no, "Undefined symbol: %s",
                       node->sym_id);
        return EXIT_FAILURE;
    }
    
    // symbol was found -> store reference
    node->table_sym = dummy;
    
    return EXIT_SUCCESS;
}
