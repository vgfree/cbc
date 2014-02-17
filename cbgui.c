/*******************************************************************************
 * cbgui --	Collection of builtin GUI functions.
 ******************************************************************************/

#ifdef _CBC_PLAT_WNDS
#include <windows.h>
#endif // _CBC_PLAT_WNDS

#include <assert.h>
#include "cbgui.h"


// #############################################################################
// builtin-functions
// #############################################################################

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
