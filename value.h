/*******************************************************************************
 * CbValue -- Codeblock values and types
 ******************************************************************************/

#ifndef VALUE_H
#define VALUE_H


#include <stdbool.h>

#define CB_BOOLEAN_TRUE_STR		"True"
#define CB_BOOLEAN_FALSE_STR	"False"

// value-types
enum cb_value_type
{
	VT_UNDEFINED,
	VT_NUMERIC,
	VT_BOOLEAN,
	VT_STRING
};

// operation-types
enum cb_operation_type
{
	OPR_ADD,
	OPR_SUB,
	OPR_MUL,
	OPR_DIV
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
typedef int		CbNumeric;
typedef char*	CbString;
typedef bool	CbBoolean;

// codeblock-value structure
typedef struct
{
	// value-type
	enum cb_value_type type;
	
	union
	{
		CbNumeric value;
		CbBoolean boolean;
		CbString string;
	};
} CbValue;

// interface functions
CbValue* cb_value_create();
CbValue* cb_numeric_create(CbNumeric value);
CbValue* cb_boolean_create(CbBoolean boolean);
CbValue* cb_string_create(CbString string);
void cb_value_free(CbValue* val);
bool cb_value_is_type(const CbValue* val, enum cb_value_type type);
void cb_value_assign(const CbValue* source, CbValue* destination);
void cb_value_assign_and_free_source(CbValue* source, CbValue* destination);
CbValue* cb_value_copy(const CbValue* val);
char* cb_value_to_string(const CbValue* val);
void cb_value_print(const CbValue* val);
CbValue* cb_value_compare(enum cb_comparison_type type, CbValue* l, CbValue* r);
CbValue* cb_value_add(CbValue* l, CbValue* r);
// CbNumeric interface functions
CbValue* cb_numeric_compare(enum cb_comparison_type type, const CbValue* l,
							const CbValue* r);
CbValue* cb_numeric_add(CbValue* l, CbValue* r);
CbValue* cb_numeric_sub(CbValue* l, CbValue* r);
CbValue* cb_numeric_mul(CbValue* l, CbValue* r);
CbValue* cb_numeric_div(CbValue* l, CbValue* r);
// CbString interface functions
CbValue* cb_string_compare(enum cb_comparison_type type, const CbValue* l,
						   const CbValue* r);
// CbBoolean interface functions
CbValue* cb_boolean_compare(enum cb_comparison_type type, const CbValue* l,
							const CbValue* r);


#endif // VALUE_H
