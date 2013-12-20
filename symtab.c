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
	symbol* newsym			= symbol_create(s->type, s->identifier);
	
	// set function-body, if symbol represents a function
	if (s->type == SYM_FUNCTION)
		newsym->function	= stmtlist;
	else
		newsym->function	= NULL;
	
	current_symbol->next	= newsym;
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
	
	s->type		= type;
	s->value	= 0;
	s->next		= NULL;
	
	return s;
}

// -----------------------------------------------------------------------------
// free symbol
// -----------------------------------------------------------------------------
void symbol_free(symbol* s)
{
	if (!s)
		return;
	
	free(s->identifier);
	free(s);
}
