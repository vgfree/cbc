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
	SNT_LOGICAL_AND,
	SNT_LOGICAL_OR,
	SNT_FUNC_DECL,
	SNT_FUNC_CALL,
	// the following node-types have one child-node
	SNT_UNARYMINUS,
	SNT_DECLARATION,
	SNT_PRINT,
	SNT_LOGICAL_NOT,
	// the following node-types have no child-nodes
	SNT_CONSTVAL,
	SNT_CONSTBOOL,
	SNT_CONSTSTR,
	SNT_VALARRAY,
	SNT_SYMREF,
	SNT_FLOW_IF,
	SNT_FLOW_WHILE,
	SNT_EXCEPTION_BLOCK
};

// forward-declarations
typedef struct syntree_node		CbSyntree;
typedef struct constval_node	CbConstvalNode;
typedef struct flow_node		CbFlowNode;
typedef struct comparison_node	CbComparisonNode;


#endif // SYNTREE_IF_H
