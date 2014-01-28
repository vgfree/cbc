/*******************************************************************************
 * scope_t -- Stores information about the current codeblock execution-scope
 ******************************************************************************/

#ifndef SCOPE_H
#define SCOPE_H


#include <stdbool.h>

// scope_t struct
typedef struct
{
	char* context;	// scope context -- e.g. function-name
	int level;		// scope level
} scope_t;


// interface functions
scope_t* scope_create(char* context, int level);
void scope_free(scope_t* scope);
bool scope_equals(const scope_t* scope1, const scope_t* scope2);


#endif // SCOPE_H
