/*******************************************************************************
 * value_t -- Codeblock values and types
 ******************************************************************************/

#ifndef VALUE_H
#define VALUE_H


#include <stdbool.h>

#define CB_BOOLEAN_TRUE_STR		"True"
#define CB_BOOLEAN_FALSE_STR	"False"

// value-types
enum value_type_t
{
	VT_UNDEFINED,
	VT_NUMERIC,
	VT_BOOLEAN,
	VT_STRING
};

// comparison types
enum comparison_type_t
{
	CMP_EQ,	// equal
	CMP_NE,	// not equal
	CMP_GE,	// greater or equal
	CMP_LE,	// lower or equal
	CMP_GT,	// greater than
	CMP_LT,	// lower than
};

// definition of codeblock-types
typedef int		cbnumeric;
typedef char*	cbstring;
typedef bool	cbboolean;

// codeblock-value structure
typedef struct
{
	// value-type
	enum value_type_t type;
	
	union
	{
		cbnumeric value;
		cbboolean boolean;
		cbstring string;
	};
} value_t;

// interface functions
value_t* value_create();
value_t* cbnumeric_create(cbnumeric value);
value_t* cbboolean_create(cbboolean boolean);
value_t* cbstring_create(cbstring string);
void value_free(value_t* val);
bool value_istype(value_t* val, enum value_type_t type);
void value_assign(value_t* source, value_t* destination);
void value_assign_freesource(value_t* source, value_t* destination);
value_t* value_copy(value_t* val);
char* value_tostring(value_t* val);
void value_print(value_t* val);
value_t* value_compare(enum comparison_type_t type, value_t* l, value_t* r);
// cbnumeric interface functions
value_t* cbnumeric_compare(enum comparison_type_t type, value_t* l, value_t* r);
value_t* cbnumeric_add(value_t* l, value_t* r);
value_t* cbnumeric_sub(value_t* l, value_t* r);
value_t* cbnumeric_mul(value_t* l, value_t* r);
value_t* cbnumeric_div(value_t* l, value_t* r);
// cbstring interface functions
value_t* cbstring_compare(enum comparison_type_t type, value_t* l, value_t* r);
// cbboolean interface functions
value_t* cbboolean_compare(enum comparison_type_t type, value_t* l, value_t* r);


#endif // VALUE_H
