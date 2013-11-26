/*******************************************************************************
 * syntree -- implementation of an abstract syntax tree structure
 ******************************************************************************/

// syntax-node types
enum syn_nodetype_t
{
	SNT_CONSTVAL,
	SNT_OPERATOR
}

// syntax-tree
typedef struct syntree_t
{
	// type of the node
	enum syn_nodetype_t type;
	// left tree-node
	struct syntree_t* l;
	// right tree-node
	struct syntree_t* r;
} syntree;
