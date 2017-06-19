/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_jmpz.c
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


instr_result i_jmpz()
{
   /* sub is called before and sets cflag if result is less-equal zero */
   /* zflag is set if result is zero */
   if(zflag){
      pc = ARGPTR(1);
      return success;
   }
   else{
      pc +=INSTR_LENGTH;
      return success;
   }
}
