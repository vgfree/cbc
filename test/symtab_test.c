/*******************************************************************************
 * symtab_test -- Testing the symbol-table structure
 ******************************************************************************/

#include "symtab_test.h"

#include "../symtab.h"
#include "../symbol.h"

// #############################################################################
// test procedures
// #############################################################################

// -----------------------------------------------------------------------------
// Test: test_symtab()
// -----------------------------------------------------------------------------
void test_symtab(CuTest* tc)
{
	symtab_t* symtab = symtab_create();
	
	
	CuAssertIntEquals(tc, 0, symtab->size);
	CuAssertPtrEquals(tc, NULL, symtab->first);
	CuAssertPtrEquals(tc, NULL, symtab->current);
	CuAssertPtrEquals(tc, NULL, symtab->last);
	CuAssertPtrNotNull(tc, symtab->scope_stack);
	
	// append variable
	symbol_t* s = symbol_create();
	symbol_setid(s, "test_symbol");
	symbol_settype(s, SYM_TYPE_VARIABLE);
	symtab_append(symtab, s);
	
	CuAssertIntEquals(tc, 1, symtab->size);
	CuAssertPtrEquals(tc, s, symtab->first);
	CuAssertPtrEquals(tc, s, symtab->current);
	CuAssertPtrEquals(tc, s, symtab->last);
	
	// append second variable
	symbol_t* s2 = symbol_create();
	symbol_setid(s2, "test_symbol_2");
	symbol_settype(s2, SYM_TYPE_VARIABLE);
	symtab_append(symtab, s2);
	
	CuAssertIntEquals(tc, 2, symtab->size);
	CuAssertPtrEquals(tc, s, symtab->first);
	CuAssertPtrEquals(tc, s, symtab->current);
	CuAssertPtrEquals(tc, s2, symtab->last);
	
	// append third variable
	symbol_t* s3 = symbol_create();
	symbol_setid(s3, "test_symbol_3");
	symbol_settype(s3, SYM_TYPE_VARIABLE);
	symtab_append(symtab, s3);
	
	CuAssertIntEquals(tc, 3, symtab->size);
	CuAssertPtrEquals(tc, s, symtab->first);
	CuAssertPtrEquals(tc, s, symtab->current);
	CuAssertPtrEquals(tc, s3, symtab->last);
	
	// test navigation: next
	CuAssertPtrEquals(tc, s2, symtab_next(symtab));
	CuAssertPtrEquals(tc, s2, symtab->current);
	CuAssertPtrEquals(tc, s3, symtab_next(symtab));
	CuAssertPtrEquals(tc, s3, symtab->current);
	CuAssertPtrEquals(tc, NULL, symtab_next(symtab));
	CuAssertPtrEquals(tc, s3, symtab->current);
	// previous
	CuAssertPtrEquals(tc, s2, symtab_previous(symtab));
	CuAssertPtrEquals(tc, s2, symtab->current);
	CuAssertPtrEquals(tc, s, symtab_previous(symtab));
	CuAssertPtrEquals(tc, s, symtab->current);
	CuAssertPtrEquals(tc, NULL, symtab_previous(symtab));
	CuAssertPtrEquals(tc, s, symtab->current);
	
	// dispatch
	symbol_t* temp = symtab_dispatch(symtab, "test_symbol_2");
	CuAssertPtrEquals(tc, s2, temp);
	CuAssertPtrEquals(tc, s, symtab->first);
	CuAssertPtrEquals(tc, s3, symtab->last);
	CuAssertPtrEquals(tc, s, symtab->current);
	CuAssertIntEquals(tc, 2, symtab->size);
	
	symtab_append(symtab, temp);
	CuAssertPtrEquals(tc, s2, symtab->last);
	CuAssertIntEquals(tc, 3, symtab->size);
	
	// remove
	symtab_remove(symtab, "test_symbol_3");
	CuAssertIntEquals(tc, 2, symtab->size);
	
	
	symtab_free(symtab);
}

// -----------------------------------------------------------------------------
// Test: test_symtab_scope_stack()
// -----------------------------------------------------------------------------
void test_symtab_scope_stack(CuTest* tc)
{
	symtab_t* symtab = symtab_create();
	
	CuAssertIntEquals(tc, 0, symtab->scope_stack->count);
	CuAssertPtrEquals(tc, NULL, symtab->scope_stack->top);
	
	// declare symbol in global scope
	symbol_t* sym = symbol_create();
	symbol_setid(sym, "symbol_global");
	symbol_settype(sym, SYM_TYPE_VARIABLE);
	symtab_append(symtab, sym);
	
	// find global symbol
	CuAssertPtrEquals(tc, sym, symtab_lookup(symtab, "symbol_global"));
	
	int i = 0;
	for (; i < 10; i++)
	{
		CuString* context	= CuStringNew();
		CuStringAppendFormat(context, "scope_%d", i);
		CuString* sym_id	= CuStringNew();
		CuStringAppendFormat(sym_id, "symbol_%d", i);
		
		CuString* old_sym_id = CuStringNew();
		if (i > 1)
			CuStringAppendFormat(old_sym_id, "symbol_%d", i - 1);
		
		symtab_enter_scope(symtab, context->buffer);	// enter new scope
		
		CuAssertIntEquals(tc, (i + 1), symtab->scope_stack->count);
		CuAssertPtrNotNull(tc, symtab->scope_stack->top);
		
		// global symbol always has to be found in any scope
		CuAssertPtrEquals(tc, sym, symtab_lookup(symtab, "symbol_global"));
		
		symbol_t* s = symbol_create();
		symbol_setid(s, sym_id->buffer);
		symbol_settype(s, SYM_TYPE_VARIABLE);
		symtab_append(symtab, s);	// declare symbol within new scope
		
		// find new symbol
		CuAssertPtrEquals(tc, s, symtab_lookup(symtab, sym_id->buffer));
		if (i > 1)
			// try to find symbol different scope -> must return NULL!
			CuAssertPtrEquals(tc, NULL, symtab_lookup(symtab, old_sym_id->buffer));
		
		CuStringDelete(old_sym_id);
		CuStringDelete(sym_id);
		CuStringDelete(context);
	}
	
	for (; i > 0; i--)
	{
		symtab_leave_scope(symtab);
		CuAssertIntEquals(tc, (i - 1), symtab->scope_stack->count);
	}
	
	CuAssertIntEquals(tc, 0, symtab->scope_stack->count);
	CuAssertPtrEquals(tc, NULL, symtab->scope_stack->top);
	
	symtab_free(symtab);
}


// #############################################################################
// make suite
// #############################################################################

CuSuite* make_suite_symtab()
{
	CuSuite* suite = CuSuiteNew();
	SUITE_ADD_TEST(suite, test_symtab);
	SUITE_ADD_TEST(suite, test_symtab_scope_stack);
	return suite;
}
