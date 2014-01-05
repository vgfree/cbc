/*******************************************************************************
 * symtab -- implementation of a symbol-table structure
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "symtab.h"
#include "errors.h"

// -----------------------------------------------------------------------------
// create a symbol-table
// -----------------------------------------------------------------------------
symbol* symtab_create()
{
	return symbol_create(SYM_SYMTAB, NULL);
}

// -----------------------------------------------------------------------------
// add a symbol to the symbol-table
// this procedure distinguishes between variable-symbols and function-symbols.
// whereas the function-symbols have an optional statement-list, that contains
// the function-code.
// -----------------------------------------------------------------------------
void symtab_append(symbol* symtab, symbol* s, syntree* stmtlist)
{
	// check for a valid symbol-table
	if (symtab->type != SYM_SYMTAB)
	{
		yyerror("not a valid symbol-table");
		exit(1);
	}
	
	symbol* current_symbol = symtab;
	
	while (current_symbol->next)
		current_symbol = current_symbol->next;
	
	// copy symbol
	symbol* newsym = symbol_create(s->type, s->identifier);
	
	switch (s->type)
	{
		case SYM_VARIABLE:
			// assign new value
			cbvalue_assign(s->value, newsym->value);
			break;
		
		case SYM_FUNCTION:
			// set function-body, if symbol represents a function
			newsym->function = stmtlist;
			break;
	}
	
	current_symbol->next = newsym;
}

// -----------------------------------------------------------------------------
// remove a symbol from the symbol-table
// -----------------------------------------------------------------------------
void symtab_remove(symbol* symtab, char* identifier)
{
	// check for a valid symbol-table
	if (symtab->type != SYM_SYMTAB)
	{
		yyerror("not a valid symbol-table");
		exit(1);
	}
	
	symbol* current_symbol = symtab;
	// search for a symbol with the identifier
	while (current_symbol->next)
	{
		symbol* temp	= current_symbol;
		current_symbol	= current_symbol->next;
		if (strcmp(identifier, current_symbol->identifier) == 0)
		{
			temp->next = current_symbol->next;
			symbol_free(current_symbol);
			break;
		}
	}
}

// -----------------------------------------------------------------------------
// lookup an identifier in the symbol-table
//  	if there was no symbol found, that matches the key-value, the function
//  	returns a NULL-pointer.
// -----------------------------------------------------------------------------
symbol* symtab_lookup(symbol* symtab, char* key)
{
	// check for a valid symbol-table
	if (symtab->type != SYM_SYMTAB)
	{
		yyerror("not a valid symbol-table");
		exit(1);
	}
	
	symbol* result = NULL;
	// current-symbol points to the symbol-table's head-item
	symbol* current_symbol = symtab;
	// search for the symbol
	while (current_symbol->next)
	{
		current_symbol = current_symbol->next;
		if (strcmp(key, current_symbol->identifier) == 0)
		{
			result = current_symbol;
			break;
		}
	}
	
	return result;
}

// -----------------------------------------------------------------------------
// free symbol-table
// -----------------------------------------------------------------------------
void symtab_free(symbol* symtab)
{
	// check for a valid symbol-table
	if (symtab->type != SYM_SYMTAB)
	{
		yyerror("not a valid symbol-table");
		exit(1);
	}
	
	// free all symbols in the table
	// current-symbol initially points to the table itself
	symbol* current_symbol = symtab;
	while (current_symbol->next)
	{
		symbol* temp		 = current_symbol->next;
		current_symbol->next = temp->next;
		symbol_free(temp);
	}
	// finally free the table
	free(symtab);
}

// -----------------------------------------------------------------------------
// create symbol
// -----------------------------------------------------------------------------
symbol* symbol_create(enum symbol_type_t type, char* identifier)
{
	symbol* s = malloc(sizeof(symbol));
	if (!s)
	{
		yyerror(ERR_BADALLOC);
		exit(1);
	}
	
	// copy identifier-string if available
	if (identifier)
		s->identifier = strdup(identifier);
	else
		s->identifier = NULL;
	
	s->type = SYM_UNDEFINED;
	s->next = NULL;
	symbol_settype(s, type);
	
	return s;
}

// -----------------------------------------------------------------------------
// set symbol-type
// -----------------------------------------------------------------------------
void symbol_settype(symbol* s, enum symbol_type_t type)
{
	if (s->type == type)
		return;
	
	if (type == SYM_VARIABLE)
		// variable-symbols and undefined symbols need a value attribute!
		s->value = cbvalue_create();
	else if (s->type == SYM_VARIABLE)
		// if symbol turns from a variable to anything else, the value
		// attribute needs to be freed explicitly
		cbvalue_free(s->value);
	
	// finally, set the new type of the symbol
	s->type = type;
}

// -----------------------------------------------------------------------------
// free symbol
// -----------------------------------------------------------------------------
void symbol_free(symbol* s)
{
	if (!s)
		return;
	
	if (s->type == SYM_VARIABLE)
		cbvalue_free(s->value);
	
	free(s->identifier);
	free(s);
}
