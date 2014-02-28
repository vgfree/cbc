/*******************************************************************************
 * main -- Running the parser and executing the abstract syntax tree.
 *         Also checking command line arguments:
 *           - if a file-name was passed, the file will be parsed and executed
 *           - if no argument was passed, stdin will be parsed and executed
 * 
 *         Used macros:
 *           - _CBC_TRACK_EXECUTION_TIME: Determines whether to print the 
 *                                        execution run-time.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "value.h"
#include "codeblock.h"
#include "symtab.h"
#include "error_handling.h"


// -----------------------------------------------------------------------------
// Main
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	bool parse_file = argc > 1;	// determine whether to parse a file
	FILE* input		= NULL;
	
	if (parse_file)
	{
		input = fopen(argv[1], "r");
		if (!input)
		{
			cb_print_error_msg("Unable to open file `%s'", argv[1]);
			return EXIT_FAILURE;
		}
	}

	Codeblock* cb	  = codeblock_create();
	int parser_result = codeblock_parse_file(cb, input);
	
	if (parse_file)		// if a file was parsed
		fclose(input);	// -> close file stream
	
	if (parser_result == EXIT_SUCCESS)
	{
		codeblock_execute(cb);		// execute ...
		cb_value_print(cb->result);	// and print result
		
#ifdef _CBC_TRACK_EXECUTION_TIME
		printf("\nExecution duration: %f seconds", cb->duration);
#endif // _CBC_TRACK_EXECUTION_TIME
	}
	
	codeblock_free(cb);	// cleanup
	
	return 0;
}
