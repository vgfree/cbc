/*******************************************************************************
 * funccall_t -- 'syntree_t'-node, that creates a function-call.
 * 
 *  	This structure is part of the abstract syntax-tree 'syntree_t'.
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
	enum cb_syntree_node_type type;	// node-type is SNT_FUNC_CALL
	char* sym_id;				// function identifier
	symbol_t* table_sym;		// reference to the instance in the symbol-table
	strlist_t* args;			// a list of arguments
} funccall_t;


// interface functions
CbSyntree* funccall_create(char* identifier, strlist_t* args);


#endif // FUNCCALL_H
