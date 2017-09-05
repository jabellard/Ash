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
int print_job_command(Job *j, int dest_fd);
void print_job_list(void);
void init_shell();
Process* create_process(void);
int insert_arg_in_process(Process *p, const char * arg);
void print_process(const Process *p);
Process* process_dup(const Process *p);
void execute_process(Process *p, pid_t pgid, int in_file, int out_file, int err_file, int foreground);
void destroy_process(Process* p);
Job* create_job(void);
int insert_process_in_job(Job *j, const Process *p);
void print_job(const Job *j);
int destroy_job(Job *j);
Job* find_job(pid_t pgid);
int job_is_stopped(Job *j);
int job_is_completed(Job *j);
void format_job_info (Job *j, const char *status);
int mark_process_status(pid_t pid, int status);
void update_status(void);
void wait_for_job(Job *j);
void do_job_notification(void);
void put_job_in_foreground(Job *j, int cont);
void put_job_in_background(Job *j, int cont);
void mark_job_as_running(Job *j);
void continue_job(Job *j, int foreground);
void add_job_to_list(Job *j);
int execute_job(Job *j);
int Ash_cd(Process *p, int in_file, int out_file, int err_file);
int Ash_exit(Process *p, int in_file, int out_file, int err_file);
int Ash_jobs(Process *p, int in_file, int out_file, int err_file);
int Ash_fg(Process *p, int in_file, int out_file, int err_file);
int Ash_bg(Process *p, int in_file, int out_file, int err_file);
int Ash_help(Process *p, int in_file, int out_file, int err_file);
int Ash_kill(Process *p, int in_file, int out_file, int err_file);
int Ash_killall(Process *p, int in_file, int out_file, int err_file);
int is_builtin(Process *p);
Job* find_job_id(int id);

#endif
