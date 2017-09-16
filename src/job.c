/**
* @file job.c
* @author Joe Nathan Abellard {https://github.com/joenatech7}
* 
* @date September 1, 2017
* @brief Contains the code for the job control sub system.
*
* @par Description
* This file contains contains the code for the job control sub system.
*
* @sa
* job.h
*
*/


#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <job.h>
#include <err.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>


/**
* @brief Process group ID of the shell.
*
* @par Description
* The process group ID of the shell. This variable is set the shell calls init_shell() to
* initialize itself. 
*
* @sa
* init_shell(), put_job_in_background(), getpgrp(), kill(), getpid(), setpgid(), and tcsetpgrp().
* 
*/
pid_t Ash_pgid;

/**
* @brief Default terminal modes of the shell.
*
* This variable stores the default terminal modes of the shell. It is set when the shell calls 
* init_shell() to initialize itself.
*
* @sa
* #Ash_terminal, #Ash_is_interactive, init_shell(), tcgetattr(), tcsetattr(), and put_job_in_foreground().
*/
struct termios Ash_tmodes;


/**
* @brief File descriptor to standard input for the shell.
* 
* File descriptor to standard input for the shell. This variable is set when the shell calls 
* init_shell() to initialize itself. It refers to a terminal device if the shell is running
* interactively.
*
* @sa 
* init_shell(), @e isatty(), @e tcgetpgrp(), tcgetattr(), tcsetpgrp(), tcsetattr(), execute_shell(),
* and #Ash_tmodes, and #Ash_is_interactive .
*/
int Ash_terminal;

/**
* @brief Indicates if the shell is running interactively.
*
* @par Description
* This variable indicates if the shell is running interactively. If its value is non-zero value (i.e. true)
* , then the shell is running interactively. Otherwise, the shell is not running interactively.
* Its value is set when the shell calls init_shell() to initialize itself. 
*
* @sa
* init_shell(), @e isatty(), execute_process(), execute_shell(), #Ash_tmodes, and #Ash_is_interactive.
*
*/
int Ash_is_interactive;


/**
* @brief Head of the job list.
*
* @par Description
* The shell uses a linked list to maintain a record of each active job (_job). This pointer points
* to the head of that list. 
*
* @sa
* _job, _job::next, Ash_jobs(), Ash_killall(), find_job(), find_job_id(), mark_process_status(),
* do_job_notification(), add_job_to_list(), and print_job_list().
*/
Job *job_list_head = NULL;

/**
* @brief Job state strings.
* 
* @par Description
* This array stores job state strings.
*
* @sa
* #_job_state, Ash_jobs(), do_job_notification(), _job, _job::notified, _process::stopped, _process::completed,
* format_job_info(), and update_status().
*/
char *job_states[] =
{
	"Running",
	"Stopped",
	"Completed"
}; // end job_states[]

/**
* @brief List of shell builtins an associated function.
*
* @par Description
* This array stores the list of shell builtins and associated functions.
*
* @sa
* _builtin, Builtin, is_builtin(), command_completion_generator(), and and execute_job().
*
*/
Builtin builtins[] = 
{

	{"cd", &Ash_cd},
	{"exit", &Ash_exit},
	{"jobs", &Ash_jobs},
	{"fg", &Ash_fg},
	{"bg", &Ash_bg},
	{"help", &Ash_help},
	{"kill", &Ash_kill},
	{"killall", &Ash_killall},
	{NULL, NULL}
}; // end builtins[]



/**
* @brief Stores the path of a help file for a shell builtin.
*
* @par Description
* This array stores the path of a help file for a shell builtin.
*
* @sa
* Ash_cd(), Ash_exit(), Ash_jobs(), Ash_fg(), Ash_bg(), Ash_help(), Ash_kill(), Ash_killall(),
* display_builtin_help(), is_builtin(), and #builtins.
*
*/
char BUILTIN_HELP_PATH[PATH_MAX];


int display_builtin_help(int dest_fd, const char *help_file)
{

	char *home_dir = getenv("HOME");
	strcpy(BUILTIN_HELP_PATH, home_dir);
	strcat(BUILTIN_HELP_PATH, "/.Ash/b-help/");
	strcat(BUILTIN_HELP_PATH, help_file);
	strcat(BUILTIN_HELP_PATH, "\0");
	
	// open the help file
	int help_fd = open(BUILTIN_HELP_PATH, O_RDONLY);
	
	// check for errors
	if (help_fd == -1)
	{
		err_msg("display_builtin_help(): open()");
		return -1;
	} // end if
	
	// find the file size
	int file_size = lseek(help_fd, 0, SEEK_END);
	if (file_size == -1)
	{
		err_msg("display_builtin_help(): lseek()");
		return -1;
	} // end if	
	
	// seek back to the beginning of the file
	int result = lseek(help_fd, 0, SEEK_SET);
	if (result == -1)
	{
		err_msg("display_builtin_help(): lseek()");
		return -1;
	} // end if
	
	// allocate a string buffer
	char *buf = malloc(file_size);
	if (!buf)
	{
		err_msg("display_builtin_help(): malloc()");
		return -1;
	} // end if
	
	// read data into the buffer
	result = read(help_fd, buf, file_size);
	if (result == -1 || result != file_size)
	{
		err_msg("display_builtin_help(): read()");
		return -1;
	} // end if
	
	// close the file descriptor
	result = close(help_fd);
	if (result == -1)
	{
		err_msg("display_builtin_help(): close()");
		return -1;
	} // end if
	
	// 99rite the help to the destination file
	result = write(dest_fd, buf, file_size);
	if (result == -1 || result != file_size)
	{
		err_msg("display_builtin_help(): write()");
		return -1;
	} // end if
	
	// free the buffer
	sfree(buf);
	
	// return
	return 0;
	
	

} // end display_builtin_help()
int Ash_cd(Process *p, int in_file, int out_file, int err_file)
{
	if (p->argc == 2 && (strcmp(p->argv[1], "h") == 0))
	{
		return display_builtin_help(out_file, "cd");
	} // end if
	else if (p->argv[1] == NULL)
	{
		dprintf(err_file, "Ash: cd: expected an argument.\n");
		dprintf(err_file, "Ash: cd: execute \"cd h\" for help.\n");		
		return -1;
	} // end else if
	else
	{
		int result = chdir(p->argv[1]);
		if (result != 0)
		{
		
			// error checking code
			err_msg("Ash: cd: chdir()");
			return -1;
		} // end if
		else
		{
		
			dprintf(out_file, "changed directory to: %s\n", p->argv[1]);
			return 0;
		} // end else
	} // end else
} // end Ash_cd()

int Ash_exit(Process *p, int in_file, int out_file, int err_file)
{
	if (p->argc == 2 && (strcmp(p->argv[1], "h") == 0))
	{
		return display_builtin_help(out_file, "exit");
	} // end if
	else if (p->argc >= 2)
	{
		dprintf(err_file, "Ash: exit: invalid argument(s).\n");
		dprintf(err_file, "Ash: exit: execute \"exit h\" for help.\n");
		return -1;
	} // end else if
	else
	{	
		exit(EXIT_SUCCESS);
	} // end else
} // end Ash_exit()
int print_job_command(Job *j, int dest_fd)
{	
	dprintf(dest_fd, "\t Command: ");
	int i = 0;
	for (i; i < j->num_processes; i++)
	{
		int k = 0;
		for (k; k < (j->processes[i])->argc; k++)
		{
			dprintf(dest_fd, "%s ", (j->processes[i])->argv[k]);
		} // end for
		if ( (i + 1) < j->num_processes)
		{
			dprintf(dest_fd, "| ");
		} // end if
	} // end for
	dprintf(dest_fd, "\n");
	return 0;
	
} // end print_job_command()
int Ash_jobs(Process *p, int in_file, int out_file, int err_file)
{
	if (p->argc == 2 && (strcmp(p->argv[1], "h") == 0))
	{
		return display_builtin_help(out_file, "jobs");
	} // end if	
	else
	{
	update_status();
	Job *j;
	for (j = job_list_head; j; j = j->next)
	{
		if (job_is_completed(j))
		{
			dprintf(out_file, "[%d] %ld %s:\n", j->id, (long)j->pgid, job_states[C]);
			print_job_command(j, out_file);
		} // end if
		else if (job_is_stopped(j))
		{
			dprintf(out_file, "[%d] %ld %s:\n", j->id, (long)j->pgid, job_states[S]);
			print_job_command(j, out_file);
		} // end else if
		else
		{
			dprintf(out_file, "[%d] %ld %s:\n", j->id, (long)j->pgid, job_states[R]);
			print_job_command(j, out_file);
		} // end else
	} // end for
	return 0;
	} // end else
} // end Ash_jobs()

int Ash_fg(Process *p, int in_file, int out_file, int err_file)
{
	// grammar fg %<job-num>
	
	if (p->argc == 2 && (strcmp(p->argv[1], "h") == 0))
	{
		return display_builtin_help(out_file, "fg");
	} // end if	
	else if (p->argc == 2)
	{
		
					//check if the first char is '%'
					if (p->argv[1][0] != '-')
					{
						// argument is  not started with '%' char
						dprintf(err_file, "Ash: fg: invalid argument \"%s\"\n", p->argv[1]);
						dprintf(err_file, "Ash: fg: execute \"fg h\" for help.\n");		
						return -1;
						
					} // end if
					else
					{
						//1. convert the rest of the string str[1..strlen(str) - 1] to an integer
						
						int job_num = atoi(&p->argv[1][1]);
					
						//2. find the job with the corresponding job number (if the job number is valid)
						Job *j;
						j = find_job_id(job_num);
						
						if (!j)
						{
							dprintf(err_file, "Ash: fg: invalid job number \"%d\"\n", job_num);
							return -1;
						} // end if
						else
						{
							// continue the job in the background
							continue_job(j, 1);
						
						} // end else
					

					} // end else			
	} // end else if
	else
	{
		// bad grammer (invalid number of arguments)
		dprintf(err_file, "Ash: fg: invalid number of arguments\n");
		dprintf(err_file, "Ash: fg: execute \"fg h\" for help.\n");		
		return -1;	
	} // end else
	return 0;
} // end Ash_fg

int Ash_bg(Process *p, int in_file, int out_file, int err_file)
{
	// grammar: bg %<job-num> ...
	//	MAKE SURE YACC CAN PARSE '%' CHAR
	//check if command grammar is OK----------------------
	// check if there are enough arguments
	
	if (p->argc == 2 && (strcmp(p->argv[1], "h") == 0))
	{
		return display_builtin_help(out_file, "bg");
	} // end if	
	else if (p->argc < 2)
	{
		// error
		// bad grammer (invalid number of arguments)
		dprintf(err_file, "Ash: bg: invalid number of arguments.\n");
		dprintf(err_file, "Ash: bg: execute \"bg h\" for help.\n");		
		return -1;
	} // end else if
	else
	{
		//parse each argument
		int i = 1;
		for (i; i < p->argc;i++)
		{
			
				// argument is not of the right lenght; print err message and continue
				if (strlen(p->argv[i]) < 2)
				{
					// argument is not of the right lenght; print err message and continue
					dprintf(err_file, "Ash: bg: invalid argument \"%s\".\n", p->argv[i]);
					dprintf(err_file, "Ash: bg: execute \"bg h\" for help.\n");		
					return -1;				
				} // end if
				else
				{
					//check if the first char is '%'
					if (p->argv[i][0] != '-')
					{
						// argument is  not started with '%' char
						dprintf(err_file, "Ash: bg: invalid argument \"%s\"\n", p->argv[i]);
						dprintf(err_file, "Ash: bg: execute \"bg h\" for help.\n");		
						return -1;
						
					} // end if
					else
					{
						//1. convert the rest of the string str[1..strlen(str) - 1] to an integer
						
						int job_num = atoi(&p->argv[i][1]);
					
						//2. find the job with the corresponding job number (if the job number is valid)
						Job *j;
						j = find_job_id(job_num);
						
						if (!j)
						{
							dprintf(err_file, "Ash: bg: invalid job number \"%d\".\n", job_num);
							return -1;
						} // end if
						else
						{
							// continue the job in the background
							continue_job(j, 0);
						
						} // end else
					

					} // end else				
				} // end else
		} // end for
	} // end else
	return 0;
} // end Ash_bg()

int Ash_help(Process *p, int in_file, int out_file, int err_file)
{
	dprintf(out_file, "Sample shell grammar:\n cmd [arg]* [| cmd [arg]*]*\n");
	dprintf(out_file, "execute \"man Ash\" for more details.\n");
	return 0;
} // end Ash_hel()

int Ash_kill(Process *p, int in_file, int out_file, int err_file)
{
	//grammar: kill l | s<sig-name> | n<sig-num> %<job-num> ...
	
	//check if there are enough arguments
	if (p->argc == 2 && (strcmp(p->argv[1], "h") == 0))
	{
		return display_builtin_help(out_file, "kill");
	} // end if	
	else if (p->argc == 2 || p->argc >= 3)
	{
		// gammar 1: list signals
		if (p->argc == 2)
		{
			// check the argument
			if (strcmp(p->argv[1], "l") == 0)
			{
				// valid argument, list signals
				// use a script to generate array of signal names
				dprintf(out_file, "list \n");
			} // end if
			else
			{
				// invalid argument
				dprintf(err_file, "Ash: kill: invalid argument \"%s\"\n", p->argv[1]);
				dprintf(err_file, "Ash: kill: execute \"kill h\" for help.\n");		
				return -1;
			} // end else
		} // end if
		// grammar 2: send signals		
		else
		{
			// the signal number to be sent
			int sig_num = 0;
			// ----------------------------------------------check the signal sepecification
			if (p->argv[1][0] == 's')
			{
				//dprintf(out_file, "inside s...\n");
				// check if the signal spec is of the right length
						//dprintf(out_file, "lenght =  %d\n", strlen(p->argv[1]));				
				if (strlen(p->argv[1]) == 7 || strlen(p->argv[1]) == 8)
				{	

					// find out which signal is to be sent
					if (strcmp(&p->argv[1][1], "SIGCONT") == 0)
					{
						sig_num = SIGCONT;
					} // end if
					else if (strcmp(&p->argv[1][1], "SIGKILL") == 0)
					{
						sig_num = SIGKILL;
					} // end else if
					else if (strcmp(&p->argv[1][1], "SIGSTOP") == 0)
					{
						sig_num = SIGSTOP;
					} // end else if
					else if (strcmp(&p->argv[1][1], "SIGQUIT") == 0)
					{
						sig_num = SIGQUIT;
					} // end else if
					else
					{
									dprintf(out_file, "1...\n");
						// invalid signal
						// invalid signal spec lenght
						dprintf(err_file, "Ash: kill: invalid signal specification \"%s\"\n", &p->argv[1][1]);
						dprintf(err_file, "Ash: kill: execute \"kill h\" for help.\n");		
						return -1;
					} // end else
				} // end if
				else
				{
									dprintf(out_file, "2...\n");				
					// invalid signal spec lenght
					dprintf(err_file, "Ash: kill: invalid signal specification \"%s\"\n", p->argv[1]);
					dprintf(err_file, "Ash: kill: execute \"kill h\" for help.\n");		
					return -1;
				} // end else
			} // end if
			else if (p->argv[1][0] == 'n')
			{
				// check if the signal spec is of the right lenght
				if (strlen(p->argv[1]) >= 2)
				{
					sig_num = atoi(&p->argv[1][1]);
				} // end if
				else
				{
					// invalid signal spec lenght
					dprintf(err_file, "Ash: kill: invalid signal specification \"%s\"\n", p->argv[1]);
					dprintf(err_file, "Ash: kill: execute \"kill h\" for help.\n");		
					return -1;
				} // end else
			} // end else if
			else
			{
				//dprintf(out_file, "3...\n");			
				dprintf(err_file, "Ash: kill: invalid signal specification \"%s\"\n", p->argv[1]);
				dprintf(err_file, "Ash: kill: execute \"kill h\" for help.\n");		
				return -1;
			} // end else
			
			// -------------------------------------------check the jobspec and send signals
			int i;
			for (i = 2; i < p->argc; i++)
			{
				//check if the arg string is of the right lenght (>= 2)
				if(strlen(p->argv[i]) < 2)
				{
					// argument is not of the right lenght; print err message and continue
					dprintf(err_file, "Ash: kill: invalid argument \"%s\"\n", p->argv[i]);
					dprintf(err_file, "Ash: kill: execute \"kill h\" for help.\n");		
					return -1;
				} // end if
				else
				{
					//check if the first char is '%'
					if (p->argv[i][0] != '-')
					{
						// argument is  not started with '%' char
						dprintf(err_file, "Ash: kill: invalid argument \"%s\"\n", p->argv[i]);
						dprintf(err_file, "Ash: kill: execute \"kill h\" for help.\n");		
						
					} // end if
					else
					{
						//1. convert the rest of the string str[1..strlen(str) - 1] to an integer
						int job_num = -1;
						job_num = atoi(&p->argv[i][1]);
					
						//2. find the job with the corresponding job number (if the job number is valid)
						Job *j;
						j = find_job_id(job_num);
						
						if (!j)
						{
							dprintf(err_file, "Ash: kill: invalid job number \"%d\"\n", job_num);
							return -1;
						} // end if
						else
						{
							// send the signal to the job
							if (kill(-j->pgid, sig_num) == -1)
							{
								dprintf(err_file, "Ash: kill: could not send signalto job # %d\n", job_num);
								return -1;
							} // end if
							
						
						} // end else
					

					} // end else
				} // end else
			} // end for
		} // end else

	} // end else if
	else
	{
		// bad grammer (invalid number of arguments)
		dprintf(err_file, "Ash: kill: invalid number of arguments\n");
		dprintf(err_file, "Ash: kill: execute \"kill h\" for help.\n");		
		return -1;
	} // end else
	return 0;
} // end Ash_kill()
int Ash_killall(Process *p, int in_file, int out_file, int err_file)
{
	//grammar: killall [l] or killall s<sig-name> | n<sig-num>
	
	//check if there are enough arguments
	if (p->argc == 2 && (strcmp(p->argv[1], "h") == 0))
	{
		return display_builtin_help(out_file, "killall");
	} // end if	
	else if (p->argc == 2)
	{
		int sig_num;
		if (strcmp(p->argv[1], "l") == 0)
		{
			dprintf(out_file, "list \n");
			return 0;
		} // end if
		else if (p->argv[1][0] == 's')
		{
				//dprintf(out_file, "inside s...\n");
				// check if the signal spec is of the right length
				//dprintf(out_file, "lenght =  %d\n", strlen(p->argv[1]));				
				if (strlen(p->argv[1]) == 7 || strlen(p->argv[1]) == 8)
				{	

					// find out which signal is to be sent
					if (strcmp(&p->argv[1][1], "SIGCONT") == 0)
					{
						sig_num = SIGCONT;
					} // end if
					else if (strcmp(&p->argv[1][1], "SIGKILL") == 0)
					{
						sig_num = SIGKILL;
					} // end else if
					else if (strcmp(&p->argv[1][1], "SIGSTOP") == 0)
					{
						sig_num = SIGSTOP;
					} // end else if
					else if (strcmp(&p->argv[1][1], "SIGQUIT") == 0)
					{
						sig_num = SIGQUIT;
					} // end else if
					else
					{
									dprintf(out_file, "1...\n");
						// invalid signal
						// invalid signal spec lenght
						dprintf(err_file, "Ash: killall: invalid signal specification \"%s\"\n", &p->argv[1][1]);
						dprintf(err_file, "Ash: killall: execute \"killall h\" for help.\n");		
						return -1;
					} // end else
					Job *j;
					for (j = job_list_head; j; j = j->next)
					{
					if (kill(-j->pgid, sig_num) == -1)
					{
						dprintf(err_file, "Ash: killall: could not send signal to job # %d\n", j->id);
						continue;
					} // end if
					} // end for
				} // end if
				else
				{
									dprintf(out_file, "2...\n");				
					// invalid signal spec lenght
					dprintf(err_file, "Ash: killall: invalid signal specification \"%s\"\n", p->argv[1]);
					dprintf(err_file, "Ash: killall: execute \"killall h\" for help.\n");
					return -1;
				} // end else
		} // end else if
		else if (p->argv[1][0] == 'n')
		{
				// check if the signal spec is of the right lenght
				if (strlen(p->argv[1]) >= 2)
				{
					sig_num = atoi(&p->argv[1][1]);
					Job *j;
					for (j = job_list_head; j; j = j->next)
					{
					if (kill(-j->pgid, sig_num) == -1)
					{
						dprintf(err_file, "Ash: killall: could not send signalto job # %d\n", j->id);
						continue;
					} // end if
					} // end for
				} // end if
				else
				{
					// invalid signal spec lenght
					dprintf(err_file, "Ash: killall: invalid signal specification \"%s\"\n", p->argv[1]);
					dprintf(err_file, "Ash: killall: execute \"killall h\" for help.\n");
					return -1;
				} // end else	

		
		} // end else if
		else
		{
			dprintf(err_file, "Ash: killall: invalid number of arguments \n");
			dprintf(err_file, "Ash: killall: execute \"killall h\" for help.\n");
			return -1;
		} // end else
		} // end else if
		else
		{
			dprintf(err_file, "Ash: killall: invalid number of arguments \n");
			dprintf(err_file, "Ash: killall: execute \"killall h\" for help.\n");
			return -1;
		} // end else
		
		return 0;
		
} // end Ash_killall()

int is_builtin(Process *p)
{
	int i = 0;
	for (i; builtins[i].name; i++)
	{
		if(strcmp(p->argv[0], builtins[i].name) == 0)
		{
			return i;
		} // end if
	} // end for
	
	return -1;
} // end is_builtin()

void init_shell()
{

    /* See if we are running interactively.  */
    Ash_terminal = STDIN_FILENO;
    Ash_is_interactive = isatty(Ash_terminal);
    if (Ash_is_interactive)
    {

        /* Loop until we are in the foreground.  */
        while (tcgetpgrp (Ash_terminal) != (Ash_pgid = getpgrp ()))
        {

            kill (-Ash_pgid, SIGTTIN);        
        } // end while


        /* Ignore interactive and job-control signals.  */
        signal (SIGINT, SIG_IGN);
        signal (SIGQUIT, SIG_IGN);
        signal (SIGTSTP, SIG_IGN);
        signal (SIGTTIN, SIG_IGN);
        signal (SIGTTOU, SIG_IGN);
        //signal (SIGCHLD, SIG_IGN);

        /* Put ourselves in our own process group.  */
        Ash_pgid = getpid ();
        if (setpgid (Ash_pgid, Ash_pgid) < 0)
        {
            err_msg("Couldn't put the shell in its own process group");
            exit (1);
        }

        /* Grab control of the terminal.  */
        tcsetpgrp (Ash_terminal, Ash_pgid);

        /* Save default terminal attributes for shell.  */
        tcgetattr (Ash_terminal, &Ash_tmodes);

    }
} // end init_shell()

Process* create_process(void)
{
	Process *p = (Process *) malloc(sizeof(Process));
	if (!p)
	{
		err_msg("malloc");
		return NULL;
	} // end if
	int i = 0;
	for (i; i < MAX_ARGS; i++)
	{
		p->argv[i] = NULL;
	} // end for
	p->argc = 0;
	p->pid = -1;
	p->completed = 0;
	p->stopped = 0;
	return p;

} // end create_process()

int insert_arg_in_process(Process *p, const char * arg)
{
	if (p->argc < (MAX_ARGS))
	{
		p->argv[p->argc] = strdup(arg);
		
		p->argc++;
		
		return 0;
	} // end if
	else
	{
		fprintf(stderr, "exeeded number of arguments allowed per a simple command (%d)\n", MAX_ARGS);
		return -1; //exit
	} // end else	

} // end insert_arg_in_process()

void print_process(const Process *p)
{
	int i = 0;
	for (i; i < p->argc; i++)
	{
		printf("%s ", p->argv[i]);
	} // end for
	printf("\n----------------------------------------\n");	
}


Process* process_dup(const Process *p)
{
	if (!p)
	{
		return NULL;
	} // end if
	Process *new_p = create_process();
	
	new_p->argc = p->argc;
	new_p->pid = p->pid;
	new_p->completed = p->completed;
	new_p->stopped = p->stopped;
	new_p->status = p->status;
	
	int i = 0;
	for (i; i < p->argc; i++)
	{
		new_p->argv[i] = strdup(p->argv[i]);
	} // end for
	
	return new_p;

} // end process_dup()


void execute_process(Process *p, pid_t pgid, int in_file, int out_file, int err_file, int foreground)
{
	pid_t pid;

	if (Ash_is_interactive)
	{
	
	 	/*
	 	 Place the process into the appropriate process group, and give it access to the
	 	 controlling terminal, if nessasary.
	 	*/
		pid = getpid ();
		if (pgid == 0)
		{
			pgid = pid;
		} // end if 
		setpgid (pid, pgid);
		if (foreground)
		{
			tcsetpgrp (Ash_terminal, pgid);		
		} // end if


		// set the disposition of job control stop signals back to default.
		signal (SIGINT, SIG_DFL);
		signal (SIGQUIT, SIG_DFL);
		signal (SIGTSTP, SIG_DFL);
		signal (SIGTTIN, SIG_DFL);
		signal (SIGTTOU, SIG_DFL);
		signal (SIGCHLD, SIG_DFL);
	}

	// perform I/O redirection, if nessasary
	if (in_file != STDIN_FILENO)
	{
		dup2 (in_file, STDIN_FILENO);
		close (in_file);
	}
	if (out_file != STDOUT_FILENO)
	{
		dup2 (out_file, STDOUT_FILENO);
		close (out_file);
	}
	if (err_file != STDERR_FILENO)
	{
		dup2 (err_file, STDERR_FILENO);
		close (err_file);
	}

	// execute the new program
	execvp(p->argv[0], p->argv);
	err_exit(p->argv[0]);
} // end execute_process()


void destroy_process(Process* p)
{
	int i = 0;
	for (i; i < p->argc; i++)
	{
		sfree(p->argv[i]);
	} // end for
	
	sfree(p);

} // end destroy_process()

Job* create_job(void)
{
	Job *j = (Job *) malloc(sizeof(Job));
	if (!j)
	{
		err_msg("malloc");
		return NULL;
	} // end if
	
	int i = 0;
	for (i; i < MAX_PROCESSES; i++)
	{
		j->processes[i] = NULL;
	} // end for
	
	j->next = NULL;
	j->id = -1;
	j->num_processes = 0;
	j->pgid = 0;
	j->notified = 0;
	//j->saved_tmodes = (struct termios)0;
	j->stdin_ = STDIN_FILENO;
	j->stdout_ = STDOUT_FILENO;
	j->stderr_ = STDERR_FILENO;
	j->in_file = NULL;
	j->out_file = NULL;
	j->err_file = NULL;
	j->foreground = 1;
	
	return j;
} // end create_job()

int insert_process_in_job(Job *j, const Process *p)
{
	if (j->num_processes < (MAX_PROCESSES))
	{
		//Process *new_p = process_dup(p);
		j->processes[j->num_processes] = process_dup(p);
		j->num_processes++;
		return 0;
	} // end if
	else
	{
		fprintf(stderr, "exeeded number of arguments allowed per a simple command (%d)\n", MAX_PROCESSES);
		return -1; //exit	
	} // end else

} // end insert_process_in_job

void print_job(const Job *j)
{
	printf("Job # %d:\n", j->id);
	int i = 0;
	for(i; i < j->num_processes; i++)
	{
		printf("process %d: ", (i + 1));
		print_process(j->processes[i]);
	} // end for
} // end print_job()


int destroy_job(Job *j)
{
	if (!j)
	{
		fprintf(stderr, "destroy_job: invalid job pointer.\n");
		return -1;
	} // end if
	int i = 0;
	for (i; i < j->num_processes; i++)
	{
		destroy_process(j->processes[i]);
	} // end for
	sfree(j->in_file);
	sfree(j->out_file);
	sfree(j->err_file);
	
	sfree(j);
	
	return 0;
} // end destroy_job()



Job* find_job(pid_t pgid)
{
	Job *j;
	
	for (j = job_list_head; j; j = j->next)
	{
		if (j->pgid == pgid)
		{
			return j;
		} // end if
	} // end for
	
	return NULL;

} // end find_job()


Job* find_job_id(int id)
{
	Job *j;
	
	for (j = job_list_head; j; j = j->next)
	{
		if (j->id == id)
		{
			return j;
		} // end if
	} // end for
	
	return NULL;

} // end find_job_id()

int job_is_stopped(Job *j)
{
	
	int i = 0;
	for (i; i < j->num_processes; i++)
	{
		if (!((j->processes[i])->completed) && !((j->processes[i])->stopped))
		{
			return 0;
		} // end if
	} // end for
	return 1;
} // end job_is_stopped()

int job_is_completed(Job *j)
{
	int i = 0;
	for (i; i < j->num_processes; i++)
	{
		if (!((j->processes[i])->completed))
		{
			return 0;
		} // end if
	} // end for
	return 1;
} // end job_is_completed()

void format_job_info (Job *j, const char *status)
{
	fprintf (stdout, "[%d] %ld %s\n", j->id, (long)j->pgid, status);
	
} // end format_job_info


int mark_process_status(pid_t pid, int status)
{
	
	if (pid > 0)
	{
		Job *j;
		j = job_list_head;
		for (j; j; j = j->next)
		{
			int i = 0;
			for (i; i < j->num_processes; i++)
			{
				if ((j->processes[i])->pid == pid)
				{
					(j->processes[i])->status = status;
					if (WIFSTOPPED(status))
					{
						(j->processes[i])->stopped = 1;
					} // end if
					else
					{
						(j->processes[i])->completed = 1;
						if (WIFSIGNALED(status))
						{
							fprintf(stderr, "%d: terminated by signal %d.\n", (int) pid, WTERMSIG(status));
						} // end if
					} // end else
					return 0;
				} // end if
				
			} // end for
		} // end for
		fprintf(stderr, "No child process with pid %d.\n", pid);
		return -1;	
	} // end if
	else if (pid == 0 || errno == ECHILD)
	{
		return -1;
	} // end else if
	else
	{
		err_msg("waitpid");
		return -1;
	} // end else

} // end mark_process_status()

void update_status(void)
{
	int status;
	pid_t pid;
	do
	{
		pid = waitpid(-1, &status, WUNTRACED | WNOHANG);
	}while (!mark_process_status(pid, status));

} // end update_status()



void wait_for_job(Job *j)
{
	int status;
	pid_t pid;

	do
	{
		pid = waitpid (-j->pgid, &status, WUNTRACED);
	}while (!mark_process_status(pid, status)
			&& !job_is_stopped(j)
			&& !job_is_completed(j));
} // end wait_for_job()



void do_job_notification(void)
{
	update_status();
	Job *last_job = NULL;
	Job *curr_job = NULL;
	Job *next_job = NULL;
	
	for (curr_job = job_list_head; curr_job; curr_job = next_job)
	{
		next_job = curr_job->next;
		
		if(job_is_completed(curr_job))
		{
			if (!curr_job->foreground)
			{
				format_job_info(curr_job, job_states[C]);
			} // end if
			
			if (last_job)
			{
				last_job->next = next_job;
			} // end if
			else
			{
				job_list_head = next_job;
			} // end else
			
			destroy_job(curr_job);
		} // end if
		else if (job_is_stopped(curr_job) && !curr_job->notified)
		{
			format_job_info(curr_job, job_states[S]);
			curr_job->notified = 1;
			last_job = curr_job;
		} // end else if
		else
		{
			last_job = curr_job;
		} // end else
	} // end for

} // end do_job_notification()



void put_job_in_foreground(Job *j, int cont)
{
	j->foreground = 1;
	/* Put the job into the foreground.  */
	tcsetpgrp (Ash_terminal, j->pgid);
	/* Send the job a continue signal, if necessary.  */
	if (cont)
	{
		tcsetattr(Ash_terminal, TCSADRAIN, &j->saved_tmodes);
		if (kill(-j->pgid, SIGCONT) < 0)
		{
			err_msg("kill (SIGCONT)");
		} // end if
	}
	/* Wait for it to report.  */
	wait_for_job (j);

	/* Put the shell back in the foreground.  */
	tcsetpgrp (Ash_terminal, Ash_pgid);

	/* Restore the shell’s terminal modes.  */
	tcgetattr (Ash_terminal, &j->saved_tmodes);
	tcsetattr (Ash_terminal, TCSADRAIN, &Ash_tmodes);	
} // end put_job_in_foreground()

void put_job_in_background(Job *j, int cont)
{
	j->foreground = 0;
	if (cont)
	{
		if (kill(-j->pgid, SIGCONT) < 0)
		{
			err_msg("kill (SIGCONT)");
		} // end if
	} // end if
	
} // end put_job_in_background()

void mark_job_as_running(Job *j)
{
	int i = 0;
	for (i; i < j->num_processes; i++)
	{
		(j->processes[i])->stopped = 0;
	} // end for
	j->notified = 0;
} // end mark_job_as_running()

void continue_job(Job *j, int foreground)
{
	mark_job_as_running(j);
	
	if (foreground)
	{
		put_job_in_foreground(j, 1);
	} // end if
	else
	{
		put_job_in_background(j, 1);
	} // end else
} // end continue_job()

void add_job_to_list(Job *j)
{

	if(job_list_head)
	{
		Job *temp;
		for(temp = job_list_head; temp->next; temp = temp->next);
		temp->next = j;
	} // end if
	else
	{
		job_list_head = j;
	} // end else
} // end add_job_to_list();


int execute_job(Job *j)
{

	Process *p;
	pid_t pid;
	int pipe_fds[2];
	int in_file;
	int out_file;
	int err_flag = 0;

	if (j->in_file)
	{
		j->stdin_ = open(j->in_file, O_RDONLY);
		if (j->stdin_ == -1)
		{
			err_msg(j->in_file);
			return -1;
		}
	}
	if (j->out_file)
	{
		j->stdout_ = open(j->out_file, O_WRONLY|O_CREAT|O_TRUNC, 0666); // 666 = rw-rw-rw-
		if (j->stdout_ == -1)
		{
			err_msg(j->out_file);
			return -1;
		}
	}
	if (j->err_file)
	{
		j->stderr_ = open(j->err_file, O_WRONLY|O_CREAT|O_TRUNC, 0666);
		if (j->stderr_ == -1)
		{
			err_msg(j->out_file);
			return -1;
		}
	}

	in_file = j->stdin_;
	int i = 0;
	for (i; i < j->num_processes; i++)
	{
		/* Set up pipes, if necessary.  */
		if (j->processes[i + 1])
		{
			if (pipe(pipe_fds) == -1)
			{
				err_msg("pipe");
				return -1;
			}
			out_file = pipe_fds[1];
		}
		else
		{
			out_file = j->stdout_;		
		} // end else
		
		int result;
		if ((result = is_builtin(j->processes[i])) != -1)
		{
			int bi_result = (builtins[result].func)(j->processes[i], in_file, out_file, j->stderr_);
			if (bi_result == -1)
			{
				return -1;
			} // end if
			(j->processes[i])->completed = 1;
		}
		else
		{
			/* Fork the child processes.  */
			pid = fork ();
			if (pid == 0)
				/* This is the child process.  */
				execute_process (j->processes[i], j->pgid, in_file,
								out_file, j->stderr_, j->foreground);
			else if (pid < 0)
			{
				/* The fork failed.  */
				err_msg("fork");
				return -1;
			}
			else
			{
				/* This is the parent process.  */
				(j->processes[i])->pid = pid;
				if (Ash_is_interactive)
				{
					if (!j->pgid)
					{
						j->pgid = pid;
					setpgid (pid, j->pgid);
					}
				}
			}
		/* Clean up after pipes.  */
		if (in_file != j->stdin_)
		{
			if (close(in_file) == -1)
			{
				err_msg("close");
				return -1;
			} // end if		
		} // end if

		if (out_file != j->stdout_)
		{
			if (close(out_file) == -1)
			{
				err_msg("close");
				return -1;
			} // end if		
		} // end if

		in_file = pipe_fds[0];
	} // end else
	} // end for

	//format_job_info(j, "launched");
	//add job to the job list
	add_job_to_list(j);
	//do_job_notification();
	

	// job control stuff
	if (!Ash_is_interactive)
	{
		wait_for_job(j);
	} // end if

	else if (j->foreground)
	{
		put_job_in_foreground(j, 0);	
	} // end else if
	else
	{
		put_job_in_background(j, 0); 
		format_job_info(j, "backgroud");
	} // end else

	
/*
	// bad job control stuff
	if (j->foreground == 1)
	{
		printf("waiting...\n");				
		// wait for all children to terminate
		while (wait(NULL) > 0);

		printf("done waiting\n");		
	} // end if
	else
	{
		//make zombies
	} // end else
	

*/
	return 0;

} // end execute_job()

void print_job_list(void)
{
	printf("Job list:\n");
	Job *j = NULL;
	for (j = job_list_head; j; j = j->next)
	{
		printf("Job # %d\n", j->id);
	} // end for

} // end print_job_list()
