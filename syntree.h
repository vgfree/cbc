/*******************************************************************************
 * syntree -- implementation of an abstract syntax tree structure
 ******************************************************************************/

#ifndef SYNTREE_H
#define SYNTREE_H

#include "symtab.h"

// syntax-node types
enum syn_nodetype_t
{
	// all node-types up to the following have two child-nodes
	SNT_ASSIGNMENT = 256,
	// the following node-types have one child-node
	SNT_NOTYPE_ONE_CHILDNODE,
	SNT_SIGNED_MINUS,
	SNT_DECLARATION,
	// the following node-types have no child-nodes
	SNT_NOTYPE_NO_CHILDNODES,
	SNT_CONSTVAL,
	SNT_SYMBOL_REF
};

// syntax-tree node
typedef struct syntree_t
{
	// type of the node
	enum syn_nodetype_t type;
	// left tree-node
	struct syntree_t* l;
	// right tree-node
	struct syntree_t* r;
} syntree;

// constant value node
typedef struct
{
	// type of the node
	enum syn_nodetype_t type;
	// value
	int value;
} constval;

// symbol-reference node
typedef struct
{
	// type of the node
	enum syn_nodetype_t type;
	// symbol
	symbol* sym;
} symref;


// interface functions
syntree* syntree_create(enum syn_nodetype_t type, syntree* left_node,
						syntree* right_node);
syntree* constval_create(int value);
syntree* symref_create(symbol* s);
void syntree_free(syntree* node);
int eval(syntree* node);

#endif // SYNTREE_H
