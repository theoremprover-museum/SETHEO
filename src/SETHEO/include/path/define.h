/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  HEADER:    define.h                                   * */
/* *                                                        * */
/* *  VERSION:   2                                          * */
/* *  MODIFIED:  Jun 10 1993 by Graf                        * */
/* *                                                        * */
/* *  AUTHORS:   P. Graf (graf@mpi-sb.mpg.de)               * */
/* *             C. Weidenbach (weidenb@mpi-sb.mpg.de)      * */
/* *             MPI fuer Informatik                        * */
/* *             Im Stadtwald                               * */
/* *             6600 Saarbruecken                          * */
/* *             Germany                                    * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/

#ifndef _DEFINE_
#define _DEFINE_

/**************************************************************/
/* Includes                                                   */
/**************************************************************/

#include <stdio.h>
#include "stdlibwrap.h"
#ifdef SUNOS4
#include <syscall.h>
#else
#include <sys/syscall.h>
#include <sys/param.h>
#endif

/**************************************************************/
/* Forwarding external libc.a functions                       */
/**************************************************************/

#ifdef SUNOS
extern int   syscall();

extern int   printf();
extern int   strcmp();
extern int   strlen();
extern char* strcpy();
extern char* strcat();
extern char* strdup();
extern char* strncpy();
extern char* strtok();
extern FILE* fopen();
extern char* fgets();
extern void  fclose();
extern void  fflush();
extern long  clock();

extern int   _filbuf(); /*Needed for getchar()*/
#endif

/**************************************************************/
/* Boolean Logic                                              */
/**************************************************************/

#define BOOL		char
#ifndef TRUE
#	define TRUE		1
#	define FALSE		0
#endif

/**************************************************************/
/* Specials                                                   */
/**************************************************************/

// #define Error__Out(A) {printf/**/A; }
#define Error__Out(A) {printf A; }


typedef void*           POINTER;

#define DumpCore()      abort()

#endif
