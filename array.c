/*******************************************************************************
 * CbArray -- Implementation of a dynamic value array structure
 ******************************************************************************/

#include <stdlib.h>
#include "array.h"


// #############################################################################
// declarations
// #############################################################################

struct CbArray
{
	size_t count;
	CbArrayItem* elements;
	int block_size;
	int alloc_size;
	bool element_ownership;
};

static bool cb_array_is_full(CbArray* array);
static bool cb_array_increase_size(CbArray* array, unsigned int blocks);


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CbArray* cb_array_create()
{
	CbArray* array    = (CbArray*) malloc(sizeof(CbArray));
	array->count      = 0;
	// default block size is the size of 16 elements
	array->block_size = sizeof(CbArrayItem) * 16;
	array->elements   = (CbArrayItem*) malloc(array->block_size);
	array->alloc_size = array->block_size;
	
	// array should own its elements by default
	array->element_ownership = true;
	
	return array;
}

// -----------------------------------------------------------------------------
// Destructur
// -----------------------------------------------------------------------------
void cb_array_free(CbArray* array)
{
	if (array->element_ownership)	// if array owns its elements -> free all
	{
		int i = 0;
		for (; i < array->count; i++)
			if (array->elements[i] != NULL)
				cb_value_free(array->elements[i]);
	}
	
	free(array->elements);
	free(array);
}

// -----------------------------------------------------------------------------
// Get count of elements in array
// -----------------------------------------------------------------------------
size_t cb_array_get_count(CbArray* array)
{
	return array->count;
}

// -----------------------------------------------------------------------------
// Append element to array
// -----------------------------------------------------------------------------
bool cb_array_append(CbArray* array, const CbArrayItem item)
{
	if (cb_array_is_full(array))
		cb_array_increase_size(array, 1);
	
	array->count++;
	array->elements[array->count - 1] = NULL;	// clear allocated memory
	
	return cb_array_set(array, (array->count - 1), item);
}

// -----------------------------------------------------------------------------
// Set element in array
// -----------------------------------------------------------------------------
bool cb_array_set(CbArray* array, int index, const CbArrayItem item)
{
	if (array->count <= index)
	{
		if (item != NULL)
			cb_value_free(item);
		
		return false;
	}
	
	// free previous element, if necessary
	if (array->elements[index] != NULL)
		cb_value_free(array->elements[index]);
	
	array->elements[index] = item;
	return true;
}

// -----------------------------------------------------------------------------
// Get element in array
// -----------------------------------------------------------------------------
bool cb_array_get(CbArray* array, int index, CbArrayItem* destination)
{
	if (array->count <= index)
		return false;
	
	if (destination != NULL)
		*destination = array->elements[index];
	
	return true;
}


// #############################################################################
// internal functions
// #############################################################################

// -----------------------------------------------------------------------------
// Check if array is full
// -----------------------------------------------------------------------------
static bool cb_array_is_full(CbArray* array)
{
	return ((array->count * sizeof(CbArrayItem)) >= array->alloc_size);
}

// -----------------------------------------------------------------------------
// Increase array allocation size
// -----------------------------------------------------------------------------
static bool cb_array_increase_size(CbArray* array, unsigned int blocks)
{
	size_t new_size   = array->alloc_size + (array->block_size * blocks);
	CbArrayItem* temp = realloc(array->elements, new_size);
	if (temp == NULL)
		return false;
	
	array->elements   = temp;
	array->alloc_size = new_size;
}
