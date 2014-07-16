/*******************************************************************************
 * CbStack -- Generic implementation of a stack structure, using a linked list.
 ******************************************************************************/

#ifndef STACK_H
#define STACK_H


#include <stdlib.h>
#include <stdbool.h>

// stack_item_t struct
typedef struct CbStackItem CbStackItem;

// stack_t struct
typedef struct
{
    CbStackItem* top;
    size_t count;
} CbStack;


// interface functions
CbStack* cb_stack_create();
void cb_stack_free(CbStack* stack);
void cb_stack_push(CbStack* stack, const void* item);
int cb_stack_pop(CbStack* stack, void** dest);
const void* cb_stack_get_top_item(const CbStack* stack);
bool cb_stack_is_empty(const CbStack* stack);


#endif // STACK_H
