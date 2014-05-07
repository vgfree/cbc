/*******************************************************************************
 * CuTestCustomUtils -- Some useful utilities for the CuTest framework
 ******************************************************************************/

#ifndef CUTESTCUSTOMUTILS_H
#define CUTESTCUSTOMUTILS_H


#include <CuTest.h>


#define CuAssertFalse(tc, cond) CuAssertTrue((tc), !(cond))


#endif // CUTESTCUSTOMUTILS_H
