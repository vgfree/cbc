/*******************************************************************************
 * Codeblock -- Implementation of the codeblock execution-environment.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "codeblock.h"
#include "cbc_lex.h"
#include "cbc_parse.h"
#include "syntree.h"
#include "builtin.h"


// #############################################################################
// declarations
// #############################################################################

static void codeblock_reset(Codeblock* cb);
static void codeblock_reset_result(Codeblock* cb);
static int codeblock_parse_internal(Codeblock* cb);


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// constructor
// -----------------------------------------------------------------------------
Codeblock* codeblock_create()
{
	Codeblock* cb = (Codeblock*) malloc(sizeof(Codeblock));
	cb->symtab	  = NULL;
	cb->ast		  = NULL;
	cb->result	  = NULL;
	
	return cb;
}

// -----------------------------------------------------------------------------
// destructor
// -----------------------------------------------------------------------------
void codeblock_free(Codeblock* cb)
{
	codeblock_reset(cb);
	codeblock_reset_result(cb);
	
	free(cb);
}

// -----------------------------------------------------------------------------
// parse a file stream
// -----------------------------------------------------------------------------
int codeblock_parse_file(Codeblock* cb, FILE* input)
{
	extern FILE* yyin;
	int result = EXIT_SUCCESS;
	
	if (input)	// determine input stream
		yyin = input;
	else
		yyin = stdin;
	
	result = codeblock_parse_internal(cb);
	
	yylex_destroy();	// cleanup lexer
	
	return result;
}

// -----------------------------------------------------------------------------
// parse a string
// -----------------------------------------------------------------------------
int codeblock_parse_string(Codeblock* cb, const char* string)
{
	int result = EXIT_SUCCESS;
	
	YY_BUFFER_STATE buffer_state = yy_scan_string(string);
	result = codeblock_parse_internal(cb);
	
	// cleanup
	// NOTE: Delete the buffer BEFORE calling yylex_destroy() !
	yy_delete_buffer(buffer_state);
	yylex_destroy();
	
	return result;
}

// -----------------------------------------------------------------------------
// execute codeblock
// -----------------------------------------------------------------------------
int codeblock_execute(Codeblock* cb)
{
	assert(cb->ast);
	
	cb->duration = 0;
	codeblock_reset_result(cb);
	
	cb->symtab = cb_symtab_create();						// create symbol table
	if (register_builtin_all(cb->symtab) == EXIT_SUCCESS)	// register builtin symbols
	{
		clock_t begin = clock();	// begin tracking of execution duration
		
		// execute codeblock
		cb->result = cb_syntree_eval(cb->ast, cb->symtab);
		
		clock_t end  = clock();		// end tracking of execution duration
		cb->duration = ((double) end - (double) begin) / CLOCKS_PER_SEC;
	}
	
	cb_symtab_free(cb->symtab);	// cleanup symbol table
	
	if (cb->result == NULL)
		return EXIT_FAILURE;
	else
		return EXIT_SUCCESS;
}


// #############################################################################
// internal functions
// #############################################################################

// -----------------------------------------------------------------------------
// reset codeblock syntax tree (internal)
// -----------------------------------------------------------------------------
static void codeblock_reset(Codeblock* cb)
{
	if (cb->ast)
	{
		cb_syntree_free(cb->ast);
		cb->ast = NULL;
	}
}

// -----------------------------------------------------------------------------
// reset codeblock result (internal)
// -----------------------------------------------------------------------------
static void codeblock_reset_result(Codeblock* cb)
{
	if (cb->result)
	{
		cb_value_free(cb->result);
		cb->result = NULL;
	}
}

// -----------------------------------------------------------------------------
// parse codeblock (internal)
// -----------------------------------------------------------------------------
static int codeblock_parse_internal(Codeblock* cb)
{
	int result = EXIT_SUCCESS;
	
	// reset codeblock, this is necessary in case the codeblock was already
	// parsed or executed before.
	codeblock_reset(cb);
	
	switch (yyparse(&cb->ast))
	{
		case 1:
			cb_print_error_msg("Parsing failed due to invalid input");
			result = EXIT_FAILURE;
			break;
		
		case 2:
			cb_print_error_msg("Parsing failed due to memory exhaustion");
			result = EXIT_FAILURE;
			break;
	}
	
	return result;
}
