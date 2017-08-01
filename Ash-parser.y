%{ 

// header file 99ith utility function declarations


%}

// symbol types
%union
{
	struct ast *a;
	char *c;
	int i;

}

// tokens
%token WORD
%token WHITESPACE
%token NUMBER
%token PIPE
%token AMPERSAND
%token PIPELINE_TERMINATOR
%token REDIRECTION0 // ">"
%token REDIRECTION1 // "<"
%token REDIRECTION2 // ">>"
%token REDIRECTION3 // "<<"
%token REDIRECTION4 // "<&"
%token REDIRECTION5 // ">&"
%token REDIRECTION6 // "<<-"
%token REDIRECTION7 // "&>"
%token REDIRECTION8 // "<>"
%token REDIRECTION9 // ">|"


// nonterminal types
%type 

// explicitely declare start rule

%%

// rules
pipeline_list: pipeline PIPELINE_TERMINATOR // verify the recursion logic
|	PIPELINE_TERMINATOR pipeline_list
;

pipeline_terminator: '\n'
|	';'

pipeline: command
|	command WHITESPACE PIPE WHITESPACE pipeline {}
;

command: simple_command	{}
;

simple_command:	command_element	{}
|	simple_command 	WORD_DELIMITER command_element {}
;

command_element: word	{}
|	redirection	{}
;


/*
word_list:
;
*/


word: WORD	{$$ = $1;}
;

// word number WORD NUMBER
redirection: REDIRECTION0 WORD		{} 
|	REDIRECTION1 WORD		{}
|	NUMBER REDIRECTION0 WORD		{}
|	NUMBER REDIRECTION1 WORD		{}
|	REDIRECTION2 WORD		{}
|	NUMBER REDIRECTION2 WORD		{}
|	REDIRECTION3 WORD		{}
|	NUMBER REDIRECTION3 WORD		{}
|	REDIRECTION4 NUMBER		{}
|	NUMBER REDIRECTION4 NUMBER	{}
|	REDIRECTION5 NUMBER		{}
|	NUMBER REDIRECTION5 NUMBER	{}
|	REDIRECTION4 WORD		{}
|	NUMBER REDIRECTION4 WORD		{}
|	REDIRECTION5 WORD		{}
|	NUMBER REDIRECTION5 WORD		{}
|	REDIRECTION6 WORD		{}
|	REDIRECTION5 '-'		{}
|	NUMBER REDIRECTION5 '-'		{}
|	REDIRECTION4 '-'		{}
|	NUMBER REDIRECTION4 '-'		{}
|	REDIRECTION7 WORD		{}
|	NUMBER REDIRECTION8 WORD		{}
|	REDIRECTION8 WORD		{}
|	REDIRECTION9 WORD		{}
|	NUMBER REDIRECTION9 WORD		{}
;


/* 
file:
;

*/

number: NUMBER	{$$ = $1;}
;



%%
