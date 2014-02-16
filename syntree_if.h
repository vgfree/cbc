/*******************************************************************************
 * CbSyntree -- Interface of the abstract syntax-tree structure.
 ******************************************************************************/

#ifndef SYNTREE_IF_H
#define SYNTREE_IF_H


// syntax-node types
enum cb_syntree_node_type
{
	// all node-types up to the following have two child-nodes
	SNT_ASSIGNMENT = 256,
	SNT_COMPARISON,
	SNT_STATEMENTLIST,
	SNT_FUNC_DECL,
	SNT_FUNC_CALL,
	SNT_LIST,
	// the following node-types have one child-node
	SNT_UNARYMINUS,
	SNT_DECLARATION,
	SNT_PRINT,
	// the following node-types have no child-nodes
	SNT_CONSTVAL,
	SNT_CONSTBOOL,
	SNT_CONSTSTR,
	SNT_SYMREF,
	SNT_FLOW_IF,
	SNT_FLOW_WHILE
};

// forward-declarations
typedef struct syntree_node		CbSyntree;
typedef struct constval_node	CbConstvalNode;
typedef struct flow_node		CbFlowNode;
typedef struct comparison_node	CbComparisonNode;


#endif // SYNTREE_IF_H
