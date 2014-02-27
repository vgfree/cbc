/*******************************************************************************
 * cbc_test -- Codeblock Unit-Test using the 'CuTest' Unit-Test framework
 ******************************************************************************/

#include <stdio.h>
#include <CuTest.h>
#include "codeblock_test.h"
#include "scope_test.h"
#include "stack_test.h"
#include "symtab_test.h"
#include "generic_codeblock_test.h"
#include "syntree_test.h"


// #############################################################################
// helper functions
// #############################################################################

// -----------------------------------------------------------------------------
// Wrapper for CuSuiteAddSuite()
// This function avoids memory leaks when adding one test-suite to another,
// since the passed CuSuite instance will be copied in the original function.
// -----------------------------------------------------------------------------
void CuSuiteAddSuite_Custom(CuSuite* testSuite, CuSuite* testSuite2)
{
	CuSuiteAddSuite(testSuite, testSuite2);
	free(testSuite2);	// free the dummy test-suite
}


// #############################################################################
// Main
// #############################################################################

int main(int argc, char *argv[])
{
	CuString* output = CuStringNew();
	CuSuite* suite	 = CuSuiteNew();
	
	CuSuiteAddSuite_Custom(suite, make_suite_codeblock());
	CuSuiteAddSuite_Custom(suite, make_suite_scope());
	CuSuiteAddSuite_Custom(suite, make_suite_stack());
	CuSuiteAddSuite_Custom(suite, make_suite_symtab());
	CuSuiteAddSuite_Custom(suite, make_suite_symtab());
	CuSuiteAddSuite_Custom(suite, make_suite_codeblock_generic());
	CuSuiteAddSuite_Custom(suite, make_suite_syntree());
	
	// run tests
	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	
	printf("%s\n", output->buffer);	// print test-result
	int result = suite->failCount;
	
	// cleanup test-environment
	CuStringDelete(output);
	CuSuiteDelete(suite);
	
	return result;
}
