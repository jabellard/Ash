#ifndef CLE_H_
#define CLE_H_

//function prototypes
int remove_lines_from_file(char * file_name, int start_line, int line_count);
int invert_case_in_region(int count, int key);
int initialize_readline();
char *_readline(const char *);
int toggle_editing_mode(int count, int key);
char **command_completion(const char *, int, int);
char *command_completion_generator(const char *, int);
int initialize_history();
int count_lines_in_file(const char *);



#endif
