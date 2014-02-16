/*******************************************************************************
 * funcdecl_t -- 'syntree_t'-node, that contains information about a function.
 * 
 *  	This structure is part of the abstract syntax-tree 'syntree_t'.
 *  	It either uses the node-type SNT_FUNC_DECL.
 ******************************************************************************/

#ifndef FUNCDECL_H
#define FUNCDECL_H


#include "symbol.h"
#include "symtab_if.h"
#include "syntree_if.h"
#include "strlist.h"
#include "function.h"

// function-declaration node
typedef struct
{
	enum cb_syntree_node_type type;	// node-type is SNT_FUNC_DECL
	char* sym_id;				// function-identifier
	CbSyntree* body;			// contains the code of the function
	strlist_t* params;			// formal parameters
	CbSymtab* symtab;			// pointer to the symbol-table that should be
								// used
} funcdecl_t;


// interface functions
CbSyntree* funcdecl_create(	char* identifier, CbSyntree* body,
							strlist_t* params);


#endif // FUNCDECL_H
