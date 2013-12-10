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

// symbol types
enum symbol_type_t
{
	SYM_SYMTAB,
	SYM_VARIABLE
};

// symbol structure
typedef struct symbol_t
{
	// type of the node
	enum symbol_type_t type;
	// symbol-name
	char* identifier;
	// symbol-value
	int value;
	// next symbol
	struct symbol_t* next;
} symbol;


// interface functions
symbol* symtab_create();
void symtab_append(symbol* symtab, symbol* s);
symbol* symtab_lookup(symbol* symtab, char* key);
void symtab_free(symbol* symtab);
symbol* symbol_create(enum symbol_type_t type, char* identifier);
void symbol_free(symbol* s);

// global symbol-table, has to be initialized and freed in the main-function
symbol* gl_symtab;

#endif // SYMTAB_H
