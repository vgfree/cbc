/*******************************************************************************
 * cblib --	Collection of builtin functions.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "cblib.h"
#include "codeblock.h"


// #############################################################################
// builtin-functions
// #############################################################################

// -----------------------------------------------------------------------------
// WriteLn() -- Print value
// -----------------------------------------------------------------------------
CbValue* bif_writeln(CbStack* arg_stack)
{
	assert(arg_stack->count == 1);
	
	CbValue* arg;
	cb_stack_pop(arg_stack, (void*) &arg);
	
	cb_value_print(arg);
	cb_value_free(arg);
	// print newline
	printf("\n");
	
	return cb_value_create(); // return empty value
}

// -----------------------------------------------------------------------------
// Mod() -- Modulo
// -----------------------------------------------------------------------------
CbValue* bif_mod(CbStack* arg_stack)
{
	assert(arg_stack->count == 2);
	
	CbValue* arg1;
	CbValue* arg2;
	cb_stack_pop(arg_stack, (void*) &arg2);	// first pop -> last argument
	cb_stack_pop(arg_stack, (void*) &arg1);
	
	CbValue* result = cb_numeric_create(arg1->value % arg2->value);
	
	cb_value_free(arg1);
	cb_value_free(arg2);
	
	return result;
}

// -----------------------------------------------------------------------------
// ValType() -- Type of a value as character
// -----------------------------------------------------------------------------
CbValue* bif_valtype(CbStack* arg_stack)
{
	assert(arg_stack->count == 1);
	
	CbValue* arg;
	cb_stack_pop(arg_stack, (void*) &arg);
	
	CbValue* result;
	switch (arg->type)
	{
		case VT_BOOLEAN:
			result = cb_string_create(strdup("L"));
			break;
		
		case VT_NUMERIC:
			result = cb_string_create(strdup("N"));
			break;
		
		case VT_STRING:
			result = cb_string_create(strdup("C"));
			break;
		
		case VT_UNDEFINED:
		default:
			result = cb_string_create(strdup("U"));
			break;
	}
	
	cb_value_free(arg);
	
	return result;
}

// -----------------------------------------------------------------------------
// Str() -- Convert any value to string
// -----------------------------------------------------------------------------
CbValue* bif_str(CbStack* arg_stack)
{
	assert(arg_stack->count == 1);
	
	CbValue* arg;
	cb_stack_pop(arg_stack, (void*) &arg);
	
	CbValue* result = cb_string_create(cb_value_to_string(arg));
	
	cb_value_free(arg);
	
	return result;
}

// -----------------------------------------------------------------------------
// Val() -- Convert a string to a numeric value
// -----------------------------------------------------------------------------
CbValue* bif_val(CbStack* arg_stack)
{
	assert(arg_stack->count == 1);
	
	CbValue* arg;
	cb_stack_pop(arg_stack, (void*) &arg);
	
	assert(cb_value_is_type(arg, VT_STRING));
	
	CbValue* result = cb_numeric_create(0);	// default result
	result->value	= strtol(arg->string, NULL, 10);
	
	cb_value_free(arg);
	
	return result;
}

// -----------------------------------------------------------------------------
// Eval() -- Evaluate a codeblock string
// -----------------------------------------------------------------------------
CbValue* bif_eval(CbStack* arg_stack)
{
	assert(arg_stack->count == 1);
	
	CbValue* arg;
	cb_stack_pop(arg_stack, (void*) &arg);
	
	assert(cb_value_is_type(arg, VT_STRING));
	
	Codeblock* cb   = codeblock_create();
	CbValue* result = NULL;
	
	// Parse codeblock string
	int parser_result = codeblock_parse_string(cb, arg->string);
	cb_value_free(arg);
	
	if (parser_result == EXIT_SUCCESS)
	{
		// Execute codeblock
		if (codeblock_execute(cb) == EXIT_SUCCESS)
			result = cb_value_copy(cb->result);
	}
	
	codeblock_free(cb);
	
	return result;
}

// -----------------------------------------------------------------------------
// GetEnv() -- Get value of an environment variable
// -----------------------------------------------------------------------------
CbValue* bif_getenv(CbStack* arg_stack)
{
	assert(arg_stack->count == 1);
	
	CbValue* arg;
	cb_stack_pop(arg_stack, (void*) &arg);
	
	assert(cb_value_is_type(arg, VT_STRING));
	
	CbValue* result = NULL;
	
	char* value = getenv(arg->string);
	if (value == NULL)
		result = cb_string_create("");
	else
		result = cb_string_create(strdup(value));
	
	cb_value_free(arg);
	
	return result;
}

// -----------------------------------------------------------------------------
// SetEnv() -- Set the value of an environment variable
// -----------------------------------------------------------------------------
CbValue* bif_setenv(CbStack* arg_stack)
{
	assert(arg_stack->count == 2);
	
	CbValue* name;
	CbValue* value;
	cb_stack_pop(arg_stack, (void*) &value);
	cb_stack_pop(arg_stack, (void*) &name);
	
	assert(cb_value_is_type(name, VT_STRING));
	
	CbValue* result = NULL;
	int error		= 0;
	char* value_str = cb_value_to_string(value);
	
#ifdef _CBC_PLAT_WNDS
	int len		 = strlen(name->string) + 1 + strlen(value_str) + 1;
	char* envstr = (char*) malloc(len);
	sprintf(envstr, "%s=%s", name->string, value_str);
	error = putenv(envstr);	// set environment variable
	free(envstr);
#else
	error = setenv(name->string, value_str, 1);
#endif // _CBC_PLAT_WNDS
	if (!error)
		result = cb_value_create();
	
	free(value_str);
	cb_value_free(name);
	cb_value_free(value);
	
	return result;
}
