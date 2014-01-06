/*******************************************************************************
 * fndecl -- part of the abstrct-syntax-tree struct 'syntree'.
 *           using the node-type SNT_FUNC_DECL.
 *           this node represents the declaration of a function in the
 *           abstract-syntax-tree.
 ******************************************************************************/

#ifndef FNDECL_H
#define FNDECL_H

#include "symtab.h"

// function-declaration node
typedef struct
{
	// type of the node
	enum syn_nodetype_t type;
	// symbol
	symbol* sym;
} fndecl;

// interface functions
syntree* fndecl_create(symbol* s, syntree* func_body, syntree* func_params);

#endif // FNDECL_H
