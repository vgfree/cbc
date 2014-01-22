/*******************************************************************************
 * cbc_test -- Codeblock Unit-Test using the 'sput' Unit-Test framework
 ******************************************************************************/

#include <string.h>

#include "cbc_test.h"

#include "../codeblock.h"
#include "../syntree.h"
#include "../symtab.h"


// #############################################################################
// test procedures
// #############################################################################

// -----------------------------------------------------------------------------
// Test: codeblock_execute()
// -----------------------------------------------------------------------------
static void test_codeblock_execute()
{
	codeblock_t* cb		= codeblock_create();
	cb->ast				= conststr_create("test");
	cb->global_symtab	= symtab_create();
	
	codeblock_execute(cb);	// execute codeblock once
	
	sput_assert(cb->result->type					== VT_STRING);
	sput_assert(strcmp(cb->result->string, "test")	== 0);
	
	codeblock_execute(cb);	// execute a second time 
	
	sput_assert(cb->result->type					== VT_STRING);
	sput_assert(strcmp(cb->result->string, "test")	== 0);
	
	syntree_free(cb->ast);
	symtab_free(cb->global_symtab);
	codeblock_free(cb);
}


// #############################################################################
// Main
// #############################################################################

int main(int argc, char *argv[])
{
	sput_start_testing();	// *************************************************
	
	sput_enter_suite("Execute simple codeblock");
	sput_run_test(test_codeblock_execute);
	
	sput_finish_testing();	// *************************************************
	
	return sput_get_return_value();
}
