/*******************************************************************************
 * cblib --	Collection of builtin functions.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "cblib.h"

#ifdef _CBC_PLAT_WNDS
#include <windows.h>
#endif // _CBC_PLAT_WNDS


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
	
	return result;
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
	
	return result;
}

// -----------------------------------------------------------------------------
// Str() -- Convert any value to string
// -----------------------------------------------------------------------------
value_t* bif_str(stack_t* arg_stack)
{
	assert(arg_stack->count == 1);
	
	value_t* arg;
	stack_pop(arg_stack, (void*) &arg);
	
	value_t* result = cbstring_create(value_tostring(arg));
	
	value_free(arg);
	
	return result;
}

#ifdef _CBC_PLAT_WNDS
// -----------------------------------------------------------------------------
// Meld() -- Display a Message-Box
// -----------------------------------------------------------------------------
value_t* bif_meld(stack_t* arg_stack)
{
	assert(arg_stack->count == 1);
	
	value_t* arg;
	stack_pop(arg_stack, (void*) &arg);
	
	assert(value_istype(arg, VT_STRING));
	
	// display message
	// NOTE: MessageBox() causes memory-leaks in MinGW32!
	MessageBox(NULL, arg->string, "Information", MB_ICONINFORMATION | MB_OK);
	
	value_free(arg);
	
	return value_create();	// return empty value
}
#endif // _CBC_PLAT_WNDS
