/*******************************************************************************
 * cbc_test -- Codeblock Unit-Test using the 'CuTest' Unit-Test framework
 ******************************************************************************/

#include <stdio.h>
#include <CuTest.h>
#include "codeblock_test.h"
#include "scope_test.h"

// #############################################################################
// Main
// #############################################################################

int main(int argc, char *argv[])
{
	CuString *output= CuStringNew();
	CuSuite* suite	= CuSuiteNew();
	
	CuSuiteAddSuite(suite, make_suite_codeblock());
	CuSuiteAddSuite(suite, make_suite_scope());
	
	CuSuiteRun(suite);
	CuSuiteSummary(suite, output);
	CuSuiteDetails(suite, output);
	
	printf("%s\n", output->buffer);
	
	return suite->failCount;
}
