/*******************************************************************************
 * codeblock -- Codeblock execution-environment
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

void codeblock_reset(codeblock_t* cb);


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
codeblock_t* codeblock_create()
{
	codeblock_t* cb		= (codeblock_t*) malloc(sizeof(codeblock_t));
	cb->global_symtab	= symtab_create();
	cb->ast				= NULL;
	cb->result			= NULL;
	
	return cb;
}

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
void codeblock_free(codeblock_t* cb)
{
	codeblock_reset(cb);
	symtab_free(cb->global_symtab);
	
	free(cb);
}

// -----------------------------------------------------------------------------
// execute codeblock
// -----------------------------------------------------------------------------
CbValue* codeblock_execute(codeblock_t* cb)
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
void codeblock_reset(codeblock_t* cb)
{
	if (cb->result)
	{
		cb_value_free(cb->result);
		cb->result = NULL;
	}
}
