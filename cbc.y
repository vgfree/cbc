	/* DEFINITIONS ---------------------------------------------------------- */
%code requires {

#include "value.h"
#include "syntree.h"
#include "symref.h"
#include "funccall.h"
#include "funcdecl.h"
#include "exception_block_node.h"
#include "strlist.h"
#include "error_handling.h"

extern int yychar;
extern int yylineno;
const char* yy_get_token_string(int* token_id);

}

%{

void yylineno_push(int ln);
int yylineno_pop();
int yylineno_temp = -1;

%}

%union {
	CbSyntree* ast;
	char* id;
	CbString str;
	CbBoolean boolval;
	CbNumeric val;
	enum cb_comparison_type cmp;
	CbStrlist* list;
};

%token				ENDOFFILE
%token				FUNCTION
%token				PRINT
%token				IF THEN ELSE ENDIF
%token				WHILE DO END
%token				STARTSEQ STOPSEQ ONERROR ALWAYS
%token	<val>		NUMBER
%token	<id>		IDENTIFIER
%token	<str>		STRING
%token	<boolval>	BOOLEAN

%right	ASSIGN
%right	NOT
%left	'+' '-'
%left	'*' '/'

%nonassoc	<cmp>	COMPARE
%nonassoc	AND
%nonassoc	OR

%type <ast>		decllist decl stmtlist stmt expr symref
%type <list>	params paramlist exprlist args

/* Output parameter: Abstract syntax tree of the parsed codeblock */
%parse-param {CbSyntree** result_tree}
%error-verbose

/* Destructors: Free discarded symbols in case of errors */
%destructor {
	cb_syntree_free($$);
} decllist decl stmtlist stmt expr symref

%destructor {
	cb_strlist_free($$);
} params paramlist

%destructor {
	CbStrlist* item = $$;
	while (item)
	{
		cb_syntree_free(item->data);	// free syntax node
		item = item->next;				// process next item
	}
	
	cb_strlist_free($$);	// finally, free the list
} exprlist args

%destructor {
	free($$);
} IDENTIFIER STRING


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
									$$->line_no = yylineno;
								}
	;

params:
	paramlist					{ $$ = $1; }
	|							{ $$ = NULL; }	// no params were specified

paramlist:
	IDENTIFIER					{
									$$ = cb_strlist_create($1);
									free($1);	// free duplicated string
								}
	| paramlist ',' IDENTIFIER	{
									cb_strlist_append($1, $3);
									free($3);	// free duplicated string
									$$ = $1;
								}
	;

decl:
	symref						{
									$$ = cb_syntree_create(SNT_DECLARATION, $1,
														   NULL);
									$$->line_no = yylineno;
								}
	;

stmtlist:
	'|' decllist '|' stmtlist	{
									if ($4 == NULL)
										$$ = $2;
									else
									{
										$$ = cb_syntree_create(SNT_STATEMENTLIST,
															   $2, $4);
										$$->line_no = yylineno;
									}
								}
	| stmt ',' stmtlist			{
									if ($3 == NULL)
										$$ = $1;
									else
									{
										$$ = cb_syntree_create(SNT_STATEMENTLIST,
															   $1, $3);
										$$->line_no = yylineno;
									}
								}
	|							{ $$ = NULL; }
	;


/*
	If an keyword occurs, that initiates a complex syntax node, the line number
	of this keyword will be temporarily stored, since a the syntax node could
	span over several lines.
*/
function_keyword:
	FUNCTION					{ yylineno_push(yylineno); }
	;
while_keyword:
	WHILE						{ yylineno_push(yylineno); }
	;
if_keyword:
	IF							{ yylineno_push(yylineno); }
	;
startseq_keyword:
	STARTSEQ					{ yylineno_push(yylineno); }
	;

/* Symbol reference */
symref:
	IDENTIFIER					{
									$$ = cb_symref_create($1);
									$$->line_no = yylineno;
									free($1);	// free duplicated string
								}
	;

stmt:
	expr						{ $$ = $1; }
	| if_keyword expr THEN stmtlist ENDIF {
									$$ = cb_flow_create(SNT_FLOW_IF, $2, $4,
														NULL);
									$$->line_no = yylineno_pop();
								}
	| if_keyword expr THEN stmtlist ELSE stmtlist ENDIF {
									$$ = cb_flow_create(SNT_FLOW_IF, $2, $4, $6);
									$$->line_no = yylineno_pop();
								}
	| while_keyword expr DO stmtlist END {
									$$ = cb_flow_create(SNT_FLOW_WHILE, $2, $4,
														NULL);
									$$->line_no = yylineno_pop();
								}
	|	function_keyword IDENTIFIER '(' params ')'
			stmtlist
		END						{
									$$ = cb_funcdecl_create($2, $6, $4);
									$$->line_no = yylineno_pop();
									free($2);	// free duplicated string
								}
	| PRINT expr				{
									// TODO: THE PRINT COMMAND IS OBSOLETE
									//       -> REMOVE IT FROM SYNTAX
									$$ = cb_syntree_create(SNT_PRINT, $2, NULL);
									$$->line_no = yylineno;
								}
	|	startseq_keyword
			stmtlist
		ONERROR
			stmtlist
		STOPSEQ					{
									$$ = cb_exception_block_create(EXBL_ONERROR,
																   $2, $4);
									$$->line_no = yylineno_pop();
								}
	|	startseq_keyword
			stmtlist
		ALWAYS
			stmtlist
		STOPSEQ					{
									$$ = cb_exception_block_create(EXBL_ALWAYS,
																   $2, $4);
									$$->line_no = yylineno_pop();
								}
	;

args:
	exprlist					{ $$ = $1; }
	|							{ $$ = NULL; } // NULL
	;

exprlist:
	expr						{
									// create empty strlist-item
									CbStrlist* args	= cb_strlist_create("");
									// fill data-attribute with
									// argument-expression
									args->data		= $1;
									$$ = args;
								}
	| exprlist ',' expr			{
									// capture new item
									CbStrlist* item	= cb_strlist_append($1, "");
									// fill data-attribute
									item->data		= $3;
									$$ = $1;
								}
	;

expr:
	NUMBER						{
									$$ = cb_constval_create($1);
									$$->line_no = yylineno;
								}
	| BOOLEAN					{
									$$ = cb_constbool_create($1);
									$$->line_no = yylineno;
								}
	| STRING					{
									$$ = cb_conststr_create($1);
									$$->line_no = yylineno;
									free($1);
								}
	| symref					{ $$ = $1; }
	| IDENTIFIER '(' args ')'	{
									$$ = cb_funccall_create($1, $3);
									$$->line_no = yylineno;
									free($1);	// free duplicated string
								}
	| symref ASSIGN expr		{
									$$ = cb_syntree_create(SNT_ASSIGNMENT, $1,
														   $3);
									$$->line_no = yylineno;
								}
	| expr '+' expr				{
									$$ = cb_syntree_create('+', $1, $3);
									$$->line_no = yylineno;
								}
	| expr '-' expr				{
									$$ = cb_syntree_create('-', $1, $3);
									$$->line_no = yylineno;
								}
	| expr '*' expr				{
									$$ = cb_syntree_create('*', $1, $3);
									$$->line_no = yylineno;
								}
	| expr '/' expr				{
									$$ = cb_syntree_create('/', $1, $3);
									$$->line_no = yylineno;
								}
	| expr COMPARE expr			{
									$$ = cb_comparison_create($2, $1, $3);
									$$->line_no = yylineno;
								}
	| expr AND expr				{
									$$ = cb_syntree_create(SNT_LOGICAL_AND, $1, $3);
								}
	| expr OR expr				{
									$$ = cb_syntree_create(SNT_LOGICAL_OR, $1, $3);
								}
	| NOT expr					{
									$$ = cb_syntree_create(SNT_LOGICAL_NOT, $2, NULL);
								}
	| '(' expr ')'				{ $$ = $2; }
	| '-' expr					{
									$$ = cb_syntree_create(SNT_UNARYMINUS, $2,
														   NULL);
									$$->line_no = yylineno;
								}
	;


%%	/* ROUTINES ------------------------------------------------------------- */


// -----------------------------------------------------------------------------
// This simple hack allows functions - that are not located in this file - to
// access the token names as string by its token id.
// -----------------------------------------------------------------------------
const char* yy_get_token_string(int* token_id)
{
	return yytname[YYTRANSLATE(*token_id)];
}

// -----------------------------------------------------------------------------
// Push current line number
// -----------------------------------------------------------------------------
void yylineno_push(int ln)
{
	yylineno_temp = ln;
}

// -----------------------------------------------------------------------------
// Pop stored line number
// -----------------------------------------------------------------------------
int yylineno_pop()
{
	int ln		  = yylineno_temp;
	yylineno_temp = -1;	// reset temporarily stored line number
	
	return ln;
}
