/*******************************************************************************
 * cbc_test -- Codeblock Unit-Test using the 'CuTest' Unit-Test framework
 ******************************************************************************/

#ifndef CBC_TEST_H
#define CBC_TEST_H


#include <stdio.h>
#include <CuTest.h>


// #############################################################################
// suites
// #############################################################################

extern CuSuite* make_suite_codeblock();
extern CuSuite* make_suite_scope();
extern CuSuite* make_suite_stack();
extern CuSuite* make_suite_symtab();
extern CuSuite* make_suite_symtab();
extern CuSuite* make_suite_codeblock_generic();
extern CuSuite* make_suite_syntree();
extern CuSuite* make_suite_error_handling();
extern CuSuite* make_suite_array();


#endif // CBC_TEST_H
