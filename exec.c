//_SVID_SOURCE
#include "exec.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// -----------------------simple_command functions

struct simple_command * create_simple_command(void)
{
	//dynamically create a simple command (not not be right, double check)
	struct simple_command *sc =  (struct simple_command*) malloc(sizeof(struct simple_command));
	
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
		free(sc->simple_command_args[i]);
	} // end for
	
	// free the simple_command struture
	free(sc);
	
	return 0;
	
} // end destroy_simple_command()

int insert_arg_in_simple_command(struct simple_command *sc, const char *arg)
{
	// check if there is available space for the insertion
	if (sc->simple_command_num_args < (MAX_ARGS - 1))
	{
		sc->simple_command_args[sc->simple_command_num_args] = strdup(arg);
		
		// increase the number of arguments currently stored in the command
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
	free(sp);
	
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

int execute_shell_pipeline(const struct shell_pipeline *sp)
{
	// save stdin, stdout, and stderr
	
	
	// make redirections
	
	// create pipes
	
	//for children and make pipe connections
	
	
	
	// have parent 99ait for children to terminate
	
	// restore initial file io settings

} // end execute_shell_pipeline()














