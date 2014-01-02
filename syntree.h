/*******************************************************************************
 * syntree -- implementation of an abstract syntax tree structure
 ******************************************************************************/

#ifndef SYNTREE_H
#define SYNTREE_H

#include "cbvalues.h"

// syntax-node types
enum syn_nodetype_t
{
	// all node-types up to the following have two child-nodes
	SNT_ASSIGNMENT = 256,
	SNT_COMPARISON,
	SNT_STATEMENTLIST,
	SNT_FUNC_DECL,
	SNT_FUNC_CALL,
	// the following node-types have one child-node
	SNT_UNARYMINUS,
	SNT_DECLARATION,
	SNT_PRINT,
	// the following node-types have no child-nodes
	SNT_CONSTVAL,
	SNT_SYMREF,
	SNT_FLOW_IF,
	SNT_FLOW_WHILE,
	SNT_CONSTSTR
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

// const-value node
typedef struct
{
	// type of the node
	enum syn_nodetype_t type;
	// value
	cbvalue* value;
} constval;

// control-flow node
typedef struct
{
	// type of the node
	enum syn_nodetype_t type;
	// condition
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
	enum comparisontype_t cmp_type;
	// left tree-node
	struct syntree_t* l;
	// right tree-node
	struct syntree_t* r;
} comparison;


// interface functions
syntree* syntree_create(enum syn_nodetype_t type, syntree* left_node,
						syntree* right_node);
syntree* constval_create(int value);
syntree* conststr_create(char* string);
syntree* flow_create(	enum syn_nodetype_t type, syntree* condition,
						syntree* then_branch, syntree* else_branch);
syntree* comparison_create(	enum comparisontype_t type, syntree* left_node,
							syntree* right_node);
void syntree_free(syntree* node);
cbvalue* eval(syntree* node);

#endif // SYNTREE_H
