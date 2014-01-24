/*******************************************************************************
 * scope_test -- Testing the scope_t structure
 ******************************************************************************/

#include "scope_test.h"

#include "../scope.h"

// #############################################################################
// test procedures
// #############################################################################

// -----------------------------------------------------------------------------
// Test: test_scope()
// -----------------------------------------------------------------------------
void test_scope(CuTest *tc)
{
	scope_t* scope = scope_create("global", 0);
	
	CuAssertStrEquals(tc, "global",	scope->context);
	CuAssertIntEquals(tc, 0,		scope->level);
	
	scope_free(scope);
}


// #############################################################################
// make suite
// #############################################################################

CuSuite* make_suite_scope()
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_scope);
	return suite;
}
