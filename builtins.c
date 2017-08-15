#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int Ash_cd(char **args)
{
	if (args[1] == NULL)
	{
		fprintf(stderr, "Ash: expected argument to \"cd\"\n");
	} // end if
	else
	{
		if (chdir(args[1]) != 0)
		{
		
			// error checking code
		
		} // end if
		else
		{
		
			printf("changed directory to: %s\n", args[1]);
		} // end else
	} // end else
	return 0;

} // end Ash_cd

int Ash_help(char **args)
{
	printf("Shell grammar: \n");
	return 0;

} // end Ash_help

int Ash_exit(char **args)
{
	printf("exiting...\n");
	// exit from the shell
	exit(EXIT_SUCCESS);

} // end Ash_exit

