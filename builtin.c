/*******************************************************************************
 * builtin -- Collection of functions to register builtin and pre-defined
 *  		  symbols like functions and variables/constants.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "builtin.h"
#include "cblib.h"
#include "symbol.h"
#include "symtab.h"
#include "function.h"


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

// registration list of all builtin functions
// (will be registered by function 'register_builtin_all()')
builtin_func_info_item_t builtin_func_decl_list[] = {
	{"WriteLn", bif_writeln, 1},
	{"Mod", bif_mod, 2},
	{"ValType", bif_valtype, 1},
	{"Str", bif_str, 1},
	{"Eval", bif_eval, 1}
#ifdef _CBC_PLAT_WNDS
	, {"Meld", bif_meld, 1}
#endif // _CBC_PLAT_WNDS
};


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// register a builtin function in a symbol-table
// -----------------------------------------------------------------------------
int register_builtin_func(CbSymtab* symtab, char* identifier,
						  func_ref_t func, int expected_param_count)
{
	symbol_t* s = symbol_create_function(identifier,
					function_create_builtin(identifier, expected_param_count, func));
	
	if (cb_symtab_append(symtab, s) != NULL)
		return EXIT_SUCCESS;
	else
		return EXIT_FAILURE;
}

// -----------------------------------------------------------------------------
// register all builtin symbols in the given symbol-table
// -----------------------------------------------------------------------------
int register_builtin_all(CbSymtab* symtab)
{
	int lenght	= sizeof(builtin_func_decl_list) /
				  sizeof(builtin_func_info_item_t);
	int i		= 0;
	int result	= EXIT_SUCCESS;
	
	for (; i < lenght; i++)	// try to register every builtin symbol
	{
		result = register_builtin_func(symtab,
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
