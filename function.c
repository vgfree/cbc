/*******************************************************************************
 * function_t -- Implementation of a function-structure
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "function.h"
#include "symbol.h"
#include "symtab.h"
#include "syntree.h"
#include "stack.h"


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor (internal)
// -----------------------------------------------------------------------------
function_t* function_create(char* identifier)
{
	function_t* f	= (function_t*) malloc(sizeof(function_t));
	f->id			= strdup(identifier);
	f->param_count	= 0;
	f->result		= NULL;
	
	f->func_ref		= NULL;
	f->params		= NULL;
	f->body			= NULL;
	
	return f;
}

// -----------------------------------------------------------------------------
// constructor (builtin function)
// -----------------------------------------------------------------------------
function_t* function_create_builtin(char* identifier, int param_count,
									func_ref_t func_ref)
{
	function_t* f	= function_create(identifier);
	f->type			= FUNC_TYPE_BUILTIN;
	f->func_ref		= func_ref;
	f->param_count	= param_count;
}

// -----------------------------------------------------------------------------
// constructor (user-defined function)
// -----------------------------------------------------------------------------
function_t* function_create_user_defined(char* identifier, CbSyntree* body)
{
	function_t* f	= function_create(identifier);
	f->type			= FUNC_TYPE_USER_DEFINED;
	f->body			= body;
}

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
void function_free(function_t* f)
{
	function_reset(f);
	
	if (f->id)	// free identifier, if necessary
		free(f->id);
	
	free(f);
}

// -----------------------------------------------------------------------------
// add param to function-definition
// -----------------------------------------------------------------------------
void function_addparam(function_t* f, char* param_id)
{
	assert(param_id);	// anonymus parameters are not allowed!
	
	if (!f->params)
		// create string-list if it does not exist yet
		f->params = strlist_create(param_id);
	else
		strlist_append(f->params, param_id);
	
	f->param_count = f->params->count;
}

// -----------------------------------------------------------------------------
// call function
// if the function has no parameters, pass a NULL-value as arguments.
// -----------------------------------------------------------------------------
CbValue* function_call(function_t* f, strlist_t* args, CbSymtab* symtab)
{
	// reset function-result
	// this is necessary in case the function was already called.
	function_reset(f);
	
	// determine parameter- and argument-count
	size_t count_params	= f->param_count;
	size_t count_args	= (args) ? (args->count) : 0;
	
	// validate params and arguments
	if (count_params != count_args)
	{
		fprintf(stderr,	"Error: Expecting %d argument(s), but %d was/were "\
						"actually passed!\n", count_params, count_args);
		exit(EXIT_FAILURE);
	}
	
	stack_t* arg_stack   = stack_create();
	stack_t* param_stack = stack_create();
	// evaluate argument values
	if (count_params > 0)
	{
		strlist_t* curr_arg   = args;
		strlist_t* curr_param = f->params;
		while (curr_arg)
		{
			if (curr_param)
				stack_push(param_stack, curr_param->string); // push param name
			
			// obtain argument value
			CbValue* arg_value = cb_syntree_eval(	((CbSyntree*) curr_arg->data),
												symtab);
			// push argument value on the stack
			stack_push(arg_stack, arg_value);
			// process next item
			curr_arg = curr_arg->next;
			if (curr_param)
				curr_param = curr_param->next;
		}
	}
	
	cb_symtab_enter_scope(symtab, f->id);	// enter function-scope
	
	if (f->type == FUNC_TYPE_USER_DEFINED)
	{
	
#ifdef _CBC_DEFAULT_FUNC_RESULT_SYMBOL
		// declare default function-result symbol
		CbSymbol* default_result = cb_symbol_create_variable("Result");
		cb_symtab_append(symtab, default_result);
#endif // _CBC_DEFAULT_FUNC_RESULT_SYMBOL
		
		// declare all arguments
		if (count_params > 0)	// TODO: If-statement is not necessary here.
		{
			while (!stack_is_empty(param_stack))
			{
				CbValue* arg_value;
				stack_pop(arg_stack, (void*) &arg_value);
				char* param_id;
				stack_pop(param_stack, (void*) &param_id);
				CbSymbol* arg = cb_symbol_create_variable(param_id);
				cb_symbol_variable_assign_value(arg, arg_value);
				cb_value_free(arg_value);
				cb_symtab_append(symtab, arg);	// declare argument within function-
											// scope
			}
		}
	
		stack_free(arg_stack);
		stack_free(param_stack);
		
#ifdef _CBC_DEFAULT_FUNC_RESULT_SYMBOL
		cb_value_free(cb_syntree_eval(f->body, symtab));
		// result is value of the "Result"-symbol
		f->result = cb_value_copy(cb_symbol_variable_get_value(default_result));
#else
		f->result = cb_syntree_eval(f->body, symtab);	// result is the last
													// expression in the function
#endif // _CBC_DEFAULT_FUNC_RESULT_SYMBOL
	}
	else
	{
		f->result = f->func_ref(arg_stack);
		stack_free(arg_stack);
		stack_free(param_stack);
	}
	
	// leave function-scope:
	// all symbols, that were declared within this scope (like parameters),
	// will be freed!
	cb_symtab_leave_scope(symtab);
	
	return f->result;
}

// -----------------------------------------------------------------------------
// reset function
// -----------------------------------------------------------------------------
void function_reset(function_t* f)
{
	if (f->result)
	{
		cb_value_free(f->result);
		f->result = NULL;
	}
}
