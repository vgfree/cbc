/*******************************************************************************
 * syntree_t -- Implementation of an abstract syntax tree structure.
 ******************************************************************************/

#ifndef SYNTREE_H
#define SYNTREE_H


#include "syntree_if.h"
#include "symtab_if.h"
#include "value.h"

// general syntax-tree node
struct syntree_node
{
	enum syn_nodetype_t type;	// type of the node
	syntree_t* l;				// left tree-node
	syntree_t* r;				// right tree-node
};

// const-value node
struct constval_node
{
	enum syn_nodetype_t type;	// node-type is SNT_CONSTVAL
	value_t* value;				// value
};

// control-flow node
struct flow_node
{
	enum syn_nodetype_t type;	// node-type is either SNT_FLOW_IF or
								// SNT_FLOW_WHILE
	syntree_t* cond;			// condition
	syntree_t* tb;				// true-branch
	syntree_t* fb;				// false-branch
};

// comparison node
struct comparison_node
{
	enum syn_nodetype_t type;		// node-type is SNT_COMPARISON
	enum comparison_type_t cmp_type;// comparison-type
	syntree_t* l;					// left tree-node
	syntree_t* r;					// right tree-node
};


// interface functions
syntree_t* syntree_create(	enum syn_nodetype_t type, syntree_t* left_node,
							syntree_t* right_node);
syntree_t* constval_create(cbnumeric value);
syntree_t* conststr_create(cbstring string);
syntree_t* constbool_create(cbboolean boolean);
syntree_t* flow_create(	enum syn_nodetype_t type, syntree_t* condition,
						syntree_t* then_branch, syntree_t* else_branch);
syntree_t* comparison_create(	enum comparison_type_t type,
								syntree_t* left_node, syntree_t* right_node);
void syntree_free(syntree_t* node);
value_t* syntree_eval(syntree_t* node, symtab_t* symtab);


#endif // SYNTREE_H
