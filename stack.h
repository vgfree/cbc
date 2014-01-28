/*******************************************************************************
 * stack_t -- Generic implementation of a stack structure, using a linked list.
 ******************************************************************************/

#ifndef STACK_H
#define STACK_H


#include <stdlib.h>
#include <stdbool.h>

// stack_item_t struct
typedef struct stack_item_t stack_item_t;

// stack_t struct
typedef struct
{
	stack_item_t* top;
	size_t count;
} stack_t;


// interface functions
stack_t* stack_create();
void stack_free(stack_t* stack);
void stack_push(stack_t* stack, const void* item);
int stack_pop(stack_t* stack, void** dest);
bool stack_is_empty(const stack_t* stack);


#endif // STACK_H
