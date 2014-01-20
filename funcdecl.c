/*******************************************************************************
 * funcdecl_t -- 'syntree_t'-node, that declares a function in the symbol-table.
 ******************************************************************************/

#include <stdlib.h>
#include "funcdecl.h"
#include "symbol.h"
#include "symtab.h"
#include "syntree.h"


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
syntree_t* funcdecl_create(	char* identifier, syntree_t* body,
							strlist_t* params, symtab_t* symtab)
{
	funcdecl_t* node= malloc(sizeof(funcdecl_t));
	node->type		= SNT_FUNC_DECL;
	
	symbol_t* s 	= symbol_create();
	symbol_setid(s, identifier);
	symbol_settype(s, SYM_TYPE_FUNCTION);
	s->function->body	= body;
	s->function->params	= params;
	s->function->symtab	= symtab;
	
	node->function_sym	= s;
	
	return (syntree_t*) node;
}
