	/* DEFINITIONS ---------------------------------------------------------- */
%{

#ifdef _CBC_TRACK_EXECUTION_TIME
#include <time.h>
#endif // _CBC_TRACK_EXECUTION_TIME

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "codeblock.h"
#include "value.h"
#include "syntree.h"
#include "symtab.h"
#include "symref.h"
#include "funccall.h"
#include "funcdecl.h"
#include "strlist.h"

codeblock_t* cb;

%}

%union {
	syntree_t* ast;
	char* id;
	cbstring str;
	cbboolean boolval;
	cbnumeric val;
	enum comparison_type_t cmp;
	strlist_t* list;
};

%token				ENDOFFILE
%token				FUNCTION
%token				PRINT
%token				IF THEN ELSE ENDIF
%token				WHILE DO END
%token	<val>		NUMBER
%token	<id>		IDENTIFIER
%token	<str>		STRING
%token	<boolval>	BOOLEAN

%right	ASSIGN
%left	'+' '-'
%left	'*' '/'

%nonassoc	<cmp>	COMPARE

%type <ast>		decllist decl stmtlist stmt expr
%type <list>	params paramlist exprlist args


%%	/* RULES ---------------------------------------------------------------- */


prog:
	stmtlist ENDOFFILE			{
									cb->ast = $1;
									YYACCEPT;
								}
	;

decllist:
	decl						{ $$ = $1; }
	| decl ',' decllist			{
									$$ = syntree_create(SNT_STATEMENTLIST,
														$1, $3);
								}
	;

params:
	paramlist					{ $$ = $1; }
	|							{ $$ = NULL; }	// no params were specified

paramlist:
	IDENTIFIER					{
									$$ = strlist_create($1);
									free($1);	// free duplicated string
								}
	| paramlist ',' IDENTIFIER	{
									strlist_append($1, $3);
									free($3);	// free duplicated string
									$$ = $1;
								}
	;

decl:
	IDENTIFIER					{
									$$ = syntree_create(SNT_DECLARATION,
														symref_create($1),
														NULL);
									free($1);	// free duplicated string
								}
	;

stmtlist:
	'|' decllist '|' stmtlist	{
									if ($4 == NULL)
										$$ = $2;
									else
										$$ = syntree_create(SNT_STATEMENTLIST,
															$2, $4);
								}
	| stmt ',' stmtlist			{
									if ($3 == NULL)
										$$ = $1;
									else
										$$ = syntree_create(SNT_STATEMENTLIST,
															$1, $3);
								}
	|							{ $$ = NULL; }
	;

stmt:
	expr						{ $$ = $1; }
	| IF expr THEN stmtlist ENDIF {
									$$ = flow_create(SNT_FLOW_IF, $2, $4, NULL);
								}
	| IF expr THEN stmtlist ELSE stmtlist ENDIF {
									$$ = flow_create(SNT_FLOW_IF, $2, $4, $6);
								}
	| WHILE expr DO stmtlist END {
									$$ = flow_create(	SNT_FLOW_WHILE, $2, $4,
														NULL);
								}
	|	FUNCTION IDENTIFIER '(' params ')'
			stmtlist
		END						{
									$$ = funcdecl_create(	$2, $6, $4,
															cb->global_symtab);
									free($2);	// free duplicated string
								}
	| PRINT expr				{ $$ = syntree_create(SNT_PRINT, $2, NULL); }
	;

args:
	exprlist					{ $$ = $1; }
	|							{ $$ = NULL; } // NULL
	;

exprlist:
	expr						{
									// create empty strlist-item
									strlist_t* args	= strlist_create("");
									// fill data-attribute with
									// argument-expression
									args->data		= $1;
									$$ = args;
								}
	| exprlist ',' expr			{
									// capture new item
									strlist_t* item	= strlist_append($1, "");
									// fill data-attribute
									item->data		= $3;
									$$ = $1;
								}
	;

expr:
	NUMBER						{ $$ = constval_create($1); }
	| BOOLEAN					{ $$ = constbool_create($1); }
	| STRING					{
									$$ = conststr_create($1);
									free($1);
								}
	| IDENTIFIER				{
									$$ = symref_create($1);
									free($1);	// free duplicated string
								}
	| IDENTIFIER '(' args ')'	{
									$$ = funccall_create($1, $3);
									free($1);	// free duplicated string
								}
	| IDENTIFIER ASSIGN expr	{
									$$ = syntree_create(SNT_ASSIGNMENT,
														symref_create($1), $3);
									free($1);	// free duplicated string
								}
	| expr '+' expr				{ $$ = syntree_create('+', $1, $3); }
	| expr '-' expr				{ $$ = syntree_create('-', $1, $3); }
	| expr '*' expr				{ $$ = syntree_create('*', $1, $3); }
	| expr '/' expr				{ $$ = syntree_create('/', $1, $3); }
	| expr COMPARE expr			{ $$ = comparison_create($2, $1, $3); }
	| '(' expr ')'				{ $$ = $2; }
	| '-' expr					{
									$$ = syntree_create(SNT_UNARYMINUS, $2,
														NULL);
								}
	;


%%	/* ROUTINES ------------------------------------------------------------- */


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

	cb					= codeblock_create();
	cb->global_symtab	= symtab_create();
	
	yyparse();			// parse codeblock-code
	
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
