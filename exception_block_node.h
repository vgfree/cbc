/*******************************************************************************
 * CbExceptionBlockNode -- Exception-handling construct
 * 
 *      This structure is part of the abstract syntax-tree CbSyntree.
 *      It uses the node-type SNT_EXCEPTION_BLOCK.
 *
 *      The construct would look like this:
 *         startseq
 *            // In this block of code an error could occur ...
 *         onerror
 *            // This code will be executed in case of an error
 *         stopseq,
 ******************************************************************************/

#ifndef EXCEPTION_BLOCK_NODE_H
#define EXCEPTION_BLOCK_NODE_H


#include "syntree_if.h"
#include "symtab_if.h"
#include "value.h"

// exception block type
enum cb_exception_block_type
{
    EXBL_ONERROR,
    EXBL_ALWAYS
};

// exception block node
typedef struct
{
    enum cb_syntree_node_type type; // node-type is SNT_EXCEPTION_HANDLING
    int line_no;                    // line number
    enum cb_exception_block_type block_type; // type of the exception-block
    CbSyntree* code_block;          // code that could cause an error
    CbSyntree* exception_block;     // code that will be executed when an error
                                    // occurs
} CbExceptionBlockNode;


// interface functions
CbSyntree* cb_exception_block_create(enum cb_exception_block_type type,
                                     CbSyntree* code_block,
                                     CbSyntree* exception_block);
CbValue* cb_exception_block_execute(CbExceptionBlockNode* node, CbSymtab* symtab);


#endif // EXCEPTION_BLOCK_NODE_H
