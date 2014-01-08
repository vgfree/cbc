/*******************************************************************************
 * fncall -- part of the abstrct-syntax-tree struct 'syntree'.
 *           using the node-type SNT_FUNC_CALL.
 *           this node represents the call to a function in the
 *           abstract-syntax-tree.
 ******************************************************************************/

#ifndef FNCALL_H
#define FNCALL_H

#include "symtab.h"

// function-call node
typedef struct
{
	// type of the node
	enum syn_nodetype_t type;
	// symbol
	symbol* sym;
	// arguments
	syntree* args;
} fncall;

// interface functions
syntree* fncall_create(symbol* s, syntree* args);

#endif // FNCALL_H
