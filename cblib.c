/*******************************************************************************
 * cblib --	Collection of builtin functions.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "cblib.h"
#include "cbc_lex.h"
#include "cbc_parse.h"
#include "codeblock.h"

#ifdef _CBC_PLAT_WNDS
#include <windows.h>
#endif // _CBC_PLAT_WNDS


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
// Eval() -- Evaluate a codeblock string
// -----------------------------------------------------------------------------
CbValue* bif_eval(CbStack* arg_stack)
{
	assert(arg_stack->count == 1);
	
	CbValue* arg;
	cb_stack_pop(arg_stack, (void*) &arg);
	
	assert(cb_value_is_type(arg, VT_STRING));
	
	// Create codeblock environment
	codeblock_t* cb = codeblock_create();
	
	// Process codeblock
	YY_BUFFER_STATE buffer_state = yy_scan_string(arg->string);
	yyparse(&cb->ast);
	yy_delete_buffer(buffer_state);
	yylex_destroy();
	cb_value_free(arg);
	
	codeblock_execute(cb);	// Execute codeblock
	
	CbValue* result = cb_value_copy(cb->result);
	
	cb_syntree_free(cb->ast);
	codeblock_free(cb);
	
	return result;
}

#ifdef _CBC_PLAT_WNDS
// -----------------------------------------------------------------------------
// Meld() -- Display a Message-Box
// -----------------------------------------------------------------------------
CbValue* bif_meld(CbStack* arg_stack)
{
	assert(arg_stack->count == 1);
	
	CbValue* arg;
	cb_stack_pop(arg_stack, (void*) &arg);
	
	assert(cb_value_is_type(arg, VT_STRING));
	
	// display message
	// NOTE: MessageBox() causes memory-leaks in MinGW32!
	MessageBox(NULL, arg->string, "Information", MB_ICONINFORMATION | MB_OK);
	
	cb_value_free(arg);
	
	return cb_value_create();	// return empty value
}
#endif // _CBC_PLAT_WNDS
