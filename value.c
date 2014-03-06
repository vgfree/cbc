/*******************************************************************************
 * CbValue -- Codeblock values and types
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "value.h"
#include "error_handling.h"

#define NO_VALUE_AS_STRING "<no value returned>"


// #############################################################################
// declarations
// #############################################################################

static CbValue* cb_numeric_operation(enum cb_operation_type type, CbValue* l,
									 CbValue* r);
static CbValue* cb_string_concat(CbValue* l, CbValue* r);


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// create a codeblock-value
// -----------------------------------------------------------------------------
CbValue* cb_value_create()
{
	CbValue* val= (CbValue*) malloc(sizeof(CbValue));
	val->type	= VT_UNDEFINED;
}

// -----------------------------------------------------------------------------
// create a numeric value
// -----------------------------------------------------------------------------
CbValue* cb_numeric_create(CbNumeric value)
{
	CbValue* val= cb_value_create();
	val->type	= VT_NUMERIC;
	val->value	= value;
}

// -----------------------------------------------------------------------------
// create a boolean value
// -----------------------------------------------------------------------------
CbValue* cb_boolean_create(CbBoolean boolean)
{
	CbValue* val= cb_value_create();
	val->type	= VT_BOOLEAN;
	val->boolean= boolean;
}

// -----------------------------------------------------------------------------
// create a string-value
// -----------------------------------------------------------------------------
CbValue* cb_string_create(CbString string)
{
	CbValue* val= cb_value_create();
	val->type	= VT_STRING;
	val->string	= string;
}

// -----------------------------------------------------------------------------
// free codeblock-value
// -----------------------------------------------------------------------------
void cb_value_free(CbValue* val)
{
	if (val->type == VT_STRING && val->string)
		free(val->string);
	
	free(val);
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
		case VT_NUMERIC:
			destination->value = source->value;
			break;
		
		case VT_BOOLEAN:
			destination->boolean = source->boolean;
			break;
		
		case VT_STRING:
			// free old string
			if (destination->type == VT_STRING && destination->string)
				free(destination->string);
			// assign new one
			destination->string = strdup(source->string);
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
		case VT_NUMERIC:
			// allocate 128 bytes buffer
			result_buf = malloc(128);
			sprintf(result_buf, "%d", val->value);
			break;
		
		case VT_BOOLEAN:
			if (val->boolean)
				result_buf = strdup(CB_BOOLEAN_TRUE_STR);
			else
				result_buf = strdup(CB_BOOLEAN_FALSE_STR);
			
			break;
		
		case VT_STRING:
			result_buf = strdup(val->string);
			break;
		
		case VT_UNDEFINED:
			result_buf = strdup(NO_VALUE_AS_STRING);
			break;
		
		default:
			assert(("Invalid value-type", false));
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
// compare two codeblock-values.
// 
// NOTE:	regardless of the value-type beeing compared, every compare-function
//			returns a cbboolean-value.
// -----------------------------------------------------------------------------
CbValue* cb_value_compare(enum cb_comparison_type type, CbValue* l, CbValue* r)
{
	assert(cb_value_is_type(l, r->type));
	
	CbValue* result = NULL;
	
	switch (l->type)
	{
		case VT_NUMERIC:
			result = cb_numeric_compare(type, l, r);
			break;
		
		case VT_BOOLEAN:
			result = cb_boolean_compare(type, l, r);
			break;
		
		case VT_STRING:
			result = cb_string_compare(type, l, r);
			break;
	}
	
	// free lhs and rhs
	cb_value_free(l);
	cb_value_free(r);
	
	return result;
}

// -----------------------------------------------------------------------------
// numerical comparison
// -----------------------------------------------------------------------------
CbValue* cb_numeric_compare(	enum cb_comparison_type type, const CbValue* l,
							const CbValue* r)
{
	assert(cb_value_is_type(l, VT_NUMERIC) && cb_value_is_type(r, VT_NUMERIC));
	
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
// perform add-operation
// -----------------------------------------------------------------------------
CbValue* cb_value_add(CbValue* l, CbValue* r)
{
	switch (l->type)
	{
		case VT_NUMERIC:
			return cb_numeric_operation(OPR_ADD, l, r);
			break;
		case VT_STRING:
			return cb_string_concat(l, r);
			break;
		default:
			assert(("Unable to perform OPR_ADD on this value-type", false));
			break;
	}
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
// string comparison
// -----------------------------------------------------------------------------
CbValue* cb_string_compare(enum cb_comparison_type type, const CbValue* l,
						   const CbValue* r)
{
	assert(cb_value_is_type(l, VT_STRING) && cb_value_is_type(r, VT_STRING));
	
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
			assert(("comparison-type %d not allowed for string comparison", false));
			break;
	}
	
	CbValue* result_val = cb_boolean_create(result);
	return result_val;
}

// -----------------------------------------------------------------------------
// boolean comparison
// -----------------------------------------------------------------------------
CbValue* cb_boolean_compare(enum cb_comparison_type type, const CbValue* l,
							const CbValue* r)
{
	assert(cb_value_is_type(l, VT_BOOLEAN));
	
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
			assert(("comparison-type %d not allowed for boolean comparison", false));
			break;
	}
	
	CbValue* result_val = cb_boolean_create(result);
	return result_val;
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
	assert(cb_value_is_type(r, VT_NUMERIC));
	
	CbValue* result	= cb_numeric_create(0);
	
	switch (type)
	{
		case OPR_ADD: result->value = l->value + r->value; break;
		case OPR_SUB: result->value = l->value - r->value; break;
		case OPR_MUL: result->value = l->value * r->value; break;
		case OPR_DIV:
			if (r->value == 0) // check for division by zero first!
			{
				cb_print_error_msg("Division by zero is not allowed");
				cb_value_free(result);	// free previously allocated value
				result = NULL;			// assign NULL to indicate failure
				break;
			}
			
			result->value = l->value / r->value;
			break;
	}
	
	// free lhs and rhs
	cb_value_free(l);
	cb_value_free(r);
	
	return result;
}

// -----------------------------------------------------------------------------
// concatenate string (internal)
// -----------------------------------------------------------------------------
static CbValue* cb_string_concat(CbValue* l, CbValue* r)
{
	assert(cb_value_is_type(r, VT_STRING));
	
	char* buffer = (char*) malloc(strlen(l->string) +
								  strlen(r->string) + 1);
	*buffer = '\0';	// terminate string
	strcat(buffer, l->string);
	strcat(buffer, r->string);
	CbValue* result = cb_string_create(buffer);
	
	// free lhs and rhs
	cb_value_free(l);
	cb_value_free(r);
	
	return result;
}
