/*******************************************************************************
 * function_t -- Implementation of a function-structure
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
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
// constructor
// -----------------------------------------------------------------------------
function_t* function_create()
{
	function_t* f	= (function_t*) malloc(sizeof(function_t));
	f->id			= NULL;
	f->body			= NULL;
	f->params		= NULL;
	f->symtab		= NULL;
	f->result		= NULL;
	
	return f;
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
}

// -----------------------------------------------------------------------------
// call function
// if the function has no parameters, pass a NULL-value as arguments.
// -----------------------------------------------------------------------------
value_t* function_call(function_t* f, strlist_t* args)
{
	assert(f->body);
	
	// reset function-result
	// this is necessary in case the function was already called.
	function_reset(f);
	
	// determine parameter- and argument-count
	size_t count_params	= (f->params)	? f->params->count	: 0;
	size_t count_args	= (args)		? (args->count)		: 0;
	
	// validate params and arguments
	if (count_params != count_args)
	{
		fprintf(stderr,	"Error: Expecting %d argument(s), but %d was/were "\
						"actually passed!\n", count_params, count_args);
		exit(EXIT_FAILURE);
	}
	
	stack_t* arg_stack = stack_create();
	// declare and assign parameters, if necessary
	if (count_params > 0)
	{
		strlist_t* curr_param	= f->params;
		strlist_t* curr_arg		= args;
		while (curr_param)
		{
			// obtain argument value
			value_t* arg_value = syntree_eval(((syntree_t*) curr_arg->data), f->symtab);
			
			symbol_t* s = symbol_create_variable(curr_param->string);
			value_assign_freesource(arg_value, s->value);	// assign argument-value
			stack_push(arg_stack, s);						// push argument on the stack
			
			// process next items
			curr_param	= curr_param->next;
			curr_arg	= curr_arg->next;
		}
	}
	
	symtab_enter_scope(f->symtab, f->id);	// enter function-scope
	
#ifdef _CBC_DEFAULT_FUNC_RESULT_SYMBOL
	// declare default function-result symbol
	symbol_t* default_result = symbol_create_variable("Result");
	symtab_append(f->symtab, default_result);
#endif // _CBC_DEFAULT_FUNC_RESULT_SYMBOL
	
	// declare all arguments
	if (count_params > 0)
	{
		strlist_t* curr_param = f->params;
		while (curr_param)
		{
			symbol_t* arg;
			stack_pop(arg_stack, (void*) &arg);
			symtab_append(f->symtab, arg);	// declare argument within function-
											// scope
			curr_param = curr_param->next;
		}
	}
	
	stack_free(arg_stack);
	
	// execute function
#ifdef _CBC_DEFAULT_FUNC_RESULT_SYMBOL
	value_free(syntree_eval(f->body, f->symtab));
	f->result = value_copy(default_result->value);	// result is value of the
													// "Result"-symbol
#else
	f->result = syntree_eval(f->body, f->symtab);	// result is the last
													// expression in the function
#endif // _CBC_DEFAULT_FUNC_RESULT_SYMBOL
	
	// leave function-scope:
	// all symbols, that were declared within this scope (like parameters),
	// will be freed!
	symtab_leave_scope(f->symtab);
	
	return f->result;
}

// -----------------------------------------------------------------------------
// reset function
// -----------------------------------------------------------------------------
void function_reset(function_t* f)
{
	if (f->result)
	{
		value_free(f->result);
		f->result = NULL;
	}
}
