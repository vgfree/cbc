/*******************************************************************************
 * CbArrayNode -- Represents an array in the syntax tree.
 * 
 *  	This structure is part of the abstract syntax-tree 'CbSyntree'.
 ******************************************************************************/

#ifndef ARRAY_NODE_H
#define ARRAY_NODE_H


#include "symtab_if.h"
#include "syntree_if.h"
#include "value.h"
#include "strlist.h"

// array node
typedef struct
{
	enum cb_syntree_node_type type;	// node-type is SNT_FUNC_CALL
	int line_no;					// line number
	CbStrlist* values;				// values of the array
} CbArrayNode;


// interface functions
CbSyntree* cb_array_node_create(CbStrlist* values);
CbValue* cb_array_node_eval(const CbArrayNode* node, CbSymtab* symtab);


#endif // ARRAY_NODE_H
