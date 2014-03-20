/*******************************************************************************
 * codeblock_test -- Testing the codeblock_t structure
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "generic_codeblock_test.h"

#include "../cbc_parse.h"
#include "../value.h"
#include "../codeblock.h"
#include "../syntree.h"


// #############################################################################
// declarations
// #############################################################################

const CbValue expected_results[] = {
	{VT_NUMERIC, 5},		// Testcase 0
	{VT_NUMERIC, 17},
	{VT_NUMERIC, -1},
	{VT_NUMERIC, 6},
	{VT_NUMERIC, 259},
	{VT_NUMERIC, 13},		// Testcase 5
	{VT_NUMERIC, -7},
	{VT_NUMERIC, 15},
	{VT_NUMERIC, -100},
	{VT_NUMERIC, 130},
	{VT_NUMERIC, 12345},	// Testcase 10
	{VT_UNDEFINED, 0},
	{VT_UNDEFINED, 0},
	{VT_NUMERIC, 0},
	{VT_NUMERIC, 5},
	{VT_NUMERIC, 1},		// Testcase 15
	{VT_NUMERIC, 1},
	{VT_NUMERIC, 5},
	{VT_NUMERIC, 5},
	{VT_NUMERIC, 1},
	{VT_NUMERIC, 5},		// Testcase 20
	{VT_NUMERIC, 1},
	{VT_NUMERIC, 1},
	{VT_NUMERIC, 5},
	{VT_NUMERIC, 5},
	{VT_NUMERIC, 1},		// Testcase 25
	{VT_NUMERIC, 5},
	{VT_NUMERIC, 5},
	{VT_NUMERIC, 5},
	{VT_NUMERIC, 14},
	{VT_NUMERIC, 2},		// Testcase 30
	{VT_NUMERIC, 15},
	{VT_NUMERIC, 30},
	{VT_NUMERIC, 0},
	{VT_STRING, (CbNumeric) "\"foo\", \"bar\""},	// Cast to cbnumeric to
													// avoid compiler-warning
	{VT_BOOLEAN, true},		// Testcase 35
	{VT_BOOLEAN, true},
	{VT_BOOLEAN, false},
	{VT_BOOLEAN, false},
	{VT_BOOLEAN, true},
	{VT_STRING, (CbNumeric) "string1"},	// Testcase 40
	{VT_NUMERIC, 118},
	{VT_NUMERIC, 8},
	{VT_NUMERIC, 0},
	{VT_STRING, (CbNumeric) "1234567890"},
	{VT_STRING, (CbNumeric) "LNCU"}		// Testcase 45
};

// CbTestString -- Combination of a test codeblock string and the expected result
typedef struct
{
	char* cb_string;
	CbValue expected_result;
} CbTestString;

// logical gates (e.g. AND, OR, NOT) test codeblock strings
static const CbTestString cbstrings_logical_gates[] = {
	{"True and False,", {VT_BOOLEAN, false}},
	{"True and True,", {VT_BOOLEAN, true}},
	{"False and False,", {VT_BOOLEAN, false}},
	{"True or False,", {VT_BOOLEAN, true}},
	{"True or True,", {VT_BOOLEAN, true}},
	{"False or False,", {VT_BOOLEAN, false}},
	{"not False,", {VT_BOOLEAN, true}},
	{"not True,", {VT_BOOLEAN, false}},
	{
		"| foo, bar | foo := True, bar := False, not (foo and bar),",
		{VT_BOOLEAN, true}
	}
};


// #############################################################################
// utilities
// #############################################################################

// -----------------------------------------------------------------------------
// Test a specific codeblock script file (internal)
// -----------------------------------------------------------------------------
static void test_codeblock_file(CuTest *tc, const char* test_file_name,
								const CbValue* expected_result)
{
	FILE* test_file = fopen(test_file_name, "r");
	if (!test_file)
	{
		fprintf(stderr, "Error: Unable to open file `%s'!\n", test_file_name);
		CuAssertTrue(tc, false);
	}
	
	Codeblock* cb = codeblock_create();
	codeblock_parse_file(cb, test_file);
	
	fclose(test_file);
	
	codeblock_execute(cb);
	
	CuAssertIntEquals(tc, expected_result->type, cb->result->type);
	switch (expected_result->type)
	{
		case VT_BOOLEAN:
			CuAssertIntEquals(tc, expected_result->boolean, cb->result->boolean);
			break;
		
		case VT_NUMERIC:
			CuAssertIntEquals(tc, expected_result->value, cb->result->value);
			break;
		
		case VT_STRING:
			CuAssertStrEquals(tc, expected_result->string, cb->result->string);
			break;
		
		case VT_UNDEFINED:	// No value has to be checked here!
			break;
		
		default:
			CuAssertTrue(tc, false);
			break;
	}
	
	codeblock_free(cb);
}

// -----------------------------------------------------------------------------
// Test a codeblock string
// -----------------------------------------------------------------------------
void test_codeblock_string(CuTest *tc, const CbTestString* test_data)
{
	Codeblock* cb = codeblock_create();
	
	CuAssertIntEquals(tc, EXIT_SUCCESS,
					  codeblock_parse_string(cb, test_data->cb_string));
	CuAssertIntEquals(tc, EXIT_SUCCESS, codeblock_execute(cb));
	
	const CbValue* expected = &(test_data->expected_result);
	
	CuAssertIntEquals(tc, expected->type, cb->result->type);
	switch (expected->type)
	{
		case VT_BOOLEAN:
			CuAssertIntEquals(tc, expected->boolean, cb->result->boolean);
			break;
		
		case VT_NUMERIC:
			CuAssertIntEquals(tc, expected->value, cb->result->value);
			break;
		
		case VT_STRING:
			CuAssertStrEquals(tc, expected->string, cb->result->string);
			break;
	}
	
	codeblock_free(cb);
}


// #############################################################################
// test procedures
// #############################################################################

// -----------------------------------------------------------------------------
// Test: test_codeblock_all_files() -- Test all codeblock script files
// -----------------------------------------------------------------------------
void test_codeblock_all_files(CuTest *tc)
{
	int testcase	   = 0;
	int testcase_count = sizeof(expected_results) / sizeof(CbValue);
	
	for (; testcase < testcase_count; testcase++)
	{
		char* file_name = (char*) malloc(256);
		*file_name		= '\0';
		sprintf(file_name, TEST_FILES_DIR "/testcase_%d.dwp", testcase);
		test_codeblock_file(tc, file_name, &expected_results[testcase]);
		free(file_name);
	}
}

// -----------------------------------------------------------------------------
// Test logical gates (such as: AND, OR, NOT)
// -----------------------------------------------------------------------------
void test_codeblock_logical_gates(CuTest *tc)
{
	int testcase	   = 0;
	int testcase_count = sizeof(cbstrings_logical_gates) / sizeof(CbTestString);
	
	for (; testcase < testcase_count; testcase++)
		test_codeblock_string(tc, &cbstrings_logical_gates[testcase]);
}


// #############################################################################
// make suite
// #############################################################################

CuSuite* make_suite_codeblock_generic()
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_codeblock_all_files);
	SUITE_ADD_TEST(suite, test_codeblock_logical_gates);
	return suite;
}
