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
	
	symbol_t* current = st->first;
	while (current)
	{
		symbol_t* temp	= current;
		current			= symbol_get_next(current);
		symbol_free(temp);
	}
	
	stack_free(st->scope_stack);
	free(st);
}

// -----------------------------------------------------------------------------
// Append a symbol to the symbol-table
// -----------------------------------------------------------------------------
symbol_t* cb_symtab_append(CbSymtab* st, symbol_t* s)
{
	assert(st);
	assert(s);
	assert(symbol_get_next(s)	  == NULL); // allow appending single symbols only
	assert(symbol_get_previous(s) == NULL);
	
	if (cb_symtab_is_empty(st))
	{
		st->first	= s;
		st->current	= s;
	}
	else
		symbol_connect(st->last, s);
	
	symbol_set_scope(s, stack_get_top_item(st->scope_stack));
	st->last = s;
	st->size++;
	
	return st->last;
}

// -----------------------------------------------------------------------------
// Dispatch the symbol with the given id from the symbol-table.
// If function returns NULL, then there was no symbol found with the given id.
// -----------------------------------------------------------------------------
symbol_t* cb_symtab_dispatch(CbSymtab* st, const char* id)
{
	symbol_t* found = cb_symtab_lookup(st, id);
	
	if (found)
	{
		symbol_t* prev = symbol_get_previous(found);
		symbol_t* next = symbol_get_next(found);
		
		symbol_set_previous(found, NULL);
		symbol_set_next(found, NULL);
		
		// remove references to dispatched item
		if (prev) symbol_set_next(prev, NULL);
		if (next) symbol_set_previous(next, NULL);
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
void cb_symtab_remove(CbSymtab* st, const char* id)
{
	symbol_t* s = cb_symtab_dispatch(st, id);
	
	if (s)
		symbol_free(s);
}

// -----------------------------------------------------------------------------
// Dispatch all symbols in the symbol-table (TODO)
// -----------------------------------------------------------------------------
void symtab_dispatch_all(CbSymtab* st)
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
void symtab_clear(CbSymtab* st)
{
	// TODO
}

// -----------------------------------------------------------------------------
// Lookup a symbol by id
// -----------------------------------------------------------------------------
symbol_t* cb_symtab_lookup(CbSymtab* st, const char* id)
{
	symbol_t* current	= st->first;
	symbol_t* result	= NULL;
	
	while (current)
	{
		if (strcmp(id, symbol_get_id(current)) == 0)
		{
			if (scope_equals(symbol_get_scope(current),
							 stack_get_top_item(st->scope_stack)))
			{
				result = current;
				break;	// Correct symbol was found -> break
			}
			// NULL-scope means, that the current symbol is in global scope.
			// -> Can potentially be used, if there is no local declaration.
			else if (symbol_get_scope(current) == NULL)
				// Do not break in this case,
				// since there could still be a local declaration of the symbol!
				result = current;
		}
		current = symbol_get_next(current);
	}
	
	return result;
}

// -----------------------------------------------------------------------------
// Get next item in the symbol-table and move the 'current'-pointer to the next
// symbol
// -----------------------------------------------------------------------------
symbol_t* cb_symtab_next(CbSymtab* st)
{
	symbol_t* result = symbol_get_next(cb_symtab_current(st));
	
	if (result)
		st->current	= result;
	
	return result;
}

// -----------------------------------------------------------------------------
// Get current item in the symbol-table
// -----------------------------------------------------------------------------
symbol_t* cb_symtab_current(CbSymtab* st)
{
	return st->current;
}

// -----------------------------------------------------------------------------
// Get previous item in the symbol-table and move the 'current'-pointer to the
// previous symbol
// -----------------------------------------------------------------------------
symbol_t* cb_symtab_previous(CbSymtab* st)
{
	symbol_t* result = symbol_get_previous(cb_symtab_current(st));
	
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
	symbol_t* current = st->last;
	while (current)
	{
		symbol_t* temp = current;
		current		   = symbol_get_previous(current);
		
		if (scope_equals(symbol_get_scope(temp),
						 (scope_t*) stack_get_top_item(st->scope_stack)))
			// TODO: Use a different method to dispatch and remove the symbol.
			// Since symtab_dispatch() calls symtab_lookup() this could be a
			// quite slow operation!
			cb_symtab_remove(st, symbol_get_id(temp));
	}
	
	scope_t* current_scope;
	stack_pop(st->scope_stack, (void*) &current_scope);	// pop and
	scope_free(current_scope);							// free current scope
}
