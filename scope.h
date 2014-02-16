/*******************************************************************************
 * CbScope -- Stores information about the current codeblock execution-scope.
 ******************************************************************************/

#ifndef SCOPE_H
#define SCOPE_H


#include <stdbool.h>

// scope_t struct
typedef struct
{
	char* context;	// scope context -- e.g. function-name
	int level;		// scope level
} CbScope;


// interface functions
CbScope* cb_scope_create(char* context, int level);
void cb_scope_free(CbScope* scope);
bool cb_scope_equals(const CbScope* scope1, const CbScope* scope2);


#endif // SCOPE_H
