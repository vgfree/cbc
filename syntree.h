/*******************************************************************************
 * syntree -- implementation of an abstract syntax tree structure
 ******************************************************************************/

#ifndef SYNTREE_H
#define SYNTREE_H

// syntax-node types
enum syn_nodetype_t
{
	SNT_CONSTVAL = 256
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


// interface functions
syntree* syntree_create(enum syn_nodetype_t type, syntree* left_node,
						syntree* right_node);
syntree* constval_create(int value);
void syntree_free(syntree* node);

#endif // SYNTREE_H
