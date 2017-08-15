%{
#include "exec.h"
#include <string.h>
#include <stdio.h>
int yylex(void);
int yylineno;
void yyerror(char *);
static struct simple_command *sc;
static struct shell_pipeline *sp;
static int i = 0;
static int j = 0;

%}

%union
{

	char *str;
	int i;

}

%token <i> AMPERSAND GREAT LESS PIPE NEWLINE BAD_TOKEN
%token <str> WORD
%start pipeline
%expect 1


%%

cmd:
	WORD
	{
		printf(" Yacc: inserting command \"%s\"\n", $1);
		if (i == 0)
		{
			//create a simple command instance
			sc = create_simple_command();
			
			// increment i
			i++;
			// insert the command
			insert_arg_in_simple_command(sc, $1);
		
		}
		else
		{
			//increment i
			i++;
		
			// insert the command
			insert_arg_in_simple_command(sc, $1);
		}
		
		if (j == 0)
		{
			// initialize the shell pipeline instance
			sp = create_shell_pipeline();
			
			// increment j
			j++;
		
		
		}
		
	
	}
;

arg:
	WORD
	{
	
		 printf(" Yacc: inserting argument \"%s\"\n", $1);
		 
		 //increment i
		i++;
		
		// insert the arg
		insert_arg_in_simple_command(sc, $1);
	
	}

;

arg_list:
	arg_list arg
	{
	
		//printf(" argument list: \n");
	}
	| // empty


;

simple_command:
	cmd arg_list
	{
	
		 printf(" Yacc: simple command:  \n");
		 
		 // print the command
		 print_simple_command(sc);
		 
		 // insert the command in pipeline
		insert_simple_command_in_shell_pipeline(sp, sc);
		 
		 
		 //free the simple command instance
		 destroy_simple_command(sc);
		 
		 // reset i to 0
		 i = 0;
		 
		 //print the pipeline
		// print_shell_pipeline(sp);
		 
	
	}

;
simple_command1:
	simple_command
	{
		printf("1A\n");
	}
	|
	simple_command ','
	{
			printf("1B\n");
	}
	;
simple_command2:
		simple_command1
		;
simple_command_list:
		 simple_command_list simple_command2
		{
			printf("2A\n");
			//print the pipeline
		 	//print_shell_pipeline(sp);
		}
		| simple_command2
		{
			printf("2B\n");
			//print the pipeline
		 	//print_shell_pipeline(sp);
		}
;

pipeline:
	simple_command_list NEWLINE
	{
	
	printf("start: in pipeline\n");
	}
	| error ',' { printf("You made en error1\n"); }  
	| error  WORD { printf("You made en error 2\n"); }  
	;




%%


void yyerror(char *s) {
    fprintf(stderr, "line %d: %s\n", yylineno, s);
}

int main(void) {
// read input from a string
//YY_BUFFER_STATE *bp;
struct yy_buffer_state *bp;

char *input = "aa , bb , cc, dd\n\0";

//  connect input buffer to specified string
bp = yy_scan_string(input);

// read from the buffer
yy_switch_to_buffer(bp);


// parse 
yyparse();

// delete the buffer
yy_delete_buffer(bp);

// delete the string (or not)


return 0;
}
