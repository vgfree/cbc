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
#include "error_handling.h"


// #############################################################################
// declarations
// #############################################################################

// {identifier, function-pointer, param_count} item
typedef struct
{
	char* identifier;
	CbBuiltinFunctionRef func;
	int param_count;
} CbBuiltinFunctionInfoItem;

// registration list of all builtin functions
// (will be registered by function 'register_builtin_all()')
CbBuiltinFunctionInfoItem builtin_func_decl_list[] = {
	{"WriteLn", bif_writeln, 1},
	{"Mod", bif_mod, 2},
	{"ValType", bif_valtype, 1},
	{"Str", bif_str, 1},
	{"Val", bif_val, 1},
	{"Eval", bif_eval, 1},
	{"GetEnv", bif_getenv, 1},
	{"SetEnv", bif_setenv, 2}
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
						  CbBuiltinFunctionRef func, int expected_param_count)
{
	CbSymbol* s = cb_symbol_create_function(identifier,
					cb_function_create_builtin(identifier, expected_param_count, func));
	
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
				  sizeof(CbBuiltinFunctionInfoItem);
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
			cb_print_error_msg("Unable to register builtin symbol `%s'",
							   builtin_func_decl_list[i].identifier);
			break;
		}
	}
	
	return result;
}
