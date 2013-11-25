	/* DEFINITIONS ---------------------------------------------------------- */
%{

#include <stdio.h>

%}

%union {
	int value;
};

%token <value> NUMBER
%left '+' '-'

%type <value> stmt expr

%%	/* RULES ---------------------------------------------------------------- */

prog:
	prog stmt			{ printf("%d\n", $2); }
	|					// NULL
	;

stmt:
	','					{ ; }
	| expr ','			{ $$ = $1; }

expr:
	NUMBER				{ $$ = $1; }
	| expr '+' expr		{ $$ = $1 + $3; }
	| expr '-' expr		{ $$ = $1 - $3; }
	;

%%	/* ROUTINES ------------------------------------------------------------- */

int main(void)
{
	yyparse();
	return 0;
}
