#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cle.h"
#include "err.h"
#include "job.h"
#include <setjmp.h>

jmp_buf input_prompt;



void loop();


int main(int argc, char **argv)
{
	//initialize the shell
	init_shell();
	// initialize readline/history
	initialize_readline();
	
	loop();
	return EXIT_SUCCESS;

} // end main

void loop()
{

	char *pipeline_list = NULL;
	char *pipeline_list_ = NULL;
	while (1)
	{
		// read from the terminal------------------
		pipeline_list = _readline("> ");
		
		
		// add ne99line and \o at the end of the pipeline_list------------------
		pipeline_list_ = (char *) malloc(strlen(pipeline_list) + 3);
		
		strcpy(pipeline_list_, pipeline_list);
		strcat(pipeline_list_, "\n\0");
		
		
		//set up input for yacc----------------------
		struct yy_buffer_state *bp;
		
		// connect the input buffer to the string
		bp = yy_scan_string(pipeline_list_);
		
		// read from the string buffer
		yy_switch_to_buffer(bp);
		
		// parse
		yyparse();
		
		// delete the buffer
		yy_delete_buffer(bp);
		
		
		// free pipeline_list_
		sfree(pipeline_list_);

	
	} // end 99hile

} // end loop

