/*******************************************************************************
 * array_test -- Testing the CbArray structure
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
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
    CbArray* a = cb_array_create_with_ownership((CbArrayItemDestructor) cb_value_free,
                                                (CbArrayItemCopy) cb_value_copy);
    CuAssertTrue(tc, cb_array_append(a, cb_numeric_create(123)));
    CuAssertIntEquals(tc, 1, cb_array_get_count(a));
    
    CbValue* item = NULL;
    CuAssertTrue(tc, cb_array_get(a, 0, (CbArrayItem*) &item));
    CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
    CuAssertIntEquals(tc, 123, cb_numeric_get(item));
    
    CuAssertTrue(tc, cb_array_set(a, 0, cb_numeric_create(321)));
    CuAssertIntEquals(tc, 1, cb_array_get_count(a));
    
    CuAssertTrue(tc, cb_array_get(a, 0, (CbArrayItem*) &item));
    CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
    CuAssertIntEquals(tc, 321, cb_numeric_get(item));
    
    cb_array_free(a);
    a = cb_array_create_with_ownership((CbArrayItemDestructor) cb_value_free,
                                       (CbArrayItemCopy) cb_value_copy);
    
    int i = 0;
    for (; i < 100; i++)    // append 100 elements
    {
        CuAssertTrue(tc, cb_array_append(a, cb_numeric_create(i)));
        CuAssertIntEquals(tc, i + 1, cb_array_get_count(a));
        
        CbValue* current_item = NULL;
        CuAssertTrue(tc, cb_array_get(a, i, (CbArrayItem*) &current_item));
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
    CbArray* a    = cb_array_create_with_ownership((CbArrayItemDestructor) cb_value_free,
                                                   (CbArrayItemCopy) cb_value_copy);
    
    CuAssertFalse(tc, cb_array_get(a, 0, NULL));
    
    cb_array_append(a, cb_numeric_create(123));
    
    CuAssertTrue(tc, cb_array_get(a, 0, NULL));
    CuAssertFalse(tc, cb_array_get(a, 1, NULL));
    CuAssertFalse(tc, cb_array_get(a, 999, NULL));
    
    CuAssertTrue(tc, cb_array_get(a, 0, (CbArrayItem*) &item));
    CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
    CuAssertIntEquals(tc, 123, cb_numeric_get(item));
    
    cb_array_append(a, cb_numeric_create(321));
    
    CuAssertTrue(tc, cb_array_get(a, 0, NULL));
    CuAssertTrue(tc, cb_array_get(a, 1, NULL));
    CuAssertFalse(tc, cb_array_get(a, 2, NULL));
    
    CuAssertTrue(tc, cb_array_get(a, 1, (CbArrayItem*) &item));
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
    CbArray* a    = cb_array_create_with_ownership((CbArrayItemDestructor) cb_value_free,
                                                   (CbArrayItemCopy) cb_value_copy);
    
    CuAssertFalse(tc, cb_array_set(a, 0, NULL));
    CuAssertFalse(tc, cb_array_set(a, 1, NULL));
    CuAssertFalse(tc, cb_array_set(a, 999, NULL));
    CuAssertFalse(tc, cb_array_set(a, 0, cb_numeric_create(123456)));
    
    cb_array_append(a, cb_numeric_create(123));
    
    CuAssertTrue(tc, cb_array_get(a, 0, (CbArrayItem*) &item));
    CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
    CuAssertIntEquals(tc, 123, cb_numeric_get(item));
    
    CuAssertTrue(tc, cb_array_set(a, 0, NULL));
    CuAssertTrue(tc, cb_array_get(a, 0, NULL));
    
    CuAssertTrue(tc, cb_array_set(a, 0, cb_numeric_create(321)));
    
    CuAssertTrue(tc, cb_array_get(a, 0, (CbArrayItem*) &item));
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
    CbArray* a    = cb_array_create_with_ownership((CbArrayItemDestructor) cb_value_free,
                                                   (CbArrayItemCopy) cb_value_copy);
    
    int i = 0;
    for (; i < 10; i++)
        cb_array_append(a, cb_numeric_create(i));
    
    CuAssertIntEquals(tc, 10, cb_array_get_count(a));
    
    CuAssertTrue(tc, cb_array_get(a, 5, (CbArrayItem*) &item));
    CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
    CuAssertIntEquals(tc, 5, cb_numeric_get(item));
    
    CuAssertTrue(tc, cb_array_get(a, 6, (CbArrayItem*) &item));
    CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
    CuAssertIntEquals(tc, 6, cb_numeric_get(item));
    
    CuAssertTrue(tc, cb_array_insert(a, cb_numeric_create(-5), 5));
    
    CuAssertIntEquals(tc, 11, cb_array_get_count(a));
    
    CuAssertTrue(tc, cb_array_get(a, 5, (CbArrayItem*) &item));
    CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
    CuAssertIntEquals(tc, -5, cb_numeric_get(item));
    
    CuAssertTrue(tc, cb_array_get(a, 6, (CbArrayItem*) &item));
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
    CbArray* a    = cb_array_create_with_ownership((CbArrayItemDestructor) cb_value_free,
                                                   (CbArrayItemCopy) cb_value_copy);
    
    int i = 0;
    for (; i < 10; i++)
        cb_array_append(a, cb_numeric_create(i));
    
    CuAssertIntEquals(tc, 10, cb_array_get_count(a));
    
    CuAssertTrue(tc, cb_array_get(a, 5, (CbArrayItem*) &item));
    CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
    CuAssertIntEquals(tc, 5, cb_numeric_get(item));
    
    CuAssertTrue(tc, cb_array_get(a, 6, (CbArrayItem*) &item));
    CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
    CuAssertIntEquals(tc, 6, cb_numeric_get(item));
    
    CuAssertTrue(tc, cb_array_remove(a, 5));
    
    CuAssertIntEquals(tc, 9, cb_array_get_count(a));
    
    CuAssertTrue(tc, cb_array_get(a, 5, (CbArrayItem*) &item));
    CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
    CuAssertIntEquals(tc, 6, cb_numeric_get(item));
    
    CuAssertTrue(tc, cb_array_get(a, 6, (CbArrayItem*) &item));
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
    CbArray* a    = cb_array_create_with_ownership((CbArrayItemDestructor) cb_value_free,
                                                   (CbArrayItemCopy) cb_value_copy);
    
    int i = 0;
    for (; i < 20; i++)
        cb_array_append(a, cb_numeric_create(i));
    
    CuAssertIntEquals(tc, 20, cb_array_get_count(a));
    
    CbArray* new_array = cb_array_copy(a);
    
    for (i = 0; i < 20; i++)
    {
        CuAssertTrue(tc, cb_array_get(new_array, i, (CbArrayItem*) &item));
        CuAssertIntEquals(tc, CB_VT_NUMERIC, cb_value_get_type(item));
        CuAssertIntEquals(tc, i, cb_numeric_get(item));
    }
    
    cb_array_free(new_array);
    
    cb_array_free(a);
}

// -----------------------------------------------------------------------------
// Test CbValArray cb_value_to_string() function
// -----------------------------------------------------------------------------
void test_valarray_value_to_string(CuTest *tc)
{
    CbArray* a1 = cb_array_create_with_ownership((CbArrayItemDestructor) cb_value_free,
                                                 (CbArrayItemCopy) cb_value_copy);
    cb_array_append(a1, cb_boolean_create(true));
    cb_array_append(a1, cb_numeric_create(-5));
    cb_array_append(a1, cb_boolean_create(false));
    cb_array_append(a1, cb_string_create(strdup("bar")));
    
    CbArray* a2 = cb_array_create_with_ownership((CbArrayItemDestructor) cb_value_free,
                                                 (CbArrayItemCopy) cb_value_copy);
    cb_array_append(a2, cb_numeric_create(1234));
    cb_array_append(a2, cb_boolean_create(true));
    cb_array_append(a2, cb_boolean_create(false));
    cb_array_append(a2, cb_string_create(strdup("foo")));
    cb_array_append(a2, cb_valarray_create(a1));
    
    CbValue* val2 = cb_valarray_create(a2);
    
    char* value_string = cb_value_to_string(val2);
    CuAssertStrEquals(tc, "{1234,True,False,\"foo\",{True,-5,False,\"bar\"}}", value_string);
    
    free(value_string);
    cb_value_free(val2);
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
    SUITE_ADD_TEST(suite, test_valarray_value_to_string);
    return suite;
}
