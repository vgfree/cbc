/*******************************************************************************
 * value_t -- Codeblock values and types
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "value.h"

#define NO_VALUE_AS_STRING "<no value returned>"

// numerical operation-types
enum cbnumeric_operation_t
{
	NUM_ADD,
	NUM_SUB,
	NUM_MUL,
	NUM_DIV
};

// -----------------------------------------------------------------------------
// create a codeblock-value
// -----------------------------------------------------------------------------
value_t* value_create()
{
	value_t* val= (value_t*) malloc(sizeof(value_t));
	val->type	= VT_UNDEFINED;
}

// -----------------------------------------------------------------------------
// create a numeric value
// -----------------------------------------------------------------------------
value_t* cbnumeric_create(cbnumeric value)
{
	value_t* val= value_create();
	val->type	= VT_NUMERIC;
	val->value	= value;
}

// -----------------------------------------------------------------------------
// create a boolean value
// -----------------------------------------------------------------------------
value_t* cbboolean_create(cbboolean boolean)
{
	value_t* val= value_create();
	val->type	= VT_BOOLEAN;
	val->boolean= boolean;
}

// -----------------------------------------------------------------------------
// create a string-value
// -----------------------------------------------------------------------------
value_t* cbstring_create(cbstring string)
{
	value_t* val= value_create();
	val->type	= VT_STRING;
	val->string	= string;
}

// -----------------------------------------------------------------------------
// free codeblock-value
// -----------------------------------------------------------------------------
void value_free(value_t* val)
{
	if (val->type == VT_STRING && val->string)
		free(val->string);
	
	free(val);
}

// -----------------------------------------------------------------------------
// check for given value-type
// -----------------------------------------------------------------------------
bool value_istype(value_t* val, enum value_type_t type)
{
	if (val->type == type)
		return true;
	else
		return false;
}

// -----------------------------------------------------------------------------
// assign attributes from one codeblock-value struct to another
// -----------------------------------------------------------------------------
void value_assign(value_t* source, value_t* destination)
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
void value_assign_freesource(value_t* source, value_t* destination)
{
	value_assign(source, destination);
	value_free(source);
}

// -----------------------------------------------------------------------------
// copy a codeblock-value struct
// -----------------------------------------------------------------------------
value_t* value_copy(value_t* val)
{
	value_t* copy = value_create();
	value_assign(val, copy);
	
	return copy;
}

// -----------------------------------------------------------------------------
// convert a codeblock-value into a string
// IMPORTANT:	the c-string must be freed after usage, since allocation occurs
//				in this function!
// -----------------------------------------------------------------------------
char* value_tostring(value_t* val)
{
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
			fprintf(stderr, "Error: Value does not have a valid value-type\n");
			exit(EXIT_FAILURE);
			break;
	}
	
	return result_buf;
}

// -----------------------------------------------------------------------------
// print a codeblock-value
// -----------------------------------------------------------------------------
void value_print(value_t* val)
{
	char* string = value_tostring(val);
	printf("%s", string);
	free(string);
}

// -----------------------------------------------------------------------------
// compare two codeblock-values.
// 
// NOTE:	regardless of the value-type beeing compared, every compare-function
//			returns a cbboolean-value.
// -----------------------------------------------------------------------------
value_t* value_compare(enum comparison_type_t type, value_t* l, value_t* r)
{
	if (!value_istype(l, r->type))
	{
		fprintf(stderr, "Type mismatch: lhs-type %d differs from rhs-type %d\n",
				l->type, r->type);
		exit(EXIT_FAILURE);
	}
	
	value_t* result = NULL;
	
	switch (l->type)
	{
		case VT_NUMERIC:
			result = cbnumeric_compare(type, l, r);
			break;
		
		case VT_BOOLEAN:
			result = cbboolean_compare(type, l, r);
			break;
		
		case VT_STRING:
			result = cbstring_compare(type, l, r);
			break;
	}
	
	// free lhs and rhs
	value_free(l);
	value_free(r);
	
	return result;
}

// -----------------------------------------------------------------------------
// numerical operation
// -----------------------------------------------------------------------------
value_t* cbnumeric_operation(	enum cbnumeric_operation_t type, value_t* l,
								value_t* r)
{
	if (!value_istype(l, VT_NUMERIC) || !value_istype(r, VT_NUMERIC))
	{
		fprintf(stderr, "Error: Value has invalid type for this operation, "\
						"expecting VT_NUMERIC!");
		exit(EXIT_FAILURE);
	}
	
	value_t* result	= cbnumeric_create(0);
	
	switch (type)
	{
		case NUM_ADD: result->value = l->value + r->value; break;
		case NUM_SUB: result->value = l->value - r->value; break;
		case NUM_MUL: result->value = l->value * r->value; break;
		case NUM_DIV:
			// check for division by zero first!
			if (r->value == 0)
			{
				fprintf(stderr, "Error: Division by zero is not allowed!\n");
				exit(EXIT_FAILURE);
			}
			
			result->value = l->value / r->value;
			break;
	}
	
	// free lhs and rhs
	value_free(l);
	value_free(r);
	
	return result;
}

// -----------------------------------------------------------------------------
// numerical comparison
// -----------------------------------------------------------------------------
value_t* cbnumeric_compare(enum comparison_type_t type, value_t* l, value_t* r)
{
	if (!value_istype(l, VT_NUMERIC) || !value_istype(r, VT_NUMERIC))
	{
		fprintf(stderr, "Error: Value has invalid type for this operation, "\
						"expecting VT_NUMERIC!");
		exit(EXIT_FAILURE);
	}
	
	value_t* result = cbboolean_create(false);
	
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
value_t* cbnumeric_add(value_t* l, value_t* r)
{
	return cbnumeric_operation(NUM_ADD, l, r);
}

// -----------------------------------------------------------------------------
// numerical subtraction
// -----------------------------------------------------------------------------
value_t* cbnumeric_sub(value_t* l, value_t* r)
{
	return cbnumeric_operation(NUM_SUB, l, r);
}

// -----------------------------------------------------------------------------
// numerical multiplication
// -----------------------------------------------------------------------------
value_t* cbnumeric_mul(value_t* l, value_t* r)
{
	return cbnumeric_operation(NUM_MUL, l, r);
}

// -----------------------------------------------------------------------------
// numerical division
// -----------------------------------------------------------------------------
value_t* cbnumeric_div(value_t* l, value_t* r)
{
	return cbnumeric_operation(NUM_DIV, l, r);
}

// -----------------------------------------------------------------------------
// string comparison
// -----------------------------------------------------------------------------
value_t* cbstring_compare(enum comparison_type_t type, value_t* l, value_t* r)
{
	if (!value_istype(l, VT_STRING) || !value_istype(r, VT_STRING))
	{
		fprintf(stderr, "Error: Value has invalid type for this operation, "\
						"expecting VT_STRING!");
		exit(EXIT_FAILURE);
	}
	
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
			fprintf(stderr, "Error: Comparison-type %d not allowed for string "\
							"comparison!", type);
			exit(EXIT_FAILURE);
			break;
	}
	
	value_t* result_val = cbboolean_create(result);
	return result_val;
}

// -----------------------------------------------------------------------------
// boolean comparison
// -----------------------------------------------------------------------------
value_t* cbboolean_compare(enum comparison_type_t type, value_t* l, value_t* r)
{
	if (!value_istype(l, VT_BOOLEAN))
	{
		fprintf(stderr, "Error: Value has invalid type for this operation, "\
						"expecting VT_BOOLEAN!");
		exit(EXIT_FAILURE);
	}
	
	bool not_flag	= false;
	cbboolean result= false;
	
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
			fprintf(stderr, "Error: Comparison-type %d not allowed for "\
							"boolean comparison!", type);
			exit(EXIT_FAILURE);
			break;
	}
	
	value_t* result_val = cbboolean_create(result);
	return result_val;
}
