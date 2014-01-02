/*******************************************************************************
 * syntree -- implementation of an abstract syntax tree structure
 ******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "syntree.h"
#include "symref.h"
#include "fncall.h"
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
syntree* constval_create(cbnumeric value)
{
	constval* node = malloc(sizeof(constval));
	if (!node)
	{
		yyerror(ERR_BADALLOC);
		exit(1);
	}
	
	node->type	= SNT_CONSTVAL;
	node->value	= cbnumeric_create(value);
	
	return (syntree*) node;
}

// -----------------------------------------------------------------------------
// create a string-node
// -----------------------------------------------------------------------------
syntree* conststr_create(cbstring string)
{
	constval* node = malloc(sizeof(constval));
	if (!node)
	{
		yyerror(ERR_BADALLOC);
		exit(1);
	}
	
	node->type	= SNT_CONSTSTR;
	node->value	= cbstring_create(strdup(string));
	
	return (syntree*) node;
}

// -----------------------------------------------------------------------------
// create a boolean-node
// -----------------------------------------------------------------------------
syntree* constbool_create(cbboolean boolean)
{
	constval* node = malloc(sizeof(constval));
	if (!node)
	{
		yyerror(ERR_BADALLOC);
		exit(1);
	}
	
	node->type	= SNT_CONSTBOOL;
	node->value	= cbboolean_create(boolean);
	
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
syntree* comparison_create(	enum comparisontype_t type, syntree* left_node,
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
		case SNT_STATEMENTLIST:
		case SNT_FUNC_DECL:
			syntree_free(node->r);
			// no break here to free left child-node as well
		
		// one child-node (left one)
		case SNT_UNARYMINUS:
		case SNT_DECLARATION:
		case SNT_PRINT:
			syntree_free(node->l);
		
		// no child-nodes
		case SNT_SYMREF:
		case SNT_FUNC_CALL:
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
		
		case SNT_CONSTVAL:
		case SNT_CONSTBOOL:
		case SNT_CONSTSTR:
			cbvalue_free(((constval*) node)->value);
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
cbvalue* eval(syntree* node)
{
	cbvalue* result = NULL;
	
	switch (node->type)
	{
		case SNT_CONSTVAL:
		case SNT_CONSTBOOL:
		case SNT_CONSTSTR:
			result = cbvalue_copy(((constval*) node)->value);
			break;
		
		case SNT_SYMREF:
			symref_setsymbolfromtable((symref*) node);
			
			if (((symref*) node)->sym->type == SYM_UNDEFINED)
				result = cbvalue_create();			// return empty value
			else
				result = cbvalue_copy(((symref*) node)->sym->value);
			
			break;
		
		case SNT_ASSIGNMENT:
			symref_setsymbolfromtable((symref*) node->l);
			cbvalue_assign_freesource(	eval(node->r),
										((symref*) node->l)->sym->value);
			result = cbvalue_copy(((symref*) node->l)->sym->value);
			break;
		
		case SNT_DECLARATION:
		{
			symbol* sym = ((symref*) node->l)->sym;
			// check if symbol already exists in symbol-table
			symbol* found = symtab_lookup(gl_symtab, sym->identifier);
			if (found)
				// TODO: memory-leaks here!
				yyerror("cannot redeclare symbol: %s", sym->identifier);
			else
			{
				symbol_settype(sym, SYM_VARIABLE);
				symtab_append(gl_symtab, sym, NULL);
				// symbol was duplicated -> free old one
				symbol_free(sym);
			}
			// return empty value
			result = cbvalue_create();
			break;
		}
		
		case SNT_FUNC_DECL:
		{
			symbol* sym = ((symref*) node->l)->sym;
			// check if symbol already exists in symbol-table
			symbol* found = symtab_lookup(gl_symtab, sym->identifier);
			if (found)
				yyerror("cannot redeclare symbol: %s", sym->identifier);
			else
			{
				symbol_settype(sym, SYM_FUNCTION);
				symtab_append(gl_symtab, sym, node->r);
				// symbol was duplicated -> free old one
				symbol_free(sym);
			}
			// return empty value
			result = cbvalue_create();
			break;
		}
		
		case SNT_PRINT:
		{
			cbvalue* temp = eval(node->l);
			cbvalue_print(temp);
			cbvalue_free(temp);
			// print newline
			printf("\n");
			// return empty value
			result = cbvalue_create();
			break;
		}
		
		case SNT_FUNC_CALL:
			// symref_setsymbolfromtable can be used in this case, since both
			// structs have the same signature
			symref_setsymbolfromtable((symref*) node);
			result = eval(((fncall*) node)->sym->function);
			break;
		
		case SNT_FLOW_IF:
		{
			// evaluate condition and check its result:
			// every non-zero value is considered as true.
			cbvalue* condition = eval(((flow*) node)->cond);
			if (condition->value != 0)
				result = eval(((flow*) node)->tb);	// condition is ture:
													// take the true-branch
			else
				result = eval(((flow*) node)->fb);	// condition is false:
													// take the false-branch
			
			cbvalue_free(condition);
			break;
		}
		
		case SNT_FLOW_WHILE:
		{
			cbvalue* temp;
			// evaluate true-branch while the condition returns a non-zero value
			while ( (temp = eval(((flow*) node)->cond))->value != 0)
			{
				cbvalue_free(temp);
				if (result)
					cbvalue_free(result);
				
				result = eval(((flow*) node)->tb);
			}
			break;
		}
		
		case SNT_COMPARISON:
		{
			comparison* cmp = ((comparison*) node);
			result = cbvalue_compare(cmp->cmp_type, eval(cmp->l), eval(cmp->r));
			break;
		}
		
		case SNT_STATEMENTLIST:
			cbvalue_free(eval(node->l));
			result = eval(node->r);
			break;
		
		case '+': result = cbnumeric_add(eval(node->l), eval(node->r)); break;
		case '-': result = cbnumeric_sub(eval(node->l), eval(node->r)); break;
		case '*': result = cbnumeric_mul(eval(node->l), eval(node->r)); break;
		case '/': result = cbnumeric_div(eval(node->l), eval(node->r)); break;
		
		case SNT_UNARYMINUS:
			result = eval(node->l);
			result->value = - result->value;
			break;
		
		default:
			yyerror("syntax-tree node-type not recognized: %d", node->type);
			exit(1);
	}
	
	return result;
}
