/*******************************************************************************
 * cbc_test -- Codeblock Unit-Test using the 'sput' Unit-Test framework
 ******************************************************************************/

#include <string.h>

#include "cbc_test.h"

#include "../codeblock.h"
#include "../syntree.h"
#include "../symtab.h"
#include "../symref.h"
#include "../funcdecl.h"
#include "../funccall.h"


// #############################################################################
// test procedures
// #############################################################################

// -----------------------------------------------------------------------------
// Test: codeblock_execute() -- simple codeblock
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

// -----------------------------------------------------------------------------
// Test: codeblock_execute() -- more complex codeblock
// -----------------------------------------------------------------------------
static void test_codeblock_execute_complex()
{
	codeblock_t* cb		= codeblock_create();
	
	strlist_t* args		= strlist_create("");
	args->data			= symref_create("foo");
	
	cb->global_symtab	= symtab_create();
	cb->ast				= 	syntree_create(
								SNT_STATEMENTLIST,
								syntree_create(SNT_DECLARATION, symref_create("foo"), NULL),
								syntree_create(
									SNT_STATEMENTLIST,
									funcdecl_create(
										"Inc",
										syntree_create(
											SNT_ASSIGNMENT,
											symref_create("nNumber"),
											syntree_create('+', symref_create("nNumber"), constval_create(1))
										)
										,
										strlist_create("nNumber"),
										cb->global_symtab
									),
									syntree_create(
										SNT_STATEMENTLIST,
										syntree_create(SNT_ASSIGNMENT, symref_create("foo"), constval_create(0)),
										syntree_create(
											SNT_STATEMENTLIST,
											flow_create(
												SNT_FLOW_WHILE,
												comparison_create(CMP_LT, symref_create("foo"), constval_create(100)),
												syntree_create(
													SNT_ASSIGNMENT,
													symref_create("foo"),
													funccall_create("Inc", args)
												), NULL
											),
											flow_create(
												SNT_FLOW_IF,
												comparison_create(CMP_EQ, symref_create("foo"), constval_create(100)),
												conststr_create("foo is 100"),
												conststr_create("foo is not 100")
											)
										)
									)
								)
							);
	
	codeblock_execute(cb);	// execute codeblock once
	
	sput_assert(cb->result->type						== VT_STRING);
	sput_assert(strcmp(cb->result->string, "foo is 100")== 0);
	
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
	sput_enter_suite("Execute complex codeblock");
	sput_run_test(test_codeblock_execute_complex);
	
	sput_finish_testing();	// *************************************************
	
	return sput_get_return_value();
}
