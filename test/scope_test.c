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
	CbScope* scope1			= cb_scope_create("scope1", 1);
	CbScope* scope2			= cb_scope_create("scope2", 2);
	CbScope* scope1_clone	= cb_scope_create("scope1", 1);
	
	CuAssertStrEquals(tc, "scope1",	scope1->context);
	CuAssertIntEquals(tc, 1,		scope1->level);
	CuAssertStrEquals(tc, "scope1",	scope1_clone->context);
	CuAssertIntEquals(tc, 1,		scope1_clone->level);
	CuAssertStrEquals(tc, "scope2",	scope2->context);
	CuAssertIntEquals(tc, 2,		scope2->level);
	
	CuAssertTrue(tc, !cb_scope_equals(scope1, scope2));
	CuAssertTrue(tc, !cb_scope_equals(NULL, scope2));
	CuAssertTrue(tc, !cb_scope_equals(scope1, NULL));
	CuAssertTrue(tc, cb_scope_equals(NULL, NULL));
	CuAssertTrue(tc, cb_scope_equals(scope1, scope1_clone));
	
	cb_scope_free(scope1);
	cb_scope_free(scope1_clone);
	cb_scope_free(scope2);
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
