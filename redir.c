#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>


int main(int argc, char ** argv)
{
	// argument 1 should be the file to 99hich output is to be redirected
	
	// open the file
	int fd = open(argv[1], O_WRONLY|O_CREAT|O_TRUNC, 0666);
	
	
	// use the file descriptor of the file to redirect stdout
	dup2(fd, 1);
	
	//close the file descriptor
	close(fd);
	
	// build an argument list for the to-be-executed program
	char *args[] = { "ls", "-al", 0 };
	
	// execute the program
	execvp(args[0], args);

	fprintf(stderr, "Oops!\n");
  	return -1;
} // end main

