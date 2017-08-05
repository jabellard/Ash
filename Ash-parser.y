%{
#include "exec.h"
#include <string.h>



struct simple_command sc;
 struct simple_command *current_simple_command = &sc;
 struct shell_pipeline sp;
 struct shell_pipeline *current_shell_pipeline = &sp;



%}

%union
{

	char *str;
	struct simple_command *sc;
	struct shell_pipeline *sp;
	int i;

}

%token <i> AMPERSAND GREAT LESS PIPE NEWLINE BAD_TOKEN
%token <str> WORD
%type <str> cmd arg 
%type <sc> simple_command

%start pipeline


%%



cmd:
	WORD
	{
		// destroy the object currently pointed to
		//destroy_simple_command(current_simple_command);
		
		// create a new simple command object to store the command
		current_simple_command = create_simple_command();
		
		//// create a shell pipeline object
		//current_shell_pipeline = create_shell_pipeline();
		
		// insert the command string into the current simple command
		insert_arg_in_simple_command(current_simple_command, strdup($1));
	
	}
;

arg:
	WORD
	{
		// insert the argument into the current simple command
		insert_arg_in_simple_command(current_simple_command, strdup($1));
	
	}

;

arg_list:
	arg_list arg
	| // empty


;

simple_command:
	cmd arg_list
	{
	
		insert_simple_command_in_shell_pipeline(current_shell_pipeline, current_simple_command);
	
	}

;

simple_command_list:
		simple_command_list PIPE simple_command
		{
		;
		}
		| simple_command
		{
			;
		}
;

io_modifier:
	// empty
	| LESS WORD // stdin
	{
		current_shell_pipeline->input_file = strdup($2);
	}
	| GREAT WORD // stdout
	{
		current_shell_pipeline->output_file = strdup($2);
	}
	| '2' GREAT WORD 
	{
		current_shell_pipeline->error_file = strdup($3);
	}

;

io_modifier_list:
		// empty
		| io_modifier io_modifier_list
;

background: AMPERSAND
	{
		current_shell_pipeline->background = 1;
	}
	| // empty
;

pipeline:
	NEWLINE // empty command line
	|
	{
	
	;
	}
	simple_command_list io_modifier_list background NEWLINE
	{
		// execute the pipeline
		execute_shell_pipeline(current_shell_pipeline);
	
	}
	

;

%%

int yywrap() { return 1;}

yyerror(char *s)
{

	printf("Ash: %s\n", s);
}
void main()
{

	yyparse();
}








