/*******************************************************************************
 * CbArrayAssignmentNode -- Represents an assignment to an array element in the
 *                          syntax tree.
 * 
 *      This structure is part of the abstract syntax-tree 'CbSyntree'.
 ******************************************************************************/

#ifndef ARRAY_ASSIGNMENT_NODE_H
#define ARRAY_ASSIGNMENT_NODE_H


#include "symbol.h"
#include "symtab_if.h"
#include "syntree_if.h"
#include "value.h"

// array aissgnment node
typedef struct
{
    enum cb_syntree_node_type type; // node-type is SNT_VALARRAY_ASSIGNMENT
    int line_no;                    // line number
    char* sym_id;                   // array identifier
    CbSymbol* table_sym;            // reference to the instance in the symbol-table
    int index;                      // index of the element
    CbSyntree* value_node;          // the value node to assign
} CbArrayAssignmentNode;


// interface functions
CbSyntree* cb_array_assignment_node_create(const char* identifier, int index,
                                           CbSyntree* value_node);
CbValue* cb_array_assignment_node_eval(const CbArrayAssignmentNode* node,
                                       CbSymtab* symtab);


#endif // ARRAY_ASSIGNMENT_NODE_H
