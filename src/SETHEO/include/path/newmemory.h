/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  HEADER:    newmemory.h                                * */
/* *                                                        * */
/* *  VERSION:   1.1                                        * */
/* *  MODIFIED:  May 2  1993 by Weidenbach                  * */
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

/*This is a dynamic memory management module.*/

#ifndef _MEMORY_
#define _MEMORY_

/**************************************************************/
/* Includes                                                   */
/**************************************************************/

#include "path/define.h"


/**************************************************************/
/* Data Structures and Constants                              */
/**************************************************************/

#define memory__MAXSIZE  200
#define memory__FREEDMARK 99
#define memory__MARKSIZE sizeof(int)
#define memory__ALIGN    sizeof(int)

struct {
  POINTER free;
  POINTER new;
  POINTER page;
  int size;
  int offset;
} memory_ARRAY[memory__MAXSIZE];

unsigned memory_NEWBYTES;
unsigned memory_FREEDBYTES;

#define memory__UsedBytes()    (memory_NEWBYTES - memory_FREEDBYTES)

#if DEBUG
#define memory__Free(_FREEPOINTER_,_SIZE_)                                 \
{                                                                          \
                                                                           \
  POINTER _FreeMemory_;                                                      \
                                                                           \
  _FreeMemory_                = _FREEPOINTER_;                               \
  if (*((int *)(((char *)_FreeMemory_) - memory__MARKSIZE)) == memory__FREEDMARK)  {\
     printf("\n       Second Free ! \n");                           \
     DumpCore();                                            \
    }                                                                          \
  else                                                                          \
    *((int *)(((char *)_FreeMemory_) - memory__MARKSIZE)) = memory__FREEDMARK;    \
  memory_FREEDBYTES        += memory_ARRAY[_SIZE_].size;                   \
  *(POINTER *)_FreeMemory_  = memory_ARRAY[_SIZE_].free;                   \
  memory_ARRAY[_SIZE_].free = _FreeMemory_;                                  \
}
#else
#define memory__Free(_FREEPOINTER_,_SIZE_)                                 \
{                                                                          \
                                                                           \
  POINTER _FreeMemory_;                                                      \
                                                                           \
  _FreeMemory_              = _FREEPOINTER_;                               \
  memory_FREEDBYTES        += memory_ARRAY[_SIZE_].size;                   \
  *(POINTER *)_FreeMemory_  = memory_ARRAY[_SIZE_].free;                   \
  memory_ARRAY[_SIZE_].free = _FreeMemory_;                                  \
}
#endif



void    memory_Init();
POINTER memory_Malloc();
void    memory_Print();
void    memory_PrintDetailed();


#endif

/**************************************************************/
/*                      END: newmemory.h                      */
/**************************************************************/


