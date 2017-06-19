/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: i_isempty.c                                     */
/* VERSION:                                           */
/* DATE:                                              */
/* AUTHOR:                                            */
/* NAME OF FILE:                                      */
/* DESCR:                                             */
/* MOD:                                               */
/* BUGS:                                              */
/******************************************************/

#include <stdio.h>
#include "tags.h"
#include "machine.h"

#define INSTR_LENGTH		2


instr_result i_isempty()
{
   /* your program text comes here */
   fprintf(stderr,"sorry, %s is not implemented\n","isempty");
   pc +=INSTR_LENGTH;
   return error;
}
