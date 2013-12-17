	/* DEFINITIONS ---------------------------------------------------------- */
%{

#include <stdio.h>
#include "syntree.h"
#include "symtab.h"

%}

%union {
	syntree* ast;
	char* id;
	int value;
	enum cmp_nodetype_t cmp;
};

%token			ENDOFFILE
%token			DECLARE
%token			IF THEN ELSE ENDIF
%token			WHILE DO END
%token	<value>	NUMBER
%token	<id>	IDENTIFIER

%right	ASSIGN
%left	'+' '-'
%left	'*' '/'

%nonassoc	<cmp>	COMPARE

%type <ast> stmtlist stmt identifier expr


%%	/* RULES ---------------------------------------------------------------- */


prog:
	stmtlist ENDOFFILE			{
									printf("%d", eval($1));
									syntree_free($1);
									YYACCEPT;
								}
	;

stmtlist:
	stmt ',' stmtlist			{
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
	| DECLARE identifier		{
									$$ = syntree_create(SNT_DECLARATION, $2,
														NULL);
								}
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


int main(void)
{
	gl_symtab = symtab_create();
	yyparse();
	symtab_free(gl_symtab);
	return 0;
}
