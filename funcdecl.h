/*******************************************************************************
 * funcdecl_t -- 'syntree_t'-node, that declares a function in the symbol-table.
 * 
 *  	This structure is part of the abstract syntax-tree 'syntree_t'.
 *  	It either uses the node-type SNT_FNCALL or SNT_FNDECL.
 ******************************************************************************/

#ifndef FUNCDECL_H
#define FUNCDECL_H


#include "symbol.h"
#include "symtab_if.h"
#include "syntree_if.h"
#include "strlist.h"

// function-declaration node
typedef struct
{
	enum syn_nodetype_t type;	// node-type is SNT_FUNC_DECL
	symbol_t* function_sym;		// reference to a prepared function-symobl
} funcdecl_t;


// interface functions

// funcdecl_create TODO: Reference to symbol-table should be determined only
// right before function run-time.
syntree_t* funcdecl_create(	char* identifier, syntree_t* body,
							strlist_t* params, symtab_t* symtab);


#endif // FUNCDECL_H
