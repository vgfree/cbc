/*******************************************************************************
 * CbValue -- Codeblock values and types
 ******************************************************************************/

#ifndef VALUE_H
#define VALUE_H


#include <stdbool.h>
#include "array.h"

#define CB_BOOLEAN_TRUE_STR		"True"
#define CB_BOOLEAN_FALSE_STR	"False"

// value-types
enum cb_value_type
{
	CB_VT_UNDEFINED,
	CB_VT_NUMERIC,
	CB_VT_BOOLEAN,
	CB_VT_STRING,
	CB_VT_VALARRAY
};

// operation-types
enum cb_operation_type
{
	OPR_ADD,
	OPR_SUB,
	OPR_MUL,
	OPR_DIV,
	OPR_AND,
	OPR_OR,
	OPR_NOT
};

// comparison types
enum cb_comparison_type
{
	CMP_EQ,	// equal
	CMP_NE,	// not equal
	CMP_GE,	// greater or equal
	CMP_LE,	// lower or equal
	CMP_GT,	// greater than
	CMP_LT	// lower than
};

// definition of codeblock-types
typedef int             CbNumeric;
typedef char*           CbString;
typedef bool            CbBoolean;
typedef struct CbArray* CbValArray;

// codeblock-value structure
typedef struct CbValue CbValue;

// interface functions
CbValue* cb_value_create();
CbValue* cb_numeric_create(CbNumeric value);
CbValue* cb_boolean_create(CbBoolean boolean);
CbValue* cb_string_create(CbString string);
CbValue* cb_valarray_create(CbValArray array);
void cb_value_free(CbValue* val);

enum cb_value_type cb_value_get_type(const CbValue* val);
bool cb_value_is_type(const CbValue* val, enum cb_value_type type);
void cb_value_assign(const CbValue* source, CbValue* destination);
void cb_value_assign_and_free_source(CbValue* source, CbValue* destination);
CbValue* cb_value_copy(const CbValue* val);
char* cb_value_to_string(const CbValue* val);
void cb_value_print(const CbValue* val);

// CbNumeric interface functions
CbNumeric cb_numeric_get(const CbValue* val);
void cb_numeric_set(CbValue* val, CbNumeric value);
CbValue* cb_numeric_compare(enum cb_comparison_type type, const CbValue* l,
							const CbValue* r);
CbValue* cb_numeric_add(CbValue* l, CbValue* r);
CbValue* cb_numeric_sub(CbValue* l, CbValue* r);
CbValue* cb_numeric_mul(CbValue* l, CbValue* r);
CbValue* cb_numeric_div(CbValue* l, CbValue* r);
CbValue* cb_numeric_and(CbValue* l, CbValue* r);
CbValue* cb_numeric_or(CbValue* l, CbValue* r);
CbValue* cb_numeric_not(CbValue* operand);

// CbString interface functions
CbString cb_string_get(const CbValue* val);
CbValue* cb_string_compare(enum cb_comparison_type type, const CbValue* l,
						   const CbValue* r);
CbValue* cb_string_concat(CbValue* l, CbValue* r);

// CbBoolean interface functions
CbBoolean cb_boolean_get(const CbValue* val);
CbValue* cb_boolean_compare(enum cb_comparison_type type, const CbValue* l,
							const CbValue* r);
CbValue* cb_boolean_and(CbValue* l, CbValue* r);
CbValue* cb_boolean_or(CbValue* l, CbValue* r);
CbValue* cb_boolean_not(CbValue* operand);

// CbValArray interface functions
const CbValArray cb_valarray_get(const CbValue* val);


#endif // VALUE_H
