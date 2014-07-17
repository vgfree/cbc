/*******************************************************************************
 * CbArrayAssignmentNode -- Represents an assignment to an array element in the
 *                          syntax tree.
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "array_assignment_node.h"
#include "syntree.h"
#include "symref.h"
#include "error_handling.h"


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
CbSyntree* cb_array_assignment_node_create(const char* identifier, int index,
                                           CbSyntree* value_node)
{
    CbArrayAssignmentNode* node = malloc(sizeof(CbArrayAssignmentNode));
    node->type                  = SNT_VALARRAY_ASSIGNMENT;
    node->line_no               = 0;
    node->sym_id                = strdup(identifier);
    node->table_sym             = NULL;
    node->index                 = index;
    node->value_node            = value_node;
    
    return (CbSyntree*) node;
}

// -----------------------------------------------------------------------------
// evaluation
// -----------------------------------------------------------------------------
CbValue* cb_array_assignment_node_eval(const CbArrayAssignmentNode* node,
                                       CbSymtab* symtab)
{
    if (cb_symref_set_symbol_from_table((CbSymref*) node, symtab) == EXIT_FAILURE)
        return NULL; // an error occurred
    
    const CbValue* valref   = cb_symbol_variable_get_value(node->table_sym);
    assert(cb_value_is_type(valref, CB_VT_REFERENCE));
    const CbValue* valarray = cb_valref_get(valref);
    assert(cb_value_is_type(valarray, CB_VT_VALARRAY));
    CbValue* value          = cb_syntree_eval(node->value_node, symtab);
    
    if (value == NULL)
        return NULL;
    
    if (cb_valarray_set_element(valarray, node->index, value))
        return value;
    else
    {
        cb_print_error(CB_ERR_RUNTIME, node->line_no,
                       "Array index out of bounds");
        return NULL;
    }
}
