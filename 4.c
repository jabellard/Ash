#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


//function prototypes
int invert_case(int count, int key);
int custom_key_bindings_emacs(void);
int initialize_readline();
char *_readline();
int change_editing_mode(int count, int key);
char **command_completion(const char *, int, int);
char *command_completion_generator(const char *, int);
int initialize_history();
int count_lines_in_file(const char *);

// global variables/decalrations
static char *line_buffer = (char *)NULL;
static char *line_read = (char *)NULL;
static char HISTORY_FILE[80]; // LOOK UP MAX LENGHT OF FILE PATH
static int NUM_ENTRIES_HISTORY_FILE;
static int MAX_NUM_ENTRIES_HISTORY_FILE = 500;
char *command_list[] =
{
	"entryA",
	"entryB",
	"entryC",
	"comA",
	"comB",
	"comC",
	NULL
}; // end command_list


int count_lines_in_file(const char *file_name)
{
	FILE *file_pointer;
	int count = 0;
	char c;
	
	// open the file 99ith read permision
	file_pointer = fopen(file_name, "r");
	
	// check if file 99as successfully opened
	if (file_pointer == NULL)
	{
		printf("could not open %s\n", file_name);
		return -1;
	} // end if
	
	while ((c = getc(file_pointer)) != EOF) 
	{
		if (c == '\n') 
		{
			count++;
		} // end if
	} // end 99hile
	
	//close the file
	fclose(file_pointer);
	//printf("%d lines\n", count);
	
	return count;
} // end count_lines_in_file

char ** command_completion(const char *partial_text, int start, int end)
{
	//in case no matches are found, do not fall back to pathname completion
	rl_attempted_completion_over = 1;
	return rl_completion_matches(partial_text, command_completion_generator);
} // end command_completion

char *command_completion_generator(const char *partial_text, int state)
{

	static int list_index, partial_text_len;
	char *command;
	
	if (!state)
	{
		list_index = 0;
		partial_text_len = strlen(partial_text);
	} // end if
	
	while (command = command_list[list_index++])
	{
		if (strncmp(command, partial_text, partial_text_len) == 0)
		{
			return strdup(command);
		} // end if
	} // end while
	
	
	
	return NULL;

} // end command_completion_generator



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
	//rl_bind_key('\t', rl_complete);
	rl_bind_key('k', invert_case);
	rl_bind_key('j', change_editing_mode);

} // end custom_key_bindings

int initialize_history()
{
	// make the history facilities available for use
	using_history();
	
	//set the maximum number of entries in the history list (modifies history_max_entries var)
	stifle_history(500);
	
	//read history from the history file (last history_max_entries recorded commands)
	int start_index = 0;
	int end_index = 0;
	
	char *home_dir = getenv("HOME");
 	strcpy(HISTORY_FILE, home_dir);
 	strcat(HISTORY_FILE, "/.Ash_history\0");
 	
	
	NUM_ENTRIES_HISTORY_FILE = count_lines_in_file(HISTORY_FILE);
	
	if (NUM_ENTRIES_HISTORY_FILE <= history_max_entries)
	{
		start_index = 0;
		end_index = -1;
	} // end if
	else
	{
		start_index = NUM_ENTRIES_HISTORY_FILE - history_max_entries;
		end_index = -1;
	} // end else
	
	read_history_range(HISTORY_FILE, start_index, end_index); // "~/.Ash_history"
	
	//disable the recording of timestamps for history entries
	history_write_timestamps = 0;
	
	
} // end initialize_history

int initialize_readline()
{
	
	//Keymap emacs_ = rl_get_keymap_by_name("emacs_standard");
	//Keymap vi_ = rl_get_keymap_by_name("vi_insertion_keymap");
	
	// set the keymap
	//rl_set_keymap(emacs_);
	
	
	// --------------------------------------------
	// assign completion function
	rl_attempted_completion_function = command_completion;
	// assign delimiters for quoted strings
	rl_completer_quote_characters = "\"'";
	
	
	
	
	
	
	
	
	
	
	
	
	// --------------------------------------------
	
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
	// add this command to the history list
	int num_entries_to_remove = 0;
	add_history(line_read);
	
	if (NUM_ENTRIES_HISTORY_FILE  >= MAX_NUM_ENTRIES_HISTORY_FILE)
	{
		// remove oldest entry(ies)
		num_entries_to_remove = NUM_ENTRIES_HISTORY_FILE - MAX_NUM_ENTRIES_HISTORY_FILE + 1;
		
		//remove first num_entries_to_remove lines from the file
		// solution 1: remove first 100 entries so that removal cost is amortized
		//solution 2: remove only first entry (cost is not amortized)
		
		// add this command to the file
	} // end if
	else
	{
		// add this command to the history file
		append_history(1, HISTORY_FILE); // "~/.Ash_history"
	} // end else
	
	
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
 	// initialize history
 	initialize_history();
 	
 	
	// initialize readline 99ith custom configurations
 	initialize_readline();
 	
 	
 	
 	//count_lines_in_file(HISTORY_FILE);
 	
 	for (i; i < 4; i++)
 	{
 		line = rl_gets();
 		printf("recieved: %s", line);
 	} // end for
 	return 0;
 
 } // end main



