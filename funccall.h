/*******************************************************************************
 * CbFuncCallNode -- 'CbSyntree'-node, that creates a function-call.
 * 
 *      This structure is part of the abstract syntax-tree 'CbSyntree'.
 ******************************************************************************/

#ifndef FUNCCALL_H
#define FUNCCALL_H


#include "symbol.h"
#include "symtab_if.h"
#include "syntree_if.h"
#include "strlist.h"

// function-call node
typedef struct
{
    enum cb_syntree_node_type type; // node-type is SNT_FUNC_CALL
    int line_no;                    // line number
    char* sym_id;                   // function identifier
    CbSymbol* table_sym;            // reference to the instance in the symbol-table
    CbStrlist* args;                // a list of arguments
} CbFuncCallNode;


// interface functions
CbSyntree* cb_funccall_create(char* identifier, CbStrlist* args);


#endif // FUNCCALL_H
