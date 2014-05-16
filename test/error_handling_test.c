/*******************************************************************************
 * error_handling_test -- Testing error messages, etc.
 ******************************************************************************/

#include <CuTest.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include "../error_handling.h"
#include "../error_messages.h"
#include "../codeblock.h"


// #############################################################################
// declarations
// #############################################################################

static const char cbstr_undeffunc[]     = "UndefinedFunction(),";
static const char cbstr_undefvar[]      = "cUndefinedVar := 'foobar',";
static const char cbstr_unexptokennum[] = "WriteLn('foo' 123, True),";
static const char cbstr_missingcomma[]  = "WriteLn('foo')";
static const char cbstr_redeclvar[]     = "| foo, foo | foo := 123,";
static const char cbstr_redeclfunc[]    = "function Foo() 1, end, function Foo() 1, end,";
static const char cbstr_paramcount1[]   = "function Foo(p) 1, end, Foo('a', 1, True),";
static const char cbstr_paramcount2[]   = "function Foo() 1, end, Foo(1),";
static const char cbstr_paramcount3[]   = "function Foo(p1,p2,p3) 1, end, Foo(1, '2'),";
static const char cbstr_exception_block1[] =
	"| foo, cMessage |"\
	"startseq"\
	"	startseq"\
	"		foo := 0,"\
	"		startseq"\
	"			SetError('Raising error (1)'),"\
	"		onerror"\
	"			foo      := 1,"\
	"			cMessage := GetErrorText(),"\
	"		stopseq,"\
	"		SetError('Raising error (2)'),"\
	"		foo := foo + 100,"\
	"	always"\
	"		foo      := foo + 1,"\
	"		cMessage := cMessage + ' / ' + GetErrorText(),"\
	"	stopseq,"\
	"	foo := foo + 100,"\
	"onerror"\
	"	foo := foo + 1,"\
	"stopseq,"\
	"cMessage := cMessage + ' / ' + Str(foo),"\
	"cMessage,";
static const char cbstr_exception_block2[] =
	"| foo |"\
	"foo := 5,"\
	"startseq"\
	"	foo := foo / 0,"\
	"always"\
	"	foo := 100,"\
	"stopseq,";
static const char cbstr_exception_block3[] =
	"| foo |"\
	"foo := 123,"\
	"startseq"\
	"   foo := foo / 0,"\
	"onerror"\
	"   1,"\
	"stopseq,"\
	"foo,";

static void test_error(CuTest* tc, const char* codeblock_string,
					   const char* expected_error_message, cb_error_type type);
static void test_error_and_result(CuTest* tc, const char* codeblock_string,
								  const char* expected_error_message,
								  CbValue* expected_result);
static void stream_to_string_helper(FILE* stream, char* string, bool trim);


// #############################################################################
// generic test procedures
// #############################################################################

// -----------------------------------------------------------------------------
// Generic test procedure to test errors (internal)
// -----------------------------------------------------------------------------
static void test_error_internal(CuTest* tc,
							    const char* codeblock_string,
								const char* expected_error_message,
								cb_error_type type,
								CbValue* expected_result,
								bool check_result)
{
	FILE* err_out = tmpfile();
	cb_set_error_output(err_out);
	
	Codeblock* cb = codeblock_create();
	
	int expected_parsing_result = EXIT_SUCCESS;
	bool expecting_parser_error = !check_result && type == CB_ERR_SYNTAX;
	if (expecting_parser_error)
		expected_parsing_result = EXIT_FAILURE;
	
	CuAssertIntEquals(tc, expected_parsing_result,
					  codeblock_parse_string(cb, codeblock_string));
	
	if (!expecting_parser_error)
	{
		int expected_execution_result = EXIT_SUCCESS;
		if (!check_result)
			expected_execution_result = EXIT_FAILURE;
		
		CuAssertIntEquals(tc, expected_execution_result, codeblock_execute(cb));
	}
	
	if (strlen(expected_error_message) > 0)	// is there even an error message?
	{
		char stream_content[512];	// allocating 512 bytes should be enough for an
									// error message
		memset(stream_content, '\0', 512);	// clear memory
		// copy stream content into string buffer
		stream_to_string_helper(err_out, stream_content, true);
		
		CuAssertStrEquals(tc, expected_error_message, stream_content);
	}
	
	if (check_result)
	{
		CuAssertPtrNotNull(tc, cb->result);
		// check execution result type
		CuAssertIntEquals(tc, cb_value_get_type(expected_result), cb_value_get_type(cb->result));
		// check execution result value
		switch (cb_value_get_type(cb->result))
		{
			case CB_VT_BOOLEAN:
				CuAssertIntEquals(tc, cb_boolean_get(expected_result), cb_boolean_get(cb->result));
				break;
			
			case CB_VT_NUMERIC:
				CuAssertIntEquals(tc, cb_numeric_get(expected_result), cb_numeric_get(cb->result));
				break;
			
			case CB_VT_STRING:
				CuAssertStrEquals(tc, cb_string_get(expected_result), cb_string_get(cb->result));
				break;
			
			default:
				CuAssertTrue(tc, false);
				break;
		}
	}
	
	fclose(err_out),
	codeblock_free(cb);
}

// -----------------------------------------------------------------------------
// Generic test procedure to test errors
// -----------------------------------------------------------------------------
static void test_error(CuTest* tc, const char* codeblock_string,
					   const char* expected_error_message, cb_error_type type)
{
	test_error_internal(tc, codeblock_string, expected_error_message, type,
						NULL, false);
	return;
}

// -----------------------------------------------------------------------------
// Generic test procedure to test errors and codeblock result
// -----------------------------------------------------------------------------
static void test_error_and_result(CuTest* tc, const char* codeblock_string,
								  const char* expected_error_message,
								  CbValue* expected_result)
{
	test_error_internal(tc, codeblock_string, expected_error_message, 0,
						expected_result, true);
}


// #############################################################################
// test procedures
// #############################################################################

// -----------------------------------------------------------------------------
// Test syntax errors
// -----------------------------------------------------------------------------
void test_error_handling_syntax_errors(CuTest *tc)
{
	test_error(tc, cbstr_unexptokennum,
			   "Syntax error: Line 1: Unexpected token NUMBER\n"\
			   "Error: Parsing failed due to invalid input",
			   CB_ERR_SYNTAX);
	test_error(tc, cbstr_missingcomma,
			   "Syntax error: Line 1: Unexpected token ENDOFFILE\n"\
			   "Error: Parsing failed due to invalid input",
			   CB_ERR_SYNTAX);
}

// -----------------------------------------------------------------------------
// Test the "Undefined symbol" runtime error
// -----------------------------------------------------------------------------
void test_error_handling_undefinedsymbol(CuTest *tc)
{
	test_error(tc, cbstr_undeffunc,
			   "Runtime error: Line 1: Undefined symbol: UndefinedFunction",
			   CB_ERR_RUNTIME);
	test_error(tc, cbstr_undefvar,
			   "Runtime error: Line 1: Undefined symbol: cUndefinedVar",
			   CB_ERR_RUNTIME);
}

// -----------------------------------------------------------------------------
// Test the "Cannot redeclare symbol ..." runtime error
// -----------------------------------------------------------------------------
void test_error_handling_symbolredecl(CuTest *tc)
{
	test_error(tc, cbstr_redeclvar,
			   "Runtime error: Cannot redeclare symbol: foo",
			   CB_ERR_RUNTIME);
	test_error(tc, cbstr_redeclfunc,
			   "Runtime error: Cannot redeclare symbol: Foo",
			   CB_ERR_RUNTIME);
}

// -----------------------------------------------------------------------------
// Test the param count mismatch runtime error
// -----------------------------------------------------------------------------
void test_error_handling_paramcount(CuTest *tc)
{
	test_error(tc, cbstr_paramcount1,
			   "Runtime error: In function `Foo': Expecting 1 argument, "\
			   "but 3 were actually passed",
			   CB_ERR_RUNTIME);
	test_error(tc, cbstr_paramcount2,
			   "Runtime error: In function `Foo': Expecting 0 arguments, "\
			   "but 1 was actually passed",
			   CB_ERR_RUNTIME);
	test_error(tc, cbstr_paramcount3,
			   "Runtime error: In function `Foo': Expecting 3 arguments, "\
			   "but 2 were actually passed",
			   CB_ERR_RUNTIME);
}

// -----------------------------------------------------------------------------
// Test global error flag
// -----------------------------------------------------------------------------
void test_error_global_flag(CuTest *tc)
{
	cb_error_handling_initialize();
	CuAssertIntEquals(tc, true, cb_error_handling_is_initialized());
	
	cb_error_set(1);
	CuAssertIntEquals(tc, 1, cb_error_get());
	CuAssertTrue(tc, cb_error_is_set());
	cb_error_set(5);
	CuAssertIntEquals(tc, 5, cb_error_get());
	CuAssertTrue(tc, cb_error_is_set());
	cb_error_catch();
	CuAssertTrue(tc, cb_error_is_catched());
	cb_error_reset_catch();
	CuAssertTrue(tc, !cb_error_is_catched());
	cb_error_set(0);
	CuAssertIntEquals(tc, 0, cb_error_get());
	CuAssertTrue(tc, !cb_error_is_set());
	cb_error_set(-2);
	CuAssertIntEquals(tc, -2, cb_error_get());
	CuAssertTrue(tc, cb_error_is_set());
	cb_error_clear();
	CuAssertIntEquals(tc, 0, cb_error_get());
	CuAssertTrue(tc, !cb_error_is_set());
	
	cb_error_handling_finalize();
	CuAssertIntEquals(tc, false, cb_error_handling_is_initialized());
}

// -----------------------------------------------------------------------------
// Test exception blocks
// -----------------------------------------------------------------------------
void test_exception_blocks(CuTest *tc)
{
	// Test 1
	CbValue* expected_value = cb_string_create(strdup("Raising error (1) / "\
													  "Raising error (2) / 3"));
	test_error_and_result(tc, cbstr_exception_block1,
						  "",
						  expected_value);
	cb_value_free(expected_value);
	
	// Test 2
	test_error(tc, cbstr_exception_block2, "Error: Division by zero is not allowed",
	           CB_ERR_RUNTIME);
	
	// Test 3
	expected_value = cb_numeric_create(123);
	test_error_and_result(tc, cbstr_exception_block3, "", expected_value);
	cb_value_free(expected_value);
}

void test_constant_error_messages(CuTest *tc)
{
	CuAssertStrEquals(	tc, "No error",
						cb_error_get_message_by_code(CB_ERR_CODE_NOERROR));
	CuAssertStrEquals(	tc, "Division by zero is not allowed",
						cb_error_get_message_by_code(CB_ERR_CODE_DIVISIONBYZERO));
	CuAssertStrEquals(	tc, "Unknown error",
						cb_error_get_message_by_code(CB_ERR_CODE_END));
	CuAssertStrEquals(	tc, "Unknown error",
						cb_error_get_message_by_code(CB_ERR_CODE_END + 1));
}


// #############################################################################
// make suite
// #############################################################################

CuSuite* make_suite_error_handling()
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_error_handling_syntax_errors);
	SUITE_ADD_TEST(suite, test_error_handling_undefinedsymbol);
	SUITE_ADD_TEST(suite, test_error_handling_symbolredecl);
	SUITE_ADD_TEST(suite, test_error_handling_paramcount);
	SUITE_ADD_TEST(suite, test_error_global_flag);
	SUITE_ADD_TEST(suite, test_exception_blocks);
	SUITE_ADD_TEST(suite, test_constant_error_messages);
	return suite;
}


// #############################################################################
// internal functions
// #############################################################################

// -----------------------------------------------------------------------------
// Copy content of a stream to a string
// -----------------------------------------------------------------------------
static void stream_to_string_helper(FILE* stream, char* string, bool trim)
{
	fseek(stream, 0, SEEK_SET);	// go to beginning of the file
	
	while (!feof(stream))
	{
		*string = fgetc(stream);
		
		if (feof(stream))
			*string = '\0';
		else
			string++;
	}
	
	if (trim)	// trim string
		while (!isprint(*string))
		{
			*string = '\0';
			string--;
		}
}
