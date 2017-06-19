/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_add.c
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

#define INSTR_LENGTH		1


instr_result i_add()
{
  short farg;

  sp--;
  farg = GETNUMBER(*sp) + GETNUMBER(*(sp-1));
  zflag = (farg)? 0 : 1;
  cflag = (farg > 0)? 0 : 1;
  GENNUMBER(*(sp-1),farg);
  pc +=INSTR_LENGTH;
  return success;
}






