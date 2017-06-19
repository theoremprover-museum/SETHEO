/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_jmp.c
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


instr_result i_jmp()
{
  pc = ARGPTR(1);
  return success;
}
