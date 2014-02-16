/*******************************************************************************
 * CbSymtab -- Implementation of a symbol-table structure.
 ******************************************************************************/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "scope.h"

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CbSymtab* cb_symtab_create()
{
	CbSymtab* st	= (CbSymtab*) malloc(sizeof(CbSymtab));
	st->first		= NULL;
	st->last		= NULL;
	st->current		= NULL;
	st->size		= 0;
	st->scope_stack = stack_create();
	
	return st;
}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
void cb_symtab_free(CbSymtab* st)
{
	assert(st);
	
	CbSymbol* current = st->first;
	while (current)
	{
		CbSymbol* temp	= current;
		current			= cb_symbol_get_next(current);
		cb_symbol_free(temp);
	}
	
	stack_free(st->scope_stack);
	free(st);
}

// -----------------------------------------------------------------------------
// Append a symbol to the symbol-table
// -----------------------------------------------------------------------------
CbSymbol* cb_symtab_append(CbSymtab* st, CbSymbol* s)
{
	assert(st);
	assert(s);
	assert(cb_symbol_get_next(s)	  == NULL); // allow appending single symbols only
	assert(cb_symbol_get_previous(s) == NULL);
	
	if (cb_symtab_is_empty(st))
	{
		st->first	= s;
		st->current	= s;
	}
	else
		cb_symbol_connect(st->last, s);
	
	cb_symbol_set_scope(s, stack_get_top_item(st->scope_stack));
	st->last = s;
	st->size++;
	
	return st->last;
}

// -----------------------------------------------------------------------------
// Dispatch the symbol with the given id from the symbol-table.
// If function returns NULL, then there was no symbol found with the given id.
// -----------------------------------------------------------------------------
CbSymbol* cb_symtab_dispatch(CbSymtab* st, const char* id)
{
	CbSymbol* found = cb_symtab_lookup(st, id);
	
	if (found)
	{
		CbSymbol* prev = cb_symbol_get_previous(found);
		CbSymbol* next = cb_symbol_get_next(found);
		
		cb_symbol_set_previous(found, NULL);
		cb_symbol_set_next(found, NULL);
		
		// remove references to dispatched item
		if (prev) cb_symbol_set_next(prev, NULL);
		if (next) cb_symbol_set_previous(next, NULL);
		// connect free items
		if (prev && next)
			cb_symbol_connect(prev, next);
		
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
void cb_symtab_remove(CbSymtab* st, const char* id)
{
	CbSymbol* s = cb_symtab_dispatch(st, id);
	
	if (s)
		cb_symbol_free(s);
}

// -----------------------------------------------------------------------------
// Lookup a symbol by id
// -----------------------------------------------------------------------------
CbSymbol* cb_symtab_lookup(CbSymtab* st, const char* id)
{
	CbSymbol* current	= st->first;
	CbSymbol* result	= NULL;
	
	while (current)
	{
		if (strcmp(id, cb_symbol_get_id(current)) == 0)
		{
			if (scope_equals(cb_symbol_get_scope(current),
							 stack_get_top_item(st->scope_stack)))
			{
				result = current;
				break;	// Correct symbol was found -> break
			}
			// NULL-scope means, that the current symbol is in global scope.
			// -> Can potentially be used, if there is no local declaration.
			else if (cb_symbol_get_scope(current) == NULL)
				// Do not break in this case,
				// since there could still be a local declaration of the symbol!
				result = current;
		}
		current = cb_symbol_get_next(current);
	}
	
	return result;
}

// -----------------------------------------------------------------------------
// Get next item in the symbol-table and move the 'current'-pointer to the next
// symbol
// -----------------------------------------------------------------------------
CbSymbol* cb_symtab_next(CbSymtab* st)
{
	CbSymbol* result = cb_symbol_get_next(cb_symtab_current(st));
	
	if (result)
		st->current	= result;
	
	return result;
}

// -----------------------------------------------------------------------------
// Get current item in the symbol-table
// -----------------------------------------------------------------------------
CbSymbol* cb_symtab_current(CbSymtab* st)
{
	return st->current;
}

// -----------------------------------------------------------------------------
// Get previous item in the symbol-table and move the 'current'-pointer to the
// previous symbol
// -----------------------------------------------------------------------------
CbSymbol* cb_symtab_previous(CbSymtab* st)
{
	CbSymbol* result = cb_symbol_get_previous(cb_symtab_current(st));
	
	if (result)
		st->current	= result;
	
	return result;
}

// -----------------------------------------------------------------------------
// Check if symbol-table is empty
// -----------------------------------------------------------------------------
bool cb_symtab_is_empty(CbSymtab* st)
{
	return st->size == 0;
}

// -----------------------------------------------------------------------------
// Enter new scope
// -----------------------------------------------------------------------------
void cb_symtab_enter_scope(CbSymtab* st, char* context)
{
	scope_t* new_scope = scope_create(context, st->scope_stack->count + 1);
	stack_push(st->scope_stack, new_scope);
}

// -----------------------------------------------------------------------------
// Leave current scope
// -----------------------------------------------------------------------------
void cb_symtab_leave_scope(CbSymtab* st)
{
	// Remove all local symbols in the current scope.
	// Begin with last symbol in the symbol-table, because recently appended
	// symbols are always at the end of the table!
	CbSymbol* current = st->last;
	while (current)
	{
		CbSymbol* temp = current;
		current		   = cb_symbol_get_previous(current);
		
		if (scope_equals(cb_symbol_get_scope(temp),
						 (scope_t*) stack_get_top_item(st->scope_stack)))
			// TODO: Use a different method to dispatch and remove the symbol.
			// Since symtab_dispatch() calls symtab_lookup() this could be a
			// quite slow operation!
			cb_symtab_remove(st, cb_symbol_get_id(temp));
	}
	
	scope_t* current_scope;
	stack_pop(st->scope_stack, (void*) &current_scope);	// pop and
	scope_free(current_scope);							// free current scope
}
