/*******************************************************************************
 * cbc_test -- Codeblock Unit-Test using the 'CuTest' Unit-Test framework
 ******************************************************************************/

#include <stdio.h>
#include <CuTest.h>
#include "codeblock_test.h"
#include "scope_test.h"
#include "stack_test.h"


// #############################################################################
// helper functions
// #############################################################################

// -----------------------------------------------------------------------------
// wrapper for CuSuiteAddSuite()
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
	CuString* output= CuStringNew();
	CuSuite* suite	= CuSuiteNew();
	
	CuSuiteAddSuite_Custom(suite, make_suite_codeblock());
	CuSuiteAddSuite_Custom(suite, make_suite_scope());
	CuSuiteAddSuite_Custom(suite, make_suite_stack());
	
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
