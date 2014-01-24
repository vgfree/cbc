/*******************************************************************************
 * scope_t -- Stores information about the current codeblock execution-scope
 ******************************************************************************/

#ifndef SCOPE_H
#define SCOPE_H


// scope_t struct
typedef struct
{
	char* context;	// scope context -- e.g. function-name
	int level;		// scope level
} scope_t;


// interface functions
scope_t* scope_create(char* context, int level);
void scope_free(scope_t* scope);


#endif // SCOPE_H
