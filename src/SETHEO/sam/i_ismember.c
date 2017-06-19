/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: i_ismember.c                                     */
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


instr_result i_ismember()
{
   /* your program text comes here */
   fprintf(stderr,"sorry, %s is not implemented\n","ismember");
   pc +=INSTR_LENGTH;
   return error;
}
