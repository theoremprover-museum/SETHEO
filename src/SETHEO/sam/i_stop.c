/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_stop.c
 * VERSION:
 * DATE: Wed Sep  2 18:17:22 MET DST 1992
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
instr_result i_stop()
{
  return success | stop_cycle;
}
