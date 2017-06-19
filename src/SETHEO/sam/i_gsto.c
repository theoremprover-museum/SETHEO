/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_gsto.c
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
#include "deref.h"

#define INSTR_LENGTH        2

instr_result i_gsto()
{
  register WORD *ga;

  ga = heap + GETVAL(ARG(1));
  trail_var(ga);
  *ga = *(--sp);
  
  pc +=INSTR_LENGTH;
  return success;
}
