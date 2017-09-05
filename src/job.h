/**
* @file job.h
* @author Joe Nathan Abellard {https://github.com/joenatech7}
* 
* @date September 1, 2017
* @brief Contains function prototypes, and data struture declarations for the job control sub system.
*
* @par Description
* This file contains contains the function prototypes and data struture declarations for the
* job control sub system.
*
* @sa
* job.c
*
*/


#ifndef JOB_H
#define JOB_H

#include <sys/types.h>
#include <termios.h>
#define MAX_ARGS 25
#define MAX_PROCESSES 25


 
//----------------------------------data structures





typedef struct _process
{
	char *argv[MAX_ARGS];
	int argc;
	pid_t pid;
	int completed;
	int stopped;
	int status;
	
} Process; // end struct _process




/*
*/
typedef struct _job
{
	struct _job *next;
	int id;
	Process *processes[MAX_PROCESSES];
	int num_processes;
	pid_t pgid;
	int notified;
	struct termios saved_tmodes;
	int stdin_;
	int stdout_;
	int stderr_;
	char *in_file;
	char *out_file;
	char *err_file;
	int foreground;


} Job; // end struct _job

/**
* @brief Type definition of pointer to shell builtin function.
*
* @par Description
* This is a type definition of a pointer to a shell builtin function. A shell builtin function is used to 
* launch a shell builtin.
*
* @sa
* #builtins, _builtin, Ash_cd(), Ash_exit(), Ash_jobs(), Ash_fg(), Ash_bg(), Ash_help(),
* Ash_kill(), Ash_killall(), is_builtin(), command_completion(), command_completion_generator(),
* and execute_job().
*
*/
 typedef int (*b_func)(Process *p, int in_file, int out_file, int err_file);
 
 
 /**
 *  @brief Maps a shell builtin to a function.
 *
 * @par Description
 * This structure is used to map a shell builtin to the function that is used to launch it.
 *
 * @sa
* #builtins, Ash_cd(), Ash_exit(), Ash_jobs(), Ash_fg(), Ash_bg(), Ash_help(), b_func
* Ash_kill(), Ash_killall(), is_builtin(), command_completion(), command_completion_generator(),
* and execute_job().
*
*/
 typedef struct _builtin
{
	/**
	* @brief The name of the shell builtin.
	*
	* @par Description
	* The name of the shell builtin.
	*
	* @sa #builtins, Ash_cd(), Ash_exit(), Ash_jobs(), Ash_fg(), Ash_help(), Ash_kill(),
	*  Ash_killall(), is_builtin(), command_completion(), command_completion_generator(),
	* b_func, _builtin::func, and execute_job().
	*/
	char *name;
	
	/**
	* @brief Pointer to the function used to launch the shell builtin.
	*
	* @par Description
	* The pointer to the function used to launch the shell builtin.
	*
	* @sa #builtins, Ash_cd(), Ash_exit(), Ash_jobs(), Ash_fg(), Ash_help(), Ash_kill(),
	*  Ash_killall(), is_builtin(), command_completion(), command_completion_generator(),
	* b_func, _builtin::name, and execute_job().
	*/
	b_func func;
}Builtin; //end struct _builtin


/**
* @brief Job states.
*
* @par Description
* This enum represents the possible states of a job (_job).
*
* @sa
* #job_states, _job, _job::notified, _process::stopped, _process::completed, Ash_jobs(), update_status(),
* format_job_info(), and do_job_notification() 
*
*/
typedef enum _job_state
{
	/**
	* @brief Running.
	*
	* @par Description
	* A job in this state is runnning.
	*
	* @sa
	* #job_states, _job, _job::notified, _process::stopped, _process::completed, update_status(), 
	* Ash_jobs(), _job_state::S, _job_state::C, format_job_info(), and do_job_notification().
	*/
	R, 
	
	/**
	* @brief Stopped.
	*
	* @par Description
	* A job in this state is stopped.
	*
	* @sa
	* #job_states, _job, _job::notified, _process::stopped, _process::completed,update_status(), 
	* Ash_jobs(), _job_state::R, _job_state::C, format_job_info(), and do_job_notification().
	*/	
	S, 
	
	/**
	* @brief Completed.
	*
	* @par Description
	* A job in this state is completed.
	*
	* @sa
	* #job_states, _job, _job::notified, _process::stopped, _process::completed, update_status(), 
	* Ash_jobs(), _job_state::S, _job_state::R, format_job_info(), and do_job_notification().
	*/	
	C
}job_state; // end enum _job_state

//----------------------------------function prototypes


/**
* @brief Prints the command associated with a job.
* 
* @par Description
* This function is used to print the command associated with a job (_job). 
*
* @param j
* The job whose command is to be printed.
*
* @param dest_fd
* A file descriptor to a file to which the command output is sent.
*
* @return
* Returns @e void .
*
* @sa _job, _job::num_processes, _job::processes, _process::argc, _process::argv, and Ash_jobs().
* 
* 
*/
int print_job_command(Job *j, int dest_fd);


/**
* @brief Prints the ID of jobs in the active job list.
*
* @par Description
* This function prints the ID (_job::id) of the jobs (_job) in the active job list (#job_list_head).
*
* @return
* Returns @e void .
*
* @sa _job, #job_list_head, and _job::id.
* 
*/
void print_job_list(void);


/**
* @brief Initializes the shell.
*
* @par Description
* The shell calls this function to initialize itself. The function does the following:
* @li Assigns STDIN_FILENO to the file descriptor for the shell's terminal (#Ash_terminal).
* @li Checks if #Ash_terminal refers to a terminal devide. If it does, then #Ash_is_interactive
* is set to a non-zero value. Otherwise it is set to @e 0 .
* @li If #Ash_is_interactive is non-zero, then  the function waits until the shell is in the foreground process group 
* for the session and the controlling terminal (#Ash_terminal).
* @li Once in the foreground process group of the session, it ignores all job control stop signals by setting thier disposition to 
* SIG_IGN. 
* @li Puts the shell in its own process group.
* @li Makes the shell's process group the foreground process group for the session and controlling
* terminal.
* @li Saves the default terminal modes for the shell in #Ash_tmodes
*
* @return
* Returns @e void .
*
* @sa 
* #Ash_terminal, #Ash_tmodes, #Ash_is_interactive, #Ash_pgid, STDIN_FILENO, @e isatty(), @e tcgetpgrp, 
* @e kill(), @e signal(), getpid(), setpgid(), err_msg(), and @e tcgetattr().
*
*/
void init_shell();


/**
* @brief Creates an instance of a process.
*
* @par Description
* This function is used to create an instance of a process (_process). It does the following :
* @li Dynamically allocate memory for an instance of a process.
* @li Assigns default values to all fields of the process instance.
*
* @return
* Returns NULL on failure, or a pointer to the allocated memory on success.
*
* @ sa
* _process, Process, #Max_ARGS, and process_dup().
*
*/
Process* create_process(void);


/**
* @brief Inserts an argument in the argument vector of a process.
* 
* @par Description
* This function is used to insert an argument in the argument vector (_process::argv) of
* a process (_process).
*
* @param p
* The process in which to perform the insertion.
*
* @param arg
* The argument to insert in the argument vector of the process.
*
* @return
* If the current number of arguments (_process::argc) in the process is less than #MAX_ARGS,
* the function inserts the argument in the argument vector of the process, and returns @e 0. Otherwise,
* it prints an error message, and returns @e -1.
*
* @sa
* _process, _process::argc, _process::argv, and #MAX_ARGS.
*
*/
int insert_arg_in_process(Process *p, const char * arg);


/**
* @brief Prints the command of a process.
*
* @par Description
* This function is used to print the command of a process (_process).
*
* @param p
* The process whose command is to be printed.
*
* @return
*  Returns @e void .
*
* @sa
* _process, _process::argv, and _process::argc.
*
*/
void print_process(const Process *p);


/**
* @brief Duplicates an instance of a process.
*
* @par Description
* This function is used to duplicate and instance of a process (_process). It does the following :
* @li Call create_process() to create an instance of a process.
* @li Transfers values from the process to be duplicated to the new process instance.
*
* @param p
* The process instance to be duplicated.
*
* @return
* Returns a pointer to the new duplicated process on success, or NULL on error. 
*
* @sa
* _process, create_process(), _process::argc, insert_process_in_job(), _process::argc,
* and @e strdup().
*
*/
Process* process_dup(const Process *p);



/**
* @brief Executes a process.
*
* @par Description 
* This function is used to execute a process (_process). It does the following:
* @li If the shell is running interactively (i.e. #Ash_is_interactive is non-zero), then this function puts the process in the
* same process group of the job (_job, _job::pgid) to which it belongs. If the process is the first in the
* job, then it becomes the group leader of the job.
* @li If the shell is running interactively, and @e foreground is non-zero,
* then this function makes the group to which the process belong the foreground process group for the session and the controlling terminal (#Ash_terminal).
* @li When the shell initializes itself via init_shell() , it sets the disposition of all job 
* control stop signals to SIG_IGN (i.e., it ignores those signals). Consequently, each child it creates
* inherits those signal dispostions. If the shell is running interactively, then 
* this function resets the disposition of those signals to SIG_DFL (i.e. default) .
* @li Performs IO redirection, if nessasary.
* @li execs the process. 
*
* @param p
* The process to be executed.
* 
* @param pgid
* If this parameter is @e 0 , then this process becomes the group leader of the job to which it
* belongs. This happens when the process is the first one in the job.
* 
* @param in_file
* The file descriptor of the file to which standard input for this process should point.
* If it is not equal to STDIN_FILENO, then standard input for this process is redirected to the
* file reffered to by this file descriptor.
*
* @param out_file
* The file descriptor of the file to which standard output for this process should point.
* If it is not equal to STDOUT_FILENO, then standard output for this process is redirected to the
* file reffered to by this file descriptor.
*
* @param err_file
* The file descriptor of the file to which standard error for this process should point.
* If it is not equal to STD_FILENO, then standard error for this process is redirected to the
* file reffered to by this file descriptor.
*
* @return
* This function is prototyped to return @e void , but never returns . If execution of the process (i.e., calling @e exec()) fails, then the process terminates with
* err_exit().
*
* @sa 
* _process, _job, _job::foreground, #Ash_is_interactive, #Ash_terminal, _job::in_file, _job::out_file,
* _job::err_file, _job::stdin_, _job::stdout_, _job::stderr_, getpid(), setpgid(), tcsetpgrp(),
* signal(), STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO, dup2(), execvp(), err_exit(), and
* execute_job().
*
*/
void execute_process(Process *p, pid_t pgid, int in_file, int out_file, int err_file, int foreground);


/**
* @brief Destroys a process instance.
*
* @par Description
* This function is used to destroy a process (_process) instance. It does the following :
* @li Calls sfree() to free the arguments (_process::argc) currently stored in the argument 
* vector (_process:argv) of the process.
* @li Calls sfree() to free the process instance itself.
*
* @return
* Returns @e void .
*
* @sa 
* _process, _process::argv, _process::argc, sfree(), and destroy_job().
*
*/
void destroy_process(Process* p);


/**
* @brief Creates a new job instance.
*
* @par Description
* This function is used to create a new job (_job) instance. It does the following:
* @li Dynamically allocate memory for a job instance.
* @li Assign default values to all the fields in the job instance.
*
* @return
* Returns a pointer to the newly create job instance on success, or NULL on failure.
* 
* @sa _job, #MAX_PROCESSES, and @e malloc().
*
*/
Job* create_job(void);


/**
* @brief Inserts a process instance in a job instance.
* 
* @par Description
* This function is used to insert a process (_process) in a job (_job) . It does the
* following:
* @li Checks if the current number of processes (_job::num_processes)  in the job's process list 
* (_job::processes) is less than the maximum number of processes allowed per job (#MAX_PROCESSES).
* If that's the case, its calls process_dup() to make a dupicate of the process, inserts the 
* duplicate in the job's process list,  increment the current number of processes in the job, and returns @e 0 . Otherwise, it prints an error message,
* and returns @e -1 .
*
* @param j
* The job in which the process is to be inserted.
*
* @param p
* The process to insert in the job's process list (_job::processes).
*
* @return
* If the current number of processes in the job is less than the maximum number processes allowed
* per job, the function returns @e 0 to indicate success. Otherwise, it returns @e -1 to indicate
* failure. 
*
* @sa
* _job, _process, _job::num_processes, #MAX_PROCESSES, and process_dup(). 
*
*/
int insert_process_in_job(Job *j, const Process *p);


/**
* @brief Prints a job.
*
* @par Description
* This function is used to print a job (_job). It does the following:
* @li Prints the job's ID (_job::id).
* @li Calls print_process() to print all the processes (_job::num_processes) in the job's
* process list (_job::processes).
*
* @param j
* The job to print.
*
* @return
* Returns @e void .
*
* @sa
* _job, _job::id, _job::num_processes, print_process(), _job::processes, and @e printf().
*
*/
void print_job(const Job *j);


/**
* @brief Destroys a job.
*
* @par Description
* This function is used to destroy a job (_job). It does the following:
* @li Calls destroy_process() to destroy all of the process (_job::num_processes) currently stored in the
* job's process list (_job::processes).
* @li Calls sfree() to free the pathnames of the job's I/O redirection files (_job::in_file,
* _job::out_file, and _job::err_file).
* @li Calls sfree() to free the job instance itself.
*
* @param j
* The job to destroy.
*
* @return
* Returns @e -1 on failure, or @e 0 on success.
*
* @sa
* _job, _job::num_processes, destroy_process(), _job::processes, _job::in_file, _job::out_file, and 
* _job::err_file.
* 
*/
int destroy_job(Job *j);


/**
* @brief Finds a job with a particular PGID.
*
* @par Description
* This function is used to find a job (_job) with a particular process group ID (_job::pgid).
* It does the following:
* @li Traverses the active job list (#job_list_head) and checks if there is a job on the list whose
* process group ID is @e pgid . If a job is found, the function returns a pointer to it.
*
* @param pgid
* The process group ID of the job to find.
*
* @return
* Returns a pointer to a job on success (i.e., a job is found), or NULL on failure (i.e., no job
* with process group ID @e pgid was found on the job list).
*
* @sa 
* _job, #job_list_head, and _job::pgid.
*/
Job* find_job(pid_t pgid);


/**
* @brief Determines if a job is stopped.
*
* @par Description
* This function is used to determine if a job (_job) is stopped. A job is stopped if all of its processes (_process)
* have either stopped or completed. 
*
* @param j
* The job whose status is to be checked.
*
* @return
* Returns @e 1 (i.e., true) if the job is stopped, or @e 0 (i.e., false) if the job is not stopped.
*
* @sa
* _process::stopped, _process::completed, _job, _process, Ash_jobs(), wait_for_job(), and
* do_job_notification().
*
*/
int job_is_stopped(Job *j);


/**
* @brief Determines if a job is completed.
*
* @par Description
* This function is used to determine if a job (_job) has completed. A job has completed if all of its processes (_process)
* have completed. 
*
* @param j
* The job whose status is to be checked.
*
* @return
* Returns @e 1 (i.e., true) if the job has completed, or @e 0 (i.e., false) if the job has not completed.
*
* @sa
*  _process::completed, _process::stopped, _job, _process, job_is_stopped(), Ash_jobs(), wait_for_job(), and
* do_job_notification().
*
*/
int job_is_completed(Job *j);


/**
* @brief Formats and prints info for a job.
*
* @par Description
* This function is used to format and print information for a job (_job).
* It does the following :
* @li Formats the information.
* @li Prints the information to standard output.
*
* @param j
* The job whose information is to be printed.
*
* @param status
* Status information for the job. This string is included in the formated information.
*
* @return
* Returns @e void.
*
* @sa 
* _job, do_job_notification(), and execute_job().
*/
void format_job_info (Job *j, const char *status);


/**
* @brief Marks the status of a stopped or terminated child process. 
*
* @par Description
* This function is used to mark the status of a stopped or terminated child process (_process). It does the following:
* @li Traverses the list of active jobs (#job_list_head), and for each job, the function checks its list of processes
* (#_job::processes) to see if @e pid matches that of one of its processes. If a match if found,
* it updates the _process::status field of the correspoinding process.
*
* @param pid
* The process ID of the stopped or terminated child process, as returned by @e waitpid().
*
* @param status
* The status of the stopped or terminated child process, as returned by @e waitpid().
*
* @return 
* Return 0 on success (i.e., the function successfully marked the status of a process), or
* -1 on error. The reasons for returning -1 are as follows:
* @li @e pid did not refer to any process in the process list of any job on the active job list.
* @li No child process of the shell reported a stopped or termination status (i.e., @e pid is @e
* 0 and @e errno is ECHILD).
* @li Some other error.
*
* @sa _job, _process, _process::pid, _process::completed, _process::stopped, @e waitpid(), _job::processes, #job_list_head, _process::status, and update_status().
*
*/
int mark_process_status(pid_t pid, int status);


/**
* @brief Updates the status of stopped or completed child processes of the shell.
*
* @par Description
* This function is used to update the status of stopped or completed child processes (_process) of the shell. 
* It does the following:
* @li Performs a non-blocking call to @e waitpid() in a loop to collect the status of stopped or
* completed children of the shell.
* @li Calls mark_process_status() to mark the status of those children.
*
* @return
* Returns void.
*
* @sa _process, _process::pid, _process::completed, _process::stopped, @e waitpid(),  _process::status, Ash_jobs(), do_job_notification(), and mark_process_status().
* 
*/
void update_status(void);


/**
* @brief waits for a job to stop or complete.
*
* @par Description
* This function is used to wait for a job (_job) to stop or complete. It does the following :
* @li Calls waitpid() to wait for all processes who's PGID is the same as the job's _job::pgid
* field (i.e., all processes belonging to this job) to stop or complete.
*
* @param j
* The job to wait for.
*
* @return 
* Returns @e void .
*
* @sa
* _job, _job::pgid, waitpid(), mark_process_status(), job_is_stopped(), job_is_completed(), and
* put_job_in_foreground().
*/
void wait_for_job(Job *j);


/**
* @brief Notifies the user of job status updates.
*
* @par Description
* This function is used to notify the user of job (_job) status updates.
* It does the following:
* @li Calls update_status() to update the status of stopped or completed processes (_process).
* @li For each job in the active job list (#job_list_head), it checks if the job has either 
* completed (by calling job_is_stopped() or stopped ( by calling job_is_completed()). If a job
* is completed and is not a foreground job, it notifies the user (by calling format_job_info()),
* and then removes the job from the active job list by calling destroy_job().
* If a job is stopped, and its changed status has not yet been reported, the function reports its
* status by calling format_job_status(), and then set its _job::notified field to @e 1.
*
* @return
* Returns void. 
* 
* @sa
* update_status(), #job_list_head, job_is_completed(), job_is_stopped(), format_job_info(), and
* _job::notified. 
*
*/
void do_job_notification(void);


/**
* @brief Puts a job in the foreground.
*
* @par Description
* This function is used to put a job in the foreground. It does the following:
* @li Sets the job's _job::foreground field to @e 1.
* @li Makes the job the foreground job for the session and the controlling terminal (#Ash_terminal).
* @li If the @e cont parameter is non-zero, it restores the job's saved terminal modes (_job::saved_tmodes), and sends the job the SIGCONT signal. 
* @li wait for the job to stop or complete by calling wait_for_job().
* @li Puts the shell back in the foreground.
* @li Saves the current terminal mode in the job's  _job::saved_tmodes field.
* @li Restores the shell's terminal modes(#Ash_tmodes).
*
* @param j
* The job to put in the foreground.
*
* @param cont
* If the parameter if true, the function does the following:
* @li Restores the job's saved terminal modes (_job::saved_tmodes).
* @li Sends the job the SIGCONT signal.
*
* @return 
* Returns @e void .
*
* @sa 
* _job, _job::foreground, #Ash_terminal, #Ash_tmodes, _job::pgid, _job::saved_tmodes, wait_for_job(), #Ash_pgid, continue_job(), 
* tcsetpgrp(), tcsetattr(), tcgetattr(), kill(), and execute_job().
* 
*/
void put_job_in_foreground(Job *j, int cont);



/**
* @brief Puts a job in the background.
*
* @par Description
* This function is used to put a job (_job) in the background.
* It does the following:
* @li Sets the job's _job::foreground field to @e 0.
* @li Sends the SIGCONT signal to the all processes (_process) in the job's process list (_job::processes) 
* if the value of @e cont is non-zero.
* 
* @param j
* The job to be put in the background.
*
* @param cont
* Determines if the SIGCONT signal is sent to the job. If this value is non-zero, then the
* SIGCONT signal is sent to all processes in the job's process list.
*
* @return
* Returns @e void .
*
* @sa
* _job, _job::foreground, _job::pgid, put_job_in_foreground(), continue_job(), 
* _process, _job::processes, and execute_job().
*
*/ 
void put_job_in_background(Job *j, int cont);



/**
* @brief Updates a job's state to @e Running .
*
* @par Description
*  This function is used to update a job's state to  @e Running (_job_state::R) .
* It does the following:
* @li Sets the _process::stopped field of each process in the job's process list (_job::processes)
* to @e 0.
* @li Sets the job's _job::notified field to @e 0.
*
* @param j
* The job who's state is to be changed.
*
* @return
* Returns @e void .
*
* @sa
* _job, _job::num_processes, _process, _process::stopped, _job::notified, _job_state, _job_state::R,
* #job_states, and continue_job().
*
*/
void mark_job_as_running(Job *j);


/**
* @brief Continues a job.
*
* This function is used to continue a job either in the foreground or in the background.
* It does the following:
* @li Calls mark_job_as_running() to update the job's state.
* @li If @e foreground is non-zero (i.e., true), it calls put_job_in_foreground() to put
* the job in the foreground. Otherwise, it calls put_job_in_background() to put the job
* in the background. 
*
* @param j
* The job to continue.
* 
* @param foreground
* Determines if the job is to be continued in the foreground or in the background. If this value
* is non-zero (i.e., true), then the job is continued in the foreground. Otherwise, it is continued
* in the background.
*
* @return
* Returns @e void .
*
* @sa 
* _job, mark_job_as_running(), put_job_in_foreground(), put_job_in_background(), Ash_bg(), _job_state,
* #job_states, and Ash_fg().
*
*/
void continue_job(Job *j, int foreground);


/**
* @brief Adds a job to the active job list.
*
* @par Description
* This function is used to add a job to the end of the linked list (#job_list_head) used to maintain
* active job records. 
*
* @param j
* The job to add to the active job list.
*
* @return
* Returns @e void .
*
* @sa
* _job, #job_list_head, and execute_job().
*/
void add_job_to_list(Job *j);


/**
* @param Executes a job.
*
* @par Description
* This function is used to execute a job (_job). It does the following :
* @li Performs I/O redirections for the job, if nessasary.
* @li Sets up pipes, if nessasary.
* @li For each process (_process) in the job, the function checks if it refers to a shell builtin via
* is_builtin(). If the process refers to a builtin, the builtin is launched. Otherwise, the function
* forks a child, and the child then calls execute_process() to @e exec a new program.
* @li Cleans up after pipes.
* @li After all processes in the job have been launched or executed, the function adds the job
* to the active job list via add_job_to_list(). In additiion, if #Ash_is_interactive is non-zero, it calls 
* wait_for_job() to wait for the job. If #Ash_is_interactive is @e 0 and the job's foreground
* field (_job::foreground) is non-zero, the function calls put_job_in_foreground() to put the job
* in the foreground. Otherwise, it calls put_job_in_background() to put the job in the background.
*
* @param j
* The job to execute.
*
* @return
* Returns @e 0 on success, or @e -1 on failure. Possible reasons for failure are as follows:
* @li Performing I/O redirection for the job failed.
* @li Setting up pipes failed. 
* @li A shell builtin failed.
* @li Forking a child failed.
* @li Cleaning up after pipes (i.e., closing file descriptors) failed.
*
* when any of the events above occurs, the function returns immidiately, and the job is never
* added to the active job list.
*
* @sa
* _job, _process, _job::in_file, _job::out_file, _job::err_file, _job::stdin_, _job::stdout_,
* _job::stderr_, @e open(), _job::num_processes, _job::processes, @e pipe(), is_builtin(), #builtins,
* @e fork(), execute_process(), err_msg(), #Ash_is_interactive, @e setpgid(), @e close(), 
* add_job_to_list(), wait_for_job(), _job::foreground, put_job_in_foreground(), put_job_in_background(),
* and format_job_info().
*
* @todo
* Find a way for a forked child to inform the function if it calling @e exec() fails.
*
*/
int execute_job(Job *j);


/**
* @brief [BUILTIN]: Changes the shell's current working directory.
* 
* @par Description
* This shell builtin is used to change the shell's current working directory. It accepts
* the following grammar : @n
* cd <path-to-new-direcotry> @n
* To change the current working directory, it does the following :
* @li Checks if the grammar of the command is correct.
* @li If the grammar of the command is correct, the function calls @e chdir() to change
* the current working directory.
*
* @param p
* This is a process (_process) . Only its argument vector (_process::argv) and argument count
* (_process::argc) fields are of interest for the builtin.
*
* @param in_file
* File descriptor to the builtin's standard input.
* 
* @param out_file
* File descriptor to the builtin's standard output.
*
* @param in_file
* File descriptor to the builtin's standard error.
*
* @return
* Returns @e 0 on success, or @e -1 on failure. Possible reasons for failure are as follows:
* @li The grammar passed to the builtin is invalid.
* @li The call to @e chdir() fails
*
* @sa
* _process, _process::argv, _process::argc, @e chdir() , err_msg(), @e dprintf(),
* Ash_exit(), Ash_jobs(), Ash_fg(), Ash_bg(), Ash_help(), Ash_kill(), and Ash_killall().
*
*/
int Ash_cd(Process *p, int in_file, int out_file, int err_file);


/**
* @brief [BUILTIN]: Exits from the shell.
* 
* @par Description
* This shell builtin is used exit from the shell. It accepts
* the following grammar : @n
* exit @n
* To exit from the shell, it does the following :
* @li calls @exit() with the status EXIT_SUCCESS.
*
* @param p
* This is a process (_process) . It is not of interest to this builtin.
*
* @param in_file
* File descriptor to the builtin's standard input.
* 
* @param out_file
* File descriptor to the builtin's standard output.
*
* @param in_file
* File descriptor to the builtin's standard error.
*
* @return
* Never returns. 
*
* @sa
* _process, @e exit(), 
* Ash_cd(), Ash_jobs(), Ash_fg(), Ash_bg(), Ash_help(), Ash_kill(), and Ash_killall()
*
*/
int Ash_exit(Process *p, int in_file, int out_file, int err_file);


/**
* @brief [BUILTIN]: List the jobs in the shell's active job list.
* 
* @par Description
* This shell builtin is used list the jobs (_job) in the shell's active job list (#job_list_head). It accepts
* the following grammar : @n
* jobs @n
* To list the jobs, it does the following :
* @li Calls update() status to update the status of child processes (_process) of the shell.
* @li Traverses the active job list, and for each job on the list, it prints its status and then
* calls print_job_command() to print the command that started the job. An active job can be in
* one of three states: (1) @e Running , (2) @e Stopped , or (3) @e Done . The function calls 
* job_is_completed(), and job_is_stopped() to determine if an active job is either completed, or
* stopped, respectively.
*
* @param p
* This is a process (_process) . It is not of interest to this builtin.
* 
* @param in_file
* File descriptor to the builtin's standard input.
* 
* @param out_file
* File descriptor to the builtin's standard output.
*
* @param in_file
* File descriptor to the builtin's standard error.
*
* @return
* Returns @e 0 .
*
* @sa
* _process, update_status(), _job, #job_list_head, job_is_completed(), job_is_stopped(), @e dprintf(),
* print_job_command(), _job_state, #job_states, 
* Ash_cd(), Ash_exit(), Ash_fg(), Ash_bg(), Ash_help(), Ash_kill(), and Ash_killall()
*
*/
int Ash_jobs(Process *p, int in_file, int out_file, int err_file);


/**
* @brief [BUILTIN]: Continues an active job in the foreground.
* 
* @par Description
* This shell builtin is used to continue an active job (_job) in the foreground. It accepts
* the following grammar : @n
* fg -<job-id> @n
* To continue the job in the foreground, it does the following :
* @li Checks if the command grammer is valid.
* @li If the grammar is valid, it calls find_job_id() to find a job on the active job list 
* (job_list_head) with job ID (_job::id) @e <job-id> .
* @li If it finds the job, it calls continue_job() to continue the job in the foreground.
*
* @param p
* This is a process (_process) . Only its argument vector (_process::argv) and argument count
* (_process::argc) are of interest to this builtin.
* 
* @param in_file
* File descriptor to the builtin's standard input.
* 
* @param out_file
* File descriptor to the builtin's standard output.
*
* @param in_file
* File descriptor to the builtin's standard error.
*
* @return
* Returns @e 0 on success, or @e -1 on error. Possible reasons for failure are as follows:
* @li The grammar of the command is invalid.
* @li The grammar of the command is valid, but @e <job-id> does not refer to a job on the active
* job list.
*
* @sa
* _process, _process::argv, _process::argc, @e dprintf(), @e atoi(), _job, find_job_id(),
* continue_job(), 
* Ash_cd(), Ash_exit(), Ash_jobs(), Ash_bg(), Ash_help(), Ash_kill(), and Ash_killall()
*/
int Ash_fg(Process *p, int in_file, int out_file, int err_file);


/**
* @brief [BUILTIN]: Continues active jobs in the background.
* 
* @par Description
* This shell builtin is used to continue active jobs (_job) in the background. It accepts
* the following grammar : @n
* bg -<job-id> ... @n
* To continue the job(s) in the background, it does the following :
* @li Checks if the command grammer is valid.
* @li If the grammar is valid, then for each job, it calls find_job_id() to find the specified jobs on the active job list 
* (job_list_head) with job ID (_job::id) @e <job-id> .
* @li For each job that is found, it calls continue_job() to continue the job in the background.
*
* @param p
* This is a process (_process) . Only its argument vector (_process::argv) and argument count
* (_process::argc) are of interest to this builtin.
* 
* @param in_file
* File descriptor to the builtin's standard input.
* 
* @param out_file
* File descriptor to the builtin's standard output.
*
* @param in_file
* File descriptor to the builtin's standard error.
*
* @return
* Returns @e 0 on success, or @e -1 on error. Possible reasons for failure are as follows:
* @li The grammar of the command is invalid.
* @li The grammar of the command is valid, but  a specified @e <job-id> does not refer to a job on the active
* job list.
*
* @sa
* _process, _process::argv, _process::argc, @e dprintf(), @e atoi(), _job, find_job_id(),
* continue_job(), @e strlen(), (#job_list_head)
* Ash_cd(), Ash_exit(), Ash_jobs(), Ash_bg(), Ash_help(), Ash_kill(), and Ash_killall()
*/
int Ash_bg(Process *p, int in_file, int out_file, int err_file);


/**
* @brief [BUILTIN]: Prints a help message for the shell.
* 
* @par Description
* This shell builtin is used print a help message for the shell. It accepts
* the following grammar : @n
* help @n
* 
* @param p
* This is a process (_process) . It is not of interest to this builtin.
* 
* @param in_file
* File descriptor to the builtin's standard input.
* 
* @param out_file
* File descriptor to the builtin's standard output.
*
* @param in_file
* File descriptor to the builtin's standard error.
*
* @return
* Always returns @e 0.
*
* @sa
* _process, @e dprintf(),
* Ash_cd(), Ash_exit(), Ash_jobs(), Ash_bg(), Ash_kill(), and Ash_killall()
*/
int Ash_help(Process *p, int in_file, int out_file, int err_file);


/**
* @brief [BUILTIN]: Sends a signal to active jobs.
* 
* @par Description
* This shell builtin is used to send a signal to active jobs. It accepts
* the following grammar : @n
* kill l | s<signal-name> | n<signal-number> -<job-id> ... @n
* The signal to be sent can be specief by using either the @e s option, or the @n option.
* The @s option is used to specified a signal using its symbolic name. The symbolic name must 
* be a case-insensitive string of one of the following patterns: @n
* @li SIGxxx or SIGxxxx (e.g., SIGHUP or SIGKILL, respectively)
* @li xxx or xxxx (e.g., HUP or KILL, respectively)
* 
* The @e n option is used to specified the signal number to be sent (e.g., @e n9 sends signal number 9).
* The @e l option is used to list the signals that can be sent.
* To send the signal, the function does the following :
* @li Checks if the command grammer is valid.
* @li If the grammar is valid, then for each specified job, it calls find_job_id() to find the job on the active job list 
* (job_list_head)
* @li For each job that is found, it calls @e kill() to send the signal to the job.
*
* @param p
* This is a process (_process) . Only its argument vector (_process::argv) and argument count
* (_process::argc) are of interest to this builtin.
* 
* @param in_file
* File descriptor to the builtin's standard input.
* 
* @param out_file
* File descriptor to the builtin's standard output.
*
* @param in_file
* File descriptor to the builtin's standard error.
*
* @return
* Returns @e 0 on success, or @e -1 on error. Possible reasons for failure are as follows:
* @li The grammar of the command is invalid.
* @li The grammar of the command is valid, but a specified @e <job-id> does not refer to a job on the active
* job list.
*
* @sa
* _process, _process::argv, _process::argc, @e dprintf(), @e atoi(), _job, find_job_id(),
* @e kill(), _job::id
* Ash_cd(), Ash_exit(), Ash_jobs(), Ash_bg(), Ash_help(), Ash_kill(), and Ash_killall()
*
* @todo
* implement more signals, build an array of signal names
*
*/
int Ash_kill(Process *p, int in_file, int out_file, int err_file);


/**
* @brief [BUILTIN]: Sends a signal to all active jobs.
* 
* @par Description
* This shell builtin is used to send a signal to all jobs on the active job list (#job_list_head).
* It accepts the following grammar : @n
* killall l or killall s<signal-name> | n<signal-number> @n
* The signal to be sent can be specief by using either the @e s option, or the @n option.
* The @s option is used to specified a signal using its symbolic name. The symbolic name must 
* be a case-insensitive string of one of the following patterns: @n
* @li SIGxxx or SIGxxxx (e.g., SIGHUP or SIGKILL, respectively)
* @li xxx or xxxx (e.g., HUP or KILL, respectively)
* 
* The @e n option is used to specified the signal number to be sent (e.g., @e n9 sends signal number 9).
* The @e l option is used to list the signals that can be sent.
* To send the signal, the function does the following :
* @li Checks if the command grammer is valid.
* @li If the grammar is valid, then it calls @kill() to send the singal to all active jobs.
*
* @param p
* This is a process (_process) . Only its argument vector (_process::argv) and argument count
* (_process::argc) are of interest to this builtin.
* 
* @param in_file
* File descriptor to the builtin's standard input.
* 
* @param out_file
* File descriptor to the builtin's standard output.
*
* @param in_file
* File descriptor to the builtin's standard error.
*
* @return
* Returns @e 0 on success, or @e -1 on error. Possible reasons for failure are as follows:
* @li The grammar of the command is invalid.
*
* @sa
* _process, _process::argv, _process::argc, @e dprintf(), 
* @e kill(),
* Ash_cd(), Ash_exit(), Ash_jobs(), Ash_bg(), Ash_help(), and Ash_kill.
* @todo
* implement more signals, build an array of signal names
*
*/
int Ash_killall(Process *p, int in_file, int out_file, int err_file);


/**
* @brief [BUILTIN]: Checks if a command refers to a shell builtin.
*
* @par Description
* This shell builtin is used to check if a the command assaociated with  process (_process)
* @e p refers to a shell builtin.
*
* @param p
* This is a process. Only its argument vector (_process::argv) and argument count 
* (_process::argc) fields are of interest to this builtin. 
*
* @return
* Returns -1 if the command associated with process @e p does not refer to a shell builtin, or a 
* non-negative value referring to an index in the #builtins array. This index is the location
* of the record for a shell builtin in the array.
*
* @sa
* _process, _process::argv, _process::argc, #builtins
* Ash_cd(), Ash_exit(), Ash_jobs(), Ash_fg(), Ash_bg(), Ash_help(), Ash_kill(), and Ash_killall().
*/
int is_builtin(Process *p);


/**
* @brief Finds a job with a particular ID.
*
* @par Description
* This function is used to find a job (_job) with a particular job ID (_job::id).
* It does the following:
* @li Traverses the active job list (#job_list_head) and checks if there is a job on the list whose
* ID is @e id . If a job is found, the function returns a pointer to it.
*
* @param id
* The ID of the job to find.
*
* @return
* Returns a pointer to a job on success (i.e., a job is found), or NULL on failure (i.e., no job
* with  ID @e id was found on the job list).
*
* @sa 
* _job, _job::id, #job_list_head, Ash_fg(), Ash_kill(), and Ash_bg().
*
*/
Job* find_job_id(int id);

#endif
