/*******************************************************************************
 * CbArrayNode -- Represents an array in the syntax tree.
 ******************************************************************************/

#include <stdlib.h>
#include "array_node.h"
#include "syntree.h"
#include "array.h"


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
CbSyntree* cb_array_node_create(CbStrlist* values)
{
    CbArrayNode* node = malloc(sizeof(CbArrayNode));
    node->type        = SNT_VALARRAY;
    node->line_no     = 0;
    node->values      = values;
    node->valarray    = NULL;
    
    return (CbSyntree*) node;
}

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
void cb_array_node_free(CbArrayNode* node)
{
    CbStrlist* item = node->values;
    while (item)
    {
        cb_syntree_free((CbSyntree*) item->data);
        item = item->next;
    }
    
    cb_strlist_free(node->values);
    if (node->valarray)
        cb_value_free(node->valarray);
    
    free(node);
}

// -----------------------------------------------------------------------------
// evaluation
// -----------------------------------------------------------------------------
CbValue* cb_array_node_eval(CbArrayNode* node, CbSymtab* symtab)
{
    CbStrlist* item  = node->values;
    CbArray*   array = cb_array_create_with_ownership((CbArrayItemDestructor) cb_value_free,
                                                      (CbArrayItemCopy) cb_value_copy);
    
    while (item)
    {
        CbValue* value = cb_syntree_eval((CbSyntree*) item->data, symtab);
        cb_array_append(array, (CbArrayItem) value);
        item = item->next;
    }
    
    // finally, wrap the array instance in a value reference to force a
    // "call by reference" of the array instead of "call by value"
    node->valarray = cb_valarray_create(array);
    return cb_valref_create(node->valarray);
}
