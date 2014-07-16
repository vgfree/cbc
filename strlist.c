/*******************************************************************************
 * CbStrlist -- Implementation of a list-structure.
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>
#include "strlist.h"


// #############################################################################
// declarations
// #############################################################################

static CbStrlist* cb_strlist_item_create();
static void cb_strlist_item_free(CbStrlist* item);


// #############################################################################
// interface-functions
// #############################################################################

// -----------------------------------------------------------------------------
// create string-list
// -----------------------------------------------------------------------------
CbStrlist* cb_strlist_create(char* string)
{
    CbStrlist* sl = cb_strlist_item_create();
    sl->string    = strdup(string);
    // item was created by default -> increase count to 1
    sl->count     = 1;
    
    return sl;
}

// -----------------------------------------------------------------------------
// free string-list
// -----------------------------------------------------------------------------
void cb_strlist_free(CbStrlist* list)
{
    CbStrlist* current = list;
    while (current)
    {
        CbStrlist* temp = current->next;
        cb_strlist_item_free(current);
        current = temp;
    }
}

// -----------------------------------------------------------------------------
// append string to list
// -----------------------------------------------------------------------------
CbStrlist* cb_strlist_append(CbStrlist* list, char* string)
{
    CbStrlist* current = list;
    
    // get last element in the list
    while (current->next)
        current = current->next;
    
    // append item
    current->next         = cb_strlist_item_create();
    current->next->string = strdup(string);
    
    list->count++; // increase count of elements in the list
    
    return current->next;
}


// #############################################################################
// internal functions
// #############################################################################

// -----------------------------------------------------------------------------
// create string-list item (internal)
// -----------------------------------------------------------------------------
static CbStrlist* cb_strlist_item_create()
{
    CbStrlist* si = (CbStrlist*) malloc(sizeof(CbStrlist));
    si->next      = NULL;
    si->string    = NULL;
    si->data      = NULL;
    si->count     = 0;
    
    return si;
}

// -----------------------------------------------------------------------------
// free string-list-item (internal)
// -----------------------------------------------------------------------------
static void cb_strlist_item_free(CbStrlist* item)
{
    if (item->string)
        free(item->string);
    
    free(item);
}
