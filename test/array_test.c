/*******************************************************************************
 * array_test -- Testing the CbArray structure
 ******************************************************************************/

#include <stdio.h>
#include <CuTest.h>
#include "CuTestCustomUtils.h"
#include "../array.h"
#include "../value.h"

// #############################################################################
// test procedures
// #############################################################################

// -----------------------------------------------------------------------------
// Test: test_array() -- Simple test for CbArray
// -----------------------------------------------------------------------------
void test_array(CuTest *tc)
{
	CbArray* a = cb_array_create();
	CuAssertTrue(tc, cb_array_append(a, cb_numeric_create(123)));
	CuAssertIntEquals(tc, 1, cb_array_get_count(a));
	
	CbValue* item = NULL;
	CuAssertTrue(tc, cb_array_get(a, 0, &item));
	CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
	CuAssertIntEquals(tc, 123, cb_numeric_get(item));
	
	CuAssertTrue(tc, cb_array_set(a, 0, cb_numeric_create(321)));
	CuAssertIntEquals(tc, 1, cb_array_get_count(a));
	
	CuAssertTrue(tc, cb_array_get(a, 0, &item));
	CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
	CuAssertIntEquals(tc, 321, cb_numeric_get(item));
	
	cb_array_free(a);
	a = cb_array_create();
	
	int i = 0;
	for (; i < 100; i++)	// append 100 elements
	{
		CuAssertTrue(tc, cb_array_append(a, cb_numeric_create(i)));
		CuAssertIntEquals(tc, i + 1, cb_array_get_count(a));
		
		CbValue* current_item = NULL;
		CuAssertTrue(tc, cb_array_get(a, i, &current_item));
		CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(current_item));
		CuAssertIntEquals(tc, i, cb_numeric_get(current_item));
	}
	
	cb_array_free(a);
}

// -----------------------------------------------------------------------------
// Test for cb_array_get()
// -----------------------------------------------------------------------------
void test_array_get(CuTest *tc)
{
	CbValue* item = NULL;
	CbArray* a    = cb_array_create();
	
	CuAssertFalse(tc, cb_array_get(a, 0, NULL));
	
	cb_array_append(a, cb_numeric_create(123));
	
	CuAssertTrue(tc, cb_array_get(a, 0, NULL));
	CuAssertFalse(tc, cb_array_get(a, 1, NULL));
	CuAssertFalse(tc, cb_array_get(a, 999, NULL));
	
	CuAssertTrue(tc, cb_array_get(a, 0, &item));
	CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
	CuAssertIntEquals(tc, 123, cb_numeric_get(item));
	
	cb_array_append(a, cb_numeric_create(321));
	
	CuAssertTrue(tc, cb_array_get(a, 0, NULL));
	CuAssertTrue(tc, cb_array_get(a, 1, NULL));
	CuAssertFalse(tc, cb_array_get(a, 2, NULL));
	
	CuAssertTrue(tc, cb_array_get(a, 1, &item));
	CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
	CuAssertIntEquals(tc, 321, cb_numeric_get(item));
	
	cb_array_free(a);
}

// -----------------------------------------------------------------------------
// Test for cb_array_set()
// -----------------------------------------------------------------------------
void test_array_set(CuTest *tc)
{
	CbValue* item = NULL;
	CbArray* a    = cb_array_create();
	
	CuAssertFalse(tc, cb_array_set(a, 0, NULL));
	CuAssertFalse(tc, cb_array_set(a, 1, NULL));
	CuAssertFalse(tc, cb_array_set(a, 999, NULL));
	CuAssertFalse(tc, cb_array_set(a, 0, cb_numeric_create(123456)));
	
	cb_array_append(a, cb_numeric_create(123));
	
	CuAssertTrue(tc, cb_array_get(a, 0, &item));
	CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
	CuAssertIntEquals(tc, 123, cb_numeric_get(item));
	
	CuAssertTrue(tc, cb_array_set(a, 0, NULL));
	CuAssertTrue(tc, cb_array_get(a, 0, NULL));
	
	CuAssertTrue(tc, cb_array_set(a, 0, cb_numeric_create(321)));
	
	CuAssertTrue(tc, cb_array_get(a, 0, &item));
	CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
	CuAssertIntEquals(tc, 321, cb_numeric_get(item));
	
	cb_array_free(a);
}

// -----------------------------------------------------------------------------
// Test for cb_array_insert()
// -----------------------------------------------------------------------------
void test_array_insert(CuTest *tc)
{
	CbValue* item = NULL;
	CbArray* a    = cb_array_create();
	
	int i = 0;
	for (; i < 10; i++)
		cb_array_append(a, cb_numeric_create(i));
	
	CuAssertIntEquals(tc, 10, cb_array_get_count(a));
	
	CuAssertTrue(tc, cb_array_get(a, 5, &item));
	CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
	CuAssertIntEquals(tc, 5, cb_numeric_get(item));
	
	CuAssertTrue(tc, cb_array_get(a, 6, &item));
	CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
	CuAssertIntEquals(tc, 6, cb_numeric_get(item));
	
	CuAssertTrue(tc, cb_array_insert(a, cb_numeric_create(-5), 5));
	
	CuAssertIntEquals(tc, 11, cb_array_get_count(a));
	
	CuAssertTrue(tc, cb_array_get(a, 5, &item));
	CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
	CuAssertIntEquals(tc, -5, cb_numeric_get(item));
	
	CuAssertTrue(tc, cb_array_get(a, 6, &item));
	CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
	CuAssertIntEquals(tc, 5, cb_numeric_get(item));
	
	cb_array_free(a);
}

// -----------------------------------------------------------------------------
// Test for cb_array_remove()
// -----------------------------------------------------------------------------
void test_array_remove(CuTest *tc)
{
	CbValue* item = NULL;
	CbArray* a    = cb_array_create();
	
	int i = 0;
	for (; i < 10; i++)
		cb_array_append(a, cb_numeric_create(i));
	
	CuAssertIntEquals(tc, 10, cb_array_get_count(a));
	
	CuAssertTrue(tc, cb_array_get(a, 5, &item));
	CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
	CuAssertIntEquals(tc, 5, cb_numeric_get(item));
	
	CuAssertTrue(tc, cb_array_get(a, 6, &item));
	CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
	CuAssertIntEquals(tc, 6, cb_numeric_get(item));
	
	CuAssertTrue(tc, cb_array_remove(a, 5));
	
	CuAssertIntEquals(tc, 9, cb_array_get_count(a));
	
	CuAssertTrue(tc, cb_array_get(a, 5, &item));
	CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
	CuAssertIntEquals(tc, 6, cb_numeric_get(item));
	
	CuAssertTrue(tc, cb_array_get(a, 6, &item));
	CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
	CuAssertIntEquals(tc, 7, cb_numeric_get(item));
	
	cb_array_free(a);
}

// -----------------------------------------------------------------------------
// Test for cb_array_copy()
// -----------------------------------------------------------------------------
void test_array_copy(CuTest *tc)
{
	CbValue* item = NULL;
	CbArray* a    = cb_array_create();
	
	int i = 0;
	for (; i < 20; i++)
		cb_array_append(a, cb_numeric_create(i));
	
	CuAssertIntEquals(tc, 20, cb_array_get_count(a));
	
	CbArray* new_array = cb_array_copy(a);
	
	for (i = 0; i < 20; i++)
	{
		CuAssertTrue(tc, cb_array_get(new_array, i, &item));
		CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
		CuAssertIntEquals(tc, i, cb_numeric_get(item));
	}
	
	cb_array_free(new_array);
	
	cb_array_free(a);
}


// #############################################################################
// make suite
// #############################################################################

CuSuite* make_suite_array()
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_array);
	SUITE_ADD_TEST(suite, test_array_get);
	SUITE_ADD_TEST(suite, test_array_set);
	SUITE_ADD_TEST(suite, test_array_insert);
	SUITE_ADD_TEST(suite, test_array_remove);
	SUITE_ADD_TEST(suite, test_array_copy);
	return suite;
}
