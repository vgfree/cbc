/*******************************************************************************
 * CbStack -- Generic implementation of a stack structure, using a linked list.
 ******************************************************************************/

#include <stdlib.h>
#include "stack.h"


// #############################################################################
// declarations
// #############################################################################

// item in the stack_t structure
struct CbStackItem
{
    struct CbStackItem* prior; // pointer to the prior item on the stack
    const void* data;          // pointer to any kind of data
};


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
CbStack* cb_stack_create()
{
    CbStack* stack = (CbStack*) malloc(sizeof(CbStack));
    stack->top     = NULL;
    stack->count   = 0;
    
    return stack;
}

// -----------------------------------------------------------------------------
// destructur
// -----------------------------------------------------------------------------
void cb_stack_free(CbStack* stack)
{
    // TODO: Print warning, if stack is not empty
    free(stack);
}

// -----------------------------------------------------------------------------
// push an item on the stack
// -----------------------------------------------------------------------------
void cb_stack_push(CbStack* stack, const void* item)
{
    CbStackItem* stack_item = (CbStackItem*) malloc(sizeof(CbStackItem));
    stack_item->data        = item;
    stack_item->prior       = stack->top;
    
    stack->top = stack_item;
    stack->count++;
}

// -----------------------------------------------------------------------------
// pop an item off the stack
// -----------------------------------------------------------------------------
int cb_stack_pop(CbStack* stack, void** dest)
{
    if (!cb_stack_is_empty(stack))
    {
        if (dest != NULL) // only set destinaion, if it is a valid address
            *dest = (void*) stack->top->data;
        
        CbStackItem* temp = stack->top;
        stack->top        = stack->top->prior; // move top to prior item
        free(temp);                            // free former top-item
        
        stack->count--;
    }
    else
        return EXIT_FAILURE; // stack-underflow
    
    return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
// get the pointer to the data-member of the top-item
// -----------------------------------------------------------------------------
const void* cb_stack_get_top_item(const CbStack* stack)
{
    const void* result = NULL;
    if (stack->top)
        result = stack->top->data;
    
    return result;
}

// -----------------------------------------------------------------------------
// check whether the stack is empty
// -----------------------------------------------------------------------------
bool cb_stack_is_empty(const CbStack* stack)
{
    return stack->count == 0;
}
