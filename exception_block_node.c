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
    // check for an uncatched error
    bool error_flag       = cb_error_is_set() && !cb_error_is_catched();
    
    switch (node->block_type)
    {
        case EXBL_ONERROR:
            if (error_flag)
            {
                if (block_result != NULL)
                    cb_value_free(block_result);
                
                // catch error in order to be able to execute the
                // exception-block and to mark that error as processed
                cb_error_catch();
                result = cb_syntree_eval(node->exception_block, symtab);
                
                // if there were no further errors in the exception-block
                // -> clear error flag, since the last error was processed
                if (cb_error_is_catched())
                {
                    cb_error_clear();
                    cb_error_reset_catch();
                }
            }
            else
                result = block_result;
            
            break;
            
        case EXBL_ALWAYS:
            cb_value_free(block_result);
            
            // temporarily catch error in order to execute the exception block
            cb_error_catch();
            // always execute exception block
            result = cb_syntree_eval(node->exception_block, symtab);
            
            bool handle_prev_error = error_flag &&
                                     (cb_error_is_set() && cb_error_is_catched());
            
            // if an error occurred in the previous code block and the
            // error-flag remained "catched" (i.e. there is no new error)
            // -> handle previous error
            if (handle_prev_error)
            {
                cb_error_reset_catch(); // -> mark error as "uncatched"
                cb_value_free(result);  // TODO: ensure result is never NULL
                result = NULL;          // return invalid result due to error
            }
            
            break;
    }
    
    return result;
}
