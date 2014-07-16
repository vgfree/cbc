/*******************************************************************************
 * CbArray -- Implementation of a dynamic value array structure
 * 
 *            The array can hold a dynamic amount of pointers to a
 *            CbValue-object (defined as CbArrayItem).
 ******************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H


#include <stdlib.h>
#include <stdbool.h>


typedef struct CbArray CbArray;
typedef void* CbArrayItem;
typedef void (*CbArrayItemDestructor)(CbArrayItem item);
typedef CbArrayItem (*CbArrayItemCopy)(const CbArrayItem item);


// interface functions
CbArray* cb_array_create();
CbArray* cb_array_create_with_ownership(CbArrayItemDestructor destructor_cb,
                                        CbArrayItemCopy copy_cb);
void cb_array_free(CbArray* array);
CbArray* cb_array_copy(CbArray* array);

size_t cb_array_get_count(CbArray* array);
bool cb_array_get_element_ownership(CbArray* array);
void cb_array_enable_element_ownership(CbArray* array,
                                       CbArrayItemDestructor destructor_cb,
                                       CbArrayItemCopy copy_cb);
void cb_array_disable_element_ownership(CbArray* array);

bool cb_array_set(CbArray* array, int index, const CbArrayItem item);
bool cb_array_get(CbArray* array, int index, CbArrayItem* destination);
bool cb_array_append(CbArray* array, const CbArrayItem item);
bool cb_array_insert(CbArray* array, const CbArrayItem item, int index);
bool cb_array_remove(CbArray* array, int index);


#endif // ARRAY_H
