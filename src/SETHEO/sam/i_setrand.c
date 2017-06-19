/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_setrand.c
 * VERSION:
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "stdlibwrap.h"
#include "constraints.h"
#include "tags.h"
#include "machine.h"

#define INSTR_LENGTH		2


instr_result i_setrand()
{
  srand48 (GETVAL(ARG(1)));
  pc +=INSTR_LENGTH;
  return success;
}
