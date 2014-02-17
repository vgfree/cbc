/*******************************************************************************
 * cblib --	Collection of builtin functions.
 ******************************************************************************/

#ifndef CBLIB_H
#define CBLIB_H


#include "value.h"
#include "stack.h"

// Other Codeblock-Library parts
#include "cbgui.h"


// builtin functions
CbValue* bif_writeln(CbStack* arg_stack);
CbValue* bif_mod(CbStack* arg_stack);
CbValue* bif_valtype(CbStack* arg_stack);
CbValue* bif_str(CbStack* arg_stack);
CbValue* bif_eval(CbStack* arg_stack);


#endif // CBLIB_H
