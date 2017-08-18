#ifndef ERR_H
#define ERR_H

void safe_free(void **pp);
#define sfree(p) safe_free((void**)&(p))

void err_msg(const char *user_msg);
void terminate(int use_exit3);

#ifdef __GNUC__

    /* This macro stops 'gcc -Wall' complaining that "control reaches
       end of non-void function" if we use the following functions to
       terminate main() or some other non-void function. */

#define NORETURN __attribute__ ((__noreturn__))
#else
#define NORETURN
#endif

void err_exit(const char *user_msg) NORETURN ;
void err__exit(const char *user_msg) NORETURN ;


#endif
