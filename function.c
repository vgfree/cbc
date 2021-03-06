/*******************************************************************************
 * CbFunction -- Implementation of a function-structure.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "function.h"
#include "symbol.h"
#include "symtab.h"
#include "syntree.h"
#include "stack.h"
#include "error_handling.h"


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor (internal)
// -----------------------------------------------------------------------------
CbFunction* function_create(char* identifier)
{
    CbFunction* f  = (CbFunction*) malloc(sizeof(CbFunction));
    f->id          = strdup(identifier);
    f->param_count = 0;
    f->result      = NULL;
    f->func_ref    = NULL;
    f->params      = NULL;
    f->body        = NULL;
    
    return f;
}

// -----------------------------------------------------------------------------
// constructor (builtin function)
// -----------------------------------------------------------------------------
CbFunction* cb_function_create_builtin(char* identifier, int param_count,
                                       CbBuiltinFunctionRef func_ref)
{
    CbFunction* f  = function_create(identifier);
    f->type        = FUNC_TYPE_BUILTIN;
    f->func_ref    = func_ref;
    f->param_count = param_count;
}

// -----------------------------------------------------------------------------
// constructor (user-defined function)
// -----------------------------------------------------------------------------
CbFunction* cb_function_create_user_defined(char* identifier, CbSyntree* body)
{
    CbFunction* f = function_create(identifier);
    f->type       = FUNC_TYPE_USER_DEFINED;
    f->body       = body;
}

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
void cb_function_free(CbFunction* f)
{
    cb_function_reset(f);
    
    if (f->id) // free identifier, if necessary
        free(f->id);
    
    free(f);
}

// -----------------------------------------------------------------------------
// add param to function-definition
// -----------------------------------------------------------------------------
void cb_function_add_param(CbFunction* f, char* param_id)
{
    assert(param_id); // anonymus parameters are not allowed!
    
    if (!f->params)
        // create string-list if it does not exist yet
        f->params = cb_strlist_create(param_id);
    else
        cb_strlist_append(f->params, param_id);
    
    f->param_count = f->params->count;
}

// -----------------------------------------------------------------------------
// call function
// if the function has no parameters, pass a NULL-value as arguments.
// -----------------------------------------------------------------------------
int cb_function_call(CbFunction* f, CbStrlist* args, CbSymtab* symtab)
{
    int result = EXIT_SUCCESS;
    // reset function-result
    // this is necessary in case the function was already called.
    cb_function_reset(f);
    
    // determine parameter- and argument-count
    size_t count_params = f->param_count;
    size_t count_args   = (args) ? (args->count) : 0;
    
    // validate params and arguments
    if (count_params != count_args)
    {
        char* exp_str = (count_params == 1) ? "argument" : "arguments";
        char* act_str = (count_args == 1)   ? "was"      : "were";
        
        cb_print_error(CB_ERR_RUNTIME, -1, "In function `%s': Expecting %d %s,"\
                                           " but %d %s actually passed",
                       f->id, count_params, exp_str, count_args, act_str);
        return EXIT_FAILURE;
    }
    
    CbStack* arg_stack   = cb_stack_create();
    CbStack* param_stack = cb_stack_create();
    // evaluate argument values
    if (count_params > 0)
    {
        CbStrlist* curr_arg   = args;
        CbStrlist* curr_param = f->params;
        while (curr_arg)
        {
            if (curr_param)
                cb_stack_push(param_stack, curr_param->string); // push param name
            
            // obtain argument value
            CbValue* arg_value = cb_syntree_eval(((CbSyntree*) curr_arg->data),
                                                 symtab);
            if (arg_value == NULL)
            {
                result = EXIT_FAILURE;
                break;
            }
            
            // push argument value on the stack
            cb_stack_push(arg_stack, arg_value);
            // process next item
            curr_arg = curr_arg->next;
            if (curr_param)
                curr_param = curr_param->next;
        }
    }
    
    if (result == EXIT_SUCCESS)
    {
        cb_symtab_enter_scope(symtab, f->id); // enter function-scope
        
        if (f->type == FUNC_TYPE_USER_DEFINED)
        {
        
#ifdef _CBC_DEFAULT_FUNC_RESULT_SYMBOL
            // declare default function-result symbol
            CbSymbol* default_result = cb_symbol_create_variable("Result");
            cb_symtab_append(symtab, default_result);
#endif // _CBC_DEFAULT_FUNC_RESULT_SYMBOL
            
            // declare all arguments
            while (!cb_stack_is_empty(param_stack))
            {
                CbValue* arg_value;
                cb_stack_pop(arg_stack, (void*) &arg_value);
                char* param_id;
                cb_stack_pop(param_stack, (void*) &param_id);
                CbSymbol* arg = cb_symbol_create_variable(param_id);
                cb_symbol_variable_assign_value(arg, arg_value);
                cb_value_free(arg_value);
                
                // declare argument within function-scope
                if (!cb_symtab_append(symtab, arg))
                {
                    cb_symbol_free(arg);
                    result = EXIT_FAILURE;
                    break;
                }
            }
            
            if (result == EXIT_SUCCESS)
            {
#ifdef _CBC_DEFAULT_FUNC_RESULT_SYMBOL
                CbValue* eval_result = cb_syntree_eval(f->body, symtab);
                if (eval_result == NULL)
                {
                    f->result = NULL;
                    result    = EXIT_FAILURE;
                }
                else
                {
                    cb_value_free(eval_result);
                    // result is value of the "Result"-symbol
                    f->result = cb_value_copy(cb_symbol_variable_get_value(default_result));
                }
#else
                // result is the last expression in the function
                f->result = cb_syntree_eval(f->body, symtab);
                if (f->result == NULL)
                    result = EXIT_FAILURE;
#endif // _CBC_DEFAULT_FUNC_RESULT_SYMBOL
            }
        }
        else
        {
            f->result = f->func_ref(arg_stack);
            if (f->result == NULL)
                result = EXIT_FAILURE;
        }
        
        // leave function-scope:
        // all symbols, that were declared within this scope (like parameters),
        // will be freed!
        cb_symtab_leave_scope(symtab);
    }
    
    cb_stack_free(arg_stack);
    cb_stack_free(param_stack);
    
    return result;
}

// -----------------------------------------------------------------------------
// reset function
// -----------------------------------------------------------------------------
void cb_function_reset(CbFunction* f)
{
    if (f->result)
    {
        cb_value_free(f->result);
        f->result = NULL;
    }
}
