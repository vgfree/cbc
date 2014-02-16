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
syntree_t* constval_create(CbNumeric value)
{
	constval_t* node= malloc(sizeof(constval_t));
	node->type		= SNT_CONSTVAL;
	node->value		= cb_numeric_create(value);
	
	return (syntree_t*) node;
}

// -----------------------------------------------------------------------------
// create a string-node
// -----------------------------------------------------------------------------
syntree_t* conststr_create(CbString string)
{
	constval_t* node= malloc(sizeof(constval_t));
	node->type		= SNT_CONSTSTR;
	node->value		= cb_string_create(strdup(string));
	
	return (syntree_t*) node;
}

// -----------------------------------------------------------------------------
// create a boolean-node
// -----------------------------------------------------------------------------
syntree_t* constbool_create(CbBoolean boolean)
{
	constval_t* node= malloc(sizeof(constval_t));
	node->type		= SNT_CONSTBOOL;
	node->value		= cb_boolean_create(boolean);
	
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
syntree_t* comparison_create(	enum cb_comparison_type type,
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
		{
			funcdecl_t* fndecl = ((funcdecl_t*) node);
			free(fndecl->sym_id);
			syntree_free(fndecl->body);
			strlist_free(fndecl->params);
			break;
		}
		
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
			cb_value_free(((constval_t*) node)->value);
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
CbValue* syntree_eval(syntree_t* node, symtab_t* symtab)
{
	CbValue* result = NULL;
	
	switch (node->type)
	{
		case SNT_CONSTVAL:
		case SNT_CONSTBOOL:
		case SNT_CONSTSTR:
			result = cb_value_copy(((constval_t*) node)->value);
			break;
		
		case SNT_SYMREF:
		{
			symref_t* sr = (symref_t*) node;
			symref_setsymbolfromtable(sr, symtab);
			result = cb_value_copy(symbol_variable_get_value(sr->table_sym));
			break;
		}
		
		case SNT_ASSIGNMENT:
		{
			// evaluate right-hand-side first, since it could contain a
			// function-call, which could change the order in the symbol-table!
			CbValue* rhs = syntree_eval(node->r, symtab);
			
			// after that, set symbol from the symbol-table
			symref_t* sr = (symref_t*) node->l;
			symref_setsymbolfromtable(sr, symtab);
			
			// assign right-hand-side expression
			symbol_variable_assign_value(sr->table_sym, rhs);
			cb_value_free(rhs);
			
			result = cb_value_copy(symbol_variable_get_value(sr->table_sym));
			break;
		}
		
		case SNT_DECLARATION:
		{
			symref_t* sr = (symref_t*) node->l;
			
			symbol_t* dummy = symbol_create_variable(sr->sym_id);
			symtab_append(symtab, dummy);	// declare symbol
			
			result = cb_value_create();		// return empty value
			break;
		}
		
		case SNT_FUNC_DECL:
		{
			funcdecl_t* fndecl = (funcdecl_t*) node;
			
			// prepare function-object
			function_t* func	  = function_create_user_defined(fndecl->sym_id,
																 fndecl->body);
			func->params		  = fndecl->params;
			if (!func->params)
				func->param_count = 0;
			else
				func->param_count = func->params->count;
			
			symbol_t* s = symbol_create_function(fndecl->sym_id, func);
			symtab_append(symtab, s);	// declare function
			
			result = cb_value_create();	// return empty value
			break;
		}
		
		case SNT_PRINT:
		{
			CbValue* temp = syntree_eval(node->l, symtab);
			cb_value_print(temp);
			cb_value_free(temp);
			// print newline
			printf("\n");
			// return empty value
			result = cb_value_create();
			break;
		}
		
		case SNT_FUNC_CALL:
		{
			funccall_t* fncall = (funccall_t*) node;
			symref_setsymbolfromtable((symref_t*) fncall, symtab);
			function_t* f = symbol_function_get_function(fncall->table_sym);
			
			function_call(f, fncall->args, symtab);
			result = cb_value_copy(f->result);
			function_reset(f);
			
			break;
		}
		
		case SNT_FLOW_IF:
		{
			CbValue* condition = syntree_eval(((flow_t*) node)->cond, symtab);
			if (!cb_value_is_type(condition, VT_BOOLEAN))
			{
				fprintf(stderr, "expecting boolean expression");
				exit(EXIT_FAILURE);
			}
			
			// evaluate condition and check its result
			if (condition->boolean)
				result = syntree_eval(((flow_t*) node)->tb, symtab);// condition is ture:
																	// take the true-branch
			else
			{
				// condition is false: take the false-branch, if there is one
				if (((flow_t*) node)->fb)
					result = syntree_eval(((flow_t*) node)->fb, symtab);
				else
					result = cb_value_create();
			}
			
			cb_value_free(condition);
			break;
		}
		
		case SNT_FLOW_WHILE:
		{
			CbValue* temp = syntree_eval(((flow_t*) node)->cond, symtab);
			if (!cb_value_is_type(temp, VT_BOOLEAN))
			{
				fprintf(stderr, "expecting boolean expression");
				exit(EXIT_FAILURE);
			}
			
			// default result (in case the while-loop won't be entered)
			result = cb_value_create();
			
			// evaluate true-branch while the condition returns true
			while (temp->boolean)
			{
				cb_value_free(temp);
				if (result)	// TODO:	Check is not necessary since result is
							//			assigend before, so it can't be NULL.
					cb_value_free(result);
				
				result	= syntree_eval(((flow_t*) node)->tb, symtab);
				temp	= syntree_eval(((flow_t*) node)->cond, symtab);
			}
			
			// free last dummy-value
			cb_value_free(temp);
			break;
		}
		
		case SNT_COMPARISON:
		{
			comparison_t* cmp = ((comparison_t*) node);
			result = cb_value_compare(	cmp->cmp_type, syntree_eval(cmp->l, symtab),
									syntree_eval(cmp->r, symtab));
			break;
		}
		
		case SNT_STATEMENTLIST:
			cb_value_free(syntree_eval(node->l, symtab));
			result = syntree_eval(node->r, symtab);
			break;
		
		case '+':
			result = cb_value_add(syntree_eval(node->l, symtab),
							   syntree_eval(node->r, symtab));
			break;
		case '-':
			result = cb_numeric_sub(	syntree_eval(node->l, symtab),
									syntree_eval(node->r, symtab));
			break;
		case '*':
			result = cb_numeric_mul(	syntree_eval(node->l, symtab),
									syntree_eval(node->r, symtab));
			break;
		case '/':
			result = cb_numeric_div(	syntree_eval(node->l, symtab),
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
