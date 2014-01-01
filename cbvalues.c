/*******************************************************************************
 * valuetypes
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "cbvalues.h"
#include "errors.h"

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
cbvalue* cbvalue_create()
{
	cbvalue* val = (cbvalue*) malloc(sizeof(cbvalue));
	if (!val)
	{
		yyerror(ERR_BADALLOC);
		exit(1);
	}
	
	val->type = VT_UNDEFINED;
}

// -----------------------------------------------------------------------------
// create a numeric-value
// -----------------------------------------------------------------------------
cbvalue* cbnumeric_create(cbnumeric value)
{
	cbvalue* val= cbvalue_create();
	val->type	= VT_NUMERIC;
	val->value	= value;
}

// -----------------------------------------------------------------------------
// create a string-value
// -----------------------------------------------------------------------------
cbvalue* cbstring_create(cbstring string)
{
	cbvalue* val= cbvalue_create();
	val->type	= VT_STRING;
	val->string	= string;
}

// -----------------------------------------------------------------------------
// free codeblock-value
// -----------------------------------------------------------------------------
void cbvalue_free(cbvalue* val)
{
	if (val->type == VT_STRING && val->string)
		free(val->string);
	
	free(val);
}

// -----------------------------------------------------------------------------
// check for given value-type
// -----------------------------------------------------------------------------
bool cbvalue_istype(enum valuetype_t type, cbvalue* val)
{
	if (val->type == type)
		return true;
	else
		return false;
}

// -----------------------------------------------------------------------------
// assign attributes from one codeblock-value struct to another
// -----------------------------------------------------------------------------
void cbvalue_assign(cbvalue* source, cbvalue* destination)
{
	destination->type = source->type;
	
	switch (destination->type)
	{
		case VT_NUMERIC:
			destination->value = source->value;
			break;
		
		case VT_STRING:
			// free old string
			if (destination->string)
				free(destination->string);
			// assign new one
			destination->string = source->string;
			break;
	}
}

// -----------------------------------------------------------------------------
// assign attributes from one codeblock-value struct to another and free the
// source struct
// -----------------------------------------------------------------------------
void cbvalue_assign_freesource(cbvalue* source, cbvalue* destination)
{
	cbvalue_assign(source, destination);
	cbvalue_free(source);
}

// -----------------------------------------------------------------------------
// copy a codeblock-value struct
// -----------------------------------------------------------------------------
cbvalue* cbvalue_copy(cbvalue* val)
{
	cbvalue* copy = cbvalue_create();
	cbvalue_assign(val, copy);
	
	return copy;
}

// -----------------------------------------------------------------------------
// convert a codeblock-value into a string
// IMPORTANT:	the c-string must be freed after usage, since allocation occurs
//				in this function!
// -----------------------------------------------------------------------------
char* cbvalue_tostring(cbvalue* val)
{
	char* result_buf;
	
	switch (val->type)
	{
		case VT_NUMERIC:
			// allocate 128 bytes buffer
			result_buf = malloc(128);
			sprintf(result_buf, "%d", val->value);
			break;
		
		case VT_STRING:
			result_buf = strdup(val->string);
			break;
		
		case VT_UNDEFINED:
			result_buf = malloc(strlen(NO_VALUE_AS_STRING) + 1);
			sprintf(result_buf, NO_VALUE_AS_STRING);
			break;
		
		default:
			yyerror("invalid value-type: %d", val->type);
			exit(1);
			break;
	}
	
	return result_buf;
}

// -----------------------------------------------------------------------------
// print a codeblock-value
// -----------------------------------------------------------------------------
void cbvalue_print(cbvalue* val)
{
	char* string = cbvalue_tostring(val);
	printf("%s", string);
	free(string);
}

// -----------------------------------------------------------------------------
// numerical operation
// -----------------------------------------------------------------------------
cbvalue* cbnumeric_operation(	enum cbnumeric_operation_t type, cbvalue* l,
								cbvalue* r)
{
	if (!cbvalue_istype(VT_NUMERIC, l) || !cbvalue_istype(VT_NUMERIC, r))
	{
		yyerror("value has invalid type for this operation, expecting VT_NUMERIC!");
		exit(1);
	}
	
	cbvalue* result	= cbnumeric_create(0);
	
	switch (type)
	{
		case NUM_ADD: result->value = l->value + r->value; break;
		case NUM_SUB: result->value = l->value - r->value; break;
		case NUM_MUL: result->value = l->value * r->value; break;
		case NUM_DIV:
			// check for division by zero first!
			if (r->value == 0)
			{
				yyerror("division by zero is not allowed!");
				exit(1);
			}
			
			result->value = l->value / r->value;
			break;
	}
	
	// free lhs and rhs
	cbvalue_free(l);
	cbvalue_free(r);
	
	return result;
}

// -----------------------------------------------------------------------------
// numerical comparison
// -----------------------------------------------------------------------------
cbvalue* cbnumeric_compare(enum comparisontype_t type, cbvalue* l, cbvalue* r)
{
	if (!cbvalue_istype(VT_NUMERIC, l) || !cbvalue_istype(VT_NUMERIC, r))
	{
		yyerror("(CMP) value has invalid type for this operation, expecting VT_NUMERIC!");
		exit(1);
	}
	
	cbvalue* result = cbnumeric_create(0);
	
	switch (type)
	{
		case CMP_EQ: result->value = l->value == r->value; break;
		case CMP_NE: result->value = l->value != r->value; break;
		case CMP_GE: result->value = l->value >= r->value; break;
		case CMP_LE: result->value = l->value <= r->value; break;
		case CMP_GT: result->value = l->value > r->value; break;
		case CMP_LT: result->value = l->value < r->value; break;
	}
	
	// free lhs and rhs
	cbvalue_free(l);
	cbvalue_free(r);
	
	return result;
}

// -----------------------------------------------------------------------------
// numerical addition
// -----------------------------------------------------------------------------
cbvalue* cbnumeric_add(cbvalue* l, cbvalue* r)
{
	return cbnumeric_operation(NUM_ADD, l, r);
}

// -----------------------------------------------------------------------------
// numerical subtraction
// -----------------------------------------------------------------------------
cbvalue* cbnumeric_sub(cbvalue* l, cbvalue* r)
{
	return cbnumeric_operation(NUM_SUB, l, r);
}

// -----------------------------------------------------------------------------
// numerical multiplication
// -----------------------------------------------------------------------------
cbvalue* cbnumeric_mul(cbvalue* l, cbvalue* r)
{
	return cbnumeric_operation(NUM_MUL, l, r);
}

// -----------------------------------------------------------------------------
// numerical division
// -----------------------------------------------------------------------------
cbvalue* cbnumeric_div(cbvalue* l, cbvalue* r)
{
	return cbnumeric_operation(NUM_DIV, l, r);
}
