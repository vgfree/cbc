/*******************************************************************************
 * strlist_t -- Implementation of a list-structure
 *
 *  	The purpose of this list is to store a variable amount of strings.
 *  	Through the 'data'-attribute it is possible to additionally store any
 *  	data. However, the data that is referenced by this attribute must be
 *  	freed separately!
 ******************************************************************************/

#ifndef STRLIST_H
#define STRLIST_H


#include <stddef.h>

// string list-item
typedef struct strlist_item
{
	struct strlist_item* next;	// reference to the next item in the list
	char* string;				// the string-attribute
	size_t count;				// number of elements in the list
	void* data;	// Refernece to a arbitrary data-structure.
				// This allows to store any type of data in the list.
				// NOTE:	The stored data won't be freed, when the list is
				//  		beeing destroyed!
} strlist_t;

strlist_t* strlist_create();
void strlist_free(strlist_t* list);
strlist_t* strlist_append(strlist_t* list, char* string);


#endif // STRLIST_H
