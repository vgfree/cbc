	/* DEFINITIONS ---------------------------------------------------------- */
%code requires {

#include "value.h"
#include "syntree.h"
#include "symref.h"
#include "funccall.h"
#include "funcdecl.h"
#include "strlist.h"

}

%union {
	CbSyntree* ast;
	char* id;
	CbString str;
	CbBoolean boolval;
	CbNumeric val;
	enum cb_comparison_type cmp;
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

/* Output parameter: Abstract syntax tree of the parsed codeblock */
%parse-param {CbSyntree** result_tree}

%%	/* RULES ---------------------------------------------------------------- */


prog:
	stmtlist ENDOFFILE			{
									*result_tree = $1;
									YYACCEPT;
								}
	;

decllist:
	decl						{ $$ = $1; }
	| decl ',' decllist			{
									$$ = cb_syntree_create(SNT_STATEMENTLIST,
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
									$$ = cb_syntree_create(SNT_DECLARATION,
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
										$$ = cb_syntree_create(SNT_STATEMENTLIST,
															   $2, $4);
								}
	| stmt ',' stmtlist			{
									if ($3 == NULL)
										$$ = $1;
									else
										$$ = cb_syntree_create(SNT_STATEMENTLIST,
															   $1, $3);
								}
	|							{ $$ = NULL; }
	;

stmt:
	expr						{ $$ = $1; }
	| IF expr THEN stmtlist ENDIF {
									$$ = cb_flow_create(SNT_FLOW_IF, $2, $4,
														NULL);
								}
	| IF expr THEN stmtlist ELSE stmtlist ENDIF {
									$$ = cb_flow_create(SNT_FLOW_IF, $2, $4, $6);
								}
	| WHILE expr DO stmtlist END {
									$$ = cb_flow_create(SNT_FLOW_WHILE, $2, $4,
														NULL);
								}
	|	FUNCTION IDENTIFIER '(' params ')'
			stmtlist
		END						{
									$$ = funcdecl_create($2, $6, $4);
									free($2);	// free duplicated string
								}
	| PRINT expr				{ $$ = cb_syntree_create(SNT_PRINT, $2, NULL); }
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
	NUMBER						{ $$ = cb_constval_create($1); }
	| BOOLEAN					{ $$ = cb_constbool_create($1); }
	| STRING					{
									$$ = cb_conststr_create($1);
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
									$$ = cb_syntree_create(SNT_ASSIGNMENT,
														   symref_create($1), $3);
									free($1);	// free duplicated string
								}
	| expr '+' expr				{ $$ = cb_syntree_create('+', $1, $3); }
	| expr '-' expr				{ $$ = cb_syntree_create('-', $1, $3); }
	| expr '*' expr				{ $$ = cb_syntree_create('*', $1, $3); }
	| expr '/' expr				{ $$ = cb_syntree_create('/', $1, $3); }
	| expr COMPARE expr			{ $$ = cb_comparison_create($2, $1, $3); }
	| '(' expr ')'				{ $$ = $2; }
	| '-' expr					{
									$$ = cb_syntree_create(SNT_UNARYMINUS, $2,
														   NULL);
								}
	;


%%	/* ROUTINES ------------------------------------------------------------- */
