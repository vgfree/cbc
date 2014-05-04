/*******************************************************************************
 * Codeblock -- Implementation of the codeblock execution-environment.
 * 
 *  	This struct is basically wraps the abstract syntax-tree, its result and
 *  	the the global symbol-table.
 ******************************************************************************/

#ifndef CODEBLOCK_H
#define CODEBLOCK_H


#include <stdio.h>
#include "symtab.h"
#include "syntree_if.h"
#include "value.h"

typedef struct
{
	CbSymtab* symtab;	// reference to the global symbol-table
	CbSyntree* ast;		// abstract syntax-tree -- the code to execute
	CbValue* result;	// the result, after executing the codeblock
	double duration;	// execution duration
	bool embedded;		// determine if codeblock is embedded
} Codeblock;


// interface-functions
Codeblock* codeblock_create();
void codeblock_free(Codeblock* cb);
int codeblock_parse_file(Codeblock* cb, FILE* input);
int codeblock_parse_string(Codeblock* cb, const char* string);
int codeblock_execute(Codeblock* cb);


#endif // CODEBLOCK_H
