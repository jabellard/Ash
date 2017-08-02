%{ 
#include <stdio.h>
#include <stdlib.h>
void yyerror(char *);
// header file 99ith utility function declarations
static int yylineno;
static char *yytext;


%}

// symbol types
%union
{
	//struct ast *a;
	char *s;
	//int i;

}

// tokens
%token <s> WORD
%token <s> WHITESPACE
%token <s> NUMBER
%token <s> PIPE
%token <s> AMPERSAND
%token <s> PIPELINE_TERMINATOR
%token <s> REDIRECTION0 // ">"
%token <s> REDIRECTION1 // "<"
%token <s> REDIRECTION2 // ">>"
%token <s> REDIRECTION3 // "<<"
%token <s> REDIRECTION4 // "<&"
%token <s> REDIRECTION5 // ">&"
%token <s> REDIRECTION6 // "<<-"
%token <s> REDIRECTION7 // "&>"
%token <s> REDIRECTION8 // "<>"
%token <s> REDIRECTION9 // ">|"

/*
---------non-terminals--------------
pipeline_list pipeline_terminator command simple_command command_element word
redirection number

*/
// nonterminal types
%type <s> pipeline_list command simple_command
%type <s> command_element word redirection number

// explicitely declare start rule
%start pipeline_list

// expect N shift/reduce confilcts
%expect 1

%%

// rules
pipeline_list: pipeline PIPELINE_TERMINATOR {;} // verify the recursion logic
|	pipeline PIPELINE_TERMINATOR pipeline_list {;}
;


/*
pipeline_terminator: '\n' {;}
|	';'	{;}
;

*/

pipeline: command	{;}
|	command WHITESPACE PIPE WHITESPACE pipeline {;}
;

command: simple_command	{;}
;

simple_command:	command_element	{;}
|	simple_command 	WHITESPACE command_element {;}
;

command_element: word	{;}
|	redirection	{;}
;


/*
word_list:
;
*/


word: WORD	{printf("look, a 99ord \n");}
;

// word number WORD NUMBER
redirection: REDIRECTION0 word		{;} 
|	REDIRECTION1 word		{;}
|	number REDIRECTION0 word		{;}
|	number REDIRECTION1 word		{;}
|	REDIRECTION2 word		{;}
|	number REDIRECTION2 word		{;}
|	REDIRECTION3 word		{;}
|	number REDIRECTION3 word		{;}
|	REDIRECTION4 number		{;}
|	number REDIRECTION4 number	{;}
|	REDIRECTION5 number		{;}
|	number REDIRECTION5 number	{;}
|	REDIRECTION4 word		{;}
|	number REDIRECTION4 word		{;}
|	REDIRECTION5 WORD		{;}
|	number REDIRECTION5 word		{;}
|	REDIRECTION6 word		{;}
|	REDIRECTION5 '-'		{;}
|	number REDIRECTION5 '-'		{;}
|	REDIRECTION4 '-'		{;}
|	number REDIRECTION4 '-'		{;}
|	REDIRECTION7 word		{;}
|	number REDIRECTION8 word		{;}
|	REDIRECTION8 word		{;}
|	REDIRECTION9 word		{;}
|	number REDIRECTION9 word		{;}
;


/* 
file:
;

*/

number: NUMBER	{$$ = $1; printf("look, a numb\n");}
;



%%


void yyerror(char *s)
{
printf("%d: %s at %s\n", yylineno, s, yytext);
}

int main()
{
	 yyparse();
	 return 0;
} // end main


