	/* DEFINITIONS ---------------------------------------------------------- */
%{

#include <stdio.h>
#include "syntree.h"
#include "symtab.h"

int last_result = -1;

%}

%union {
	syntree* ast;
	symbol* sym;
	int value;
};

%token			ENDOFFILE
%token			DECLARE
%token	<value>	NUMBER
%token	<sym>	IDENTIFIER

%right	ASSIGN
%left	'+' '-'
%left	'*' '/'

%type <ast> stmt expr


%%	/* RULES ---------------------------------------------------------------- */


prog:
	prog stmt					{
									last_result = eval($2);
									syntree_free($2);
								}
	| prog ENDOFFILE			{ printf("%d", last_result); YYACCEPT; }
	|							// NULL
	;

stmt:
	expr ','					{ $$ = $1; }
	| DECLARE IDENTIFIER ','	{
									$$ = syntree_create(SNT_DECLARATION,
														symref_create($2), NULL);
								}

expr:
	NUMBER						{ $$ = constval_create($1); }
	| IDENTIFIER				{ $$ = symref_create($1); }
	| IDENTIFIER ASSIGN expr	{
									$$ = syntree_create(SNT_ASSIGNMENT,
														symref_create($1), $3);
								}
	| expr '+' expr				{ $$ = syntree_create('+', $1, $3); }
	| expr '-' expr				{ $$ = syntree_create('-', $1, $3); }
	| expr '*' expr				{ $$ = syntree_create('*', $1, $3); }
	| expr '/' expr				{ $$ = syntree_create('/', $1, $3); }
	| '(' expr ')'				{ $$ = $2; }
	| '-' expr					{
									$$ = syntree_create(SNT_SIGNED_MINUS, $2,
														NULL);
								}
	;


%%	/* ROUTINES ------------------------------------------------------------- */


int main(void)
{
	gl_symtab = symtab_create();
	yyparse();
	symtab_free(gl_symtab);
	return 0;
}
