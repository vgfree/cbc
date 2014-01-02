/*******************************************************************************
 * codeblock values and types
 ******************************************************************************/

#ifndef CBVALUES_H
#define CBVALUES_H

#include <stdbool.h>

#define CB_BOOLEAN_TRUE_STR		"True"
#define CB_BOOLEAN_FALSE_STR	"False"

// value-types
enum valuetype_t
{
	VT_UNDEFINED,
	VT_NUMERIC,
	VT_BOOLEAN,
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
typedef bool	cbboolean;

// codeblock-value structure
typedef struct
{
	// value-type
	enum valuetype_t type;
	
	union
	{
		cbnumeric value;
		cbboolean boolean;
		cbstring string;
	};
} cbvalue;

// interface functions
cbvalue* cbvalue_create();
cbvalue* cbnumeric_create(cbnumeric value);
cbvalue* cbboolean_create(cbboolean boolean);
cbvalue* cbstring_create(cbstring string);
void cbvalue_free(cbvalue* val);
bool cbvalue_istype(enum valuetype_t type, cbvalue* val);
void cbvalue_assign(cbvalue* source, cbvalue* destination);
void cbvalue_assign_freesource(cbvalue* source, cbvalue* destination);
cbvalue* cbvalue_copy(cbvalue* val);
char* cbvalue_tostring(cbvalue* val);
void cbvalue_print(cbvalue* val);
cbvalue* cbvalue_compare(enum comparisontype_t type, cbvalue* l, cbvalue* r);
// cbnumeric interface functions
cbvalue* cbnumeric_compare(enum comparisontype_t type, cbvalue* l, cbvalue* r);
cbvalue* cbnumeric_add(cbvalue* l, cbvalue* r);
cbvalue* cbnumeric_sub(cbvalue* l, cbvalue* r);
cbvalue* cbnumeric_mul(cbvalue* l, cbvalue* r);
cbvalue* cbnumeric_div(cbvalue* l, cbvalue* r);
// cbstring interface functions
cbvalue* cbstring_compare(enum comparisontype_t type, cbvalue* l, cbvalue* r);
// cbboolean interface functions
cbvalue* cbboolean_compare(enum comparisontype_t type, cbvalue* l, cbvalue* r);

#endif // CBVALUES_H
