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
	scope_t* scope1			= scope_create("scope1", 1);
	scope_t* scope2			= scope_create("scope2", 2);
	scope_t* scope1_clone	= scope_create("scope1", 1);
	
	CuAssertStrEquals(tc, "scope1",	scope1->context);
	CuAssertIntEquals(tc, 1,		scope1->level);
	CuAssertStrEquals(tc, "scope1",	scope1_clone->context);
	CuAssertIntEquals(tc, 1,		scope1_clone->level);
	CuAssertStrEquals(tc, "scope2",	scope2->context);
	CuAssertIntEquals(tc, 2,		scope2->level);
	
	CuAssertTrue(tc, !scope_equals(scope1, scope2));
	CuAssertTrue(tc, !scope_equals(NULL, scope2));
	CuAssertTrue(tc, !scope_equals(scope1, NULL));
	CuAssertTrue(tc, scope_equals(scope1, scope1_clone));
	CuAssertTrue(tc, scope_equals(NULL, NULL));
	
	scope_free(scope1);
	scope_free(scope1_clone);
	scope_free(scope2);
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
