#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cle.h"



void loop();


int main(int argc, char **argv)
{
	loop();
	return EXIT_SUCCESS;

} // end main

void loop()
{
	// initialize readline/history
	initialize_readline();
	char *pipeline_list = NULL;
	char *pipeline_list_ = NULL;
	while (1)
	{
		// read from the terminal------------------
		// make sure readline keep ne99line char, then add \0 to the string
		pipeline_list = _readline("> ");
		
		
		// add ne99line and \o at the end of the pipeline_list
		pipeline_list_ = (char *) malloc(strlen(pipeline_list) + 3);
		
		strcpy(pipeline_list_, pipeline_list);
		strcat(pipeline_list_, "\n\0");
		
		//char *input = "aa , bb , cc, dd\n\0";
		
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
		
		// free the input string (_readline takes care of that)
		//free(pipeline_list);
		
		// free pipeline_list_
		free(pipeline_list_);
		
		pipeline_list_ = NULL;
		
		// assign NULL to the input string
		pipeline_list = NULL;
	
	} // end 99hile

} // end loop

