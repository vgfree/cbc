/*******************************************************************************
 * CbArrayAccessNode -- Represents an access to an array element in the
 *                      syntax tree.
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "array_access_node.h"
#include "syntree.h"
#include "symref.h"
#include "array.h"
#include "error_handling.h"


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
CbSyntree* cb_array_access_node_create(const char* identifier, int index)
{
    CbArrayAccessNode* node = malloc(sizeof(CbArrayAccessNode));
    node->type        = SNT_VALARRAY_ACCESS;
    node->line_no     = 0;
    node->sym_id      = strdup(identifier);
    node->table_sym   = NULL;
    node->index       = index;
    
    return (CbSyntree*) node;
}

// -----------------------------------------------------------------------------
// evaluation
// -----------------------------------------------------------------------------
CbValue* cb_array_access_node_eval(const CbArrayAccessNode* node,
                                   CbSymtab* symtab)
{
    if (cb_symref_set_symbol_from_table((CbSymref*) node, symtab) == EXIT_FAILURE)
        return NULL; // an error occurred
    
    const CbValue* valarray = cb_symbol_variable_get_value(node->table_sym);
    CbValue* element        = cb_valarray_get_element(valarray, node->index);
    
    if (element)
        return element;
    else
    {
        cb_print_error(CB_ERR_RUNTIME, node->line_no,
                       "Array index out of bounds");
        return NULL;
    }
}
