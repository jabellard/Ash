/**
* @file main.c
* @author Joe Nathan Abellard {https://github.com/joenatech7}
* 
* @date September 1, 2017
* @brief Contains main().
*
* @par Description
* Contains main().
*
*/


#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cle.h"
#include "err.h"
#include "job.h"
#include <setjmp.h>


/**
* @brief Points to the @e readline buffer.
*
* This pointer points to the @e readline buffer returned by _readline(). It aliases #line_buffer.
* To avoid a possible double free exeption, this pointer should never be used to free the 
* @e readline buffer.
*
* @sa
* _readline(), @e readline, #line_buffer, and #pipeline_list_.
*
* @todo
* Check if the buffer returned by _readline() ends with "\n\0".
*
*/
char *pipeline_list = NULL;


/**
* @brief Makes sure the @e readline buffer end with "\n\0".
* I am unsure if the buffer returned by _readline() ends with "\n\0". There I use this pointer
* to allocate enough memory to to store the buffer returned by _readline() and "\n\0". This new
* buffer is the input source for @e yacc. 
*
* @sa
* _readline(), @e readline, #line_buffer, and #pipeline_list_.
*
* @todo
* Check if the buffer returned by _readline() ends with "\n\0".
*
*/
char *pipeline_list_ = NULL;
jmp_buf input_prompt;


/**
* @brief The shell's input loop.
*
* This function runs the shell's main input loop. In an infinite loop, it does the following:
* @li Calls _readline() to read input from the user.
* @li Sets up @e yacc to read input from the @e readline buffer.
* @li Calls @e yyparse() (i.e., the parser) to parse the input read.
* @li Cleans up  buffers
*
* @sa
* main(), _readline(), @e yacc, @e bison, @e yyparse(), and sfree().
*
*/
void input_loop();



/**
* @brief The @e main function.
*
* @par Description
* This is the main function. It does the following:
* @li Calls init_shell() to initialize the shell.
* @li Calls initialize_readline() to initialize the shell's command line editing facilities.
* @li Calls input_loop() to run the shell's input loop.
*
* @sa
* input_loop(), initialize_readline(), and init_shell().
*
*/
int main(int argc, char **argv)
{
	//initialize the shell
	init_shell();
	// initialize readline/history
	initialize_readline();
	
	input_loop();
	return EXIT_SUCCESS;

} // end main

void input_loop()
{
	while (1)
	{
		// read from the terminal------------------
		pipeline_list = _readline("> ");
		
		
		// add newline and \o at the end of the pipeline_list------------------
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

	
	} // end while

} // end loop

