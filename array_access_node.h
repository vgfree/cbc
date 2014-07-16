/*******************************************************************************
 * CbArrayAccessNode -- Represents an access to an array element in the
 *                      syntax tree.
 * 
 *      This structure is part of the abstract syntax-tree 'CbSyntree'.
 ******************************************************************************/

#ifndef ARRAY_ACCESS_NODE_H
#define ARRAY_ACCESS_NODE_H


#include "symbol.h"
#include "symtab_if.h"
#include "syntree_if.h"
#include "value.h"
#include "strlist.h"

// array access node
typedef struct
{
    enum cb_syntree_node_type type; // node-type is SNT_FUNC_CALL
    int line_no;                    // line number
    char* sym_id;                   // array identifier
    CbSymbol* table_sym;            // reference to the instance in the symbol-table
    int index;                      // index of the element
} CbArrayAccessNode;


// interface functions
CbSyntree* cb_array_access_node_create(const char* identifier, int index);
CbValue* cb_array_access_node_eval(const CbArrayAccessNode* node,
                                   CbSymtab* symtab);


#endif // ARRAY_ACCESS_NODE_H
