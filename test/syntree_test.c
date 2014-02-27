/*******************************************************************************
 * syntree_test -- Testing abstrct syntax tree structure.
 ******************************************************************************/

#include "syntree_test.h"

#include "../codeblock.h"
#include "../syntree.h"
#include "../symref.h"
#include "../funccall.h"
#include "../funcdecl.h"
#include "../error_handling.h"


// #############################################################################
// declarations
// #############################################################################

const char codeblock_string[] =
	"| foo | \n"\
	"function FooBar() \n"\
	"   WriteLn('Foobar'), \n"\
	"end, \n"\
	"foo := 0, \n"\
	"while foo < 10 do \n"\
	"   foo := foo + 1, \n"\
	"end, \n"\
	"foo, \n"\
	"";


// #############################################################################
// test procedures
// #############################################################################

// -----------------------------------------------------------------------------
// Test line_no attribute in all syntree nodes
// -----------------------------------------------------------------------------
void test_syntree_lineno_simple(CuTest *tc)
{
	CbSyntree* s;
	
	// CbSyntree
	s = cb_syntree_create(SNT_STATEMENTLIST, NULL, NULL);
	CuAssertIntEquals(tc, 0, s->line_no);
	free(s);
	
	// CbConstvalNode
	s = cb_constbool_create(1);
	CuAssertIntEquals(tc, 0, s->line_no);
	cb_syntree_free(s);
	
	// CbConstvalNode
	s = cb_conststr_create("");
	CuAssertIntEquals(tc, 0, s->line_no);
	cb_syntree_free(s);
	
	// CbConstvalNode
	s = cb_constval_create(1);
	CuAssertIntEquals(tc, 0, s->line_no);
	cb_syntree_free(s);
	
	// CbFlowNode
	s = cb_flow_create(SNT_FLOW_IF, NULL, NULL, NULL);
	CuAssertIntEquals(tc, 0, s->line_no);
	free(s);
	
	// CbComparisonNode
	s = cb_comparison_create(CMP_EQ, NULL, NULL);
	CuAssertIntEquals(tc, 0, s->line_no);
	free(s);
	
	// CbSymref
	s = cb_symref_create("");
	CuAssertIntEquals(tc, 0, s->line_no);
	cb_syntree_free(s);
	
	// CbFuncCallNode
	s = cb_funccall_create("", NULL);
	CuAssertIntEquals(tc, 0, s->line_no);
	cb_syntree_free(s);
	
	// CbFuncDeclarationNode
	s = cb_funcdecl_create("", NULL, NULL);
	CuAssertIntEquals(tc, 0, s->line_no);
	free(((CbFuncDeclarationNode*) s)->sym_id); free(s);
}


//	| foo |
//	function FooBar()
//	   WriteLn('Foobar'),
//	end,
//	foo := 0,
//	while foo < 10 do
//	   foo := foo + 1,
//	end,
//	foo,

// -----------------------------------------------------------------------------
// Test line_no attribute in a real parsed codeblock
// NOTE: This testcase will currently fail, since the line number detection is
//       not implemented properly yet.
// -----------------------------------------------------------------------------
void test_syntree_lineno_real_codeblock(CuTest *tc)
{
	Codeblock* cb = codeblock_create();
	
	codeblock_parse_string(cb, codeblock_string);
	
	CbSyntree* nodel = cb->ast->l; CuAssertIntEquals(tc, SNT_DECLARATION, nodel->type);
	CbSyntree* noder = cb->ast->r; CuAssertIntEquals(tc, SNT_STATEMENTLIST, noder->type);
	CuAssertIntEquals(tc, 1, nodel->line_no);
	
	nodel = noder->l; CuAssertIntEquals(tc, SNT_FUNC_DECL, nodel->type);
	noder = noder->r; CuAssertIntEquals(tc, SNT_STATEMENTLIST, noder->type);
	CuAssertIntEquals(tc, 2, nodel->line_no);
	CuAssertIntEquals(tc, 3, ((CbFuncDeclarationNode*) nodel)->body->line_no);
	
	nodel = noder->l; CuAssertIntEquals(tc, SNT_ASSIGNMENT, nodel->type);
	noder = noder->r; CuAssertIntEquals(tc, SNT_STATEMENTLIST, noder->type);
	CuAssertIntEquals(tc, 5, nodel->line_no);
	CuAssertIntEquals(tc, 5, nodel->l->line_no);
	CuAssertIntEquals(tc, 5, nodel->r->line_no);
	
	nodel = noder->l; CuAssertIntEquals(tc, SNT_FLOW_WHILE, nodel->type);
	noder = noder->r; CuAssertIntEquals(tc, SNT_SYMREF, noder->type);
	CuAssertIntEquals(tc, 6, nodel->line_no);
	CuAssertIntEquals(tc, 6, ((CbFlowNode*) nodel)->cond->line_no);
	CuAssertIntEquals(tc, 7, ((CbFlowNode*) nodel)->tb->line_no);
	
	CuAssertIntEquals(tc, 9, noder->line_no);
	
	codeblock_free(cb);
}


// #############################################################################
// make suite
// #############################################################################

CuSuite* make_suite_syntree()
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_syntree_lineno_simple);
	SUITE_ADD_TEST(suite, test_syntree_lineno_real_codeblock);
	return suite;
}
