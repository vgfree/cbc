/*******************************************************************************
 * symtab_t -- Implementation of a symbol-table structure
 * 
 *      The symbol-table is supposed to store all symbols ocourring in the
 *      codeblock source-code being parsed.
 ******************************************************************************/

#ifndef SYMTAB_H
#define SYMTAB_H


#include <stdbool.h>
#include <stddef.h>
#include "symtab_if.h"
#include "symbol.h"
#include "stack.h"

struct symbol_table
{
	symbol_t* first;
	symbol_t* last;
	symbol_t* current;
	size_t size;
	stack_t* scope_stack;
};

// Interface functions
symtab_t* symtab_create();
void symtab_free(symtab_t* st);
symbol_t* symtab_append(symtab_t* st, symbol_t* s);
symbol_t* symtab_dispatch(symtab_t* st, char* id);
void symtab_remove(symtab_t* st, char* id);
void symtab_dispatch_all(symtab_t* st);
void symtab_clear(symtab_t* st);
symbol_t* symtab_lookup(symtab_t* st, char* id);
symbol_t* symtab_next(symtab_t* st);
symbol_t* symtab_current(symtab_t* st);
symbol_t* symtab_previous(symtab_t* st);
bool symtab_isempty(symtab_t* st);
void symtab_enter_scope(symtab_t* st, char* context);
void symtab_leave_scope(symtab_t* st);

#endif // SYMTAB_H
