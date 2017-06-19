/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_checkulemma.c
 * VERSION:
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "tags.h"
#include "machine.h"

#define INSTR_LENGTH		3


instr_result i_checkulemma()
{
  /* your program text comes here */
  fprintf(stderr,"sorry, %s is not implemented\n","checkulemma");
  pc +=INSTR_LENGTH;
  return error;
}

