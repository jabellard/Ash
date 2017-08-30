%{
#include <string.h>
#include <stdio.h>
#include "e.h"
// function prototypes
int yylex(void);
void yyerror(char *);

// global declarations
//static struct simple_command *sc = (struct simple_command*) NULL;
//static struct shell_pipeline *sp = (struct shell_pipeline*) NULL;
static Process *p = (Process *)NULL;
static Job *j = (Job *)NULL;
static int i = 0; // simple command flag -- controls 99hen a ne99 simple command instance should be created
static int jj = 0; // shell pipeline flag -- controls 99hen a ne99 shell pipeline instance should be created
static int job_num = 0;

%}

%union
{

	char *str;
	int i;

}

%token <i> AMPERSAND GREAT LESS PIPE NEWLINE BAD_TOKEN SEMICOLON
%token <str> ID
%start pipeline_list


%%

cmd:
	ID
	{
		//printf("command \"%s\"\n", $1);
		if (i == 0)
		{
			//create a simple command instance
			//sc = create_simple_command();
			p = create_process();
			
			
		
			
			// set simple command flag
			i = 1;
			// insert the command
			//insert_arg_in_simple_command(sc, $1);
			insert_arg_in_process(p, $1);
				
		} // end if
		else
		{
			//increment i
			//i++;
		
			// insert the command
			//insert_arg_in_simple_command(sc, $1);	
			insert_arg_in_process(p, $1);				
		} // end else
		
		if (jj == 0)
		{

			// initialize the shell pipeline instance
			//sp = create_shell_pipeline();
			j = create_job();
			
			// set shell pipeline flag 
			jj = 1;		
		
		} // end if
		
	}
;

arg:
	ID
	{
	
		 //printf("argument \"%s\"\n", $1);
		 
		 //increment i
		//i++;
		
		// insert the arg
		//insert_arg_in_simple_command(sc, $1);	
		insert_arg_in_process(p, $1);	 
	}

;

arg_list:
	arg_list arg
	{
	
		//printf(" argument list: \n");
	}
	| // empty
	{
	
	
	}

;

simple_command:
	cmd arg_list
	{
	
		 //printf("simple command \n");
		 
		 // print the command
		// print_simple_command(sc);
		 
		 // insert the command in pipeline
		//insert_simple_command_in_shell_pipeline(sp, sc);
		insert_process_in_job(j, p);
		 
		 
		 //free the simple command instance
		 //destroy_simple_command(sc);
		 destroy_process(p);
		 
		 // reset simple command flag
		 i = 0;
		 	 
	}

;

simple_command_list:
		 simple_command_list PIPE simple_command
		{
			//printf("1A\n");
		}
		| simple_command
		{
			//printf("1B\n");
		}
;
redirection:
	LESS ID 
	{
		// stdin redirection
		//printf("in redirection\n");
		
		// set the input file
		//sp->input_file = strdup($2);
		j->in_file = strdup($2);
	}
	| GREAT ID 
	{
		// stdout redirection
		//printf("out redirection\n");
		
		// set the output file
		//sp->output_file = strdup($2);
		j->out_file = strdup($2);
	}
	| '2' GREAT ID
	{
		// stderr redirection
		//printf("err redirection\n");
		
		// set the error file
		//sp->error_file = strdup($3);
		j->err_file = strdup($3);
	}
;

redirection_list:
	redirection_list redirection
	{
		//printf("redirection_list--A\n");
	}
	|
	{
		// empty
		//printf("redirection_list--B\n");	
	}
;
background: 
	AMPERSAND
	{
		//printf("background on\n");
		
		// set background flag on
		//sp->background = 1;
		j->foreground = 0;
	}
	|
	{
		// empty
		//printf("background off\n");		
	}

;
pipeline_terminator:
	NEWLINE
	{
	
		//printf("pipeline terminator--A\n");
	}
	|
	SEMICOLON
	{
	
		//printf("pipeline terminator--B\n");
	}
	|
	SEMICOLON NEWLINE
	{
		//printf("pipeline terminator--C\n");
	}
	;

pipeline:
	simple_command_list redirection_list background pipeline_terminator
	{
	
	
		// assign a number to the job
		j->id = job_num;
		//printf("p-A\n");
		
		// print the pipeline
		//print_shell_pipeline(sp);
		//print_job(j);
		
		
		
		
		// execute the pipeline
		//execute_shell_pipeline(sp);
		// EXECUTE THE JOB AND ADD IT TO THE JOB TABLE (IF APPROPRIATE)
		execute_job(j);
		
		//increase the job number
		job_num++;
		
		//notify
		do_job_notification();
		
		
		// destroy the shell pipeline instance
		//destroy_shell_pipeline(sp);
		//destroy_job(j);
		
		// print job table
		print_job_table();
		// reset shell pipeline flag
		jj = 0;
	}
	| pipeline_terminator
	{
		//printf("p-B\n");
		// print job table
		//print_job_table();
		//notify
		do_job_notification();	
	}	
	;
pipeline_list:
	pipeline_list pipeline
	{
		//printf("pl-A\n");	
	}
	|
	pipeline
	{
	
		//printf("pl-B\n");	
	}
	;
	

%%


void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

/*
int main(void) {
	// read input from a string
	//YY_BUFFER_STATE *bp;
	//struct yy_buffer_state *bp;

	//char *input = "command1 | command2 | command3 | command4\n\0";

	//  connect input buffer to specified string
	//bp = yy_scan_string(input);

	// read from the buffer
	//yy_switch_to_buffer(bp);


	// parse 
	while(1)
	{
		yyparse();
	} // end while

	// delete the buffer
	//yy_delete_buffer(bp);

	// delete the string (or not)


	return 0;
}


*/

