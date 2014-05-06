/*******************************************************************************
 * array_test -- Testing the CbArray structure
 ******************************************************************************/

#include <stdio.h>
#include <CuTest.h>
#include "../array.h"
#include "../value.h"

// #############################################################################
// test procedures
// #############################################################################

// -----------------------------------------------------------------------------
// Test: test_array()
// -----------------------------------------------------------------------------
void test_array(CuTest *tc)
{
	CbArray* a = cb_array_create();
	CuAssertTrue(tc, cb_array_append(a, cb_numeric_create(123)));
	CuAssertIntEquals(tc, 1, cb_array_get_count(a));
	CuAssertIntEquals(tc, VT_NUMERIC, cb_array_get(a, 0)->type);
	CuAssertIntEquals(tc, 123, cb_array_get(a, 0)->value);
	
	int i = 1;
	for (; i <= 200; i++)
	{
		CuAssertTrue(tc, cb_array_append(a, cb_numeric_create(i)));
		CuAssertIntEquals(tc, i + 1, cb_array_get_count(a));
		
		CuAssertIntEquals(tc, VT_NUMERIC, cb_array_get(a, i)->type);
		CuAssertIntEquals(tc, i, cb_array_get(a, i)->value);
		printf("element %d: %d\n", i, cb_array_get(a, i)->value);
	}
	
	cb_array_free(a);
}


// #############################################################################
// make suite
// #############################################################################

CuSuite* make_suite_array()
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_array);
	return suite;
}
