/*******************************************************************************
 * builtin --	Collection of functions to register builtin and pre-defined
 *  			symbols like functions and variables/constants.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "builtin.h"
#include "symbol.h"
#include "symtab.h"
#include "function.h"
#include "strlist.h"


// #############################################################################
// declarations
// #############################################################################

// {identifier, function-pointer, param_count} item
typedef struct
{
	char* identifier;
	func_ref_t func;
	int param_count;
} builtin_func_info_item_t;

// forward-declaration of all builtin symbols
value_t* bif_writeln(stack_t* arg_stack);
value_t* bif_mod(stack_t* arg_stack);
value_t* bif_valtype(stack_t* arg_stack);

// registration-list of all builtin functions (will be registered by function
//                                             'register_builtin_all()')
builtin_func_info_item_t builtin_func_decl_list[] = {
	{"WriteLn", bif_writeln, 1},
	{"Mod", bif_mod, 2},
	{"ValType", bif_valtype, 1}
};


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// register a builtin function in a symbol-table
// -----------------------------------------------------------------------------
int register_builtin_func(	symtab_t* symtab, char* identifier,
							func_ref_t func, int expected_param_count)
{
	symbol_t* s = symbol_create_function(identifier,
					function_create_builtin(identifier, expected_param_count, func));
	
	if (symtab_append(symtab, s) != NULL)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

// -----------------------------------------------------------------------------
// register all builtin symbols in the given symbol-table
// -----------------------------------------------------------------------------
int register_builtin_all(symtab_t* symtab)
{
	int lenght	= sizeof(builtin_func_decl_list) / sizeof(builtin_func_info_item_t);
	int i		= 0;
	int result	= EXIT_SUCCESS;
	
	for (; i < lenght; i++)	// try to register every builtin symbol
	{
		result = register_builtin_func(	symtab,
										builtin_func_decl_list[i].identifier,
										builtin_func_decl_list[i].func,
										builtin_func_decl_list[i].param_count);
		if (result != EXIT_SUCCESS)
		{
			fprintf(stderr, "Error: Unable to register builtin symbol `%s'!\n",
					builtin_func_decl_list[i].identifier);
			exit(EXIT_FAILURE);
		}
	}
	
	return result;
}


// #############################################################################
// builtin-functions
// #############################################################################

// -----------------------------------------------------------------------------
// WriteLn() -- Print value
// -----------------------------------------------------------------------------
value_t* bif_writeln(stack_t* arg_stack)
{
	assert(arg_stack->count == 1);
	
	value_t* arg;
	stack_pop(arg_stack, (void*) &arg);
	
	value_print(arg);
	value_free(arg);
	// print newline
	printf("\n");
	
	return value_create(); // return empty value
}

// -----------------------------------------------------------------------------
// Mod() -- Modulo
// -----------------------------------------------------------------------------
value_t* bif_mod(stack_t* arg_stack)
{
	assert(arg_stack->count == 2);
	
	value_t* arg1;
	value_t* arg2;
	stack_pop(arg_stack, (void*) &arg2);	// first pop -> last argument
	stack_pop(arg_stack, (void*) &arg1);
	
	value_t* result = cbnumeric_create(arg1->value % arg2->value);
	
	value_free(arg1);
	value_free(arg2);
	
	return result; // return empty value
}

// -----------------------------------------------------------------------------
// ValType() -- Type of a value as character
// -----------------------------------------------------------------------------
value_t* bif_valtype(stack_t* arg_stack)
{
	assert(arg_stack->count == 1);
	
	value_t* arg;
	stack_pop(arg_stack, (void*) &arg);
	
	value_t* result;
	switch (arg->type)
	{
		case VT_BOOLEAN:
			result = cbstring_create(strdup("L"));
			break;
		
		case VT_NUMERIC:
			result = cbstring_create(strdup("N"));
			break;
		
		case VT_STRING:
			result = cbstring_create(strdup("C"));
			break;
		
		case VT_UNDEFINED:
		default:
			result = cbstring_create(strdup("U"));
			break;
	}
	
	value_free(arg);
	
	return result; // return empty value
}
