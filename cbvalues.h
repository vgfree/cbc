/*******************************************************************************
 * codeblock values and types
 ******************************************************************************/

#ifndef CBVALUES_H
#define CBVALUES_H

#include <stdbool.h>

// value-types
enum valuetype_t
{
	VT_UNDEFINED,
	VT_NUMERIC,
	VT_STRING
};

// comparison types
enum comparisontype_t
{
	CMP_EQ,	// equal
	CMP_NE,	// not equal
	CMP_GE,	// greater or equal
	CMP_LE,	// lower or equal
	CMP_GT,	// greater than
	CMP_LT,	// lower than
};

// definition of types
typedef int		cbnumeric;
typedef char*	cbstring;

// codeblock-value structure
typedef struct
{
	// value-type
	enum valuetype_t type;
	
	union
	{
		cbnumeric value;
		cbstring string;
	};
} cbvalue;

// interface functions
cbvalue* cbvalue_create();
cbvalue* cbnumeric_create(cbnumeric value);
cbvalue* cbstring_create(cbstring string);
void cbvalue_free(cbvalue* val);
bool cbvalue_istype(enum valuetype_t type, cbvalue* val);
void cbvalue_assign(cbvalue* source, cbvalue* destination);
void cbvalue_assign_freesource(cbvalue* source, cbvalue* destination);
cbvalue* cbvalue_copy(cbvalue* val);
char* cbvalue_tostring(cbvalue* val);
void cbvalue_print(cbvalue* val);
// cbnumeric interface functions
cbvalue* cbnumeric_compare(enum comparisontype_t type, cbvalue* l, cbvalue* r);
cbvalue* cbnumeric_add(cbvalue* l, cbvalue* r);
cbvalue* cbnumeric_sub(cbvalue* l, cbvalue* r);
cbvalue* cbnumeric_mul(cbvalue* l, cbvalue* r);
cbvalue* cbnumeric_div(cbvalue* l, cbvalue* r);

#endif // CBVALUES_H
