/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: i_selnth.c                                     */
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


instr_result i_selnth()
{
   /* your program text comes here */
   fprintf(stderr,"sorry, %s is not implemented\n","selnth");
   pc +=INSTR_LENGTH;
   return error;
}
