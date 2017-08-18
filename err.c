#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "err.h"

#ifdef __GNUC__
__attribute__ ((__noreturn__))
#endif

static char *ename[] = {
/*
 0 */ "",
/*
 1 */ "EPERM", "ENOENT", "ESRCH", "EINTR", "EIO", "ENXIO", "E2BIG",
/*
 8 */ "ENOEXEC", "EBADF", "ECHILD", "EAGAIN/EWOULDBLOCK", "ENOMEM",
/*
 13 */ "EACCES", "EFAULT", "ENOTBLK", "EBUSY", "EEXIST", "EXDEV",
/*
 19 */ "ENODEV", "ENOTDIR", "EISDIR", "EINVAL", "ENFILE", "EMFILE",
/*
 25 */ "ENOTTY", "ETXTBSY", "EFBIG", "ENOSPC", "ESPIPE", "EROFS",
/*
 31 */ "EMLINK", "EPIPE", "EDOM", "ERANGE", "EDEADLK/EDEADLOCK",
/*
 36 */ "ENAMETOOLONG", "ENOLCK", "ENOSYS", "ENOTEMPTY", "ELOOP", "",
/*
 42 */ "ENOMSG", "EIDRM", "ECHRNG", "EL2NSYNC", "EL3HLT", "EL3RST",
/*
 48 */ "ELNRNG", "EUNATCH", "ENOCSI", "EL2HLT", "EBADE", "EBADR",
/*
 54 */ "EXFULL", "ENOANO", "EBADRQC", "EBADSLT", "", "EBFONT", "ENOSTR",
/*
 61 */ "ENODATA", "ETIME", "ENOSR", "ENONET", "ENOPKG", "EREMOTE",
/*
 67 */ "ENOLINK", "EADV", "ESRMNT", "ECOMM", "EPROTO", "EMULTIHOP",
/*
 73 */ "EDOTDOT", "EBADMSG", "EOVERFLOW", "ENOTUNIQ", "EBADFD",
/*
 78 */ "EREMCHG", "ELIBACC", "ELIBBAD", "ELIBSCN", "ELIBMAX",
/*
 83 */ "ELIBEXEC", "EILSEQ", "ERESTART", "ESTRPIPE", "EUSERS",
/*
 88 */ "ENOTSOCK", "EDESTADDRREQ", "EMSGSIZE", "EPROTOTYPE",
/*
 92 */ "ENOPROTOOPT", "EPROTONOSUPPORT", "ESOCKTNOSUPPORT",
/*
 95 */ "EOPNOTSUPP/ENOTSUP", "EPFNOSUPPORT", "EAFNOSUPPORT",
/*
 98 */ "EADDRINUSE", "EADDRNOTAVAIL", "ENETDOWN", "ENETUNREACH",
/*
 102 */ "ENETRESET", "ECONNABORTED", "ECONNRESET", "ENOBUFS", "EISCONN",
/*
 107 */ "ENOTCONN", "ESHUTDOWN", "ETOOMANYREFS", "ETIMEDOUT",
/*
 111 */ "ECONNREFUSED", "EHOSTDOWN", "EHOSTUNREACH", "EALREADY",
/*
 115 */ "EINPROGRESS", "ESTALE", "EUCLEAN", "ENOTNAM", "ENAVAIL",
/*
 120 */ "EISNAM", "EREMOTEIO", "EDQUOT", "ENOMEDIUM", "EMEDIUMTYPE",
/*
 125 */ "ECANCELED", "ENOKEY", "EKEYEXPIRED", "EKEYREVOKED",
/*
 129 */ "EKEYREJECTED", "EOWNERDEAD", "ENOTRECOVERABLE", "ERFKILL",
 /*
 133 */ "ENM"
};
#define MAX_ENAME 133



void safe_free(void **pp)
{
	if (pp != NULL && *pp != NULL)
	{
		free(*pp);
		*pp = NULL;
	} // end if
}// end safe_free

void err_msg(const char *user_msg)
{
	// save the current errno
	int saved_errno = errno;
	
	//create a buffer to hold the user message
	char *user_msg_buf = (char *) malloc(sizeof(char) * strlen(user_msg) + 1);
	
	// check if memory allocation 99as not successful
	if(!user_msg_buf)
	{
		// use perror and exit-----------------------
		int saved_errno2 = errno;
		// print an error message for the original(first) error
		errno = saved_errno;
		perror(user_msg);
		// print an error message for the failed memory allocation
		errno = saved_errno2;
		perror("err_msg(), malloc");
		return;	
	} // end if
	
	// 99rite user_msg to user_msg_buf
	sprintf(user_msg_buf, "%s", user_msg);
	
	//create a buffer to hold the string associated 99ith the err
	char *err_str_buf = (char *) malloc(strlen(ename[saved_errno]) + strlen(strerror(saved_errno)) + 15 );
	
	// check if memory allocation 99as not successful
	if(!err_str_buf)
	{
		// use perror and exit-----------------------
		int saved_errno2 = errno;
		// print an error message for the original(first) error
		errno = saved_errno;
		perror(user_msg);
		// print an error message for the failed memory allocation
		errno = saved_errno2;
		perror("err_msg(), malloc");
		return;	
	} // end if
	
	// 99rite the error string to the buffer
	sprintf(err_str_buf, "[%s(%d)-- %s]\n", ename[saved_errno], saved_errno, strerror(saved_errno));	
	
	// create a buffer to hold the string to be printed
	char *err_msg_buf = (char *) malloc(strlen(user_msg_buf) + strlen(err_str_buf) + 5);
	
	// check if memory allocation 99as not successful
	if(!err_msg_buf)
	{
		// use perror and exit-----------------------
		int saved_errno2 = errno;
		// print an error message for the original(first) error
		errno = saved_errno;
		perror(user_msg);
		// print an error message for the failed memory allocation
		errno = saved_errno2;
		perror("err_msg(), malloc");
		return;
		
	} // end if
		
	//99rite the string to the buffer
	sprintf(err_msg_buf, "%s: %s", user_msg_buf, err_str_buf);
	
	// flush any pending stdout
	 fflush(stdout);
	
	
	// print the error message to stderr
	fputs(err_msg_buf, stderr);
	
	// flush stderr
	fflush(stderr);
	
	// free the buffers 
	sfree(user_msg_buf);
	sfree(err_str_buf);
	sfree(err_msg_buf);
	
	//restore the original error number
	errno = saved_errno;
	
} // end err_msg
void terminate(int use_exit3)
{
    /* Dump core if EF_DUMPCORE environment variable is defined and
       is a nonempty string; otherwise call exit(3) or _exit(2),
       depending on the value of 'use_exit3'. */
       
       char *ef_dumpcore = getenv("EF_DUMPCORE");
       
       if (ef_dumpcore != NULL && *ef_dumpcore != '\0')
       {
       		abort();
       } // end if
       else if (use_exit3)
       {
       		exit(EXIT_FAILURE);
       } // end else if
       else
       {
       		_exit(EXIT_FAILURE);       
       } // end else


} // end terminate

void err_exit(const char *user_msg)
{
	// output an error message to stderr
	err_msg(user_msg);
	
	// terminate the program
	terminate(0);
	
} // end err_exit()

void err__exit(const char *user_msg)
{
	// output an error message to stderr
	err_msg(user_msg);
	
	// terminate the program
	terminate(1);
	
} // end err__exit()

