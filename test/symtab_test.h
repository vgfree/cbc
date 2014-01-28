/*******************************************************************************
 * symtab_test -- Testing the symbol-table structure
 ******************************************************************************/

#ifndef SYMTAB_TEST_H
#define SYMTAB_TEST_H


#include <CuTest.h>

// test procedures
void test_symtab();
void test_symtab_scope_stack();

// make suite
CuSuite* make_suite_symtab();


#endif // SYMTAB_TEST_H
