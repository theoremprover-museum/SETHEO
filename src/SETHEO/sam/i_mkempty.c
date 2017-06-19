/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: i_mkempty.c                                     */
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


instr_result i_mkempty()
{
   /* your program text comes here */
   fprintf(stderr,"sorry, %s is not implemented\n","mkempty");
   pc +=INSTR_LENGTH;
   return error;
}
