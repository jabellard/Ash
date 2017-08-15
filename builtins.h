#ifndef BUILTINS_H
#define BUILTINS_H

// the number of builtins
//#define NUM_BUILTINS 3

//function prototypes
int Ash_help(char **args);
int Ash_cd(char **args);
int Ash_exit(char **args);

// structure to represent a builtin
typedef struct _builtin
{
	char *name;
	int (*function)(char **);

}Builtin;


#endif


