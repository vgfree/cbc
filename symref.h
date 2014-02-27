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
	int line_no;					// line number
	char* sym_id;					// symbol identifier
	CbSymbol* table_sym;			// reference to the instance in the symbol-table
} CbSymref;


// interface functions
CbSyntree* cb_symref_create(char* identifier);
void cb_symref_set_symbol_from_table(CbSymref* node, CbSymtab* symtab);


#endif // SYMREF_H
