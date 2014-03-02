/*******************************************************************************
 * error_handling_test -- Testing error messages, etc.
 ******************************************************************************/

#include "error_handling_test.h"

#include "../error_handling.h"
#include "../codeblock.h"


// #############################################################################
// declarations
// #############################################################################

static const char codeblock_string_undeffunc[] = "UndefinedFunction(),";
static const char codeblock_string_undefvar[] = "cUndefinedVar := 'foobar',";

static void test_runtime_error(CuTest* tc, const char* codeblock_string,
							   const char* expected_error_message);
static void stream_to_string_helper(FILE* stream, char* string);


// #############################################################################
// test procedures
// #############################################################################

// -----------------------------------------------------------------------------
// Generic test procedure to test runtime errors
// -----------------------------------------------------------------------------
static void test_runtime_error(CuTest* tc, const char* codeblock_string,
							   const char* expected_error_message)
{
	Codeblock* cb = codeblock_create();
	
	CuAssertIntEquals(tc, EXIT_SUCCESS,
					  codeblock_parse_string(cb, codeblock_string));
	
	FILE* err_out = tmpfile();
	cb_set_error_output(err_out);
	
	CuAssertIntEquals(tc, EXIT_FAILURE, codeblock_execute(cb));
	
	char stream_content[512];	// allocating 512 bytes should be enough for an
								// error message
	stream_to_string_helper(err_out, stream_content);
	
	CuAssertStrEquals(tc, expected_error_message, stream_content);
	
	fclose(err_out);
	
	codeblock_free(cb);
}

// -----------------------------------------------------------------------------
// Test the "Undefined symbol" runtime error
// -----------------------------------------------------------------------------
void test_error_handling_undefinedsymbol(CuTest *tc)
{
	test_runtime_error(tc, codeblock_string_undeffunc,
					   "Runtime error: Line 1: Undefined symbol: UndefinedFunction\n");
	test_runtime_error(tc, codeblock_string_undefvar,
					   "Runtime error: Line 1: Undefined symbol: cUndefinedVar\n");
}


// #############################################################################
// make suite
// #############################################################################

CuSuite* make_suite_error_handling()
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_error_handling_undefinedsymbol);
	return suite;
}


// #############################################################################
// make suite
// #############################################################################

// -----------------------------------------------------------------------------
// Copy content of a stream to a string
// -----------------------------------------------------------------------------
static void stream_to_string_helper(FILE* stream, char* string)
{
	fseek(stream, 0, SEEK_SET);	// got to beginning of the file
	while (!feof(stream))
	{
		*string = fgetc(stream);
		if (*string == EOF)
			*string = '\0';	// terminate string
		else
			string++;
	}
}
