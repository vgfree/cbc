/*******************************************************************************
 * syntree_t -- Implementation of an abstract syntax tree structure.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "syntree.h"
#include "symbol.h"
#include "symref.h"
#include "funccall.h"
#include "funcdecl.h"


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// create a syntax-tree node
// -----------------------------------------------------------------------------
syntree_t* syntree_create(	enum syn_nodetype_t type, syntree_t* left_node,
							syntree_t* right_node)
{
	syntree_t* node	= malloc(sizeof(syntree_t));
	node->type		= type;
	node->l			= left_node;
	node->r			= right_node;
	
	return node;
}

// -----------------------------------------------------------------------------
// create a syntax-tree value-node
// -----------------------------------------------------------------------------
syntree_t* constval_create(cbnumeric value)
{
	constval_t* node= malloc(sizeof(constval_t));
	node->type		= SNT_CONSTVAL;
	node->value		= cbnumeric_create(value);
	
	return (syntree_t*) node;
}

// -----------------------------------------------------------------------------
// create a string-node
// -----------------------------------------------------------------------------
syntree_t* conststr_create(cbstring string)
{
	constval_t* node= malloc(sizeof(constval_t));
	node->type		= SNT_CONSTSTR;
	node->value		= cbstring_create(strdup(string));
	
	return (syntree_t*) node;
}

// -----------------------------------------------------------------------------
// create a boolean-node
// -----------------------------------------------------------------------------
syntree_t* constbool_create(cbboolean boolean)
{
	constval_t* node= malloc(sizeof(constval_t));
	node->type		= SNT_CONSTBOOL;
	node->value		= cbboolean_create(boolean);
	
	return (syntree_t*) node;
}

// -----------------------------------------------------------------------------
// create a control-flow-node
// -----------------------------------------------------------------------------
syntree_t* flow_create(	enum syn_nodetype_t type, syntree_t* condition,
						syntree_t* then_branch, syntree_t* else_branch)
{
	// check if passed type is valid for this node
	if (type != SNT_FLOW_IF && type != SNT_FLOW_WHILE)
	{
		fprintf(stderr, "Error: Invalid node-type for control-flow-node: %d",
				type);
		exit(EXIT_FAILURE);
	}
	
	flow_t* node= malloc(sizeof(flow_t));
	node->type	= type;
	node->cond	= condition;
	node->tb	= then_branch;
	node->fb	= else_branch;
	
	return (syntree_t*) node;
}

// -----------------------------------------------------------------------------
// create a comparison-node
// -----------------------------------------------------------------------------
syntree_t* comparison_create(	enum comparison_type_t type,
								syntree_t* left_node, syntree_t* right_node)
{
	comparison_t* node	= malloc(sizeof(comparison_t));
	node->type			= SNT_COMPARISON;
	node->cmp_type		= type;
	node->l				= left_node;
	node->r				= right_node;
	
	return (syntree_t*) node;
}

// -----------------------------------------------------------------------------
// free a syntax-tree
// -----------------------------------------------------------------------------
void syntree_free(syntree_t* node)
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
			syntree_free(node->r);
			// no break here to free left child-node as well
		
		// one child-node (left one)
		case SNT_UNARYMINUS:
		case SNT_DECLARATION:
		case SNT_PRINT:
			syntree_free(node->l);
			break;
		
		// special nodes
		case SNT_LIST:
			syntree_free(node->r);
			if (node->l)
				syntree_free(node->l);
			break;
		
		case SNT_FUNC_DECL:
			break;
			
		case SNT_SYMREF:
			free(((symref_t*) node)->sym_id);
			break;
		
		case SNT_FLOW_IF:
		case SNT_FLOW_WHILE:
			syntree_free(((flow_t*) node)->cond);
			if (((flow_t*) node)->tb)	// true-branch
				syntree_free(((flow_t*) node)->tb);
			if (((flow_t*) node)->fb)	// false-branch
				syntree_free(((flow_t*) node)->fb);
			break;
		
		case SNT_FUNC_CALL:
		{
			strlist_t* args = ((funccall_t*) node)->args;
			if (args)
			{
				strlist_t* current = args;
				// free argument-nodes,
				// since they won't be freed by 'strlist_free()'
				while (current)
				{
					strlist_t* temp = current;
					current = current->next;
					syntree_free(temp->data);
				}
				
				strlist_free(args);
			}
			
			free(((funccall_t*) node)->sym_id);
			
			break;
		}
		
		case SNT_CONSTVAL:
		case SNT_CONSTBOOL:
		case SNT_CONSTSTR:
			value_free(((constval_t*) node)->value);
			break;
		
		case SNT_COMPARISON:
			syntree_free(((comparison_t*) node)->l);
			syntree_free(((comparison_t*) node)->r);
			break;
			
		default:
			fprintf(stderr, "syntax-tree node-type not recognized: %d", node->type);
			exit(EXIT_FAILURE);
	}
	// always free node itself at the end
	free(node);
}

// -----------------------------------------------------------------------------
// evaluate a complete syntax tree and return its result
// -----------------------------------------------------------------------------
value_t* syntree_eval(syntree_t* node, symtab_t* symtab)
{
	value_t* result = NULL;
	
	switch (node->type)
	{
		case SNT_CONSTVAL:
		case SNT_CONSTBOOL:
		case SNT_CONSTSTR:
			result = value_copy(((constval_t*) node)->value);
			break;
		
		case SNT_SYMREF:
			symref_setsymbolfromtable((symref_t*) node, symtab);
			result = value_copy(((symref_t*) node)->table_sym->value);
			break;
		
		case SNT_ASSIGNMENT:
		{
			symref_t* sr = (symref_t*) node->l;
			symref_setsymbolfromtable(sr, symtab);
			value_assign_freesource(syntree_eval(node->r, symtab), sr->table_sym->value);
			
			result = value_copy(sr->table_sym->value);
			break;
		}
		
		case SNT_DECLARATION:
		{
			symref_t* sr = (symref_t*) node->l;
			
			symbol_t* dummy = symbol_create();		// create new symbol
			symbol_setid(dummy, sr->sym_id);
			symbol_settype(dummy, SYM_TYPE_VARIABLE);
			symtab_append(symtab, dummy);			// declare symbol
			
			result = value_create();				// return empty value
			break;
		}
		
		case SNT_FUNC_DECL:
		{
			funcdecl_t* fndecl = (funcdecl_t*) node;
			symtab_append(symtab, fndecl->function_sym);	// declare function
			
			result = value_create();						// return empty value
			break;
		}
		
		case SNT_PRINT:
		{
			value_t* temp = syntree_eval(node->l, symtab);
			value_print(temp);
			value_free(temp);
			// print newline
			printf("\n");
			// return empty value
			result = value_create();
			break;
		}
		
		case SNT_FUNC_CALL:
		{
			funccall_t* fncall = (funccall_t*) node;
			symref_setsymbolfromtable((symref_t*) fncall, symtab);
			function_t* f = fncall->table_sym->function;
			
			function_call(f, fncall->args);
			result = value_copy(f->result);
			
			break;
		}
		
		case SNT_FLOW_IF:
		{
			value_t* condition = syntree_eval(((flow_t*) node)->cond, symtab);
			if (!value_istype(condition, VT_BOOLEAN))
			{
				fprintf(stderr, "expecting boolean expression");
				exit(EXIT_FAILURE);
			}
			
			// evaluate condition and check its result
			if (condition->boolean)
				result = syntree_eval(((flow_t*) node)->tb, symtab);// condition is ture:
																	// take the true-branch
			else
				result = syntree_eval(((flow_t*) node)->fb, symtab);// condition is false:
																	// take the false-branch
			
			value_free(condition);
			break;
		}
		
		case SNT_FLOW_WHILE:
		{
			value_t* temp = syntree_eval(((flow_t*) node)->cond, symtab);
			if (!value_istype(temp, VT_BOOLEAN))
			{
				fprintf(stderr, "expecting boolean expression");
				exit(EXIT_FAILURE);
			}
			
			// evaluate true-branch while the condition returns true
			while (temp->boolean)
			{
				value_free(temp);
				if (result)
					value_free(result);
				
				result	= syntree_eval(((flow_t*) node)->tb, symtab);
				temp	= syntree_eval(((flow_t*) node)->cond, symtab);
			}
			
			// free last dummy-value
			value_free(temp);
			break;
		}
		
		case SNT_COMPARISON:
		{
			comparison_t* cmp = ((comparison_t*) node);
			result = value_compare(	cmp->cmp_type, syntree_eval(cmp->l, symtab),
									syntree_eval(cmp->r, symtab));
			break;
		}
		
		case SNT_STATEMENTLIST:
			value_free(syntree_eval(node->l, symtab));
			result = syntree_eval(node->r, symtab);
			break;
		
		case '+':
			result = cbnumeric_add(	syntree_eval(node->l, symtab),
									syntree_eval(node->r, symtab));
			break;
		case '-':
			result = cbnumeric_sub(	syntree_eval(node->l, symtab),
									syntree_eval(node->r, symtab));
			break;
		case '*':
			result = cbnumeric_mul(	syntree_eval(node->l, symtab),
									syntree_eval(node->r, symtab));
			break;
		case '/':
			result = cbnumeric_div(	syntree_eval(node->l, symtab),
									syntree_eval(node->r, symtab));
			break;
		
		case SNT_UNARYMINUS:
			result			= syntree_eval(node->l, symtab);
			result->value	= - result->value;
			break;
		
		default:
			fprintf(stderr, "syntax-tree node-type not recognized: %d", node->type);
			exit(EXIT_FAILURE);
	}
	
	return result;
}
