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
static const char codeblock_string_unexptokennum[] = "WriteLn('foo' 123, True),";
static const char codeblock_string_missingcomma[] = "WriteLn('foo')";

static void test_error(CuTest* tc, const char* codeblock_string,
					   const char* expected_error_message, cb_error_type type);
static void stream_to_string_helper(FILE* stream, char* string, char read_until);


// #############################################################################
// test procedures
// #############################################################################

// -----------------------------------------------------------------------------
// Generic test procedure to test errors
// -----------------------------------------------------------------------------
static void test_error(CuTest* tc, const char* codeblock_string,
					   const char* expected_error_message, cb_error_type type)
{
	FILE* err_out = tmpfile();
	cb_set_error_output(err_out);
	
	Codeblock* cb = codeblock_create();
	
	int expected_parsing_result = EXIT_SUCCESS;
	if (type == CB_ERR_SYNTAX)
		expected_parsing_result = EXIT_FAILURE;
	
	CuAssertIntEquals(tc, expected_parsing_result,
					  codeblock_parse_string(cb, codeblock_string));
	
	if (type != CB_ERR_SYNTAX)
		CuAssertIntEquals(tc, EXIT_FAILURE, codeblock_execute(cb));
	
	char stream_content[512];	// allocating 512 bytes should be enough for an
								// error message
	// copy first line in the stream to the string buffer
	stream_to_string_helper(err_out, stream_content, '\n');
	
	CuAssertStrEquals(tc, expected_error_message, stream_content);
	
	fclose(err_out),
	codeblock_free(cb);
}

// -----------------------------------------------------------------------------
// Test syntax errors
// -----------------------------------------------------------------------------
void test_error_handling_syntax_errors(CuTest *tc)
{
	test_error(tc, codeblock_string_unexptokennum,
			   "Syntax error: Line 1: Unexpected token NUMBER",
			   CB_ERR_SYNTAX);
	test_error(tc, codeblock_string_missingcomma,
			   "Syntax error: Line 1: Unexpected token ENDOFFILE",
			   CB_ERR_SYNTAX);
}

// -----------------------------------------------------------------------------
// Test the "Undefined symbol" runtime error
// -----------------------------------------------------------------------------
void test_error_handling_undefinedsymbol(CuTest *tc)
{
	test_error(tc, codeblock_string_undeffunc,
			   "Runtime error: Line 1: Undefined symbol: UndefinedFunction",
			   CB_ERR_RUNTIME);
	test_error(tc, codeblock_string_undefvar,
			   "Runtime error: Line 1: Undefined symbol: cUndefinedVar",
			   CB_ERR_RUNTIME);
}


// #############################################################################
// make suite
// #############################################################################

CuSuite* make_suite_error_handling()
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_error_handling_syntax_errors);
	SUITE_ADD_TEST(suite, test_error_handling_undefinedsymbol);
	return suite;
}


// #############################################################################
// make suite
// #############################################################################

// -----------------------------------------------------------------------------
// Copy content of a stream to a string
// -----------------------------------------------------------------------------
static void stream_to_string_helper(FILE* stream, char* string, char read_until)
{
	fseek(stream, 0, SEEK_SET);	// got to beginning of the file
	while (!feof(stream))
	{
		*string = fgetc(stream);
		if (*string == read_until)
		{
			*string = '\0';	// terminate string
			break;
		}
		else
			string++;
	}
}
