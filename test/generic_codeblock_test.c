/*******************************************************************************
 * codeblock_test -- Testing the codeblock_t structure
 ******************************************************************************/

#include <CuTest.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../cbc_parse.h"
#include "../value.h"
#include "../codeblock.h"
#include "../syntree.h"


// #############################################################################
// declarations
// #############################################################################

#define TEST_FILES_DIR	"./testfiles"


// CbTestValue -- Emulates CbValue, with "public" properties to be able to hold
//                a list with expected values.
typedef struct
{
	enum cb_value_type type;
	union
	{
		CbNumeric value;
		CbString  string;
		CbBoolean boolean;
	};
} CbTestValue;

const CbTestValue expected_results[] = {
	{CB_VT_NUMERIC, 5},		// Testcase 0
	{CB_VT_NUMERIC, 17},
	{CB_VT_NUMERIC, -1},
	{CB_VT_NUMERIC, 6},
	{CB_VT_NUMERIC, 259},
	{CB_VT_NUMERIC, 13},		// Testcase 5
	{CB_VT_NUMERIC, -7},
	{CB_VT_NUMERIC, 15},
	{CB_VT_NUMERIC, -100},
	{CB_VT_NUMERIC, 130},
	{CB_VT_NUMERIC, 12345},	// Testcase 10
	{CB_VT_UNDEFINED, 0},
	{CB_VT_UNDEFINED, 0},
	{CB_VT_NUMERIC, 0},
	{CB_VT_NUMERIC, 5},
	{CB_VT_NUMERIC, 1},		// Testcase 15
	{CB_VT_NUMERIC, 1},
	{CB_VT_NUMERIC, 5},
	{CB_VT_NUMERIC, 5},
	{CB_VT_NUMERIC, 1},
	{CB_VT_NUMERIC, 5},		// Testcase 20
	{CB_VT_NUMERIC, 1},
	{CB_VT_NUMERIC, 1},
	{CB_VT_NUMERIC, 5},
	{CB_VT_NUMERIC, 5},
	{CB_VT_NUMERIC, 1},		// Testcase 25
	{CB_VT_NUMERIC, 5},
	{CB_VT_NUMERIC, 5},
	{CB_VT_NUMERIC, 5},
	{CB_VT_NUMERIC, 14},
	{CB_VT_NUMERIC, 2},		// Testcase 30
	{CB_VT_NUMERIC, 15},
	{CB_VT_NUMERIC, 30},
	{CB_VT_NUMERIC, 0},
	{CB_VT_STRING, (CbNumeric) "\"foo\", \"bar\""},	// Cast to cbnumeric to
													// avoid compiler-warning
	{CB_VT_BOOLEAN, true},		// Testcase 35
	{CB_VT_BOOLEAN, true},
	{CB_VT_BOOLEAN, false},
	{CB_VT_BOOLEAN, false},
	{CB_VT_BOOLEAN, true},
	{CB_VT_STRING, (CbNumeric) "string1"},	// Testcase 40
	{CB_VT_NUMERIC, 118},
	{CB_VT_NUMERIC, 8},
	{CB_VT_NUMERIC, 0},
	{CB_VT_STRING, (CbNumeric) "1234567890"},
	{CB_VT_STRING, (CbNumeric) "LNCU"}		// Testcase 45
};

// CbTestString -- Combination of a test codeblock string and the expected result
typedef struct
{
	char* cb_string;
	CbTestValue expected_result;
} CbTestString;

// logical gates (e.g. AND, OR, NOT) test codeblock strings
static const CbTestString cbstrings_logical_gates[] = {
	// boolean
	{"True and False,", {CB_VT_BOOLEAN, false}},
	{"True and True,", {CB_VT_BOOLEAN, true}},
	{"False and False,", {CB_VT_BOOLEAN, false}},
	{"True or False,", {CB_VT_BOOLEAN, true}},
	{"True or True,", {CB_VT_BOOLEAN, true}},
	{"False or False,", {CB_VT_BOOLEAN, false}},
	{"not False,", {CB_VT_BOOLEAN, true}},
	{"not True,", {CB_VT_BOOLEAN, false}},
	{
		"| foo, bar | foo := True, bar := False, not (foo and bar),",
		{CB_VT_BOOLEAN, true}
	},
	// numeric
	{"15 and 31,", {CB_VT_NUMERIC, 15}},
	{"31 and 15,", {CB_VT_NUMERIC, 15}},
	{"15 and 15,", {CB_VT_NUMERIC, 15}},
	{"15 or 31,", {CB_VT_NUMERIC, 31}},
	{"31 or 15,", {CB_VT_NUMERIC, 31}},
	{"123 or 456,", {CB_VT_NUMERIC, 507}},
	{"not 5,", {CB_VT_NUMERIC, -6}},
	{"not 123456,", {CB_VT_NUMERIC, -123457}},
	{
		"| foo, bar | foo := 31, bar := 15, not (foo and bar),",
		{CB_VT_NUMERIC, -16}
	}
};


// #############################################################################
// utilities
// #############################################################################

// -----------------------------------------------------------------------------
// Test a specific codeblock script file (internal)
// -----------------------------------------------------------------------------
static void test_codeblock_file(CuTest *tc, const char* test_file_name,
								const CbTestValue* expected_result)
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
	
	CuAssertIntEquals(tc, expected_result->type, cb_value_get_type(cb->result));
	switch (expected_result->type)
	{
		case CB_VT_BOOLEAN:
			CuAssertIntEquals(tc, expected_result->boolean, cb_boolean_get(cb->result));
			break;
		
		case CB_VT_NUMERIC:
			CuAssertIntEquals(tc, expected_result->value, cb_numeric_get(cb->result));
			break;
		
		case CB_VT_STRING:
			CuAssertStrEquals(tc, expected_result->string, cb_string_get(cb->result));
			break;
		
		case CB_VT_UNDEFINED:	// No value has to be checked here!
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
	
	const CbTestValue* expected = &(test_data->expected_result);
	
	CuAssertIntEquals(tc, expected->type, cb_value_get_type(cb->result));
	switch (expected->type)
	{
		case CB_VT_BOOLEAN:
			CuAssertIntEquals(tc, expected->boolean, cb_boolean_get(cb->result));
			break;
		
		case CB_VT_NUMERIC:
			CuAssertIntEquals(tc, expected->value, cb_numeric_get(cb->result));
			break;
		
		case CB_VT_STRING:
			CuAssertStrEquals(tc, expected->string, cb_string_get(cb->result));
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
	int testcase_count = sizeof(expected_results) / sizeof(CbTestValue);
	
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
