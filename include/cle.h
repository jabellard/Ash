/**
* @file cle.h
* @author Joe Nathan Abellard {https://github.com/joenatech7}
* 
* @date September 1, 2017
* @brief Contains function prototypes for the command line editing sub system.
*
* @par Description
* This file contains contains function prototypes for the command lin editing sub subsystem.
*
* @sa
* cle.c
*
*/



#ifndef CLE_H_
#define CLE_H_

//function prototypes-----------------------------------


/**
* @brief Removes lines from a file.
*
* @par Description
* This function is used to remove lines within a region of a file. It removes lines
* <tt> [start_line.. (start_line + line_count) - 1] </tt> from the file.
*
* @param file_name
* The pathname of the file.
*
* @param start_line
* The first line to remove.
*
* @param line_count
* The number of lines to remove.
*
* @return
* Returns @e -1 on failure, or @e 0 on success. Possible reasons for failure are as follows:
@li The file specified by @e file_name could not be opened.
@li The specified file region is invalid.
*
* @sa 
* count_lines_in_file(), @e fopen(), @e fprintf(), @e fseek(), @e rewind(), @e malloc(),
* @e fgetc(), sfree(), @e fclose(), @e memmove(), @e freopen(), err_msg(), @e fwrite(),
* and _readline().
*
*/
int remove_lines_from_file(char * file_name, int start_line, int line_count);


/**
* @brief Inverts the case of characters in a region of the current line buffer of @e readline .
*
* @par Description
* This function is used to inverts the case of characters in a region of the current line buffer of
* @e readline.
*
* @param count
* The number of characters whose case is to be inverted.
*
* @param key
* Not used in the function.
*
* @return
* Return @e -1 on failure, or the number of characters inverted on success.
*
* @sa 
* @e readline, and initialize_readline().
*
*/
int invert_case_in_region(int count, int key);


/**
* @brief Initializes the shell's commmandline editing facilities.
*
* @par Description
* This function is used to initialize the shell's commandline editing facilities. It does the following :
* @li Calls initialize_history() to initialize the shell's command history facilities.
* @li Registers the shell's command completion function (command_completion()).
* @li Registers custom binable functions (invert_case_in_region(), and toggle_editing_mode()).
* @li Obtains the path of @e readline 's @e inputrc file and assign the value to #INPUTRC_FILE.
* @li Sets the INPUTRC environment variable.
* @li Reads settings from the input file (#INPUTRC_FILE).
*
* @return
* Returns @e -1 on failure, or @e 0 on success.
*
* @sa
* initialize_history(), command_completion(), invert_case_in_region(), toggle_editing_mode(),
* #INPUTRC_FILE, @e readline , @e inputrc, and @main().
*
*/
int initialize_readline();


/**
* @brief Calls @e readline() to read input and manages the shell's command history.
*
* @par Description
* This function is used to call @ readline() to read input from the shell prompt, and to manage the shell's command history. It does the following:
* @li Calls #sfree to de-allocate the memory block of the last line buffer returned by @readline(),
* if nessasary. This buffer is pointed to by #line_buffer. #line_buffer is aliased by #pipeline_list
* in input_loop(). To avoid a possible double free exeption, #pipeline_list should never be used to free
* the @e readline buffer.
* @li Calls @e readline() to read input, and assigns the location of the @e readline buffer
* to #line_buffer.
* @li If the @e readline buffer is not empty, then add the line read to the history list and to
* the history file (#HISTORY_FILE).  If insertion of the new entry would result in the history file having more than 
* #MAX_NUM_ENTRIES_HISTORY_FILE, or that the history file already contains more than 
* MAX_NUM_ENTRIES_HISTORY_FILE, then the function invokes removed_lines_from_file() to remove lines from the history file before
* performing the insertion.
*
* @param promt_string
* The prompt string that is displayed on the terminal.
*
* @return
* Returns #line_buffer.
*
* @sa
* #sfree, #line_buffer, readline(), #MAX_NUM_ENTRIES_HISTORY_FILE, #NUM_ENTRIES_HISTORY_FILE,
* remove_lines_from_file(), #pipeline_list, and input_loop().
*
*/
char *_readline(const char *);


/**
* @brief Toggles @e readline 's editing mode.
*
* @par Description
* This function is used to toggle @e readline 's edditing mode between @e vi mode and @e emacs mode.
*
* @bug
* Does not work.
*
* @sa @e readline .
*/
int toggle_editing_mode(int count, int key);


/**
* @brief Registred function to perform command completion.
*
* @par Description
* This function is assigned to @e readline's @e rl_attempted_completion variable. Consequently, when
* the tab key is pressed on the command line, it is invoked to perform command completion on
* a partial text. It uses @e readline's @e rl_completion_matches function with the 
* command_completion_generator() genarator function to find possible completions for the partial text in the list of commands in the #builtins array
* 
* @param partial_text
* The partial text to be completed.
*
* @param start
* The starting position of @e partial_text in @e readline's line buffer.
*
* @param end
* The ending position of @e partial_text in @e readline's line buffer.
*
* @return
* Returns an array of pointers to strings. Three outcomes are possible :
* @li No possible completions found. In that case, the first element of the array points
* to NULL.
* @li One possible completion was found. In that case, the first element of the array points
* to the possible completion string, and the second element points to NULL.
* @li @e n>=2 possible completion were found. In that case, the first @e n elements of the array point
* to the @e n possible completion strings, and the @e (n + 1)th element of the array points to NULL.
*
* @sa
*  @e readline , initialize_readline(), command_completion_genarator(), and #builtins.
*
*/
char **command_completion(const char *, int, int);



/**
* @brief Generation function used for command completion.
*
* @par Description
* This generation function is used by command_completion() to find possible command completions in the #builtins array.
*
* @param partial_text
* The partial text to be completed.
*
* @param state
* The first time the function is called, state is @e 0. When state is zero, the function initialize some static variables. 
*
* @return 
* If a possible completion for @e partial_text is found in the #builtins array, the function returns a copy to the
* completion string. Otherwise, it returns NULL.
*
* @sa command_completion(), #builtins, and @e readline .
*
*/
char *command_completion_generator(const char *, int);


/**
* @brief Initializes the shell's command history facilities.
*
* @par Description
* This function is used to initialize the shell's command history facilities. It does the
* following :
* @li Calls using_history() to make history facilities available.
* @li Sets the maximum number of enties in the shell's history list to #MAX_NUM_ENTRIES_HISTORY_FILE.
* @li Obtains the pathname of the shell's history file and stores the result in #HISTORY_FILE.
* @li Obtains the number of lines in the shell's history file (#HISTORY_FILE) via count_lines_in_file()
* , and assigns the value to #NUM_ENTRIES_HISTORY_FILE.
* @li Reads history from the history file (#HISTORY_FILE) into the shell's history list.
* @li Disables the recording of timestamps for history entries.
*
* @return
* Returns @e void.
*
* @sa and initialize_readline().
* #MAX_NUM_ENTRIES_HISTORY_FILE, #HISTORY_FILE, #NUM_ENTRIES_HISTORY_FILE, count_lines_in_file(),
* 
*
*/
int initialize_history();


/**
* @brief Counts the number of lines in a file.
*
* @par Description
* This function is used to count the number of lines in a file.
*
* @param file_name
* The pathname of the file.
*
* @return
* Returns @e -1 on failure, or the number of lines in the file on success. Possible reasons for
* failure are as follows:
* @li The specified file could not be opened.
*
* @sa 
* remove_lines_from_file(), initialize_history(), #HISTORY_FILE, @e fopen(), @e getc(), @e fclose(), 
* and #NUM_ENTRIES_HISTORY_FILE.
*
*/
int count_lines_in_file(const char *);



#endif
