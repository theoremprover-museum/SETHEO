/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_break.c
 * VERSION:
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 * BUGS:
 ******************************************************/

#include <signal.h>
#include <stdio.h>
#include <sys/types.h>

#include "my_unistd.h"
#include "constraints.h"
#include "tags.h"
#include "machine.h"

#define INSTR_LENGTH		1


instr_result i_break()
{
  pc +=INSTR_LENGTH;
#ifdef SIGNALS
  kill(getpid(),SIGINT);
#endif
  return success;
}
