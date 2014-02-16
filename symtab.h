/*******************************************************************************
 * CbSymtab -- Implementation of a symbol-table structure.
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
CbSymtab* cb_symtab_create();
void cb_symtab_free(CbSymtab* st);
symbol_t* cb_symtab_append(CbSymtab* st, symbol_t* s);
symbol_t* cb_symtab_dispatch(CbSymtab* st, const char* id);
void cb_symtab_remove(CbSymtab* st, const char* id);
symbol_t* cb_symtab_lookup(CbSymtab* st, const char* id);
symbol_t* cb_symtab_next(CbSymtab* st);
symbol_t* cb_symtab_current(CbSymtab* st);
symbol_t* cb_symtab_previous(CbSymtab* st);
bool cb_symtab_is_empty(CbSymtab* st);
void cb_symtab_enter_scope(CbSymtab* st, char* context);
void cb_symtab_leave_scope(CbSymtab* st);

#endif // SYMTAB_H
