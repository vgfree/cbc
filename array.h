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
#include "value.h"

// CbArray struct
typedef struct CbArray CbArray;
typedef CbValue* CbArrayItem;


// interface functions
CbArray* cb_array_create();
void cb_array_free(CbArray* array);
CbArray* cb_array_copy(CbArray* array);

size_t cb_array_get_count(CbArray* array);
bool cb_array_get_element_ownership(CbArray* array);
void cb_array_set_element_ownership(CbArray* array, bool value);

bool cb_array_set(CbArray* array, int index, const CbArrayItem item);
bool cb_array_get(CbArray* array, int index, CbArrayItem* destination);
bool cb_array_append(CbArray* array, const CbArrayItem item);
bool cb_array_insert(CbArray* array, const CbArrayItem item, int index);
bool cb_array_remove(CbArray* array, int index);


#endif // ARRAY_H
