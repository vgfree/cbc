/*******************************************************************************
 * codeblock_test -- Testing the codeblock_t structure
 ******************************************************************************/

#ifndef CODEBLOCK_TEST_H
#define CODEBLOCK_TEST_H


#include <CuTest.h>

// test procedures
void test_codeblock_execute();
void test_codeblock_execute_complex();

// make suite
CuSuite* make_suite_codeblock();


#endif // CODEBLOCK_TEST_H
