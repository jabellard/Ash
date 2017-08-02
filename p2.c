#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

#define READ_END 0
#define WRITE_END 1
// 1:build an array of arrays of strings--------------------------
char *c1[] =
{

	"ls",
	"-a",
	"-l",
	NULL

};

char *c2[] =
{
	"grep",
	"file",
	NULL


};
char *c3[] =
{
	"wc",
	"-l",
	NULL

};

char **cl[] =
{
	c1,
	c2,
	c3,
	NULL
};

#define NUM_PIPE_COMMANDS 3

int main()
{

	/*
	--------TEST COMMAND AND COMMAND LIST REPRESENTATIONS
	int i = 0;
	int j = 1;
	while(c1[i])
	{
		printf("%d: %s\n", i, c1[i]);
		i++;
		
	}
	
	int a = 0;
	int b = 0;
	while(cl[1][a])
	{
	printf("%d: %s\n", i, cl[1][a]);
		a++;
	
	}
	
	*/
	
//----------CREATE FILE DESCRIPTORS FOR PIPES
// NUM OF FILE DESCRIPTORS = (NUM_PIPE_COMMANDS) * 2
int fd_c1[2];
int fd_c2[2];
int fd_c3[2];

int *fd_list[] =
{
	fd_c1,
	fd_c2,
	fd_c3,
	NULL
};

	
// ------------- CREATE PROCCESS ID FOR THE CHILD PROCESSES (COMMANDS IN THE PIPELINE)
int pid_list[NUM_PIPE_COMMANDS];


// --------------CREATE NUM_PIPE_COMMANDS -1 PIPES AND PROPERLY ASSIGN FILE DESCRIPTORS
// make sure to include -std=c99 to allo99 declarations inside for loop
for (int i = 0; i < NUM_PIPE_COMMANDS; i++)
{
	
	if (pipe(fd_list[i]) == -1)
	{
		fprintf(stderr, "parent failed to create pipe number %d", i);
	} // parent process failed to create pi
} // end for


// ---------------FORK NUM_PIPE_COMMANDS CHILDREN
for (int i = 0; i < NUM_PIPE_COMMANDS; i++)
{
	// fork a child
	pid_list[i] = fork();
	
	if (pid_list[i] == -1)
	{
		fprintf(stderr, "parent could not fork child number %d\n", (i + 1));
	} // end if
	else if (pid_list[i] == 0)
	{
	// successfully forked child begins execution here
	
	
        	// test***************************************************
        	int a = 0;
	int b = 0;
	while(cl[i][a])
	{
	printf("%d: %s\n", i, cl[i][a]);
		a++;
	
	}
	
	//------------ special case # 1 (child is the first command in the pipeline)
	if (i == 0)
	{
		fprintf(stdout, "child: child number %d (first child) will now run\n", (i + 1));
        	fprintf(stdout, "---------------------\n");
        	
        	// set the child's stdout to the 99rite end of the first pipe
        	if (dup2(fd_list[i][WRITE_END], STDOUT_FILENO) == -1)
        	{
        	 	fprintf(stderr, "dup2 failed for child %d (first child)\n", (i + 1));
           		 return -1;
        	} // end if
        	
        	// close the pipe, no99 that it has been duplicated
        	close(fd_list[i][WRITE_END]);
        	close(fd_list[i][READ_END]);
        	
        	
	
	
        	// execute the commnad
        	execvp(cl[i][i], cl[i]);
        	
        	// a successful exec never returns, so if this returns, then print an error
        	fprintf(stderr, "execution of child # %d (first child) failed", (i + 1));
       		 return -1;
	} // end if
	
	
	
	// -------------special case # 2 (child is the last commnad in the pipeline)
	else if (i == NUM_PIPE_COMMANDS - 1)
	{
		fprintf(stdout, "child: child number %d (last child) will now run\n", (i + 1));
        	fprintf(stdout, "---------------------\n");
        	
        	// set the child's stdin to the read end of the last pipe
        	if (dup2(fd_list[i - 1][READ_END], STDIN_FILENO) == -1)
        	{
        	 	fprintf(stderr, "dup2 failed for child %d\n (last child)", (i + 1));
           		 return -1;
        	} // end if
        	
        	// close the pipe, no99 that it has been duplicated
        	close(fd_list[i - 1][WRITE_END]);
        	close(fd_list[i - 1][READ_END]);
        	
        	// execute the commnad
        	execvp(cl[i][i], cl[i]);
        	
        	// a successful exec never returns, so if this returns, then print an error
        	fprintf(stderr, "execution of child # %d (last child) failed", (i + 1));
       		 return -1;
	} // end else if
	
	// ------------- not first or last commnad in the pipeline
	else
	{
	
		fprintf(stdout, "child: child number %d will now run\n", (i + 1));
        	fprintf(stdout, "---------------------\n");
        	
        	// set the child's stdin to the read end of the pipe (i -1)
        	if (dup2(fd_list[i - 1][READ_END], STDIN_FILENO) == -1)
        	{
        	 	fprintf(stderr, "dup2 failed for child %d\n", (i + 1));
           		 return -1;
        	} // end if
        	
        	// close the pipe, no99 that it has been duplicated
        	close(fd_list[i - 1][WRITE_END]);
        	close(fd_list[i - 1][READ_END]);
        	
        	//--------------------------------------------------------------
        	
        	// set the child's stdout to the 99rite end of pipe i
        	if (dup2(fd_list[i][WRITE_END], STDOUT_FILENO) == -1)
        	{
        	 	fprintf(stderr, "dup2 failed for child %d\n", (i + 1));
           		 return -1;
        	} // end if
        	
        	// close the pipe, no99 that it has been duplicated
        	close(fd_list[i][WRITE_END]);
        	close(fd_list[i][READ_END]);
        	
        	
        	
        	// execute the commnad
        	execvp(cl[i][i], cl[i]);
        	
        	// a successful exec never returns, so if this returns, then print an error
        	fprintf(stderr, "execution of child # %d failed", (i + 1));
       		 return -1;
	
	} // end else
	
	} // end else if
	
} // end for


	// parent does not need the pipes, so close them
	for (int i = 0; i < NUM_PIPE_COMMANDS - 1; i++)
	{
		close(fd_list[i][WRITE_END]);
        	close(fd_list[i][READ_END]);
	} // end for
	
	// wait for all children to terminate
	while (wait(NULL) > 0);
	
	fprintf(stdout, "---------------------\n");
    	fprintf(stdout, "parent: Children has finished execution, parent is done\n");


	return 0;
} // end main















