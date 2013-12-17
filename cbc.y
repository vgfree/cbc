	/* DEFINITIONS ---------------------------------------------------------- */
%{

#include <stdio.h>
#include <stdlib.h>
#include "syntree.h"
#include "symtab.h"

syntree* result_tree;

%}

%union {
	syntree* ast;
	char* id;
	int value;
	enum cmp_nodetype_t cmp;
};

%token			ENDOFFILE
%token			IF THEN ELSE ENDIF
%token			WHILE DO END
%token	<value>	NUMBER
%token	<id>	IDENTIFIER

%right	ASSIGN
%left	'+' '-'
%left	'*' '/'

%nonassoc	<cmp>	COMPARE

%type <ast> decllist decl stmtlist stmt identifier expr


%%	/* RULES ---------------------------------------------------------------- */


prog:
	stmtlist ENDOFFILE			{
									result_tree = $1;
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

decl:
	identifier					{
									$$ = syntree_create(SNT_DECLARATION, $1,
														NULL);
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
	;

identifier:
	IDENTIFIER					{
									$$ = symref_create(
											symbol_create(SYM_UNDEFINED, $1));
									// string was copied in symbol creation
									// -> free string
									free($1);
								}
	;

expr:
	NUMBER						{ $$ = constval_create($1); }
	| identifier				{ $$ = $1; }
	| identifier ASSIGN expr	{ $$ = syntree_create(SNT_ASSIGNMENT, $1, $3); }
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
	
	if (argc > 1)
	{
		FILE* input = fopen(argv[1], "r");
		if (!input)
		{
			printf("error: unable to open file `%s'!\n", argv[1]);
			exit(1);
		}
		
		yyin = input;
	}
	
	gl_symtab = symtab_create();
	yyparse();
	
	if (result_tree)
	{
		printf("%d", eval(result_tree));
		syntree_free(result_tree);
	}
	
	symtab_free(gl_symtab);
	return 0;
}
