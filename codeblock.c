/*******************************************************************************
 * Codeblock -- Implementation of the codeblock execution-environment.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "codeblock.h"
#include "syntree.h"
#include "builtin.h"


// #############################################################################
// declarations
// #############################################################################

void codeblock_reset(Codeblock* cb);


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
Codeblock* codeblock_create()
{
	Codeblock* cb		= (Codeblock*) malloc(sizeof(Codeblock));
	cb->global_symtab	= cb_symtab_create();
	cb->ast				= NULL;
	cb->result			= NULL;
	
	return cb;
}

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
void codeblock_free(Codeblock* cb)
{
	codeblock_reset(cb);
	cb_symtab_free(cb->global_symtab);
	
	free(cb);
}

// -----------------------------------------------------------------------------
// execute codeblock
// -----------------------------------------------------------------------------
CbValue* codeblock_execute(Codeblock* cb)
{
	assert(cb->global_symtab);
	
	if (!cb->ast)
	{
		fprintf(stderr, "Error: Empty codeblock is not allowed!\n");
		exit(EXIT_FAILURE);
	}
	
	// reset codeblock-result
	// this is necessary in case the codeblock was already executed and prduced
	// a result.
	codeblock_reset(cb);
	
	// register builtin symbols
	register_builtin_all(cb->global_symtab);
	
	// execute codeblock
	cb->result = cb_syntree_eval(cb->ast, cb->global_symtab);
	
	return cb->result;
}


// #############################################################################
// internal functions
// #############################################################################

// -----------------------------------------------------------------------------
// reset codeblock (internal)
// -----------------------------------------------------------------------------
void codeblock_reset(Codeblock* cb)
{
	if (cb->result)
	{
		cb_value_free(cb->result);
		cb->result = NULL;
	}
}
