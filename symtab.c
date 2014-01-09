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
void symtab_append(symbol* symtab, symbol* s)
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
	
	current_symbol->next = s;
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
// declare symbol in the symbol-table.
// this function requires a predefined symbol to be passed as second parameter.
// -----------------------------------------------------------------------------
void symtab_declare(symbol* symtab, symbol* s)
{
	// check for a valid symbol-table
	if (symtab->type != SYM_SYMTAB)
	{
		yyerror("not a valid symbol-table");
		exit(1);
	}
	
	// check if symbol already exists in symbol-table
	if (symtab_lookup(symtab, s->identifier))
	{
		yyerror("cannot redeclare symbol: %s", s->identifier);
		exit(1);
	}
	else
		symtab_append(symtab, s);
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
	
	// variable specific
	if (type == SYM_VARIABLE)
		// variable-symbols and undefined symbols need a value attribute!
		s->value = cbvalue_create();
	else if (s->type == SYM_VARIABLE)
		// if symbol turns from a variable to anything else, the value
		// attribute needs to be freed explicitly
		cbvalue_free(s->value);
	
	// function specific
	if (type == SYM_FUNCTION)
		// function-symbols need a func attribute!
		s->func = function_create();
	else if (s->type == SYM_FUNCTION)
		function_free(s->func);
	
	// finally, set the new type of the symbol
	s->type = type;
}

// -----------------------------------------------------------------------------
// free symbol
// -----------------------------------------------------------------------------
void symbol_free(symbol* s)
{
	// set type to undefined, to automatically free the appropriate attributes
	symbol_settype(s, SYM_UNDEFINED);
	
	free(s->identifier);
	free(s);
}

// -----------------------------------------------------------------------------
// create function
// -----------------------------------------------------------------------------
function* function_create()
{
	function* f	= malloc(sizeof(function));
	f->params	= NULL;
	f->body		= NULL;
	
	return f;
}

// -----------------------------------------------------------------------------
// free function
// -----------------------------------------------------------------------------
void function_free(function* f)
{
	if (f->params)
		syntree_free(f->params);
	
	syntree_free(f->body);
	free(f);
}

// -----------------------------------------------------------------------------
// declare a function
// -----------------------------------------------------------------------------
void function_declare(symbol* symtab, symbol* s)
{
	if(s->type != SYM_FUNCTION)
	{
		yyerror("wrong symbol-type, expecting SYM_FUNCTION");
		exit(1);
	}
	
	symtab_declare(symtab, s);
}

// -----------------------------------------------------------------------------
// declare a variable
// -----------------------------------------------------------------------------
void variable_declare(symbol* symtab, symbol* s)
{
	if(s->type != SYM_VARIABLE)
	{
		yyerror("wrong symbol-type, expecting SYM_VARIABLE");
		exit(1);
	}
	
	symtab_declare(symtab, s);
}
