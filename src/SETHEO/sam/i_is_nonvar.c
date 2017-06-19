/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_is_nonvar.c
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
/**/
#include "deref.h"

#define INSTR_LENGTH		2


instr_result i_is_nonvar()
{
  register WORD *ga;
  
  ga = deref (ARGPTR(1), bp);
  
  if (GETTAG (*ga) == T_FVAR) {
    return failure;
  }
  pc +=INSTR_LENGTH;
  return success;
}
