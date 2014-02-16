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
	cb->ast				= cb_conststr_create("test");
	
	codeblock_execute(cb);	// execute codeblock once
	
	CuAssertIntEquals(tc, VT_STRING,cb->result->type);
	CuAssertStrEquals(tc, "test",	cb->result->string);
	
	codeblock_execute(cb);	// execute a second time
	
	CuAssertIntEquals(tc, VT_STRING,cb->result->type);
	CuAssertStrEquals(tc, "test",	cb->result->string);
	
	cb_syntree_free(cb->ast);
	codeblock_free(cb);
}

// -----------------------------------------------------------------------------
// Test: codeblock_execute() -- more complex codeblock
// -----------------------------------------------------------------------------
void test_codeblock_execute_complex(CuTest *tc)
{
	codeblock_t* cb		= codeblock_create();
	
	CbStrlist* args		= cb_strlist_create("");
	args->data			= symref_create("foo");
	
	cb->ast				= 	cb_syntree_create(
								SNT_STATEMENTLIST,
								cb_syntree_create(SNT_DECLARATION, symref_create("foo"), NULL),
								cb_syntree_create(
									SNT_STATEMENTLIST,
									funcdecl_create(
										"Inc",
										cb_syntree_create(
											SNT_ASSIGNMENT,
											symref_create("Result"),
											cb_syntree_create('+', symref_create("nNumber"), cb_constval_create(1))
										)
										,
										cb_strlist_create("nNumber")
									),
									cb_syntree_create(
										SNT_STATEMENTLIST,
										cb_syntree_create(SNT_ASSIGNMENT, symref_create("foo"), cb_constval_create(0)),
										cb_syntree_create(
											SNT_STATEMENTLIST,
											cb_flow_create(
												SNT_FLOW_WHILE,
												cb_comparison_create(CMP_LT, symref_create("foo"), cb_constval_create(100)),
												cb_syntree_create(
													SNT_ASSIGNMENT,
													symref_create("foo"),
													funccall_create("Inc", args)
												), NULL
											),
											cb_flow_create(
												SNT_FLOW_IF,
												cb_comparison_create(CMP_EQ, symref_create("foo"), cb_constval_create(100)),
												cb_conststr_create("foo is 100"),
												cb_conststr_create("foo is not 100")
											)
										)
									)
								)
							);
	
	codeblock_execute(cb);	// execute codeblock once
	
	CuAssertIntEquals(tc, VT_STRING,	cb->result->type);
	CuAssertStrEquals(tc, "foo is 100",	cb->result->string);
	
	cb_syntree_free(cb->ast);
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
