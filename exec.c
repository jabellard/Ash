//_SVID_SOURCE
#include "exec.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "builtins.h"
#include "err.h"

// mapping of shell builtin to function
Builtin builtins[] =
{
	{"cd", &Ash_cd},
	{"exit", &Ash_exit},
	{"help", &Ash_help}

};

// calculate the number of builtins
int num_builtins()
{

	return sizeof(builtins) / sizeof(Builtin);
}

// -----------------------simple_command functions

struct simple_command * create_simple_command(void)
{
	//dynamically create a simple command (not not be right, double check)
	struct simple_command *sc =  (struct simple_command*) malloc(sizeof(struct simple_command));
	if(!sc)
	{
		err_msg("malloc");
	} // end if
	// make sure all pointers in the simple_command_ars array initially point to NULL
	int i = 0;
	for (i; i < MAX_ARGS; i++)
	{
		sc->simple_command_args[i] = NULL;
	} // end for
	
	// set current number of arguments to zero
	sc->simple_command_num_args = 0;
		//return a pointer to the structure
	return sc;
} // end create_simple_command()

int destroy_simple_command(struct simple_command *sc)
{
	// free the space allocated to store the arguments in the commmand
	int i = 0;
	for (i; i < sc->simple_command_num_args; i++)
	{
		// check for de-allocation error here
		sfree(sc->simple_command_args[i]);
	} // end for
	
	// free the simple_command struture
	sfree(sc);
	
	return 0;
	
} // end destroy_simple_command()

int insert_arg_in_simple_command(struct simple_command *sc, const char *arg)
{
	// check if there is available space for the insertion
	if (sc->simple_command_num_args < (MAX_ARGS - 1))
	{
		sc->simple_command_args[sc->simple_command_num_args] = strdup(arg);
		
		// increment the number of arguments currently stored in the command
		sc->simple_command_num_args++;
		
		return 0;
	} // end if
	else
	{
		fprintf(stderr, "exeeded number of arguments allowed per a simple command (%d)\n", MAX_ARGS);
		return -1; //exit
	} // end else

} // end insert_arg_in_simple_command()



void print_simple_command(const struct simple_command *sc)
{
	int i = 0;
	for (i; i < sc->simple_command_num_args; i++)
	{
		printf("%s ", sc->simple_command_args[i]);
	} // end for
	printf("\n----------------------------------------\n");

} // end print_simple_command()

struct simple_command * simple_command_dup(const struct simple_command * sc)
{
	// create a ne99 simple command
	struct simple_command *new_sc = create_simple_command();
	
	// copy value of current arguments member
	new_sc->simple_command_num_args = sc->simple_command_num_args;
	
	// copy data pointed to by pointer array
	int i = 0;
	for (i; i < sc->simple_command_num_args; i++)
	{
		new_sc->simple_command_args[i] = strdup(sc->simple_command_args[i]);
	
	} // end for
	
	//return pointer to the new simple command structure
	return new_sc;

} // end simple_command_dup()

// ---------------------------------shell_pipeline functions

struct shell_pipeline * create_shell_pipeline(void)
{
	struct shell_pipeline *sp = (struct shell_pipeline*) malloc(sizeof(struct shell_pipeline));
	if(!sp)
	{
		err_msg("malloc");
	} // end if
	
	// make sure all pointers in the simple_commands array initially point to NULL
	int i = 0;
	for (i; i < MAX_COMMANDS; i++)
	{
		sp->simple_commands[i] = NULL;
	} // end for
	
	// set the number of simple commands allocated in the array to 0
	sp->shell_pipeline_num_commands = 0;
	
	// set I/O redirection filename pointers to NULL
	sp->input_file = NULL;
	sp->output_file = NULL;
	sp->error_file = NULL;
	
	// set background/foreground info to off (o)
	sp->background = 0;
	
	// return a pointer to the allocated structure
	return sp;

} // end create_shell_pipeline()

int destroy_shell_pipeline(struct shell_pipeline *sp)
{
	// free the space allocated to store the commands in the pipeline
	int i = 0;
	for (i; i < sp->shell_pipeline_num_commands; i++)
	{
		destroy_simple_command(sp->simple_commands[i]);
	} // end for
	
	// free the shell_pipeline structure
	sfree(sp);
	
	return 0;

} // end destroy_shell_pipeline()



int insert_simple_command_in_shell_pipeline(struct shell_pipeline *sp, const struct simple_command *sc)
{
	// check if there is available space for the insertion

	if (sp->shell_pipeline_num_commands < (MAX_COMMANDS - 1))
	{
		// duplicate the simple_command structure
		struct simple_command *sc_dup = simple_command_dup(sc);
		
		// make the insertion
		sp->simple_commands[sp->shell_pipeline_num_commands] = sc_dup;
		
		// increment number of commands stored in the pipeline
		sp->shell_pipeline_num_commands++;
		
		return 0;
	} // end if
	else
	{
		fprintf(stderr, "exeeded number of arguments allowed per a simple command (%d)\n", MAX_COMMANDS);
		return -1; //exit
	} // end else

} // end insert_simple_command_in_shell_pipeline()


void print_shell_pipeline(const struct shell_pipeline *sp)
{
	int i = 0;
	for (i; i < sp->shell_pipeline_num_commands; i++)
	{
		printf("command %d: ", (i + 1));
		print_simple_command(sp->simple_commands[i]);
	} // end for

} // end print_shell_pipeline()


int execute_simple_command(int input_file, int output_file, int error_file, struct simple_command *sc)
{
	// check if simple command is a shell builtin
	int i = 0;
	for (i; i < num_builtins(); i++)
	{
		if (strcmp(sc->simple_command_args[0], builtins[i].name) == 0)
		{
			// the command is a builtin
			return (*builtins[i].function)(sc->simple_command_args);
		} // end if
	
	
	} // end for
	
	pid_t child_pid = fork();
	if (child_pid == 0)
	{
		// make redirections (if nessasary)
		if (input_file != 0)
		{
			dup2(input_file, 0);
			close(input_file);
		} // end if
		
		if (output_file != 1)
		{
			dup2(output_file, 1);
			close(output_file);
		} // end if
		
		if (error_file != 2)
		{
			dup2(error_file, 2);
			close(error_file);
			//fprintf(stderr, "99riting to error\n");
		} // end if
		
		// execute the command
		return execvp(sc->simple_command_args[0], sc->simple_command_args);
		
		// do error checking
	} // end if
	return child_pid;

} // end execute_simple_command()

int setup_pipeline(int num_commands, struct shell_pipeline *sp)
{


} // end setup_pipeline()
int execute_shell_pipeline(const struct shell_pipeline *sp)
{
	// check if the pipeline is empty
	if (sp->shell_pipeline_num_commands == 0)
	{
		printf("empty pipeline\n");
		return 0;
	} // end if
	else
	{
		// save the state of the parent process' stdin, stdout, and stderr
		int saved_stdin = 0;
		int saved_stdout = 1;
		int saved_stderr = 2;
		int initial_input_file;
		int input_file;
		int output_file;
		int error_file;
		
		// set the source of the initial input for the first command in the pipeline
		if (sp->input_file)
		{
			// open the file for reading
			input_file = open(sp->input_file, O_RDONLY);
			if(input_file == -1)
			{
				err_msg("open, input file");
			} // end if
			//printf("in fd # %d\n", input_file);
			//input_file = initial_input_file;
		} // end if
		else
		{
			input_file = saved_stdin;
			
			//initial_input_file = saved_stdin;
		} // end else
		
		// set the destination file for stderr
		if (sp->error_file != NULL)
		{
			// open the file for 99riting
			error_file = open(sp->error_file, O_WRONLY);
			if(error_file == -1)
			{
				err_msg("open, error file");
			} // end if			
			
			
		} // end if
		else
		{
			error_file = saved_stderr;
		} // end else
		
		// set the destination file for stdout
		if (sp->output_file != NULL)
		{
			// open the file for 99riting
			output_file = open(sp->output_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
			if(output_file == -1)
			{
				err_msg("open, output file");
			} // end if			
			//printf("out fd # %d\n", output_file);
			//output_file = open(sp->output_file, O_WRONLY);
			
			
		} // end if
		else
		{
			output_file = saved_stdout;
		} // end else
		
		int i = 0;
		int pipe_fd[2];
		pid_t child_pid;
		for (i = 0; i < sp->shell_pipeline_num_commands - 1; i++)
		{
			// create a pipe
			pipe(pipe_fd);
			// read from the input of the last iteration, and 99rite to the 99rite end of the pipe
			execute_simple_command(input_file, pipe_fd[1], error_file, sp->simple_commands[i]);
			
			// stop loop if the command just executed 99as a shell builtin????
			
			
			// close the 99rite end of the pipe
			close(pipe_fd[1]);
			
			// keep the read end of the pipe; next child reads from there
			input_file = pipe_fd[0];
			
		} // end for
		
		// execute the last command in the pipeline
		pid_t last_child = execute_simple_command(input_file, output_file, error_file, sp->simple_commands[i]);
		
	// determine if the parent (shell) should 99ait for the children to terminate
	if (sp->background == 1)
	{
		// don't 99ait
		// return the process id of the last child
		return last_child;
	} // end if
	else
	{
		// 99ait for all children to terminate
		while (wait(NULL) > 0);
		// return the process id of the last child
		//printf("done 99aiting\n");
		return last_child;
	} // end else
	} // end else
	
	
	
} // end execute_shell_pipeline()














