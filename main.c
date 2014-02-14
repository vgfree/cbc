/*******************************************************************************
 * main -- Running the parser and executing the abstract syntax tree.
 *         Also checking command line arguments:
 *           - If a file-name was passed, the file will be parsed and executed
 *           - if no argument was passed, stdin will be parsed and executed
 * 
 *         Used macros:
 *           - _CBC_TRACK_EXECUTION_TIME: Determines whether to print the run-
 *                                        time of the interpreter.
 ******************************************************************************/

#ifdef _CBC_TRACK_EXECUTION_TIME
#include <time.h>
#endif // _CBC_TRACK_EXECUTION_TIME

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cbc_parse.h"
#include "value.h"
#include "codeblock.h"
#include "symtab.h"


// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	extern FILE* yyin;
	bool parse_file = argc > 1;	// determine whether to parse a file
	
	if (parse_file)
	{
		FILE* input = fopen(argv[1], "r");
		if (!input)
		{
			printf("Error: Unable to open file `%s'!\n", argv[1]);
			exit(EXIT_FAILURE);
		}
		
		yyin = input;
	}

	codeblock_t* cb		= codeblock_create();
	cb->global_symtab	= symtab_create();
	
	yyparse(&cb->ast);	// parse codeblock-code
	
	if (parse_file)		// if parsed stream is not stdin
		fclose(yyin);	// -> close file stream
	
#ifdef _CBC_TRACK_EXECUTION_TIME
	clock_t begin;
	begin = clock();
#endif // _CBC_TRACK_EXECUTION_TIME
	
	codeblock_execute(cb);		// execute ...
	
#ifdef _CBC_TRACK_EXECUTION_TIME
	clock_t end;
	end = clock();
	float diff = ((float) end - (float) begin);
#endif // _CBC_TRACK_EXECUTION_TIME
	
	value_print(cb->result);	// and print result
	
#ifdef _CBC_TRACK_EXECUTION_TIME
	printf("\nExecution duration: %f seconds", diff / CLOCKS_PER_SEC);
#endif // _CBC_TRACK_EXECUTION_TIME
	
	// cleanup
	symtab_free(cb->global_symtab);
	syntree_free(cb->ast);
	codeblock_free(cb);
	
	return 0;
}
