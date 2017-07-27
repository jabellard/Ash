#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <unistd.h>


//function prototypes
int invert_case(int count, int key);
int custom_key_bindings_emacs(void);
int initialize_readline();
char *_readline();
int change_editing_mode(int count, int key);

int change_editing_mode(int count, int key)
{


	//if current editing mode is set to emacs
	if (rl_editing_mode)
	{
		//rl_parse_and_bind("set editing-mode vi");
	} // end if
	else
	{
		//rl_parse_and_bind("set editing-mode emacs");
	} // end else


	return 0;

} // end change_editing_mode
int invert_case(int count, int key)
{
int direction;
register int start, end, i;
//use of readline variable in custom function
start = rl_point;
if (rl_point >= rl_end)
return (0);
if (count < 0)
{
direction = -1;
count = -count;
}
else
direction = 1;
/* Find the end of the range to modify. */
end = start + (count * direction);

/* Force it to be within range. */
if (end > rl_end)
end = rl_end;
else if (end < 0)
end = 0;
if (start == end)
return (0);
if (start > end)
{
int temp = start;
start = end;
end = temp;
}
/* Tell readline that we are modifying the line,
so it will save the undo information. */
rl_modifying (start, end);
for (i = start; i != end; i++)
{
if (_rl_uppercase_p (rl_line_buffer[i]))
rl_line_buffer[i] = _rl_to_lower (rl_line_buffer[i]);
else if (_rl_lowercase_p (rl_line_buffer[i]))
rl_line_buffer[i] = _rl_to_upper (rl_line_buffer[i]);
}
/* Move point to on top of the last character changed. */
rl_point = (direction == 1) ? end - 1 : start;
	return 0;
} // end invert_case
	
int custom_key_bindings_emacs(void)
{
	//Keymap emacs_ = rl_get_keymap_by_name("emacs_standard");
	//rl_bind_keyseq_in_map("\M-i", invert_case, emacs_);
	//rl_bind_keyseq("k", invert_case);
	rl_bind_key('k', invert_case);
	rl_bind_key('j', change_editing_mode);

} // end custom_key_bindings



int initialize_readline()
{
	
	//Keymap emacs_ = rl_get_keymap_by_name("emacs_standard");
	//Keymap vi_ = rl_get_keymap_by_name("vi_insertion_keymap");
	
	// set the keymap
	//rl_set_keymap(emacs_);
	
	
	// register custom binable functions
	rl_add_funmap_entry("invert-case", &invert_case);
	rl_add_funmap_entry("change-editing-mode", &change_editing_mode);
	
	
	// make sure readline() does not ovveride custom bindings
	rl_startup_hook = &custom_key_bindings_emacs;
	

	// set INPUTRC environment variable
	int r = setenv("INPUTRC", "~/.Ash_inputrc", 0);
	if (r != 0);
	{
		return 1;
	} // end if
	
	//read the input file
	rl_read_init_file(getenv("INPUTRC"));

	return 0;
} // end initialize_readline



//calls readline and processes input
static char *line_buffer = (char *)NULL;
char *_readline()
{
	// if the line buffer has already pointed to allocated memory, then free it
	if (line_buffer)
	{
		free(line_buffer);
	} // end if
	
	printf("1\n");
	// read a line
	line_buffer = readline("Enter text: ");
	
		printf("2\n");

	
	// if line read is not empty, then add it to the command history
	if (line_buffer && *line_buffer)
	{
		add_history(line_buffer);
	} // end if
	
	return line_buffer;
} // end _readline()



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











/*


int main()
{
	initialize_readline();
	int i = 0;
	for (i = 0; i < 5; i++)
	{
		line_read = rl_gets();
	} // end for

	return 0;
} // end main

*/

int main()
 {
 	
 	
 	char *line = (char *) NULL;
 
 	int i = 0;
 	
 		//rl_bind_key('k', invert_case);
 		initialize_readline();
 	for (i; i < 4; i++)
 	{
 		line = rl_gets();
 		printf("recieved: %s", line);
 	} // end for
 	return 0;
 
 } // end main



