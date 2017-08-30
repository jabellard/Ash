#ifndef EXEC_H
#define EXEC_H

#include <sys/types.h>
#include <termios.h>
#define MAX_ARGS 25
#define MAX_PROCESSES 25


//----------------------------------data structures
/*
*/
typedef struct _process
{
	char *argv[MAX_ARGS];
	int argc;
	pid_t pid;
	int completed;
	int stopped;
	int status;
	
} Process; // end struct _process

#endif


/*
*/
typedef struct _job
{
	struct _job *next;
	int id;
	int valid;
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


//----------------------------------function prototypes
void print_job_table(void);
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
void add_job_to_table(Job *j);
void execute_job(Job *j);
int Ash_cd(Process *p, int in_file, int out_file, int err_file);
int Ash_exit(Process *p, int in_file, int out_file, int err_file);
int Ash_jobs(Process *p, int in_file, int out_file, int err_file);
int Ash_fg(Process *p, int in_file, int out_file, int err_file);
int Ash_bg(Process *p, int in_file, int out_file, int err_file);
int Ash_help(Process *p, int in_file, int out_file, int err_file);
int Ash_kill(Process *p, int in_file, int out_file, int err_file);
int is_builtin(Process *p);
Job* find_job_id(int id);






































