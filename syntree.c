/*******************************************************************************
 * syntree -- implementation of an abstract syntax tree structure
 ******************************************************************************/

#include <stdlib.h>
#include "syntree.h"

syntree* syntree_create(enum syn_nodetype_t type, syntree* left_node,
						syntree* right_node)
{
	syntree* node	= malloc(sizeof(syntree));
	node->type		= type;
	node->l			= left_node;
	node->r			= right_node;
	
	return node;
}

syntree* constval_create(int value)
{
	constval* node	= malloc(sizeof(constval));
	node->type		= SNT_CONSTVAL;
	node->value		= value;
	
	return (syntree*) node;
}

void syntree_free(syntree* node)
{
	if (!node)
		return;
	
	// free child-nodes
	syntree_free(node->l);
	syntree_free(node->r);
	// free node itself
	free(node);
}
