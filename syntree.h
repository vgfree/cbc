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
	SNT_COMPARISON,
	SNT_STATEMENTLIST,
	// the following node-types have one child-node
	SNT_UNARYMINUS,
	SNT_DECLARATION,
	// the following node-types have no child-nodes
	SNT_CONSTVAL,
	SNT_SYMREF,
	SNT_FLOW_IF,
	SNT_FLOW_WHILE
};

// comparison-node types
enum cmp_nodetype_t
{
	CMP_EQ,	// equal
	CMP_NE,	// not equal
	CMP_GE,	// greater or equal
	CMP_LE,	// lower or equal
	CMP_GT,	// greater than
	CMP_LT,	// lower than
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

// control-flow node
typedef struct
{
	// type of the node
	enum syn_nodetype_t type;
	// symbol
	syntree* cond;
	// true-branch
	syntree* tb;
	// false-branch
	syntree* fb;
} flow;

// comparison node
typedef struct
{
	// type of the node
	enum syn_nodetype_t type;
	// comparison-type
	enum cmp_nodetype_t cmp_type;
	// left tree-node
	struct syntree_t* l;
	// right tree-node
	struct syntree_t* r;
} comparison;


// interface functions
syntree* syntree_create(enum syn_nodetype_t type, syntree* left_node,
						syntree* right_node);
syntree* constval_create(int value);
syntree* symref_create(symbol* s);
syntree* flow_create(	enum syn_nodetype_t type, syntree* condition,
						syntree* then_branch, syntree* else_branch);
syntree* comparison_create(	enum cmp_nodetype_t type, syntree* left_node,
							syntree* right_node);
void syntree_free(syntree* node);
int eval(syntree* node);

#endif // SYNTREE_H
