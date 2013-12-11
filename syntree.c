/*******************************************************************************
 * syntree -- implementation of an abstract syntax tree structure
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include "syntree.h"
#include "errors.h"

// -----------------------------------------------------------------------------
// create a syntax-tree node
// -----------------------------------------------------------------------------
syntree* syntree_create(enum syn_nodetype_t type, syntree* left_node,
						syntree* right_node)
{
	syntree* node = malloc(sizeof(syntree));
	if (!node)
	{
		yyerror(ERR_BADALLOC);
		exit(1);
	}
	
	node->type	= type;
	node->l		= left_node;
	node->r		= right_node;
	
	return node;
}

// -----------------------------------------------------------------------------
// create a syntax-tree value-node
// -----------------------------------------------------------------------------
syntree* constval_create(int value)
{
	constval* node = malloc(sizeof(constval));
	if (!node)
	{
		yyerror(ERR_BADALLOC);
		exit(1);
	}
	
	node->type	= SNT_CONSTVAL;
	node->value	= value;
	
	return (syntree*) node;
}

// -----------------------------------------------------------------------------
// create a syntax-tree symbol-reference-node
// -----------------------------------------------------------------------------
syntree* symref_create(symbol* s)
{
	symref* node = malloc(sizeof(symref));
	if (!node)
	{
		yyerror(ERR_BADALLOC);
		exit(1);
	}
	
	node->type	= SNT_SYMBOL_REF;
	node->sym	= s;
	
	return (syntree*) node;
}

// -----------------------------------------------------------------------------
// free a syntax-tree
// -----------------------------------------------------------------------------
void syntree_free(syntree* node)
{
	// nodes of a type below the pseudo type SNT_NOTYPE_ONE_CHILDNODE have two
	// child-nodes. That is, free both of them.
	if (node->type < SNT_NOTYPE_ONE_CHILDNODE)
	{
		// free child-nodes
		syntree_free(node->l);
		syntree_free(node->r);
	}
	// nodes of a type below the pseudo type SNT_NOTYPE_NO_CHILDNODES have at
	// least one - the left one - child-node.
	else if (node->type < SNT_NOTYPE_NO_CHILDNODES)
		// free left child node
		syntree_free(node->l);
	// DO NOT TRY TO DELETE ANY CHILD-NODE OF A NODE A TYPE ABOVE
	// SNT_NOTYPE_NO_CHILDNODES! SINCE THIS COULD CAUSE UNDEFINED BEHAVIOUR!
	
	// always free node itself at the end
	free(node);
}

// -----------------------------------------------------------------------------
// evaluate a complete syntax tree and return its result
// -----------------------------------------------------------------------------
int eval(syntree* node)
{
	int result;
	symbol* sym;
	
	switch (node->type)
	{
		case SNT_CONSTVAL:
			result = ((constval*) node)->value;
			break;
		
		case SNT_SYMBOL_REF:
			sym = ((symref*) node)->sym;
			// check if symbol is defined
			if (sym->type == SYM_UNDEFINED)
			{
				yyerror("undefined symbol: %s", sym->identifier);
				// dummy-symbols are not contained in the symbol-table -> free
				// them separately!
				symbol_free(sym);
				exit(1);
			}
			else
				result = sym->value;
			break;
		
		case SNT_ASSIGNMENT:
			sym = ((symref*) node->l)->sym;
			// check if symbol is defined
			if (sym->type == SYM_UNDEFINED)
			{
				yyerror("undefined symbol: %s", sym->identifier);
				// free dummy-symbol
				symbol_free(sym);
				exit(1);
			}
			else
				result = sym->value = eval(node->r);
			break;
			
		case SNT_DECLARATION:
			sym = ((symref*) node->l)->sym;
			if (sym->type == SYM_UNDEFINED)
			{
				sym->type = SYM_VARIABLE;
				symtab_append(gl_symtab, sym);
			}
			else
				yyerror("cannot redeclare symbol: %s", sym->identifier);
			
			result = 0;
			break;
		
		case '+': result = eval(node->l) + eval(node->r); break;
		case '-': result = eval(node->l) - eval(node->r); break;
		case '*': result = eval(node->l) * eval(node->r); break;
		case '/':
			result	= 0;
			int rhs	= eval(node->r);
			// check for division by zero first!
			if (rhs != 0)
				result = eval(node->l) / rhs;
			else
			{
				yyerror("division by zero is not allowed!");
				exit(1);
			}
			break;
		
		case SNT_SIGNED_MINUS:
			result = - eval(node->l);
			break;
		
		default:
			yyerror("syntax-tree node-type not recognized: %d", node->type);
			exit(1);
	}
	
	return result;
}
