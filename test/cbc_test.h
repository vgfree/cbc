/*******************************************************************************
 * cbc_test -- Codeblock Unit-Test using the 'sput' Unit-Test framework
 ******************************************************************************/

#ifndef CBC_TEST_H
#define CBC_TEST_H


#include <sput.h>

// helper-macros
#define S(x) #x
#define S_(x) S(x)

// custom sput-framework additions
#define sput_assert(expr)			sput_fail_if(!expr, __FILE__ ":" S_(__LINE__))
#define sput_assert_msg(expr, msg)	sput_fail_if(!expr, msg)


#endif // CBC_TEST_H
