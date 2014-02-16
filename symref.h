/*******************************************************************************
 * symref_t -- 'syntree_t'-node, that references a symbol in the symbol-table.
 * 
 *  	This structure is part of the abstract syntax-tree 'syntree_t'.
 *  	It uses the node-type SNT_SYMREF.
 ******************************************************************************/

#ifndef SYMREF_H
#define SYMREF_H


#include "symbol.h"
#include "symtab_if.h"
#include "syntree_if.h"

// symbol-reference node
typedef struct
{
	enum cb_syntree_node_type type;	// node-type is SNT_SYMREF
	char* sym_id;				// symbol identifier
	symbol_t* table_sym;		// reference to the instance in the symbol-table
} symref_t;


// interface functions
CbSyntree* symref_create(char* identifier);
void symref_setsymbolfromtable(symref_t* node, symtab_t* symtab);


#endif // SYMREF_H
