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
	SYM_CONSTANT,
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
} symbol;


// interface functions
symbol* symbol_create(enum symbol_type_t type, char* identifier);
void symbol_setvalue(int value);
void symbol_free(symbol* node);

#endif // SYMTAB_H
