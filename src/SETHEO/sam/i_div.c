/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_div.c
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
/**/
#include "errors.h"

#define INSTR_LENGTH		1


instr_result i_div()
{
  short farg;

  sp--;
  if (!GETNUMBER(*(sp))){
    sam_error("Division by zero: 0 taken",NULL,0);
    farg = 0;
  }
  else 
    farg = GETNUMBER(*(sp-1)) / GETNUMBER(*(sp));
  
  zflag = (farg)? 0 : 1;
  cflag = (farg > 0)? 0 : 1;
  GENNUMBER(*(sp-1),farg);
  
  pc +=INSTR_LENGTH;
  return success;
}
