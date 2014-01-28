/*******************************************************************************
 * stack_test -- Testing the stack_t structure
 ******************************************************************************/

#ifndef STACK_TEST_H
#define STACK_TEST_H


#include <stdlib.h>
#include <CuTest.h>

// test procedures
void test_stack();

// make suite
CuSuite* make_suite_stack();


#endif // STACK_TEST_H
