	/* DEFINITIONS ---------------------------------------------------------- */
%{

#include <stdio.h>
#include <stdlib.h>
#include "cbvalues.h"
#include "syntree.h"
#include "symtab.h"
#include "symref.h"
#include "fncall.h"
#include "fndecl.h"

syntree* result_tree;

%}

%union {
	syntree* ast;
	struct symbol_t* sym;
	char* id;
	cbstring str;
	cbboolean boolval;
	cbnumeric value;
	enum comparisontype_t cmp;
};

%token				ENDOFFILE
%token				FUNCTION
%token				PRINT
%token				IF THEN ELSE ENDIF
%token				WHILE DO END
%token	<value>		NUMBER
%token	<id>		IDENTIFIER
%token	<str>		STRING
%token	<boolval>	BOOLEAN

%right	ASSIGN
%left	'+' '-'
%left	'*' '/'

%nonassoc	<cmp>	COMPARE

%type <ast> decllist decl paramlist_decl stmtlist stmt id expr
%type <sym> params_decl


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

paramlist_decl:
	params_decl					{ $$ = symref_create($1); }
	|							{ $$ = NULL; }	// NULL

params_decl:
	IDENTIFIER					{
									symbol* symtab	= symtab_create();
									symbol* sym		= symbol_create(SYM_VARIABLE,
																	$1);
									// identifier was copied in symbol creation
									// -> free identifier-string
									free($1);
									variable_declare(symtab, sym);
									$$ = symtab;
								}
	| params_decl ',' IDENTIFIER{
									symbol* sym = symbol_create(SYM_VARIABLE, $3);
									// identifier was copied in symbol creation
									// -> free identifier-string
									free($3);
									variable_declare($1, sym);
									$$ = $1;
								}
	;

decl:
	id							{
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
	|	FUNCTION IDENTIFIER '(' paramlist_decl ')'
			stmtlist
		END						{
									symbol* f = symbol_create(SYM_UNDEFINED, $2);
									$$ = fndecl_create(f, $6, $4);
									// identifier was copied in symbol creation
									// -> free identifier-string
									free($2);
								}
	| PRINT expr				{ $$ = syntree_create(SNT_PRINT, $2, NULL); }
	;

id:
	IDENTIFIER					{
									// TODO: result should be a symbol!
									$$ = symref_create(
											symbol_create(SYM_UNDEFINED, $1));
									// identifier was copied in symbol creation
									// -> free identifier-string
									free($1);
								}
	;

expr:
	NUMBER						{ $$ = constval_create($1); }
	| BOOLEAN					{ $$ = constbool_create($1); }
	| STRING					{
									$$ = conststr_create($1);
									free($1);
								}
	| id						{ $$ = $1; }
	| IDENTIFIER '(' ')'		{
									$$ = fncall_create(
											symbol_create(SYM_UNDEFINED, $1));
									// identifier was copied in symbol creation
									// -> free identifier-string
									free($1);
								}
	| id ASSIGN expr			{ $$ = syntree_create(SNT_ASSIGNMENT, $1, $3); }
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
		cbvalue* cbresult = eval(result_tree);
		cbvalue_print(cbresult);
		cbvalue_free(cbresult);
		syntree_free(result_tree);
	}
	
	symtab_free(gl_symtab);
	return 0;
}
