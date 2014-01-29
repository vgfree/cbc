/*******************************************************************************
 * symtab_t -- Implementation of a symbol-table structure
 ******************************************************************************/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "scope.h"

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
symtab_t* symtab_create()
{
	symtab_t* st	= (symtab_t*) malloc(sizeof(symtab_t));
	st->first		= NULL;
	st->last		= NULL;
	st->current		= NULL;
	st->size		= 0;
	st->scope_stack	= stack_create();
	
	return st;
}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
void symtab_free(symtab_t* st)
{
	assert(st);
	
	symbol_t* current = st->first;
	while (current)
	{
		symbol_t* temp	= current;
		current			= current->next;
		symbol_free(temp);
	}
	
	stack_free(st->scope_stack);
	free(st);
}

// -----------------------------------------------------------------------------
// Append a symbol to the symbol-table
// -----------------------------------------------------------------------------
symbol_t* symtab_append(symtab_t* st, symbol_t* s)
{
	assert(st);
	assert(s);
	assert(s->next		== NULL); // allow appending single symbols only
	assert(s->previous	== NULL);
	
	// do not append undefined symbol-objects
	if (s->state == SYM_STATE_INITIAL)
		return NULL;
	
	if (symtab_isempty(st))
	{
		st->first	= s;
		st->current	= s;
	}
	else
		symbol_connect(st->last, s);
	
	s->scope = stack_get_top_item(st->scope_stack);
	st->last = s;
	st->size++;
	
	return st->last;
}

// -----------------------------------------------------------------------------
// Dispatch the symbol with the given id from the symbol-table
// 
//  	If function returns NULL, then there was no symbol found with the given
//  	id.
// -----------------------------------------------------------------------------
symbol_t* symtab_dispatch(symtab_t* st, char* id)
{
	symbol_t* found = symtab_lookup(st, id);
	
	if (found)
	{
		symbol_t* prev = found->previous;
		symbol_t* next = found->next;
		
		found->previous	= NULL;
		found->next		= NULL;
		
		// remove references to dispatched item
		if (prev) prev->next	= NULL;
		if (next) next->previous= NULL;
		// connect free items
		if (prev && next)
			symbol_connect(prev, next);
		
		// update first, last and current item-references
		if (st->current == found)
			st->current = next;
		if (st->last == found)
		{
			if (next == NULL && (st->size - 1) > 0)
				st->last = prev;
			else
				st->last = next;
		}
		if (st->first == found)
		{
			if (prev == NULL && (st->size - 1) > 0)
				st->first = next;
			else
				st->first = prev;
		}
		
		st->size--;
	}
	
	return found;
}

// -----------------------------------------------------------------------------
// Dispatch and destroy the symbol with the given id from the symbol-table
// -----------------------------------------------------------------------------
void symtab_remove(symtab_t* st, char* id)
{
	symbol_t* s = symtab_dispatch(st, id);
	
	if (s)
		symbol_free(s);
}

// -----------------------------------------------------------------------------
// Dispatch all symbols in the symbol-table (TODO)
// -----------------------------------------------------------------------------
void symtab_dispatch_all(symtab_t* st)
{
	//~ // TODO: Move at the beginning of table
	
	//~ while (st->size > 0)
	//~ {
		//~ symtab_next(st);
		//~ symbol_free(st->current);
	//~ }
}

// -----------------------------------------------------------------------------
// Remove all symbols in the symbol-table (TODO)
// -----------------------------------------------------------------------------
void symtab_clear(symtab_t* st)
{
	// TODO
}

// -----------------------------------------------------------------------------
// Lookup a symbol by id
// -----------------------------------------------------------------------------
symbol_t* symtab_lookup(symtab_t* st, char* id)
{
	symbol_t* current	= st->first;
	symbol_t* result	= NULL;
	
	while (current)
	{
		if (strcmp(id, current->id) == 0)
		{
			if (scope_equals(	current->scope,
								stack_get_top_item(st->scope_stack)))
			{
				result = current;
				break;	// Correct symbol was found -> break
			}
			// NULL-scope means, that the current symbol is in global scope.
			// -> Can potentially be used, if there is no local declaration.
			else if (current->scope == NULL)
				// Do not break in this case,
				// since there could still be a local declaration of the symbol!
				result = current;
		}
		current = current->next;
	}
	
	return result;
}

// -----------------------------------------------------------------------------
// Get next item in the symbol-table and move the 'current'-pointer to the next
// symbol
// -----------------------------------------------------------------------------
symbol_t* symtab_next(symtab_t* st)
{
	symbol_t* result = symtab_current(st)->next;
	
	if (result)
		st->current	= result;
	
	return result;
}

// -----------------------------------------------------------------------------
// Get current item in the symbol-table
// -----------------------------------------------------------------------------
symbol_t* symtab_current(symtab_t* st)
{
	return st->current;
}

// -----------------------------------------------------------------------------
// Get previous item in the symbol-table and move the 'current'-pointer to the
// previous symbol
// -----------------------------------------------------------------------------
symbol_t* symtab_previous(symtab_t* st)
{
	symbol_t* result = symtab_current(st)->previous;
	
	if (result)
		st->current	= result;
	
	return result;
}

// -----------------------------------------------------------------------------
// Check if symbol-table is empty
// -----------------------------------------------------------------------------
bool symtab_isempty(symtab_t* st)
{
	return st->size == 0;
}

// -----------------------------------------------------------------------------
// Enter new scope
// -----------------------------------------------------------------------------
void symtab_enter_scope(symtab_t* st, char* context)
{
	scope_t* new_scope = scope_create(context, st->scope_stack->count + 1);
	stack_push(st->scope_stack, new_scope);
}

// -----------------------------------------------------------------------------
// Leave current scope
// -----------------------------------------------------------------------------
void symtab_leave_scope(symtab_t* st)
{
	// Remove all local symbols in the current scope.
	// Begin with last symbol in the symbol-table, because recently appended
	// symbols are always at the end of the table!
	symbol_t* current = st->last;
	while (current)
	{
		symbol_t* temp	= current;
		current			= current->previous;
		
		if (scope_equals(	temp->scope,
							(scope_t*) stack_get_top_item(st->scope_stack)))
			// TODO: Use a different method to dispatch and remove the symbol.
			// Since symtab_dispatch() calls symtab_lookup() this could be a
			// quite slow operation!
			symtab_remove(st, temp->id);
	}
	
	scope_t* current_scope;
	stack_pop(st->scope_stack, (void*) &current_scope);	// pop and
	scope_free(current_scope);							// free current scope
}
