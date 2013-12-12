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
	enum cmp_nodetype_t cmp;
};

%token			ENDOFFILE
%token			DECLARE
%token			IF THEN ELSE ENDIF
%token			WHILE DO END
%token	<value>	NUMBER
%token	<sym>	IDENTIFIER

%right	ASSIGN
%left	'+' '-'
%left	'*' '/'

%nonassoc	<cmp>	COMPARE

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
	| IF expr THEN stmt ENDIF ',' {
									$$ = flow_create(SNT_FLOW_IF, $2, $4, NULL);
								}
	| IF expr THEN stmt ELSE stmt ENDIF ','	{
									$$ = flow_create(SNT_FLOW_IF, $2, $4, $6);
								}
	| WHILE expr DO stmt END ',' {
									$$ = flow_create(SNT_FLOW_WHILE, $2, $4, NULL);
								}
	;

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
	| expr COMPARE expr			{ $$ = comparison_create($2, $1, $3); }
	| '(' expr ')'				{ $$ = $2; }
	| '-' expr					{
									$$ = syntree_create(SNT_UNARYMINUS, $2,
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
