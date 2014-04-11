/*******************************************************************************
 * CbExceptionBlockNode -- Exception-handling construct
 ******************************************************************************/

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "exception_block_node.h"
#include "syntree.h"
#include "error_handling.h"


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
CbSyntree* cb_exception_block_create(enum cb_exception_block_type type,
                                     CbSyntree* code_block,
                                     CbSyntree* exception_block)
{
	assert(code_block);
	assert(exception_block);
	
	CbExceptionBlockNode* node = malloc(sizeof(CbExceptionBlockNode));
	node->type                 = SNT_EXCEPTION_BLOCK;
	node->line_no              = 0;
	node->block_type           = type;
	node->code_block           = code_block;
	node->exception_block      = exception_block;
	
	return (CbSyntree*) node;
}

// -----------------------------------------------------------------------------
// execute node
// -----------------------------------------------------------------------------
CbValue* cb_exception_block_execute(CbExceptionBlockNode* node, CbSymtab* symtab)
{
	assert(node);
	
	// execute code block
	CbValue* result       = NULL;
	CbValue* block_result = cb_syntree_eval(node->code_block, symtab);
	bool error_flag       = cb_error_get() == 1;
	
	switch (node->block_type)
	{
		case EXBL_ONERROR:
			if (error_flag)
			{
				cb_error_clear(); // to be able to execute the exception block
				result = cb_syntree_eval(node->exception_block, symtab);
			}
			else
				result = block_result;
			
			break;
			
		case EXBL_ALWAYS:
			cb_value_free(block_result);
			// always execute exception block
			result = cb_syntree_eval(node->exception_block, symtab);
			
			if (error_flag)		// if an error occurred in the code block
				cb_error_set();	// -> set error flag
			
			break;
	}
	
	return result;
}
