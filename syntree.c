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
	
	node->type	= SNT_SYMREF;
	node->sym	= s;
	
	return (syntree*) node;
}

// -----------------------------------------------------------------------------
// create a control-flow-node
// -----------------------------------------------------------------------------
syntree* flow_create(	enum syn_nodetype_t type, syntree* condition,
						syntree* then_branch, syntree* else_branch)
{
	// check if passed type is valid for this node
	if (type != SNT_FLOW_IF && type != SNT_FLOW_WHILE)
	{
		yyerror("invalid node-type for control-flow-node: %d", type);
		exit(1);
	}
	
	flow* node = malloc(sizeof(flow));
	if (!node)
	{
		yyerror(ERR_BADALLOC);
		exit(1);
	}
	
	node->type	= type;
	node->cond	= condition;
	node->tb	= then_branch;
	node->fb	= else_branch;
	
	return (syntree*) node;
}

// -----------------------------------------------------------------------------
// create a comparison-node
// -----------------------------------------------------------------------------
syntree* comparison_create(	enum cmp_nodetype_t type, syntree* left_node,
							syntree* right_node)
{
	comparison* node = malloc(sizeof(comparison));
	if (!node)
	{
		yyerror(ERR_BADALLOC);
		exit(1);
	}
	
	node->type		= SNT_COMPARISON;
	node->cmp_type	= type;
	node->l			= left_node;
	node->r			= right_node;
	
	return (syntree*) node;
}

// -----------------------------------------------------------------------------
// free a syntax-tree
// -----------------------------------------------------------------------------
void syntree_free(syntree* node)
{
	switch (node->type)
	{
		// two child-nodes
		case '+':
		case '-':
		case '*':
		case '/':
		case SNT_ASSIGNMENT:
			syntree_free(node->r);
			// no break here to free left child-node as well
		
		// one child-node (left one)
		case SNT_UNARYMINUS:
		case SNT_DECLARATION:
			syntree_free(node->l);
		
		// no child-nodes
		case SNT_CONSTVAL:
		case SNT_SYMREF:
			break;
		
		// special nodes
		case SNT_FLOW_IF:
		case SNT_FLOW_WHILE:
			syntree_free(((flow*) node)->cond);
			if (((flow*) node)->tb)	// true-branch
				syntree_free(((flow*) node)->tb);
			if (((flow*) node)->fb)	// false-branch
				syntree_free(((flow*) node)->fb);
			break;
		
		case SNT_COMPARISON:
			syntree_free(((comparison*) node)->l);
			syntree_free(((comparison*) node)->r);
			break;
		
		default:
			yyerror("syntax-tree node-type not recognized: %d", node->type);
			exit(1);
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
	symbol* sym;
	comparison* cmp;
	
	switch (node->type)
	{
		case SNT_CONSTVAL:
			result = ((constval*) node)->value;
			break;
		
		case SNT_SYMREF:
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
		
		case SNT_FLOW_IF:
			// evaluate condition and check its result:
			// every non-zero value is considered as true.
			if (eval(((flow*) node)->cond) != 0)
				result = eval(((flow*) node)->tb);	// condition is ture:
													// take the true-branch
			else
				result = eval(((flow*) node)->fb);	// condition is false:
													// take the false-branch
			break;
		
		case SNT_FLOW_WHILE:
			// evaluate true-branch while the condition returns a non-zero value
			while (eval(((flow*) node)->cond) != 0)
				result = eval(((flow*) node)->tb);
			break;
		
		case SNT_COMPARISON:
			cmp = ((comparison*) node);
			// evaluate comparison
			switch (cmp->cmp_type)
			{
				case CMP_EQ:
					result = (eval(cmp->l) == eval(cmp->r));
					break;
				
				case CMP_NE:
					result = (eval(cmp->l) != eval(cmp->r));
					break;
				
				case CMP_GE:
					result = (eval(cmp->l) >= eval(cmp->r));
					break;
				
				case CMP_LE:
					result = (eval(cmp->l) <= eval(cmp->r));
					break;
				
				case CMP_GT:
					result = (eval(cmp->l) > eval(cmp->r));
					break;
				
				case CMP_LT:
					result = (eval(cmp->l) < eval(cmp->r));
					break;
				
				default:
					result = 0;	// false
					yyerror("unknown comparison-type: %d",
							((comparison*) node)->cmp_type);
			}
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
		
		case SNT_UNARYMINUS:
			result = - eval(node->l);
			break;
		
		default:
			yyerror("syntax-tree node-type not recognized: %d", node->type);
			exit(1);
	}
	
	return result;
}
