/*******************************************************************************
 * strlist_t -- Implementation of a list-structure
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "strlist.h"


// #############################################################################
// declarations
// #############################################################################

strlist_t* strlist_item_create();
void strlist_item_free(strlist_t* item);


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// create string-list
// -----------------------------------------------------------------------------
strlist_t* strlist_create(char* string)
{
	strlist_t* sl	= strlist_item_create();
	sl->string		= strdup(string);
	// item was created by default -> increase count to 1
	sl->count		= 1;
	
	return sl;
}

// -----------------------------------------------------------------------------
// free string-list
// -----------------------------------------------------------------------------
void strlist_free(strlist_t* list)
{
	strlist_t* current = list;
	while (current)
	{
		strlist_t* temp = current->next;
		strlist_item_free(current);
		current = temp;
	}
}

// -----------------------------------------------------------------------------
// append string to list
// -----------------------------------------------------------------------------
strlist_t* strlist_append(strlist_t* list, char* string)
{
	strlist_t* current = list;
	
	// get last element in the list
	while (current->next)
		current = current->next;
	
	// append item
	current->next			= strlist_item_create();
	current->next->string	= strdup(string);
	
	list->count++; // increase count of elements in the list
	
	return current->next;
}


// #############################################################################
// internal functions
// #############################################################################

// -----------------------------------------------------------------------------
// create string-list item (internal)
// -----------------------------------------------------------------------------
strlist_t* strlist_item_create()
{
	strlist_t* si	= (strlist_t*) malloc(sizeof(strlist_t));
	si->next		= NULL;
	si->string		= NULL;
	si->data		= NULL;
	si->count		= 0;
	
	return si;
}

// -----------------------------------------------------------------------------
// free string-list-item (internal)
// -----------------------------------------------------------------------------
void strlist_item_free(strlist_t* item)
{
	if (item->string)
		free(item->string);
	
	free(item);
}
