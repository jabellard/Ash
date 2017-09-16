/**
* @file err.h
* @author Joe Nathan Abellard {https://github.com/joenatech7}
* 
* @date September 1, 2017
* @brief  Header file for  the error handling  subsystem.
*
* @par Description
* This file contains function prototypes, and global declarations for the error handing sub system.
*
* @sa
* err.c
*
*/


#ifndef ERR_H
#define ERR_H


/**
* @brief Safer version of free.
*
* @par Description
* This function is a safer version of @e free(), which is used for de-allocating memory from
* the heap. The are two potential pillfalls when using the standard @e free() function:
* (1) passing it a NULL pointer, which results in a run-time exeption, and (2) passing it 
* a pointer to a memory block that has already been previously de-allocated, resulting in a double free exeption.
* It is good practice to ensure that (1) a pointer that is to be passed is @e free() is non-NULL, 
* and (2) and to assign NULL to a pointer right after the memory block it points to has been de-allocated
* with @e free() . Doing (1) can be inconvinient, and since a pointer is only passed by value
* to @e free(), @e free() can not do (2) on behalf of the programmer. safe_free() seeks to
* eliminate these inconveniences. It calls @e free() internally to de-allocate memory, but
* does some things differently than @e free(). Before calling @e free() safe_free() checks if the 
* pointer to the momory block is NULL, and if that is the case, does nothing. Otherwise, it calls 
* @e free() to de-allocate the memory block. It's argument is a pointer to pointer pointing to
* memory block to be de-allocated (i.e, a double pointer). Accepting a double pointer allows it
* to assign NULL to the pointer after the memory de-allocation.
* 
* @param pp 
* Pointer to the pointer pointing to the memory block to be de-allocated.
*
* @return 
* returns @e void
*
* @sa 
* sfree(p), and @e free()
*
* @callgraph
*/
void safe_free(void **pp);



/**
* @brief Caller-fiendly interface to safe_free().
*
* @par Description
* This macro provides a caller friendly interface to the safe_free() function.
* By using this marco it is not nessasary to: (1) prefix the pointer to the memory block 
* to be allocated with the address of aperator (i.e., &), and (2) to then cast the pointer
* to void.
*
* @sa
* safe_free().
*
*/
#define sfree(p) safe_free((void**)&(p))


/**
* @brief Prints an error message corresponding to the current value of @e errno.
*
* @par Description
* This function is used to print an error message coresponding to the current value of @errno.
* It is a more verbose version of the standard @e perror() function.
* 
* @param user_msg
* This string prefixes the error message that will be printed.
*
* @return
* Returns @e void.
*
* @sa 
* @e perror(), @e errno, @e malloc(), @e sprintf(), @e fflush(), terminate(), err_exit(), 
* @e fputs(), sfree(), and err__exit().
*/
void err_msg(const char *user_msg);


/**
* @brief Terminates the calling process.
*
* @par Description
* This function is used to terminate the calling process. It does that by calling either @e abort() ,
* @e exit() , or @e _exit() . If the environment variable EF_DUMPCORE is defined, and is a
* non-empty string, it ends the process by calling @a abort(). Otherwise 
* the function ends the process by calling  either @e exit() , or @e _exit() . 
*
* @param use_exit3
* This interger determines whether the function calls @e exit() , or @e _exit() . If it is 
* non-zero, it calls @e exit() with the value EXIT_FAILURE, otherwise it calls
* @e _exit() with the value EXIT_FAILURE.
*
* @return
* Returns @e void.
*
* @sa 
* EF_DUMPCORE, @e exit(), @e _exit(), @e abort(), err_msg(), err_exit(), EXIT_FAILURE, 
* and err__exit().
*/
void terminate(int use_exit3);

#ifdef __GNUC__

    /* This macro stops 'gcc -Wall' complaining that "control reaches
       end of non-void function" if we use the following functions to
       terminate main() or some other non-void function. */

#define NORETURN __attribute__ ((__noreturn__))
#else
#define NORETURN
#endif



/**
* @brief Prints and error message, and terminates the calling process.
*
* @par Description
* This function is used to print an error message and then terminate the calling process.
* It prints the error message by calling err_msg(), and then terminates the process by calling
* terminate() wiht the value @e 0 .
*
* @param user_msg
* This string is passed to err_msg().
*
* @return
* Returns @e void.
*
* @sa
* err_msg(), terminate(), and err__exit().
*/
void err_exit(const char *user_msg) NORETURN ;



/**
* @brief Prints and error message, and terminates the calling process.
*
* @par Description
* This function is used to print an error message and then terminate the calling process.
* It prints the error message by calling err_msg(), and then terminates the process by calling
* terminate() wiht the value @e 1 .
*
* @param user_msg
* This string is passed to err_msg().
*
* @return
* Returns @e void.
*
* @sa
* err_msg(), terminate(), and err_exit().
*/
void err__exit(const char *user_msg) NORETURN ;


#endif
