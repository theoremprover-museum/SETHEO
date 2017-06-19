/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_jmpg.c
 * VERSION:
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"

#define INSTR_LENGTH		2


instr_result i_jmpg()
{
   /* sub is called before and sets cflag if result is less-equal zero */
   /* zflag is set if result is zero */
   if(cflag || zflag){
      pc +=INSTR_LENGTH;
      return success;
   }
   else{
      pc = ARGPTR(1);
      return success;
   }
}
