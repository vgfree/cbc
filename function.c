/*******************************************************************************
 * function_t -- Implementation of a function-structure
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "function.h"
#include "symtab.h"
#include "syntree.h"


// #############################################################################
// declarations
// #############################################################################

void function_reset(function_t* f);


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
function_t* function_create()
{
	function_t* f	= (function_t*) malloc(sizeof(function_t));
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
	
	if (f->params)
		strlist_free(f->params);
	if (f->body)
		syntree_free(f->body);
	
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
	
	// declare and assign parameters, if necessary
	if (count_params > 0)
	{
		strlist_t* curr_param	= f->params;
		strlist_t* curr_arg		= args;
		while (curr_param)
		{
			symbol_t* s = symbol_create();
			symbol_setid(s, curr_param->string);	// set symbol-identifier
			symbol_settype(s, SYM_TYPE_VARIABLE);	// set type
			
			// obtain argument value
			value_t* arg_value = syntree_eval(((syntree_t*) curr_arg->data), f->symtab);
			
			value_assign_freesource(arg_value, s->value);	// assign argument-value
			symtab_append(f->symtab, s);					// declare symbol
			
			// process next items
			curr_param	= curr_param->next;
			curr_arg	= curr_arg->next;
		}
	}
	
	// execute function
	f->result = syntree_eval(f->body, f->symtab);
	
	// remove declared arguments from symbol-table, if necessary
	if (count_params > 0)
	{
		strlist_t* curr_param = f->params;
		while (curr_param)
		{
			symtab_remove(f->symtab, curr_param->string);
			curr_param = curr_param->next;
		}
	}
	
	return f->result;
}


// #############################################################################
// internal functions
// #############################################################################

// -----------------------------------------------------------------------------
// reset function (internal)
// -----------------------------------------------------------------------------
void function_reset(function_t* f)
{
	if (f->result)
	{
		value_free(f->result);
		f->result = NULL;
	}
}
