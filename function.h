/*******************************************************************************
 * CbFunction -- Implementation of a function-structure.
 * 
 *  	This struct stores all information about a function, such as parameters,
 *  	function-code and result.
 *  	Methods to call a function_t-struct are provided.
 ******************************************************************************/

#ifndef FUNCTION_H
#define FUNCTION_H


#include "value.h"
#include "strlist.h"
#include "symtab_if.h"
#include "syntree_if.h"
#include "builtin.h"

enum cb_function_type
{
	FUNC_TYPE_BUILTIN,
	FUNC_TYPE_USER_DEFINED
};

// function_t struct
typedef struct
{
	enum cb_function_type type;
	char* id;			// name of the function
	int param_count;	// count of expected parameters
	CbValue* result;	// result of the function after calling it
	
	// type-specific attributes: FUNC_TYPE_BUILTIN
	CbBuiltinFunctionRef func_ref;
	
	// type-specific attributes: FUNC_TYPE_USER_DEFINED
	CbSyntree* body;	// contains actual function-code
	CbStrlist* params;	// list of identifiers that represent formal parameters
	
	// TODO:	Implement a state-attribute that indicates whether a function
	//			was already called.
	
} CbFunction;


// interface-functions
CbFunction* cb_function_create_builtin(char* identifier, int param_count,
									   CbBuiltinFunctionRef func_ref);
CbFunction* cb_function_create_user_defined(char* identifier, CbSyntree* body);
void cb_function_free(CbFunction* f);
void cb_function_add_param(CbFunction* f, char* param_id);
int cb_function_call(CbFunction* f, CbStrlist* args, CbSymtab* symtab);
void cb_function_reset(CbFunction* f);


#endif // FUNCTION_H
