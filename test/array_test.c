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
	
	CuAssertTrue(tc, cb_array_set(a, 0, cb_numeric_create(321)));
	CuAssertIntEquals(tc, 1, cb_array_get_count(a));
	CuAssertIntEquals(tc, VT_NUMERIC, cb_array_get(a, 0)->type);
	CuAssertIntEquals(tc, 321, cb_array_get(a, 0)->value);
	
	cb_array_free(a);
	a = cb_array_create();
	
	int i = 0;
	for (; i < 100; i++)	// append 100 elements
	{
		CuAssertTrue(tc, cb_array_append(a, cb_numeric_create(i)));
		CuAssertIntEquals(tc, i + 1, cb_array_get_count(a));
		
		CbValue* element = cb_array_get(a, i);
		CuAssertPtrNotNull(tc, element);
		CuAssertIntEquals(tc, VT_NUMERIC, element->type);
		CuAssertIntEquals(tc, i, element->value);
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
