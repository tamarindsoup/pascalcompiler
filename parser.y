%{
/* 
 * parser; Parser for PL-*
 */

#define MAXLENGTH 16

#include <stdio.h>
#include "stack.h"
#include "gen.h"
  
extern int yylineno;
extern char *yytext;
 
int yylex();
int yyerror();

int depth = 0;
int nextinst = 0;
Item *item;

%}

%union {
    int num;
    char ident[MAXLENGTH+1];
}

%token SBEGIN DO ELSE SEND
%token FOR FORWARD FUNCTION IF PROCEDURE
%token PROGRAM READ THEN TO VAR
%token WHILE WRITE

%left PLUS MINUS
%left MULT DIV

%token EQ NEQ LE LT GE GT
%token LPAREN RPAREN LBRACKET RBRACKET
%token COMMA SEMICOLON COLON INTERVAL
%token PERIOD ASSIGN
%token <num> NUMBER
%token <ident> IDENT

%type <num> label

%%

program
        : PROGRAM IDENT SEMICOLON outblock PERIOD
	{ output(); }
        ;

outblock
        : var_decl_part subprog_decl_part statement
        ;

var_decl_part
        : /* empty */
        | var_decl_list SEMICOLON
        ;
var_decl_list
	: var_decl_list SEMICOLON var_decl
	| var_decl
	;
var_decl
	: VAR id_list
	;
subprog_decl_part
	: subprog_decl_list SEMICOLON
	| /*empty*/
	;
subprog_decl_list
	: subprog_decl_list SEMICOLON subprog_decl
	| subprog_decl
	;
subprog_decl
	: proc_decl
	;
proc_decl
	: PROCEDURE label proc_name SEMICOLON inblock
	{ 
	  delete(); depth--;
	  generate(RTN, 0, 0, 0); nextinst++;
	  backpatch($2, nextinst);
	}
	;
proc_name
	: IDENT
	{ 
	  generate(JMP, 0, 0, 0); nextinst++;
	  insert($1, nextinst ,PROC); depth++;
	}
	;
inblock
	: var_decl_part statement
	;
statement_list
	: statement_list SEMICOLON statement
	| statement
	;
statement
	: assignment_statement
	| if_statement
	| while_statement
	| for_statement
	| proc_call_statement
	| null_statement
	| block_statement
	| read_statement
	| write_statement
	;
assignment_statement
	: IDENT ASSIGN expression
	{ 
	  item = lookup($1);
	  generate(STO, item->type, 0, item->addr);
	  nextinst++;
	}
	;
if_statement
        : IF condition label
	{ generate(JPC, 0, 0, 0); nextinst++; }
        THEN statement label
	{
	  generate(JMP, 0, 0, 0); nextinst++;
	  backpatch($3, nextinst);
	}
        else_statement
	{ backpatch($7, nextinst); }
	;
else_statement
	: ELSE statement
        | /*empty*/
	;
while_statement
        : WHILE label condition label
	{ generate(JPC, 0, 0, 0); nextinst++; }
        DO statement
	{ 
	  generate(JMP, 0, 0, $2); nextinst++; 
	  backpatch($4, nextinst);
	}
	;
for_statement
        : FOR IDENT ASSIGN expression
	{ 
	  item = lookup($2); 
	  generate(STO, item->type, 0, item->addr); nextinst++;
	}
        TO label expression
	{
	  item = lookup($2); 
	  generate(LOD, item->type, 0, item->addr); nextinst++;
	  generate(OPR, 0, 0, 10); nextinst++; 
	}
	label
        {
	  generate(JPC, 0, 0, 0); nextinst++; 
	} 
        DO statement
	{
	  item = lookup($2);  
	  generate(LOD, item->type, 0, item->addr); nextinst++;
	  generate(LIT, 0, 0, 1); nextinst++;
	  generate(OPR, 0, 0, 1); nextinst++;
	  generate(STO, item->type, 0, item->addr); nextinst++;
	  generate(JMP, 0, 0, $7); nextinst++;
	  backpatch($10, nextinst);
	}
	;
proc_call_statement
	: proc_call_name
	;
proc_call_name
	: IDENT
	{ 
	  item = lookup($1);
	  generate(CAL, item->type, 0 , item->addr); nextinst++;
	}
	;
block_statement
	: SBEGIN statement_list SEND
	;
read_statement
	: READ LPAREN IDENT RPAREN
	{ 
	  item = lookup($3); 
	  generate(GET, 0, 0, 0); nextinst++;
	  generate(STO, item->type, 0, item->addr); nextinst++;
	}
	;
write_statement
	: WRITE LPAREN expression RPAREN
	{ generate(PUT, 0, 0, 0); nextinst++; }
	;
null_statement
	: /*empty*/
	;
condition
	: expression EQ expression
	{ generate(OPR, 0, 0, 5); nextinst++; }
	| expression NEQ expression
	{ generate(OPR, 0, 0, 6); nextinst++; }
	| expression LT expression
	{ generate(OPR, 0, 0, 7); nextinst++; }
	| expression LE expression
	{ generate(OPR, 0, 0, 8); nextinst++; }
	| expression GT expression
	{ generate(OPR, 0, 0, 9); nextinst++; }
	| expression GE expression
	{ generate(OPR, 0, 0, 10); nextinst++; }
	;
expression
	: term
	| PLUS term
	| MINUS term
	{ generate(OPR, 0, 0, 0); nextinst++; }
	| expression PLUS term
	{ generate(OPR, 0, 0, 1); nextinst++; }
	| expression MINUS term
	{ generate(OPR, 0, 0, 2); nextinst++; }
	;
term
	: factor
	| term MULT factor
	{ generate(OPR, 0, 0, 3); nextinst++; }
	| term DIV factor
	{ generate(OPR, 0, 0, 4); nextinst++; }
	;
factor
	: var_name
	| NUMBER
	{ generate(LIT, 0, 0, $1); nextinst++; }
	| LPAREN expression RPAREN
	;
var_name
	: IDENT
	{ 
	  item = lookup($1);
	  generate(LOD, item->type, 0, item->addr);
	  nextinst++;
	}
	;
arg_list
	: expression
	| arg_list COMMA expression
	;
id_list
        : IDENT
	{ 
	  insert($1, 0, (depth ? LOC : GLOB)); 
	  generate(LIT, 0, 0, 0);
	  nextinst++;
	}
        | id_list COMMA IDENT
	{ 
	  insert($3, 0, (depth ? LOC : GLOB)); 
	  generate(LIT, 0, 0, 0);
	  nextinst++;
	}
	;
label
        : /*empty*/
        { $$ = nextinst; } 
        ;


%%
int
yyerror(char *s)
{
  fprintf(stderr,
	  "%s\n%d: %s\n",
	  s, yylineno, yytext);
}
