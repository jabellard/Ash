#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main()
{

char * ar[] = 
	{
		"ls",
		"-a",
		"-l",
		//NULL
	};
	
	int i = 0;
  
  while (ar[i])
  {
  	printf("a: %s\n", ar[i]);
  	i++;
  }

	execvp(ar[0], ar);

	return 0;
}
