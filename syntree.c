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
#include "exception_block_node.h"
#include "array_node.h"
#include "error_handling.h"


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
	node->line_no	= 0;
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
	node->line_no		 = 0;
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
	node->line_no		 = 0;
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
	node->line_no		 = 0;
	node->value			 = cb_boolean_create(boolean);
	
	return (CbSyntree*) node;
}

// -----------------------------------------------------------------------------
// create a control-flow-node
// -----------------------------------------------------------------------------
CbSyntree* cb_flow_create(enum cb_syntree_node_type type, CbSyntree* condition,
						  CbSyntree* then_branch, CbSyntree* else_branch)
{
	assert(type == SNT_FLOW_IF || type == SNT_FLOW_WHILE);
	
	CbFlowNode* node = malloc(sizeof(CbFlowNode));
	node->type		 = type;
	node->line_no	 = 0;
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
	node->line_no		   = 0;
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
		case SNT_LOGICAL_AND:
		case SNT_LOGICAL_OR:
			cb_syntree_free(node->r);
			// no break here to free left child-node as well
		
		// one child-node (left one)
		case SNT_UNARYMINUS:
		case SNT_DECLARATION:
		case SNT_PRINT:
		case SNT_LOGICAL_NOT:
			cb_syntree_free(node->l);
			break;
		
		// special nodes
		case SNT_FUNC_DECL:
		{
			CbFuncDeclarationNode* fndecl = ((CbFuncDeclarationNode*) node);
			free(fndecl->sym_id);
			cb_syntree_free(fndecl->body);
			cb_strlist_free(fndecl->params);
			break;
		}
		
		case SNT_SYMREF:
			free(((CbSymref*) node)->sym_id);
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
			CbStrlist* args = ((CbFuncCallNode*) node)->args;
			if (args)
			{
				CbStrlist* current = args;
				// free argument-nodes,
				// since they won't be freed by 'strlist_free()'
				while (current)
				{
					CbStrlist* temp = current;
					current = current->next;
					cb_syntree_free(temp->data);
				}
				
				cb_strlist_free(args);
			}
			
			free(((CbFuncCallNode*) node)->sym_id);
			
			break;
		}
		
		case SNT_CONSTVAL:
		case SNT_CONSTBOOL:
		case SNT_CONSTSTR:
			cb_value_free(((CbConstvalNode*) node)->value);
			break;
		
		case SNT_VALARRAY:
        {
            CbStrlist* item = ((CbArrayNode*) node)->values;
            while (item)
            {
                cb_syntree_free((CbSyntree*) item->data);
                item = item->next;
            }
			cb_strlist_free(((CbArrayNode*) node)->values);
            
			break;
		}
        
		case SNT_COMPARISON:
			cb_syntree_free(((CbComparisonNode*) node)->l);
			cb_syntree_free(((CbComparisonNode*) node)->r);
			break;
		
		case SNT_EXCEPTION_BLOCK:
			cb_syntree_free(((CbExceptionBlockNode*) node)->code_block);
			cb_syntree_free(((CbExceptionBlockNode*) node)->exception_block);
			break;
		
		default:
			// do not report errors if the node type is not reckognized, since
			// the syntax tree is being freed anyway
			break;
	}
	// always free node itself at the end
	free(node);
}

// -----------------------------------------------------------------------------
// Evaluate a complete syntax tree and return its result
// 
//    In case of an error the return value is NULL
// -----------------------------------------------------------------------------
CbValue* cb_syntree_eval(CbSyntree* node, CbSymtab* symtab)
{
	CbValue* result = NULL;
	
	// Check for uncatched errors
	if ((cb_error_is_set()) && !cb_error_is_catched())
		return NULL;
	
	switch (node->type)
	{
		case SNT_CONSTVAL:
		case SNT_CONSTBOOL:
		case SNT_CONSTSTR:
			result = cb_value_copy(((CbConstvalNode*) node)->value);
			break;
        
		case SNT_VALARRAY:
			result = cb_array_node_eval((CbArrayNode*) node, symtab);
			break;
		
		case SNT_SYMREF:
		{
			CbSymref* sr = (CbSymref*) node;
			if (cb_symref_set_symbol_from_table(sr, symtab) == EXIT_SUCCESS)
				result = cb_value_copy(cb_symbol_variable_get_value(sr->table_sym));
			
			break;
		}
		
		case SNT_ASSIGNMENT:
		{
			// evaluate right-hand-side first, since it could contain a
			// function-call, which could change the order in the symbol-table!
			CbValue* rhs = cb_syntree_eval(node->r, symtab);
			if (rhs == NULL)
				break;
			
			// after that, set symbol from the symbol-table
			CbSymref* sr = (CbSymref*) node->l;
			// check if an error occurred
			if (cb_symref_set_symbol_from_table(sr, symtab) == EXIT_FAILURE)
			{
				cb_value_free(rhs);	// free allocated memory
				break;
			}
			
			if (!cb_error_is_set() || cb_error_is_catched())
				// assign right-hand-side expression
				cb_symbol_variable_assign_value(sr->table_sym, rhs);
			
			cb_value_free(rhs);
			
			result = cb_value_copy(cb_symbol_variable_get_value(sr->table_sym));
			break;
		}
		
		case SNT_DECLARATION:
		{
			CbSymref* sr = (CbSymref*) node->l;
			
			CbSymbol* dummy = cb_symbol_create_variable(sr->sym_id);
			if (cb_symtab_append(symtab, dummy))	// declare symbol
				result = cb_value_create();	// return empty value,
											// if there were no errors
			else
				cb_symbol_free(dummy);
			
			break;
		}
		
		case SNT_FUNC_DECL:
		{
			CbFuncDeclarationNode* fndecl = (CbFuncDeclarationNode*) node;
			
			// prepare function-object
			CbFunction* func = cb_function_create_user_defined(fndecl->sym_id,
															fndecl->body);
			func->params	 = fndecl->params;
			if (!func->params)
				func->param_count = 0;
			else
				func->param_count = func->params->count;
			
			CbSymbol* s = cb_symbol_create_function(fndecl->sym_id, func);
			if (cb_symtab_append(symtab, s))	// declare function
				result = cb_value_create();	// return empty value,
											// if there were no errors
			else
				cb_symbol_free(s);
			
			break;
		}
		
		case SNT_PRINT:
		{
			CbValue* temp = cb_syntree_eval(node->l, symtab);
			if (temp == NULL)
				break;
			
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
			CbFuncCallNode* fncall = (CbFuncCallNode*) node;
			if (cb_symref_set_symbol_from_table((CbSymref*) fncall, symtab) == EXIT_FAILURE)
				break;	// an error occurred -> break
			
			CbFunction* f = cb_symbol_function_get_function(fncall->table_sym);
			if (cb_function_call(f, fncall->args, symtab) == EXIT_SUCCESS)
			{
				result = cb_value_copy(f->result);
				cb_function_reset(f);
			}
			break;
		}
		
		case SNT_FLOW_IF:
		{
			CbValue* condition = cb_syntree_eval(((CbFlowNode*) node)->cond, symtab);
			if (condition == NULL)
				break;
			
			assert(cb_value_is_type(condition, CB_VT_BOOLEAN));
			
			// evaluate condition and check its result
			if (cb_boolean_get(condition))
			{
				// condition is ture -> evaluate the true-branch
				result = cb_syntree_eval(((CbFlowNode*) node)->tb, symtab);
			}
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
			if (temp == NULL)
				break;
			
			assert(cb_value_is_type(temp, CB_VT_BOOLEAN));
			
			// default result (in case the while-loop won't be entered)
			result = cb_value_create();
			
			// evaluate true-branch while the condition returns true
			while (cb_boolean_get(temp))
			{
				cb_value_free(temp);
				if (result)	// TODO:	Check is not necessary since result is
							//			assigend before, so it can't be NULL.
					cb_value_free(result);
				
				result = cb_syntree_eval(((CbFlowNode*) node)->tb, symtab);
				temp   = cb_syntree_eval(((CbFlowNode*) node)->cond, symtab);
				
				if (result == NULL || temp == NULL)
					break;
			}
			
			if (temp)
				// free last dummy-value
				cb_value_free(temp);
			break;
		}
		
		case SNT_COMPARISON:
		{
			CbComparisonNode* cmp = ((CbComparisonNode*) node);
			
			CbValue* l = cb_syntree_eval(cmp->l, symtab);
			CbValue* r = cb_syntree_eval(cmp->r, symtab);
			
			if (l && r) // both values of left and right syntax node must be valid
			{
				switch (cb_value_get_type(l))
				{
					case CB_VT_NUMERIC:
						result = cb_numeric_compare(cmp->cmp_type, l, r);
						break;
					
					case CB_VT_STRING:
						result = cb_string_compare(cmp->cmp_type, l, r);
						break;
					
					case CB_VT_BOOLEAN:
						result = cb_boolean_compare(cmp->cmp_type, l, r);
						break;
				}
				
				// free lhs and rhs
				cb_value_free(l);
				cb_value_free(r);
			}
			else if (l)
				cb_value_free(l);
			else
				cb_value_free(r);
			
			break;
		}
		
		case SNT_EXCEPTION_BLOCK:
			result = cb_exception_block_execute((CbExceptionBlockNode*) node,
			                                    symtab);
			break;
		
		case SNT_STATEMENTLIST:
		{
			CbValue* temp = cb_syntree_eval(node->l, symtab);
			
			// Check if returned value is valid and no uncatched errors occurred
			if (temp && (!cb_error_is_set() || cb_error_is_catched()))
			{
				cb_value_free(temp);
				result = cb_syntree_eval(node->r, symtab);
			}
			else
				result = temp;		// At least return result of first statement
			
			break;
		}
		
		case '+':
		case '-':
		case '*':
		case '/':
		case SNT_LOGICAL_AND:
		case SNT_LOGICAL_OR:
		{
			CbValue* l = cb_syntree_eval(node->l, symtab);
			if (l == NULL)
				break;
			
			CbValue* r = cb_syntree_eval(node->r, symtab);
			if (r == NULL)
			{
				cb_value_free(l);
				break;
			}
			
			// value type of rhs and lhs must be equal!
			if (!cb_value_is_type(r, cb_value_get_type(l)))
			{
				cb_print_error(CB_ERR_RUNTIME, node->line_no,
							   "Node type of left-hand side differs from right-hand side");
			}
			else
				switch (node->type)
				{
					case '+':
						switch (cb_value_get_type(l))
						{
							case CB_VT_NUMERIC:
								result = cb_numeric_add(l, r);
								break;
							
							case CB_VT_STRING:
								result = cb_string_concat(l, r);
								break;
							
							default:
								cb_print_error(CB_ERR_RUNTIME, node->line_no,
											   "Binary addition is only allowed for string and numeric values");
						}
						break;
					
					case '-':
						result = cb_numeric_sub(l, r);
						break;
					
					case '*':
						result = cb_numeric_mul(l, r);
						break;
					
					case '/':
						result = cb_numeric_div(l, r);
						break;
					
					case SNT_LOGICAL_AND:
						switch (cb_value_get_type(l))
						{
							case CB_VT_NUMERIC:
								result = cb_numeric_and(l, r);
								break;
							
							case CB_VT_BOOLEAN:
								result = cb_boolean_and(l, r);
								break;
							
							default:
								cb_print_error(CB_ERR_RUNTIME, node->line_no,
											   "Binary AND is only allowed for boolean and numeric values");
						}
						break;
					
					case SNT_LOGICAL_OR:
						switch (cb_value_get_type(l))
						{
							case CB_VT_NUMERIC:
								result = cb_numeric_or(l, r);
								break;
							
							case CB_VT_BOOLEAN:
								result = cb_boolean_or(l, r);
								break;
							
							default:
								cb_print_error(CB_ERR_RUNTIME, node->line_no,
											   "Binary OR is only allowed for boolean and numeric values");
						}
						break;
				}
			
			// free lhs and rhs
			cb_value_free(l);
			cb_value_free(r);
			
			break;
		}
		
		case SNT_LOGICAL_NOT:
		{
			CbValue* operand = cb_syntree_eval(node->l, symtab);
			if (operand == NULL)
				break;
			
			switch (cb_value_get_type(operand))
			{
				case CB_VT_BOOLEAN:
					result = cb_boolean_create(!cb_boolean_get(operand));
					break;
				
				case CB_VT_NUMERIC:
					result = cb_numeric_not(operand);
					break;
				
				default:
					cb_print_error(CB_ERR_RUNTIME, node->line_no,
								   "Wrong value-type");
					break;
			}
			
			cb_value_free(operand);
			
			break;
		}
		
		case SNT_UNARYMINUS:
			result = cb_syntree_eval(node->l, symtab);
			if (result)
			{
				CbNumeric new_value = - cb_numeric_get(result);
				cb_numeric_set(result, new_value);
			}
			break;
		
		default:
			cb_print_error(CB_ERR_RUNTIME, node->line_no,
						   "Syntax-tree node-type not recognized");
	}
	
	return result;
}
