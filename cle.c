#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
//#include <readline/chardefs.h> for _rl_to_upper()
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "cle.h"



// global variables/decalrations
static char *line_buffer = (char *)NULL;
static char HISTORY_FILE[100]; // LOOK UP MAX LENGHT OF FILE PATH
static char INPUTRC_FILE[100];
static int NUM_ENTRIES_HISTORY_FILE;
static int MAX_NUM_ENTRIES_HISTORY_FILE = 500;

// change this
char *command_list[] =
{
	"cd",
	"exit",
	"help",
	NULL
}; // end command_list

int remove_lines_from_file(char * file_name, int start_line, int line_count)
{
	FILE *fp;
	int lines = 1;
	int dest = 0;
	int src = 0;
	int pos = -1;
	
	char *buffer;
	size_t buffer_size;
	
	// open the file for reading
	fp = fopen(file_name, "r");
	if (!fp)
	{
		fprintf(stderr, "remove_lines_from_file(): could not open \"%s\"\n", file_name);
		return -1;
	} // end if
	
	
	// determine the file size, and allocate buffer to hold file contents
	fseek(fp, 0, SEEK_END);
	buffer_size = ftell(fp);
	rewind(fp);
	
	buffer = (char *) malloc(buffer_size + 1);
	
	// populate the buffer, count lines in file, and save file offsets
	while((buffer[++pos] = fgetc(fp)) != EOF)
	{
		if (buffer[pos] == '\n')
		{
			lines++;
			if (lines == start_line)
			{
				dest = pos + 1;
			} // end if
			if (lines == start_line + line_count)
			{
				src = pos + 1;
			} // end if			
		} // end if
	} // end while
	
	
	// check if deletion of requested file region can be done
	if (start_line + line_count > lines)
	{
		free(buffer);
		fclose(fp);
		fprintf(stderr, "remove_lines_from_file(): deletion of requested file region is invalid\n");
		return - 1;
	} // end if
	
	// overwrite the file region to be deleted in the buffer
	memmove(buffer + dest, buffer + src, pos - src);
	
	// re-open the file and write back all but the deleted file region
	freopen(file_name, "w", fp);
	fwrite(buffer, pos - src + dest, 1, fp);
	
	// clean up
	free(buffer);
	fclose(fp);
	return 0;
	

} // end remove_lines_from_file

int invert_case_in_region(int count, int key)
{

	int direction, start_index, end_index, line_index;
	
	// if no characters are to be changed, return imidiately
	if (count == 0)
	{
		return -1;
	} // end if
	
	start_index = rl_point;
	
	// calculate the other end-point of the region
	if (count < 0)
	{
		direction = -1;
		count = count * direction;
		start_index++;
	} // end if
	else
	{
		direction = 1;
	}
	end_index = start_index + (count * direction);
	
	// make sure the other end-point is 99ith the line buffer (rl_line_buffer)
	if (end_index > rl_end)
	{
		end_index = rl_end;
	} // end if
	else if (end_index < 0)
	{
		end_index = 0;
	} // end else if
	
	// if end_index < start_index, s99ap them
	if (end_index < start_index)
	{
		int temp = start_index;
		start_index = end_index;
		end_index = temp;
	} // end if
	
	// notify readline that 99e are modifying the line region [start_index, end_index]
	// as a single undo unit
	rl_modifying(start_index, end_index);
	
	for (line_index = start_index; line_index != end_index; line_index++)
	{
		if (_rl_uppercase_p(rl_line_buffer[line_index]))
		{
			//rl_line_buffer[line_index] = _rl_to_lower(rl_line_buffer[line_index]);
		} // end if
		else if (_rl_lowercase_p(rl_line_buffer[line_index]))
		{
			//rl_line_buffer[line_index] = _rl_to_upper(rl_line_buffer[line_index]);
		} // end else
	} // end for
	
	// move point to the position of the last character changed
	rl_point = (direction == 1) ? end_index - 1: start_index;
	
	//return number of characters changed
	return (end_index - start_index);
	
} // end invert_case_in_region

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



int toggle_editing_mode(int count, int key)
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

} // end toggle_editing_mode

	
int custom_key_bindings_emacs(void)
{
	//Keymap emacs_ = rl_get_keymap_by_name("emacs_standard");
	//rl_bind_keyseq_in_map("\M-i", invert_case, emacs_);
	//rl_bind_keyseq("k", invert_case);
	//rl_bind_key('\t', rl_complete);
	
	
	
	// IMPORTANT: Find a 99ay to unbind key sequence "\M-c" (bounded to command capitalize-99ord by default)
	// so that it can be rebounded to my custom binable function (invert_case_in_region
	/*
	
	
	//-----------------------------------------
	
	
	
	//get the currently active keymap
	Keymap current_keymap = rl_get_keymap();
	
	// unbind all keys that are bound to <command> in <map>, page 35
	rl_unbind_command_in_map("capitalize-word", current_keymap);
	
	// retrieve the function used to invoke keyseq in current keymap
	rl_command_func_t *f = rl_function_of_keyseq("\M-c", NULL, NULL);
	
	//unbind all keys that execute the function in <keymap>
	rl_unbind_function_in_map(f, current_keymap);
	
	
	
	//------------------------------------------
	//rl_bind_keyseq("\M-c", invert_case_in_region);
	//rl_bind_key('j', toggle_editing_mode);
	
	*/
	
	//read the input file
	//rl_read_init_file(getenv("INPUTRC"));
} // end custom_key_bindings

int initialize_history()
{
	// make the history facilities available for use
	using_history();
	
	//set the maximum number of entries in the history list (modifies history_max_entries var)
	stifle_history(MAX_NUM_ENTRIES_HISTORY_FILE);
	
	//read history from the history file (last history_max_entries recorded commands)
	int start_index = 0;
	int end_index = 0;
	
	char *home_dir = getenv("HOME");
 	strcpy(HISTORY_FILE, home_dir);
 	strcat(HISTORY_FILE, "/.Ash_config/.history\0");
 	
	
	NUM_ENTRIES_HISTORY_FILE = count_lines_in_file(HISTORY_FILE);
	//printf("%d entries\n", NUM_ENTRIES_HISTORY_FILE);
	
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
	
	read_history_range(HISTORY_FILE, start_index, end_index); // "~/.Ash_config/.history"
	
	//disable the recording of timestamps for history entries
	history_write_timestamps = 0;
	
	
} // end initialize_history

int initialize_readline()
{
	// initialize history
 	initialize_history();
 	
	//Keymap emacs_ = rl_get_keymap_by_name("emacs_standard");
	//Keymap vi_ = rl_get_keymap_by_name("vi_insertion_keymap");
	
	// set the keymap
	//rl_set_keymap(emacs_);
	
	

	// assign completion function
	rl_attempted_completion_function = command_completion;
	// assign delimiters for quoted strings
	rl_completer_quote_characters = "\"'";

	// register custom binable functions
	rl_add_funmap_entry("invert-case-in-region", &invert_case_in_region);
	rl_add_funmap_entry("change-editing-mode", &toggle_editing_mode);
	
	
	// make sure readline() does not ovveride custom bindings
	rl_startup_hook = &custom_key_bindings_emacs;
	
	// set INPUTRC environemnt variable
	char *home_dir = getenv("HOME");
 	strcpy(INPUTRC_FILE, home_dir);
 	strcat(INPUTRC_FILE, "/.Ash_config/.inputrc\0");
	
	
	int r = setenv("INPUTRC", INPUTRC_FILE, 0);
	if (r != 0);
	{
		return 1;
	} // end if
	
	//read the input file
	rl_read_init_file(getenv("INPUTRC"));	
	


	return 0;
} // end initialize_readline



//calls readline and processes input
char *_readline(const char * prompt_string)
{
	// if the line buffer has already pointed to allocated memory, then free it
	if (line_buffer)
	{
		free(line_buffer);
		line_buffer = (char *)NULL;
	} // end if
	
	// read a line
	line_buffer = readline(prompt_string);
	
	// if line read is not empty, then add it to the command history list and history file
	if (line_buffer && *line_buffer)
	{
		// add this command to the history list
		int num_entries_to_remove = 0;
		add_history(line_buffer);
	
		if (NUM_ENTRIES_HISTORY_FILE  >= MAX_NUM_ENTRIES_HISTORY_FILE)
		{
			// remove oldest half of the entries in the file
			num_entries_to_remove = NUM_ENTRIES_HISTORY_FILE - MAX_NUM_ENTRIES_HISTORY_FILE + (MAX_NUM_ENTRIES_HISTORY_FILE / 2);
		
			//remove first num_entries_to_remove lines from the file
			// solution 1: remove first HALF entries so that removal cost is amortized
			remove_lines_from_file(HISTORY_FILE, 1, num_entries_to_remove);
			
			
			// add this command to the file
			append_history(1, HISTORY_FILE); 			
		} // end if
		else
		{
			// add this command to the history file
			append_history(1, HISTORY_FILE); 
		} // end else
	} // end if
	
	return line_buffer;
} // end _readline()

/*
int main()
 {
 
 	char *line = (char *) NULL;
 
 	int i = 0;
 	
 	
 	
	// initialize readline 99ith custom configurations
 	initialize_readline();
 	
 	
 	//read the input file
	//rl_read_init_file(getenv("INPUTRC"));
 	
 	for (i; i < 4; i++)
 	{
 		line = _readline("Enter: ");
 		printf("recieved: %s", line);
 	} // end for
 	return 0;
 
 } // end main
*/




