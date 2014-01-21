/*******************************************************************************
 * cbc_test -- Unit-Test for cbc using the 'Check'-framework.
 ******************************************************************************/

#include "cbc_test.h"
#include "../codeblock.h"
#include "../syntree.h"
#include "../symtab.h"


// #############################################################################
// test-functions
// #############################################################################


// -----------------------------------------------------------------------------
// codeblock test-function
// -----------------------------------------------------------------------------
START_TEST (codeblock_test)
{
	// set up
	codeblock_t* cb		= codeblock_create();
	cb->global_symtab	= symtab_create();
	
	cb->ast = conststr_create("test");	// set codeblock-code
	codeblock_execute(cb);				// execute ...
	
	
	// checks
	ck_assert(cb->result->type					== VT_STRING);
	ck_assert(strcmp(cb->result->string, "test")== 0);
	
	
	// tear down
	syntree_free(cb->ast);
	symtab_free(cb->global_symtab);
	codeblock_free(cb);
}
END_TEST

// -----------------------------------------------------------------------------
// codeblock test-suite
// -----------------------------------------------------------------------------
Suite* codeblock_suite()
{
	Suite* suite = suite_create("codeblock");
	
	TCase* tc_codeblock = tcase_create("codeblock");
	tcase_add_test(tc_codeblock, codeblock_test);
	suite_add_tcase(suite, tc_codeblock);
	
	return suite;
}


// #############################################################################
// main
// #############################################################################

int main(void)
{
	int failed;
	
	Suite* cb_suite		= codeblock_suite();
	SRunner* cb_runner	= srunner_create(cb_suite);
	
	srunner_run_all(cb_runner, CK_NORMAL);
	
	failed = srunner_ntests_failed(cb_runner);
	srunner_free(cb_runner);
	
	return failed;
}
