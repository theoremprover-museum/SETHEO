/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_galloc.c
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

#define INSTR_LENGTH		2


instr_result i_galloc()
{
  register int i;

  /* allocate farg global cells on the heap */
  i = GETVAL(ARG(1));
  
  while (i--){
    GENOBJ(*hp,0,T_GLOBL);
    hp++;
  }
  pc +=INSTR_LENGTH;
  return success;
}
