/*******************************************************************************
 * syntree -- implementation of an abstract syntax tree structure
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "syntree.h"

// -----------------------------------------------------------------------------
// create a syntax-tree node
// -----------------------------------------------------------------------------
syntree* syntree_create(enum syn_nodetype_t type, syntree* left_node,
						syntree* right_node)
{
	syntree* node	= malloc(sizeof(syntree));
	node->type		= type;
	node->l			= left_node;
	node->r			= right_node;
	
	return node;
}

// -----------------------------------------------------------------------------
// create a syntax-tree value-node
// -----------------------------------------------------------------------------
syntree* constval_create(int value)
{
	constval* node	= malloc(sizeof(constval));
	node->type		= SNT_CONSTVAL;
	node->value		= value;
	
	return (syntree*) node;
}

// -----------------------------------------------------------------------------
// free a syntax-tree
// -----------------------------------------------------------------------------
void syntree_free(syntree* node)
{
	if (!node)
		return;
	
	// nodes of type SNT_CONSTVAL have no child-nodes.
	// DO NOT FREE CHILD-NODE POINTERS IN THIS CASE!
	if (node->type != SNT_CONSTVAL)
	{
		// free child-nodes
		syntree_free(node->l);
		syntree_free(node->r);
	}
	
	// always free node itself at the end
	free(node);
}

// -----------------------------------------------------------------------------
// evaluate a complete syntax tree and return its result
// -----------------------------------------------------------------------------
int eval(syntree* node)
{
	int result;
	
	switch (node->type)
	{
		case SNT_CONSTVAL:
			result = ((constval*) node)->value;
			break;
		
		case '+': result = eval(node->l) + eval(node->r); break;
		case '-': result = eval(node->l) - eval(node->r); break;
		case '*': result = eval(node->l) * eval(node->r); break;
		case '/': result = eval(node->l) / eval(node->r); break;
		
		case SNT_SIGNED_MINUS:
			result = - eval(node->l);
			break;
		
		default:
			printf("error: syntax-tree node-type not recognized: %d\n", node->type);
	}
	
	return result;
}
