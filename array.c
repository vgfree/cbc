/*******************************************************************************
 * CbArray -- Implementation of a dynamic value array structure
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "array.h"


// #############################################################################
// declarations
// #############################################################################

struct CbArray
{
    size_t count;
    CbArrayItem* elements;
    int element_size;
    int block_size;
    int alloc_size;
    bool element_ownership;
    CbArrayItemDestructor element_destructor_cb;
    CbArrayItemCopy element_copy_cb;
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
    CbArray* array      = (CbArray*) malloc(sizeof(CbArray));
    array->count        = 0;
    // default block size is the size of 16 elements
    array->element_size = sizeof(CbArrayItem);
    array->block_size   = array->element_size * 16;
    array->elements     = (CbArrayItem*) malloc(array->block_size);
    array->alloc_size   = array->block_size;
    
    // array should not own its elements by default, since there is no
    // destructor callback available yet
    array->element_ownership     = false;
    array->element_destructor_cb = NULL;
    array->element_copy_cb       = NULL;
    
    return array;
}

// -----------------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------------
CbArray* cb_array_create_with_ownership(CbArrayItemDestructor destructor_cb,
                                        CbArrayItemCopy copy_cb)
{
    CbArray* array = cb_array_create();
    cb_array_enable_element_ownership(array, destructor_cb, copy_cb);
    
    return array;
}

// -----------------------------------------------------------------------------
// Destructur
// -----------------------------------------------------------------------------
void cb_array_free(CbArray* array)
{
    if (array->element_ownership) // if array owns its elements -> free all
    {
        int i = 0;
        for (; i < array->count; i++)
            if (array->elements[i] != NULL)
                array->element_destructor_cb(array->elements[i]);
    }
    
    free(array->elements);
    free(array);
}

// -----------------------------------------------------------------------------
// Copy array
// -----------------------------------------------------------------------------
CbArray* cb_array_copy(CbArray* array)
{
    CbArray* new_array               = (CbArray*) malloc(sizeof(CbArray));
    new_array->count                 = array->count;
    new_array->element_size          = array->element_size;
    new_array->block_size            = array->block_size;
    new_array->alloc_size            = array->alloc_size;
    new_array->elements              = (CbArrayItem*) malloc(array->alloc_size);
    new_array->element_ownership     = array->element_ownership;
    new_array->element_destructor_cb = array->element_destructor_cb;
    new_array->element_copy_cb       = array->element_copy_cb;
    
    // apply all values within array as well
    int i = 0;
    for (; i < array->count; i++)
    {
        if (array->element_ownership) // copy values if array owns its elements
            new_array->elements[i] = array->element_copy_cb(array->elements[i]);
        else
            new_array->elements[i] = array->elements[i];
    }
    
    return new_array;
}

// -----------------------------------------------------------------------------
// Get count of elements in array
// -----------------------------------------------------------------------------
size_t cb_array_get_count(CbArray* array)
{
    return array->count;
}

// -----------------------------------------------------------------------------
// Get element ownership attribute of array
// -----------------------------------------------------------------------------
bool cb_array_get_element_ownership(CbArray* array)
{
    return array->element_ownership;
}

// -----------------------------------------------------------------------------
// Enable element ownership
// -----------------------------------------------------------------------------
void cb_array_enable_element_ownership(CbArray* array,
                                       CbArrayItemDestructor destructor_cb,
                                       CbArrayItemCopy copy_cb)
{
    assert(destructor_cb != NULL);
    assert(copy_cb != NULL);
    
    array->element_ownership     = true;
    array->element_destructor_cb = destructor_cb;
    array->element_copy_cb       = copy_cb;
}
// -----------------------------------------------------------------------------
// Disable element ownership
// -----------------------------------------------------------------------------
void cb_array_disable_element_ownership(CbArray* array)
{
    array->element_ownership     = false;
    array->element_destructor_cb = NULL;
    array->element_copy_cb       = NULL;
}

// -----------------------------------------------------------------------------
// Append element to array
// -----------------------------------------------------------------------------
bool cb_array_append(CbArray* array, const CbArrayItem item)
{
    if (cb_array_is_full(array))
        cb_array_increase_size(array, 1);
    
    array->count++;
    array->elements[array->count - 1] = NULL; // clear allocated memory
    
    return cb_array_set(array, (array->count - 1), item);
}

// -----------------------------------------------------------------------------
// Insert element in array
// -----------------------------------------------------------------------------
bool cb_array_insert(CbArray* array, const CbArrayItem item, int index)
{
    bool result = false;
    
    if (index >= array->count)
        return cb_array_append(array, item);
    
    if (cb_array_is_full(array))
        cb_array_increase_size(array, 1);
    
    array->count++;
    array->elements[array->count - 1] = NULL; // clear allocated memory
    
    void* source      = array->elements + index;
    void* destination = array->elements + index + 1;
    size_t size       = (array->count - index) * array->element_size;
    memmove(destination, source, size);
    
    bool ownership_backup    = array->element_ownership;
    array->element_ownership = false; // temporarily disable element ownership
    result = cb_array_set(array, index, item);
    array->element_ownership = ownership_backup; // restore ownership attribute
    
    return result;
}

// -----------------------------------------------------------------------------
// Delete element from array
// -----------------------------------------------------------------------------
bool cb_array_remove(CbArray* array, int index)
{
    if (index >= array->count)
        return false;
    
    if (!cb_array_set(array, index, NULL))
        return false;
    
    void* source      = array->elements + index + 1;
    void* destination = array->elements + index;
    size_t size       = (array->count - (index + 1)) * array->element_size;
    memmove(destination, source, size);
    
    array->count--;
    
    return true;
}

// -----------------------------------------------------------------------------
// Set element in array
// -----------------------------------------------------------------------------
bool cb_array_set(CbArray* array, int index, const CbArrayItem item)
{
    if (array->count <= index)
    {
        if (array->element_ownership && item != NULL)
            array->element_destructor_cb(item);
        
        return false;
    }
    
    // free previous element, if necessary
    if (array->element_ownership && array->elements[index] != NULL)
        array->element_destructor_cb(array->elements[index]);
    
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
    return ((array->count * array->element_size) >= array->alloc_size);
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
