/*******************************************************************************
 * CbSyntree -- Implementation of an abstract syntax tree structure.
 ******************************************************************************/

#ifndef SYNTREE_H
#define SYNTREE_H


#include "syntree_if.h"
#include "symtab_if.h"
#include "value.h"

// general syntax-tree node
struct syntree_node
{
	enum cb_syntree_node_type type;	// type of the node
	int line_no;					// line number in the source file/stream
	CbSyntree* l;					// left tree-node
	CbSyntree* r;					// right tree-node
};

// const-value node
struct constval_node
{
	enum cb_syntree_node_type type;	// node-type is SNT_CONSTVAL
	int line_no;					// line number
	CbValue* value;					// value
};

// control-flow node
struct flow_node
{
	enum cb_syntree_node_type type;	// node-type is either SNT_FLOW_IF or
									// SNT_FLOW_WHILE
	int line_no;					// line number
	CbSyntree* cond;				// condition
	CbSyntree* tb;					// true-branch
	CbSyntree* fb;					// false-branch
};

// comparison node
struct comparison_node
{
	enum cb_syntree_node_type type;		// node-type is SNT_COMPARISON
	int line_no;						// line number
	enum cb_comparison_type cmp_type;	// comparison-type
	CbSyntree* l;						// left tree-node
	CbSyntree* r;						// right tree-node
};


// interface functions
CbSyntree* cb_syntree_create(enum cb_syntree_node_type type,
							 CbSyntree* left_node, CbSyntree* right_node);
CbSyntree* cb_constval_create(CbNumeric value);
CbSyntree* cb_conststr_create(CbString string);
CbSyntree* cb_constbool_create(CbBoolean boolean);
CbSyntree* cb_flow_create(enum cb_syntree_node_type type, CbSyntree* condition,
						  CbSyntree* then_branch, CbSyntree* else_branch);
CbSyntree* cb_comparison_create(enum cb_comparison_type type,
								CbSyntree* left_node, CbSyntree* right_node);
void cb_syntree_free(CbSyntree* node);
CbValue* cb_syntree_eval(CbSyntree* node, CbSymtab* symtab);


#endif // SYNTREE_H
