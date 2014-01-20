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

// function_t struct
typedef struct
{
	syntree_t* body;	// contains actual function-code
	strlist_t* params;	// list of identifiers that represent parameters
	symtab_t* symtab;	// reference to the symbol-table, that will be used to
						// declare the parameters.
	value_t* result;	// result of the function after calling it
	
	// TODO:	Implement a state-attribute that indicates whether a function
	//			was already called.
	
} function_t;


// interface-functions
function_t* function_create();
void function_free(function_t* f);
void function_addparam(function_t* f, char* param_id);
value_t* function_call(function_t* f, strlist_t* args);


#endif // FUNCTION_H
