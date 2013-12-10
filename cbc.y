	/* DEFINITIONS ---------------------------------------------------------- */
%{

#include <stdio.h>
#include "syntree.h"
#include "symtab.h"

%}

%union {
	syntree* ast;
	symbol* sym;
	int value;
};

%token <value>	NUMBER
%token <sym>	IDENTIFIER
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
	| IDENTIFIER		{ $$ = symref_create($1); }
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
	gl_symtab = symtab_create();
	yyparse();
	symtab_free(gl_symtab);
	return 0;
}
