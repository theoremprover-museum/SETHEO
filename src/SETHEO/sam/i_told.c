/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_told.c
 * VERSION:		@(#)i_told.c	8.1
 * DATE:		17.9.93
 * AUTHOR:		J. Schumann
 * NAME OF FILE:
 * DESCR:
 * MOD:
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "tags.h"
#include "machine.h"

#define INSTR_LENGTH		1


/******************************************************
 * told/0 closes the current output file (unless it is
 * stdout) and sets stdout as the current output
 * file
 ******************************************************/
instr_result i_told()
{
  if (outputfile != stdout){
    /* close it */
    fclose(outputfile);
    outputfile = stdout;
  }
  
  pc +=INSTR_LENGTH;
  return success;
}
