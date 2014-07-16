/*******************************************************************************
 * cbgui -- Collection of builtin GUI functions.
 ******************************************************************************/

#ifndef CBGUI_H
#define CBGUI_H


#include "value.h"
#include "stack.h"

// builtin functions
#ifdef _CBC_PLAT_WNDS // Meld()-function currently only available under Windows
CbValue* bif_meld(CbStack* arg_stack);
#endif // _CBC_PLAT_WNDS


#endif // CBGUI_H
