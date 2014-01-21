/*******************************************************************************
 * codeblock_t -- Codeblock execution-environment
 * 
 *  	This struct is basically wraps the abstract syntax-tree, its result and
 *  	the the global symbol-table.
 ******************************************************************************/

#ifndef CODEBLOCK_H
#define CODEBLOCK_H


#include "symtab.h"
#include "syntree_if.h"
#include "value.h"

// codeblock_t struct
typedef struct
{
	symtab_t* global_symtab;	// reference to the global symbol-table
	syntree_t* ast;				// abstract syntax-tree -- the code to execute
	value_t* result;			// the result, after executing the codeblock
} codeblock_t;


// interface-functions
codeblock_t* codeblock_create();
void codeblock_free(codeblock_t* cb);
value_t* codeblock_execute(codeblock_t* cb);


#endif // CODEBLOCK_H