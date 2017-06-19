/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_out.c
 * VERSION:
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *	17.9.93		johann	outputfile
 * BUGS:
 *		is save of bp really necessary?
 ******************************************************/

#include <stdio.h>
#include "tags.h"
#include "machine.h"
/**/
#include "disp.h"

#define INSTR_LENGTH		2


instr_result i_out()
{

  disp_(outputfile,ARGPTR(1),bp);
  fflush(outputfile);
  
  pc +=INSTR_LENGTH;
  return success;
}
