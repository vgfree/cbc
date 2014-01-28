/*******************************************************************************
 * stack_t -- Generic implementation of a stack structure, using a linked list.
 ******************************************************************************/

#include <stdlib.h>
#include "stack.h"


// #############################################################################
// declarations
// #############################################################################

// item in the stack_t structure
struct stack_item_t
{
	struct stack_item_t* prior;	// pointer to the prior item on the stack
	const void* data;			// pointer to any kind of data
};


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
stack_t* stack_create()
{
	stack_t* stack	= (stack_t*) malloc(sizeof(stack_t));
	stack->top		= NULL;
	stack->count	= 0;
	
	return stack;
}

// -----------------------------------------------------------------------------
// destructur
// -----------------------------------------------------------------------------
void stack_free(stack_t* stack)
{
	// TODO: Print warning, if stack is not empty
	free(stack);
}

// -----------------------------------------------------------------------------
// push an item on the stack
// -----------------------------------------------------------------------------
void stack_push(stack_t* stack, const void* item)
{
	stack_item_t* stack_item	= (stack_item_t*) malloc(sizeof(stack_item_t));
	stack_item->data			= item;
	stack_item->prior			= stack->top;
	
	stack->top = stack_item;
	stack->count++;
}

// -----------------------------------------------------------------------------
// pop an item off the stack
// -----------------------------------------------------------------------------
int stack_pop(stack_t* stack, void** dest)
{
	if (!stack_is_empty(stack))
	{
		if (dest != NULL)	// only set destinaion, if it is a valid address
			*dest			= (void*) stack->top->data;
		
		stack_item_t* temp	= stack->top;
		stack->top			= stack->top->prior;	// move top to prior item
		free(temp);									// free former top-item
		
		stack->count--;
	}
	else
		return EXIT_FAILURE;						// stack-underflow
	
	return EXIT_SUCCESS;
}

// -----------------------------------------------------------------------------
// get the pointer to the data-member of the top-item
// -----------------------------------------------------------------------------
const void* stack_get_top_item(const stack_t* stack)
{
	const void* result = NULL;
	if (stack->top)
		result = stack->top->data;
	
	return result;
}

// -----------------------------------------------------------------------------
// check whether the stack is empty
// -----------------------------------------------------------------------------
bool stack_is_empty(const stack_t* stack)
{
	return stack->count == 0;
}
