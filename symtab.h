/*******************************************************************************
 * symtab -- implementation of a symbol-table structure
 * 
 *  	the symbol-table is supposed to store all symbols ocourring in the
 *  	codeblock source-code being parsed.
 *  	every symbol contains a symbol-identifier, the type and the value of
 *  	the symbol.
 ******************************************************************************/

#ifndef SYMTAB_H
#define SYMTAB_H

#include "syntree.h"

// symbol types
enum symbol_type_t
{
	SYM_UNDEFINED,
	SYM_SYMTAB,
	SYM_VARIABLE,
	SYM_FUNCTION
};

// symbol structure:
// a symbol either represents a variable or a function.
// therefore the specific elements of a symbol are contained within an union.
// these are on one hand the value-element for variables and the
// function-element for functions on the other hand.
typedef struct symbol_t
{
	// type of the node
	enum symbol_type_t type;
	// symbol-name
	char* identifier;
	
	union
	{
		// symbol-value
		int value;
		// function-code
		syntree* function;
	};
	
	// next symbol
	struct symbol_t* next;
} symbol;

// interface functions
symbol* symtab_create();
void symtab_append(symbol* symtab, symbol* s, syntree* stmtlist);
symbol* symtab_lookup(symbol* symtab, char* key);
void symtab_free(symbol* symtab);
symbol* symbol_create(enum symbol_type_t type, char* identifier);
void symbol_free(symbol* s);

// global symbol-table, has to be initialized and freed in the main-function
symbol* gl_symtab;

#endif // SYMTAB_H
