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
CbSyntree* cb_array_access_node_create(const char* identifier,
                                       CbStrlist* indices)
{
    CbArrayAccessNode* node = malloc(sizeof(CbArrayAccessNode));
    node->type        = SNT_VALARRAY_ACCESS;
    node->line_no     = 0;
    node->sym_id      = strdup(identifier);
    node->table_sym   = NULL;
    node->indices     = indices;
    
    return (CbSyntree*) node;
}

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
void cb_array_access_node_free(CbArrayAccessNode* node)
{
    CbStrlist* item = node->indices;
    while (item)
    {
        cb_syntree_free((CbSyntree*) item->data);
        item = item->next;
    }
    
    cb_strlist_free(node->indices);
    free(node->sym_id);
    free(node);
}

// -----------------------------------------------------------------------------
// evaluation
// -----------------------------------------------------------------------------
const CbValue* cb_array_access_node_eval(const CbArrayAccessNode* node,
                                         CbSymtab* symtab)
{
    if (cb_symref_set_symbol_from_table((CbSymref*) node, symtab) == EXIT_FAILURE)
        return NULL; // an error occurred
    
    const CbValue* valref   = cb_symbol_variable_get_value(node->table_sym);
    assert(cb_value_is_type(valref, CB_VT_REFERENCE));
    const CbValue* valarray = cb_valref_get(valref);
    assert(cb_value_is_type(valarray, CB_VT_VALARRAY));
    
    const CbValue* element = valarray;
    CbStrlist* item  = node->indices;
    while (item)
    {
        CbValue* index = cb_syntree_eval((CbSyntree*) item->data, symtab);
        assert(cb_value_is_type(index, CB_VT_NUMERIC));
        if (cb_value_is_type(element, CB_VT_REFERENCE))
            element = cb_valref_get(element);
        
        element = cb_valarray_get_element(element, cb_numeric_get(index));
        if (!element)
            break;
        
        cb_value_free(index);
        item = item->next;
    }
    
    
    if (element)
        return element;
    else
    {
        cb_print_error(CB_ERR_RUNTIME, node->line_no,
                       "Array index out of bounds");
        return NULL;
    }
}
