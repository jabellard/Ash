#include <stdio.h>
 #include <stdio.h>
  #include <readline/readline.h>
 #include <readline/history.h>
 #include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
 
 int func0(int count, int key)
 {
 
 	// use built-in vars and funcs to do some stuff
 	return 0;
 } // end func0
 
 int initialize_readline()
 {
 // input file should be used to ovveride settings defined here
 
 	//print list of macros, variable settings, and bindings associated 99ith a keymap
 	// get and print the current keymap
 	//set path for input config file--------------------
 	
 	
 	
 	//use rl_save_state() to save the internal state of readline
 	
 	
 	
 	//set custom key bindings--------------------
 	//readline install default key bindings the first time it is invoked
 	//so custom key bindings should be defined in an initialization function that is 
 	// assigned to the rl_startup_hook variable

 	rl_command_func_t *func0_p = &func0;
 	// add descriptive function name to the list of of named functions
 	int r1 = rl_add_defun("change-char-case", func0_p, -1);
 	int r0 = bind_keyseq("<keyseq>", func0_p);
 	
 	rl_bind_key_in_map(<key-numb>, <function>, <keymap>);
 	
 	//as line as if it is line from inputrc and perform key bindings
 	rl_parse_and_bind(<line>)
 	
 	// select key maps (print current keymap, ect...)
 	
 	//associating function names and bindings
 	
 	
 
 	return 0;
 } // end initialize_readline

 
 /* A static variable for holding the line. */
static char *line_read = (char *)NULL;
/* Read a string, and return a pointer to it.
Returns NULL on EOF. */
char *
rl_gets ()
{
/* If the buffer has already been allocated,
return the memory to the free pool. */
if (line_read)
{
free(line_read);

line_read = (char *)NULL;
}
/* Get a line from the user. */
line_read = readline("Enter line: ");
/* If the line has any text in it,
save it on the history. */
if (line_read && *line_read)
{
	add_history(line_read);
}

return (line_read);
}

 
 int main()
 {
 	char *line = (char *) NULL;
 
 	int i = 0;
 	
 	for (i; i < 4; i++)
 	{
 		line = rl_gets();
 		printf("recieved: %s", line);
 	} // end for
 	return 0;
 
 } // end main
