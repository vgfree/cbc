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
    
    return (CbSyntree*) node;
}

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
CbValue* cb_array_node_eval(const CbArrayNode* node, CbSymtab* symtab)
{
    CbStrlist* item  = node->values;
    CbArray*   array = cb_array_create();
    
    while (item)
    {
        CbValue* value = cb_syntree_eval((CbSyntree*) item->data, symtab);
        cb_array_append(array, (CbArrayItem) value);
        item = item->next;
    }
    
    return cb_valarray_create(array);
}
