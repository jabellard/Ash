%{
#include <string.h>
#include <stdio.h>
#include "job.h"
#include <setjmp.h>

// function prototypes-------------------
int yylex(void);
void yyerror(char *);


extern jmp_buf input_prompt;
static Process *p = (Process *)NULL;
static Job *j = (Job *)NULL;
static int cp = 0; // this flag controls 99hen a ne99 process instance should be created.
static int cj = 0; // this flag controls 99hen a ne99 process instance should be create.
static int job_num = 0; // last assigned job number

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
		if (cp == 0)
		{
			// create a ne99 process isntance
			p = create_process();
			
			
		
			
			// set the create process flag 
			cp = 1;
			
			//insert the argument in the current process 
			insert_arg_in_process(p, $1);
				
		} // end if
		else
		{

			//insert the argument in the current process
			insert_arg_in_process(p, $1);				
		} // end else
		
		if (cj == 0)
		{

			// create a ne99 job instance
			j = create_job();
			
			// set the create job flag
			cj = 1;		
		
		} // end if
		
	}
;

arg:
	ID
	{
	
		//insert the argument in the current process	
		insert_arg_in_process(p, $1);	 
	}

;

arg_list:
	arg_list arg
	{
	
	}
	| // empty
	{
	
	
	}

;

simple_command:
	cmd arg_list
	{
	
		// insert the current process instance in the current job instance
		insert_process_in_job(j, p);
		 
		 
		// destroy the current simple process instance
		 destroy_process(p);
		 
		 // reset the create process flag
		 cp = 0;
		 	 
	}

;

simple_command_list:
		 simple_command_list PIPE simple_command
		{

		}
		| simple_command
		{

		}
;
redirection:
	LESS ID 
	{
		// stdin redirection
		
		// set the input file
		j->in_file = strdup($2);
	}
	| GREAT ID 
	{
		// stdout redirection
		
		// set the output file
		j->out_file = strdup($2);
	}
	| '2' GREAT ID
	{
		// stderr redirection
		
		// set the error file
		j->err_file = strdup($3);
	}
;

redirection_list:
	redirection_list redirection
	{

	}
	|
	{
		// empty
	}
;
background: 
	AMPERSAND
	{
		
		// reset the foreground flag for the current job instance
		j->foreground = 0;
	}
	|
	{
		// empty
	}

;
pipeline_terminator:
	NEWLINE
	{
	
	}
	|
	SEMICOLON
	{
	
	}
	|
	SEMICOLON NEWLINE
	{
	
	}
	;

pipeline:
	simple_command_list redirection_list background pipeline_terminator
	{
	
	
		// assign a number to the job
		j->id = job_num;
		
		//printf("p-A\n");
		
		
/*
		// execute the job, and check if everything 99ent 99ell
		if (execute_job(j) == -1)
		{
			// destroy the job
			//destroy_job(j);
			
			// stop parsing and return back to the shell prompt
			//1. make sure parse ignores the rest of the input
			
			//2. return to to shell prompt
			//printf("returning to shell prompt\n");
			//YYERROR;
			//longjmp(input_prompt, 1);
		} // end if
		else
		{
			// increase the job number
			job_num++;
		} // end else

*/		
		
		//execute the job
		execute_job(j);
		
		// increase the job number
		job_num++;
		
		// do a job notification
		do_job_notification();
		
		// print job table

		// reset the create job flag
		cj = 0;
	}
	| pipeline_terminator
	{

		// do a job notification
		do_job_notification();	
	}	
	;
pipeline_list:
	pipeline_list pipeline
	{

	}
	|
	pipeline
	{
	

	}
	;
	

%%


void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}


