/*******************************************************************************
 * codeblock_test -- Testing the codeblock_t structure
 ******************************************************************************/

#include "codeblock_test.h"

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
void test_codeblock_execute(CuTest *tc)
{
	codeblock_t* cb		= codeblock_create();
	cb->ast				= conststr_create("test");
	cb->global_symtab	= symtab_create();
	
	codeblock_execute(cb);	// execute codeblock once
	
	CuAssertIntEquals(tc, VT_STRING,cb->result->type);
	CuAssertStrEquals(tc, "test",	cb->result->string);
	
	codeblock_execute(cb);	// execute a second time
	
	CuAssertIntEquals(tc, VT_STRING,cb->result->type);
	CuAssertStrEquals(tc, "test",	cb->result->string);
	
	syntree_free(cb->ast);
	symtab_free(cb->global_symtab);
	codeblock_free(cb);
}

// -----------------------------------------------------------------------------
// Test: codeblock_execute() -- more complex codeblock
// -----------------------------------------------------------------------------
void test_codeblock_execute_complex(CuTest *tc)
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
											symref_create("Result"),
											syntree_create('+', symref_create("nNumber"), constval_create(1))
										)
										,
										strlist_create("nNumber")
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
	
	CuAssertIntEquals(tc, VT_STRING,	cb->result->type);
	CuAssertStrEquals(tc, "foo is 100",	cb->result->string);
	
	syntree_free(cb->ast);
	symtab_free(cb->global_symtab);
	codeblock_free(cb);
}


// #############################################################################
// make suite
// #############################################################################

CuSuite* make_suite_codeblock()
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_codeblock_execute);
	SUITE_ADD_TEST(suite, test_codeblock_execute_complex);
	return suite;
}
