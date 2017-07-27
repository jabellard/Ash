#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cle.h"


/*
constants
*/

#define COMMAND_BUFFER_SIZE 1024
#define TOKEN_BUFFER_SIZE 64
#define TOKEN_DELIMITERS "\t\r\n\a"

/*
function prototypes
*/
void loop(void);
char *get_command(void);
char **parse_command(char * command);
int execute_command(char **args);
int _execute(char **args);
int num_builtins (void);
int bi_cd(char **args);
int bi_help(char **args);
int bi_exit(char **args);

// maps builtins to corresponding functions
struct builtin
{
	char *builtin_name;
	int (*builtin_function) (char **);

}; // end struct built_in

// builtins and corresponding functions
struct builtin builtins[] = 
{
	{"cd", &bi_cd},
	{"help", &bi_help},
	{"exit", &bi_exit}
}; // end builtins




int main(int argc, char **argv)
{

	loop();
	return EXIT_SUCCESS;

} // end main


void loop(void)
{
	char *command = (char *)NULL;
	char **args;
	int status = 1;
	
	do{
	
		command = _readline("> ");
		printf("recieved: %s\n", command);
		args = parse_command(command);
		
		/*
		int i = 0;
		while (args[i] != NULL)
		{
			printf("-->>> %s \n", args[i]);
			i++;
		} // end
		
		*/
		status = execute_command(args);
		
		free(args);
	}while (status);
} // end loop

char *get_command(void)
{
	int command_buffer_size = COMMAND_BUFFER_SIZE;
	char *command_buffer = malloc(sizeof(char) * command_buffer_size);

	if (!command_buffer)
	{
		fprintf(stderr, "Ash: allocation error\n");
		exit(EXIT_FAILURE);
	} // end if

	int c = 0;
	int position = 0;
	while (1)
	{
		c = getchar();
		if (c == EOF || c == '\n')
		{
			command_buffer[position] = '\0';
			return command_buffer;
		} // end if
		else
		{
			command_buffer[position] = c;
		} // end else
		
		position++;
		
		if (position >= command_buffer_size)
		{
			command_buffer_size *= 2;
			command_buffer = realloc(command_buffer, command_buffer_size);
			if (!command_buffer)
			{
				fprintf(stderr, "Ash: allocation error\n");
				exit(EXIT_FAILURE);
			} // end if
		} // end if
		
		
	} // end while
} // end get_command


char **parse_command(char * command)
{
	int token_buffer_size = TOKEN_BUFFER_SIZE;
	char **tokens = malloc(token_buffer_size * sizeof(char *));
	char *token;
	
	if (!tokens)
	{
		fprintf(stderr, "Ash: allocation error\n");
		exit(EXIT_FAILURE);
	} // end if
	
	token = strtok(command, TOKEN_DELIMITERS);
	int position = 0;
	while (token != NULL)
	{
		tokens[position] = token;
		position++;
		
		if (position >= token_buffer_size)
		{
			token_buffer_size *= 2;
			tokens = realloc(tokens, token_buffer_size * sizeof(char *));
			
			if (!tokens)
			{
				fprintf(stderr, "Ash: allocation error\n");
				exit(EXIT_FAILURE);
			} // end if
		} // end if
		token = strtok(NULL, TOKEN_DELIMITERS);
	} // end while
	tokens[position] = NULL;
	return tokens;
} // end parse_command



int execute_command(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < num_builtins(); i++) {
    if (strcmp(args[0], builtins[i].builtin_name) == 0) {
    
   // printf("is a built");
      return (*builtins[i].builtin_function)(args);
    }
  }
 //printf("not a buil");
  return _execute(args);
} // end execute_command


int _execute(char **args)
{
	pid_t pid;
	pid_t wpid;
	int status;
	
	pid = fork();
  if (pid == 0) {
    // Child process
   // printf("before error");
   
   /* 
   
   if (execvp(args[0], args) == -1) {
    	//printf("error...... ");
      //perror("lshhhhh");
      exit(EXIT_FAILURE);
    }
   
   
   */
    
		// create array of strings (argument list)
	
	 char **argss = (char *[]){"ls", "-i", "-l"};
	 int i;
	 for (i = 0; i <= 2; i++)
	 {
	 	printf("%d: %s \n", i, argss[i]);
	 }
	 
	 //char *argv[] = args;
	
	
	// execute ls 99ith the arguments list
	execvp(argss[0], argss);
		printf("after");

  } else if (pid < 0) {
    // Error forking
    perror("Ash");
  } else {
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
	
} // end _execute

// return the number of builtins
int num_builtins (void)
{
	return sizeof(builtins) / sizeof(builtins[0]);
} // end num_builtins

int bi_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "Ash: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("Ash");
    }
  }
  return 1;
} // end bi_cd

int bi_help(char **args)
{
  
  printf("help...\n");
  
  return 1;
} // ene bi_help

int bi_exit(char **args)
{
  return 0;
} // end bi_exit


