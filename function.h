/*******************************************************************************
 * function_t -- Implementation of a function-structure
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

enum function_type_t
{
	FUNC_TYPE_BUILTIN,
	FUNC_TYPE_USER_DEFINED
};

// function_t struct
typedef struct
{
	enum function_type_t type;
	char* id;			// name of the function
	int param_count;	// count of expected parameters
	CbValue* result;	// result of the function after calling it
	
	// type-specific attributes: FUNC_TYPE_BUILTIN
	func_ref_t func_ref;
	
	// type-specific attributes: FUNC_TYPE_USER_DEFINED
	CbSyntree* body;	// contains actual function-code
	strlist_t* params;	// list of identifiers that represent formal parameters
	
	// TODO:	Implement a state-attribute that indicates whether a function
	//			was already called.
	
} function_t;


// interface-functions
function_t* function_create_builtin(char* identifier, int param_count,
									func_ref_t func_ref);
function_t* function_create_user_defined(char* identifier, CbSyntree* body);
void function_free(function_t* f);
void function_addparam(function_t* f, char* param_id);
CbValue* function_call(function_t* f, strlist_t* args, symtab_t* symtab);
void function_reset(function_t* f);


#endif // FUNCTION_H
