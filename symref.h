/*******************************************************************************
 * symref -- part of the abstrct-syntax-tree struct 'syntree'.
 *           using the node-type SNT_SYMREF.
 ******************************************************************************/

#ifndef SYMREF_H
#define SYMREF_H

#include "symtab.h"

// symbol-reference node
typedef struct
{
	// type of the node
	enum syn_nodetype_t type;
	// symbol
	symbol* sym;
} symref;

// interface functions
syntree* symref_create(symbol* s);
void symref_setsymbolfromtable(symref* node);

#endif // SYMREF_H
