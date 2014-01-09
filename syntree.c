/*******************************************************************************
 * syntree -- implementation of an abstract syntax tree structure
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "syntree.h"
#include "symref.h"
#include "fncall.h"
#include "fndecl.h"
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
		case SNT_SYMREF:
			symbol_free(((symref*) node)->sym);	// free dummy-symbol
			break;
		
		case SNT_FLOW_IF:
		case SNT_FLOW_WHILE:
			syntree_free(((flow*) node)->cond);
			if (((flow*) node)->tb)	// true-branch
				syntree_free(((flow*) node)->tb);
			if (((flow*) node)->fb)	// false-branch
				syntree_free(((flow*) node)->fb);
			break;
		
		case SNT_FUNC_CALL:
			if (((fncall*) node)->args)
				syntree_free(((fncall*) node)->args);
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
			result = cbvalue_copy(((symref*) node)->table_sym->value);
			break;
		
		case SNT_ASSIGNMENT:
		{
			symref* sr = (symref*) node->l;
			symref_setsymbolfromtable(sr);
			cbvalue_assign_freesource(eval(node->r), sr->table_sym->value);
			result = cbvalue_copy(sr->table_sym->value);
			break;
		}
		
		case SNT_DECLARATION:
		{
			symref* sr = (symref*) node->l;
			// dummy-symbol 'sym' must be freed separately -> copy symbol
			symbol* copy = symbol_create(SYM_VARIABLE, sr->sym->identifier);
			variable_declare(gl_symtab, copy);
			// return empty value
			result = cbvalue_create();
			break;
		}
		
		case SNT_FUNC_DECL:
		{
			fndecl* fn = (fndecl*) node;
			
			// declare function-symbol
			function_declare(gl_symtab, fn->sym);
			
			// remove reference from declaration-node, otherwise the declared
			// function-symbol would be freed too early!
			fn->sym = symbol_create(SYM_UNDEFINED, fn->sym->identifier);
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
		{
			// 'symref_setsymbolfromtable' can be used in this case, since both
			// structs have the same signature
			symref_setsymbolfromtable((symref*) node);
			
			syntree* args	= ((fncall*) node)->args;
			symbol* f		= ((fncall*) node)->table_sym;
			
			// validate function-parameters and arguments
			if (f->func->params && args)
			{
				// declare a copy of all parameters in the global symbol-table
				symbol* current_param = ((symref*) f->func->params)->sym->next;
				while (current_param)
				{
					symbol* copy = symbol_create(	SYM_VARIABLE,
													current_param->identifier);
					variable_declare(gl_symtab, copy);
					current_param = current_param->next;
				}
				
				// assign arguments to previously declared parameter-symbols
				syntree* current_arg= args;
				current_param		= ((symref*) f->func->params)->sym->next;
				while (current_param && current_arg)
				{
					// get the actually declared symbol-reference from the
					// global symbol-table
					symbol* table_param = symtab_lookup(gl_symtab,
														current_param->identifier);
					// assign argument-value
					cbvalue_assign_freesource(	eval(current_arg->r),
												table_param->value);
					// next parameter and next argument
					current_param = current_param->next;
					current_arg = current_arg->l;
				}
				
				// expecting all arguments and parameters to be processed.
				// check if there are any parameters or arguments pending
				if (current_param || current_arg)
				{
					yyerror("parameter count does not match the count of "\
							"passed arguments");
					exit(1);
				}
			}
			
			// execute function
			result = eval(f->func->body);
			
			// remove parameters from the global symbol-table after function-
			// execution
			if (f->func->params)
			{
				symbol* current_param = ((symref*) f->func->params)->sym->next;
				// dispatch all parameters from symbol-table
				while (current_param)
				{
					// current_param will be freed indirectly in symtab_remove!
					// that is, the 'next' pointer is stored into temp to keep
					// concatenation of remaining items.
					symbol* temp = current_param->next;
					symtab_remove(gl_symtab, current_param->identifier);
					current_param = temp;
				}
				
				// TODO: free or not
				//~ free(((symref*) f->func->params)->sym);
			}
			
			break;
		}
		
		case SNT_FLOW_IF:
		{
			cbvalue* condition = eval(((flow*) node)->cond);
			if (!cbvalue_istype(VT_BOOLEAN, condition))
			{
				yyerror("expecting boolean expression");
				exit(1);
			}
			
			// evaluate condition and check its result
			if (condition->boolean)
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
			cbvalue* temp = eval(((flow*) node)->cond);
			if (!cbvalue_istype(VT_BOOLEAN, temp))
			{
				yyerror("expecting boolean expression");
				exit(1);
			}
			
			// evaluate true-branch while the condition returns true
			while (temp->boolean)
			{
				cbvalue_free(temp);
				if (result)
					cbvalue_free(result);
				
				result	= eval(((flow*) node)->tb);
				temp	= eval(((flow*) node)->cond);
			}
			
			// free last dummy-value
			cbvalue_free(temp);
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
