/*******************************************************************************
 * generic_codeblock_test -- Testing codeblock test script files.
 *                           Every script file belongs to a specific category.
 *                           A category can be one of these:
 *                             - mathematical operations
 *                             - symbols
 *                             - control-flow
 *                             - functions
 *                             - data-types
 *                             - misc
 ******************************************************************************/

#ifndef GENERIC_CODEBLOCK_TEST_H
#define GENERIC_CODEBLOCK_TEST_H


#include <CuTest.h>

#define TEST_FILES_DIR	"./testfiles"

// test procedures
void test_codeblock_all_files();

// make suite
CuSuite* make_suite_codeblock_generic();


#endif // GENERIC_CODEBLOCK_TEST_H
