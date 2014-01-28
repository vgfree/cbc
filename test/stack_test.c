/*******************************************************************************
 * stack_test -- Testing the stack_t structure
 ******************************************************************************/

#include "stack_test.h"

#include "../stack.h"
#include "../scope.h"

// #############################################################################
// test procedures
// #############################################################################

// -----------------------------------------------------------------------------
// Test: test_stack() --	Also demonstrates how to use the stack with
//							primitive types.
// -----------------------------------------------------------------------------
void test_stack(CuTest *tc)
{
	stack_t* stack = stack_create();
	
	CuAssertIntEquals(tc, 0, stack->count);
	CuAssertPtrEquals(tc, NULL, stack->top);
	
	int i = 0;
	for (; i < 10; i++)
		stack_push(stack, &i);
	
	CuAssertIntEquals(tc, 10, stack->count);
	CuAssertPtrNotNull(tc, stack->top);
	
	for (; i > 0; i--)
	{
		int* dummy;
		stack_pop(stack, (void*) &dummy);
		CuAssertPtrEquals(tc, &i, dummy);
	}
	
	CuAssertIntEquals(tc, 0, stack->count);
	CuAssertPtrEquals(tc, NULL, stack->top);
	
	stack_free(stack);
}

// -----------------------------------------------------------------------------
// Test: test_stack_complex() -- Also demonstrates how to use a "scope-stack"
// -----------------------------------------------------------------------------
void test_stack_complex(CuTest *tc)
{
	stack_t* stack = stack_create();
	
	int i = 0;
	for (; i < 10; i++)
		stack_push(stack, scope_create("scope", i + 1));
	
	CuAssertIntEquals(tc, 10, stack->count);
	CuAssertPtrNotNull(tc, stack->top);
	
	for (; i > 0; i--)
	{
		scope_t* dummy;
		stack_pop(stack, (void*) &dummy);
		CuAssertStrEquals(tc, "scope", dummy->context);
		CuAssertIntEquals(tc, i, dummy->level);
		
		scope_free(dummy);
	}
	
	CuAssertIntEquals(tc, 0, stack->count);
	CuAssertPtrEquals(tc, NULL, stack->top);
	
	stack_free(stack);
}

// -----------------------------------------------------------------------------
// Test: test_stack_error() -- Test stack-underflow
// -----------------------------------------------------------------------------
void test_stack_error(CuTest *tc)
{
	stack_t* stack = stack_create();
	
	CuAssertIntEquals(tc, 0, stack->count);						// no items on the stack
	CuAssertIntEquals(tc, EXIT_FAILURE, stack_pop(stack, NULL));// -> stack underflow
	
	int i = 0;
	for (; i < 10; i++)
		stack_push(stack, NULL);
	
	CuAssertIntEquals(tc, 10, stack->count);
	
	for (; i > 0; i--)
		stack_pop(stack, NULL);
	
	CuAssertIntEquals(tc, 0, stack->count);
	CuAssertIntEquals(tc, EXIT_FAILURE, stack_pop(stack, NULL));// -> stack underflow
	
	stack_free(stack);
}


// #############################################################################
// make suite
// #############################################################################

CuSuite* make_suite_stack()
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_stack);
	SUITE_ADD_TEST(suite, test_stack_complex);
	SUITE_ADD_TEST(suite, test_stack_error);
	return suite;
}
