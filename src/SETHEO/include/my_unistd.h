#ifndef MY_UNISTD_H
#define MY_UNISTD_H


#ifdef hpux

#include <sys/types.h>

/* The following function declarations have been taken from <unistd.h>. 
   <unistd.h> could not be directly included because it contains the 
   definition of a datatype environ which conflicts with the definition 
   of environ in machine.h */

extern unsigned int  alarm();
   /* extern unsigned int  alarm(unsigned int sec); */

extern pid_t getpid();
   /* pid_t getpid(void); */

#else

#ifdef linux

#define __my__environ environ
#define environ stdlib_environ
#include <unistd.h>
#define environ __my__environ
#undef __my__environ

#else

#include <unistd.h>

#endif
#endif


#endif

