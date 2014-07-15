/*******************************************************************************
 * CbValue -- Codeblock values and types
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "value.h"
#include "array.h"
#include "error_handling.h"
#include "error_messages.h"

#define NO_VALUE_AS_STRING "<no value returned>"


// #############################################################################
// declarations
// #############################################################################

struct CbValue
{
	// value-type
	enum cb_value_type type;
	
	union
	{
		CbNumeric value;
		CbBoolean boolean;
		CbString string;
		CbArray* array;
	};
};


static CbValue* cb_numeric_operation(enum cb_operation_type type, CbValue* l,
									 CbValue* r);
static CbValue* cb_boolean_operation(enum cb_operation_type type, CbValue* l,
									 CbValue* r);


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// create a codeblock-value
// -----------------------------------------------------------------------------
CbValue* cb_value_create()
{
	CbValue* val= (CbValue*) malloc(sizeof(CbValue));
	val->type	= CB_VT_UNDEFINED;
}

// -----------------------------------------------------------------------------
// create a numeric value
// -----------------------------------------------------------------------------
CbValue* cb_numeric_create(CbNumeric value)
{
	CbValue* val= cb_value_create();
	val->type	= CB_VT_NUMERIC;
	val->value	= value;
}

// -----------------------------------------------------------------------------
// create a boolean value
// -----------------------------------------------------------------------------
CbValue* cb_boolean_create(CbBoolean boolean)
{
	CbValue* val= cb_value_create();
	val->type	= CB_VT_BOOLEAN;
	val->boolean= boolean;
}

// -----------------------------------------------------------------------------
// create a string-value
// -----------------------------------------------------------------------------
CbValue* cb_string_create(CbString string)
{
	CbValue* val= cb_value_create();
	val->type	= CB_VT_STRING;
	val->string	= string;
}

// -----------------------------------------------------------------------------
// create an array
// -----------------------------------------------------------------------------
CbValue* cb_valarray_create(CbValArray array)
{
	CbValue* valarray = cb_value_create();
	valarray->type    = CB_VT_VALARRAY;
	valarray->array   = array;
}

// -----------------------------------------------------------------------------
// free codeblock-value
// -----------------------------------------------------------------------------
void cb_value_free(CbValue* val)
{
	if (val->type == CB_VT_STRING && val->string)
		free(val->string);
	else if (val->type == CB_VT_VALARRAY && val->array)
		cb_array_free(val->array);
	
	free(val);
}

// -----------------------------------------------------------------------------
// get value-type
// -----------------------------------------------------------------------------
enum cb_value_type cb_value_get_type(const CbValue* val)
{
	return val->type;
}

// -----------------------------------------------------------------------------
// check for given value-type
// -----------------------------------------------------------------------------
bool cb_value_is_type(const CbValue* val, enum cb_value_type type)
{
	if (val->type == type)
		return true;
	else
		return false;
}

// -----------------------------------------------------------------------------
// assign attributes from one codeblock-value struct to another
// -----------------------------------------------------------------------------
void cb_value_assign(const CbValue* source, CbValue* destination)
{
	switch (source->type)
	{
		case CB_VT_NUMERIC:
			destination->value = source->value;
			break;
		
		case CB_VT_BOOLEAN:
			destination->boolean = source->boolean;
			break;
		
		case CB_VT_STRING:
			// free old string
			if (destination->type == CB_VT_STRING && destination->string)
				free(destination->string);
			// assign new one
			destination->string = strdup(source->string);
			break;
		
		case CB_VT_VALARRAY:
			// free old array
			if (destination->type == CB_VT_VALARRAY && destination->array)
				cb_array_free(destination->array);
			// assign new one
			destination->array = cb_array_copy(source->array);
			break;
	}
	
	destination->type = source->type;
}

// -----------------------------------------------------------------------------
// assign attributes from one codeblock-value struct to another and free the
// source struct
// -----------------------------------------------------------------------------
void cb_value_assign_and_free_source(CbValue* source, CbValue* destination)
{
	cb_value_assign(source, destination);
	cb_value_free(source);
}

// -----------------------------------------------------------------------------
// copy a codeblock-value struct
// -----------------------------------------------------------------------------
CbValue* cb_value_copy(const CbValue* val)
{
	CbValue* copy = cb_value_create();
	cb_value_assign(val, copy);
	
	return copy;
}

// -----------------------------------------------------------------------------
// convert a codeblock-value into a string
// IMPORTANT:	the c-string must be freed after usage, since allocation occurs
//				in this function!
// -----------------------------------------------------------------------------
char* cb_value_to_string(const CbValue* val)
{
	assert(val);
	
	char* result_buf;
	
	switch (val->type)
	{
		case CB_VT_NUMERIC:
			// allocate 128 bytes buffer
			result_buf = malloc(128);
			sprintf(result_buf, "%d", val->value);
			break;
		
		case CB_VT_BOOLEAN:
			if (val->boolean)
				result_buf = strdup(CB_BOOLEAN_TRUE_STR);
			else
				result_buf = strdup(CB_BOOLEAN_FALSE_STR);
			
			break;
		
		case CB_VT_STRING:
			result_buf = strdup(val->string);
			break;
		
		case CB_VT_UNDEFINED:
			result_buf = strdup(NO_VALUE_AS_STRING);
			break;
		
		case CB_VT_VALARRAY:
		{
			// allocate memory for "{}" (including null termination)
			result_buf  = (char*) malloc(2);
			*result_buf = '\0';      // terminate string
			strcat(result_buf, "{"); // open array
			
			int i             = 0;
			int count         = cb_array_get_count(val->array);
			bool last_element = false;
			
			for (; i < count; i++)
			{
				CbValue* item = NULL;
				cb_array_get(val->array, i, (CbArrayItem*) &item);
				char* value_string = NULL;
				
				if (item == NULL)
					value_string = "<NIL>";
				else
					value_string = cb_value_to_string(item);
				
				int n = 2;
				
				// check if there are still elements left
				last_element = (i == count - 1);
				if (last_element)
					n = 1;
				
				// if item is a string...
				bool string_value = item->type == CB_VT_STRING;
				if (string_value)
					n += 2; // increase memory by 2
				
				result_buf = realloc(result_buf, strlen(result_buf) +
				                                 strlen(value_string) + n);
				
				if (string_value)
					strcat(result_buf, "\""); // add additional double quotes
				
				strcat(result_buf, value_string);
				free(value_string); // free temporary value_string
				
				if (string_value)
					strcat(result_buf, "\""); // add additional double quotes
				
				if (!last_element)
					// append additional comma for further elements
					strcat(result_buf, ",");
			}
			
			result_buf = realloc(result_buf, strlen(result_buf) + 2);
			strcat(result_buf, "}"); // close array
			
			break;
		}
		
		default:
			assert(("Invalid value type", false));
			break;
	}
	
	return result_buf;
}

// -----------------------------------------------------------------------------
// print a codeblock-value
// -----------------------------------------------------------------------------
void cb_value_print(const CbValue* val)
{
	char* string = cb_value_to_string(val);
	printf("%s", string);
	free(string);
}

// -----------------------------------------------------------------------------
// get numeric value
// -----------------------------------------------------------------------------
CbNumeric cb_numeric_get(const CbValue* val)
{
	return val->value;
}

// -----------------------------------------------------------------------------
// set numeric value
// -----------------------------------------------------------------------------
void cb_numeric_set(CbValue* val, CbNumeric value)
{
	assert(cb_value_is_type(val, CB_VT_NUMERIC));
	
	val->value = value;
}

// -----------------------------------------------------------------------------
// numerical comparison
// -----------------------------------------------------------------------------
CbValue* cb_numeric_compare(enum cb_comparison_type type, const CbValue* l,
							const CbValue* r)
{
	assert(cb_value_is_type(l, CB_VT_NUMERIC));
	assert(cb_value_is_type(r, CB_VT_NUMERIC));
	
	CbValue* result = cb_boolean_create(false);
	
	switch (type)
	{
		case CMP_EQ: result->boolean = l->value == r->value; break;
		case CMP_NE: result->boolean = l->value != r->value; break;
		case CMP_GE: result->boolean = l->value >= r->value; break;
		case CMP_LE: result->boolean = l->value <= r->value; break;
		case CMP_GT: result->boolean = l->value > r->value; break;
		case CMP_LT: result->boolean = l->value < r->value; break;
	}
	
	return result;
}

// -----------------------------------------------------------------------------
// numerical addition
// -----------------------------------------------------------------------------
CbValue* cb_numeric_add(CbValue* l, CbValue* r)
{
	return cb_numeric_operation(OPR_ADD, l, r);
}

// -----------------------------------------------------------------------------
// numerical subtraction
// -----------------------------------------------------------------------------
CbValue* cb_numeric_sub(CbValue* l, CbValue* r)
{
	return cb_numeric_operation(OPR_SUB, l, r);
}

// -----------------------------------------------------------------------------
// numerical multiplication
// -----------------------------------------------------------------------------
CbValue* cb_numeric_mul(CbValue* l, CbValue* r)
{
	return cb_numeric_operation(OPR_MUL, l, r);
}

// -----------------------------------------------------------------------------
// numerical division
// -----------------------------------------------------------------------------
CbValue* cb_numeric_div(CbValue* l, CbValue* r)
{
	return cb_numeric_operation(OPR_DIV, l, r);
}

// -----------------------------------------------------------------------------
// binary AND for numerical values
// -----------------------------------------------------------------------------
CbValue* cb_numeric_and(CbValue* l, CbValue* r)
{
	return cb_numeric_operation(OPR_AND, l, r);
}

// -----------------------------------------------------------------------------
// binary OR for numerical values
// -----------------------------------------------------------------------------
CbValue* cb_numeric_or(CbValue* l, CbValue* r)
{
	return cb_numeric_operation(OPR_OR, l, r);
}

// -----------------------------------------------------------------------------
// unary NOT for numerical values
// -----------------------------------------------------------------------------
CbValue* cb_numeric_not(CbValue* operand)
{
	assert(cb_value_is_type(operand, CB_VT_NUMERIC));
	
	// IMPORTANT: Use bitwise negation (~) !
	return cb_numeric_create(~ operand->value);
}

// -----------------------------------------------------------------------------
// get string value
// -----------------------------------------------------------------------------
CbString cb_string_get(const CbValue* val)
{
	return val->string;
}

// -----------------------------------------------------------------------------
// string comparison
// -----------------------------------------------------------------------------
CbValue* cb_string_compare(enum cb_comparison_type type, const CbValue* l,
						   const CbValue* r)
{
	assert(cb_value_is_type(l, CB_VT_STRING));
	assert(cb_value_is_type(r, CB_VT_STRING));
	
	bool not_flag	= false;
	bool result		= false;
	
	switch (type)
	{
		case CMP_NE:
			// set not-flag -> invert result
			not_flag = true;
		
		case CMP_EQ:
		{
			result = (strcmp(l->string, r->string) == 0);
			result = (result ^ not_flag);
			break;
		}
		
		default:
			assert(("comparison type not allowed for string comparison", false));
			break;
	}
	
	CbValue* result_val = cb_boolean_create(result);
	return result_val;
}

// -----------------------------------------------------------------------------
// concatenate strings
// -----------------------------------------------------------------------------
CbValue* cb_string_concat(CbValue* l, CbValue* r)
{
	assert(cb_value_is_type(l, CB_VT_STRING));
	assert(cb_value_is_type(r, CB_VT_STRING));
	
	char* buffer = (char*) malloc(strlen(l->string) +
								  strlen(r->string) + 1);
	*buffer = '\0';	// terminate string
	strcat(buffer, l->string);
	strcat(buffer, r->string);
	CbValue* result = cb_string_create(buffer);
	
	return result;
}

// -----------------------------------------------------------------------------
// get boolean value
// -----------------------------------------------------------------------------
CbBoolean cb_boolean_get(const CbValue* val)
{
	return val->boolean;
}

// -----------------------------------------------------------------------------
// boolean comparison
// -----------------------------------------------------------------------------
CbValue* cb_boolean_compare(enum cb_comparison_type type, const CbValue* l,
							const CbValue* r)
{
	assert(cb_value_is_type(l, CB_VT_BOOLEAN));
	assert(cb_value_is_type(r, CB_VT_BOOLEAN));
	
	bool not_flag	= false;
	CbBoolean result= false;
	
	switch (type)
	{
		case CMP_NE:
			// set not-flag
			not_flag = true;
		
		case CMP_EQ:
		{
			result = (l->boolean == r->boolean);
			result = (result ^ not_flag); // XOR result with not-flag -> invert result
			break;
		}
		
		default:
			assert(("comparison type not allowed for boolean comparison", false));
			break;
	}
	
	CbValue* result_val = cb_boolean_create(result);
	return result_val;
}

// -----------------------------------------------------------------------------
// binary AND for boolean values
// -----------------------------------------------------------------------------
CbValue* cb_boolean_and(CbValue* l, CbValue* r)
{
	return cb_boolean_operation(OPR_AND, l, r);
}

// -----------------------------------------------------------------------------
// binary OR for boolean values
// -----------------------------------------------------------------------------
CbValue* cb_boolean_or(CbValue* l, CbValue* r)
{
	return cb_boolean_operation(OPR_OR, l, r);
}

// -----------------------------------------------------------------------------
// unary NOT for boolean values
// -----------------------------------------------------------------------------
CbValue* cb_boolean_not(CbValue* operand)
{
	assert(cb_value_is_type(operand, CB_VT_BOOLEAN));
	
	return cb_boolean_create(! operand->value);
}

// -----------------------------------------------------------------------------
// get array
// -----------------------------------------------------------------------------
const CbValArray cb_valarray_get(const CbValue* val)
{
	return val->array;
}


// #############################################################################
// internal functions
// #############################################################################

// -----------------------------------------------------------------------------
// numerical operation (internal)
// -----------------------------------------------------------------------------
static CbValue* cb_numeric_operation(enum cb_operation_type type, CbValue* l,
									 CbValue* r)
{
	assert(cb_value_is_type(l, CB_VT_NUMERIC));
	assert(cb_value_is_type(r, CB_VT_NUMERIC));
	
	CbValue* result	= cb_numeric_create(0);
	
	switch (type)
	{
		case OPR_ADD: result->value = l->value + r->value; break;
		case OPR_SUB: result->value = l->value - r->value; break;
		case OPR_MUL: result->value = l->value * r->value; break;
		case OPR_DIV:
			if (r->value == 0) // check for division by zero first!
			{
				cb_error_set(CB_ERR_CODE_DIVISIONBYZERO);
				cb_value_free(result);		// free previously allocated value
				result = cb_value_create();	// assign empty value
				break;
			}
			
			result->value = l->value / r->value;
			break;
		case OPR_AND: result->value = l->value & r->value; break;
		case OPR_OR:  result->value = l->value | r->value; break;
	}
	
	return result;
}

// -----------------------------------------------------------------------------
// boolean operation (internal)
// -----------------------------------------------------------------------------
static CbValue* cb_boolean_operation(enum cb_operation_type type, CbValue* l,
									 CbValue* r)
{
	assert(cb_value_is_type(l, CB_VT_BOOLEAN));
	assert(cb_value_is_type(r, CB_VT_BOOLEAN));
	
	CbValue* result	= cb_boolean_create(false);
	
	switch (type)
	{
		case OPR_AND: result->boolean = l->boolean && r->boolean; break;
		case OPR_OR:  result->boolean = l->boolean || r->boolean; break;
	}
	
	return result;
}
