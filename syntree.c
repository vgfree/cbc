/*******************************************************************************
 * CbSyntree -- Implementation of an abstract syntax tree structure.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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
CbSyntree* cb_syntree_create(enum cb_syntree_node_type type,
							 CbSyntree* left_node, CbSyntree* right_node)
{
	CbSyntree* node	= malloc(sizeof(CbSyntree));
	node->type		= type;
	node->l			= left_node;
	node->r			= right_node;
	
	return node;
}

// -----------------------------------------------------------------------------
// create a syntax-tree value-node
// -----------------------------------------------------------------------------
CbSyntree* cb_constval_create(CbNumeric value)
{
	CbConstvalNode* node = malloc(sizeof(CbConstvalNode));
	node->type			 = SNT_CONSTVAL;
	node->value			 = cb_numeric_create(value);
	
	return (CbSyntree*) node;
}

// -----------------------------------------------------------------------------
// create a string-node
// -----------------------------------------------------------------------------
CbSyntree* cb_conststr_create(CbString string)
{
	CbConstvalNode* node = malloc(sizeof(CbConstvalNode));
	node->type			 = SNT_CONSTSTR;
	node->value			 = cb_string_create(strdup(string));
	
	return (CbSyntree*) node;
}

// -----------------------------------------------------------------------------
// create a boolean-node
// -----------------------------------------------------------------------------
CbSyntree* cb_constbool_create(CbBoolean boolean)
{
	CbConstvalNode* node = malloc(sizeof(CbConstvalNode));
	node->type			 = SNT_CONSTBOOL;
	node->value			 = cb_boolean_create(boolean);
	
	return (CbSyntree*) node;
}

// -----------------------------------------------------------------------------
// create a control-flow-node
// -----------------------------------------------------------------------------
CbSyntree* cb_flow_create(enum cb_syntree_node_type type, CbSyntree* condition,
						  CbSyntree* then_branch, CbSyntree* else_branch)
{
	// check if passed type is valid for this node
	if (type != SNT_FLOW_IF && type != SNT_FLOW_WHILE)
	{
		fprintf(stderr, "Error: Invalid node-type for control-flow-node: %d",
				type);
		exit(EXIT_FAILURE);
	}
	
	CbFlowNode* node = malloc(sizeof(CbFlowNode));
	node->type		 = type;
	node->cond		 = condition;
	node->tb		 = then_branch;
	node->fb		 = else_branch;
	
	return (CbSyntree*) node;
}

// -----------------------------------------------------------------------------
// create a comparison-node
// -----------------------------------------------------------------------------
CbSyntree* cb_comparison_create(enum cb_comparison_type type,
								CbSyntree* left_node, CbSyntree* right_node)
{
	CbComparisonNode* node = malloc(sizeof(CbComparisonNode));
	node->type			   = SNT_COMPARISON;
	node->cmp_type		   = type;
	node->l				   = left_node;
	node->r				   = right_node;
	
	return (CbSyntree*) node;
}

// -----------------------------------------------------------------------------
// free a syntax-tree
// -----------------------------------------------------------------------------
void cb_syntree_free(CbSyntree* node)
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
			cb_syntree_free(node->r);
			// no break here to free left child-node as well
		
		// one child-node (left one)
		case SNT_UNARYMINUS:
		case SNT_DECLARATION:
		case SNT_PRINT:
			cb_syntree_free(node->l);
			break;
		
		// special nodes
		case SNT_LIST:
			cb_syntree_free(node->r);
			if (node->l)
				cb_syntree_free(node->l);
			break;
		
		case SNT_FUNC_DECL:
		{
			funcdecl_t* fndecl = ((funcdecl_t*) node);
			free(fndecl->sym_id);
			cb_syntree_free(fndecl->body);
			strlist_free(fndecl->params);
			break;
		}
		
		case SNT_SYMREF:
			free(((symref_t*) node)->sym_id);
			break;
		
		case SNT_FLOW_IF:
		case SNT_FLOW_WHILE:
			cb_syntree_free(((CbFlowNode*) node)->cond);
			if (((CbFlowNode*) node)->tb)	// true-branch
				cb_syntree_free(((CbFlowNode*) node)->tb);
			if (((CbFlowNode*) node)->fb)	// false-branch
				cb_syntree_free(((CbFlowNode*) node)->fb);
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
					cb_syntree_free(temp->data);
				}
				
				strlist_free(args);
			}
			
			free(((funccall_t*) node)->sym_id);
			
			break;
		}
		
		case SNT_CONSTVAL:
		case SNT_CONSTBOOL:
		case SNT_CONSTSTR:
			cb_value_free(((CbConstvalNode*) node)->value);
			break;
		
		case SNT_COMPARISON:
			cb_syntree_free(((CbComparisonNode*) node)->l);
			cb_syntree_free(((CbComparisonNode*) node)->r);
			break;
			
		default:
			fprintf(stderr, "syntax-tree node-type not recognized: %d",
					node->type);
			exit(EXIT_FAILURE);
	}
	// always free node itself at the end
	free(node);
}

// -----------------------------------------------------------------------------
// evaluate a complete syntax tree and return its result
// -----------------------------------------------------------------------------
CbValue* cb_syntree_eval(CbSyntree* node, CbSymtab* symtab)
{
	CbValue* result = NULL;
	
	switch (node->type)
	{
		case SNT_CONSTVAL:
		case SNT_CONSTBOOL:
		case SNT_CONSTSTR:
			result = cb_value_copy(((CbConstvalNode*) node)->value);
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
			CbValue* rhs = cb_syntree_eval(node->r, symtab);
			
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
			cb_symtab_append(symtab, dummy);	// declare symbol
			
			result = cb_value_create();		// return empty value
			break;
		}
		
		case SNT_FUNC_DECL:
		{
			funcdecl_t* fndecl = (funcdecl_t*) node;
			
			// prepare function-object
			function_t* func = function_create_user_defined(fndecl->sym_id,
															fndecl->body);
			func->params	 = fndecl->params;
			if (!func->params)
				func->param_count = 0;
			else
				func->param_count = func->params->count;
			
			symbol_t* s = symbol_create_function(fndecl->sym_id, func);
			cb_symtab_append(symtab, s);	// declare function
			
			result = cb_value_create();	// return empty value
			break;
		}
		
		case SNT_PRINT:
		{
			CbValue* temp = cb_syntree_eval(node->l, symtab);
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
			CbValue* condition = cb_syntree_eval(((CbFlowNode*) node)->cond, symtab);
			assert(cb_value_is_type(condition, VT_BOOLEAN));
			
			// evaluate condition and check its result
			if (condition->boolean)
				// condition is ture -> evaluate the true-branch
				result = cb_syntree_eval(((CbFlowNode*) node)->tb, symtab);
			else
			{
				// condition is false -> evaluate the false-branch,
				// if there is one
				if (((CbFlowNode*) node)->fb)
					result = cb_syntree_eval(((CbFlowNode*) node)->fb, symtab);
				else
					// otherwise, return an empty value
					result = cb_value_create();
			}
			
			cb_value_free(condition);
			break;
		}
		
		case SNT_FLOW_WHILE:
		{
			CbValue* temp = cb_syntree_eval(((CbFlowNode*) node)->cond, symtab);
			assert(cb_value_is_type(temp, VT_BOOLEAN));
			
			// default result (in case the while-loop won't be entered)
			result = cb_value_create();
			
			// evaluate true-branch while the condition returns true
			while (temp->boolean)
			{
				cb_value_free(temp);
				if (result)	// TODO:	Check is not necessary since result is
							//			assigend before, so it can't be NULL.
					cb_value_free(result);
				
				result = cb_syntree_eval(((CbFlowNode*) node)->tb, symtab);
				temp   = cb_syntree_eval(((CbFlowNode*) node)->cond, symtab);
			}
			
			// free last dummy-value
			cb_value_free(temp);
			break;
		}
		
		case SNT_COMPARISON:
		{
			CbComparisonNode* cmp = ((CbComparisonNode*) node);
			result = cb_value_compare(cmp->cmp_type,
									  cb_syntree_eval(cmp->l, symtab),
									  cb_syntree_eval(cmp->r, symtab));
			break;
		}
		
		case SNT_STATEMENTLIST:
			cb_value_free(cb_syntree_eval(node->l, symtab));
			result = cb_syntree_eval(node->r, symtab);
			break;
		
		case '+':
			result = cb_value_add(cb_syntree_eval(node->l, symtab),
								  cb_syntree_eval(node->r, symtab));
			break;
		case '-':
			result = cb_numeric_sub(cb_syntree_eval(node->l, symtab),
									cb_syntree_eval(node->r, symtab));
			break;
		case '*':
			result = cb_numeric_mul(cb_syntree_eval(node->l, symtab),
									cb_syntree_eval(node->r, symtab));
			break;
		case '/':
			result = cb_numeric_div(cb_syntree_eval(node->l, symtab),
									cb_syntree_eval(node->r, symtab));
			break;
		
		case SNT_UNARYMINUS:
			result			= cb_syntree_eval(node->l, symtab);
			result->value	= - result->value;
			break;
		
		default:
			fprintf(stderr, "syntax-tree node-type not recognized: %d",
					node->type);
			exit(EXIT_FAILURE);
	}
	
	return result;
}
