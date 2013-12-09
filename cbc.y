	/* DEFINITIONS ---------------------------------------------------------- */
%{

#include <stdio.h>
#include "syntree.h"

%}

%union {
	syntree* ast;
	int value;
};

%token <value> NUMBER
%left '+' '-'
%left '*' '/'

%type <ast> stmt expr

%%	/* RULES ---------------------------------------------------------------- */

prog:
	prog stmt			{
							printf("%d\n", eval($2));
							syntree_free($2);
						}
	|					// NULL
	;

stmt:
	expr ','			{ $$ = $1; }

expr:
	NUMBER				{ $$ = constval_create($1); }
	| expr '+' expr		{ $$ = syntree_create('+', $1, $3); }
	| expr '-' expr		{ $$ = syntree_create('-', $1, $3); }
	| expr '*' expr		{ $$ = syntree_create('*', $1, $3); }
	| expr '/' expr		{ $$ = syntree_create('/', $1, $3); }
	| '(' expr ')'		{ $$ = $2; }
	| '-' expr			{ $$ = syntree_create(SNT_SIGNED_MINUS, $2, NULL); }
	;

%%	/* ROUTINES ------------------------------------------------------------- */

int main(void)
{
	yyparse();
	return 0;
}
