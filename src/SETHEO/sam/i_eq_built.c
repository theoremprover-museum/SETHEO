/*****************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_eq_built.c
 * VERSION:
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *	26.3.93		jsc	av ist first arg
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "unifeq.h"

#define INSTR_LENGTH		2


instr_result i_eq_built()
{
  if (chk_arg (ARGPTR(1), bp, ARGPTR(1) + 1, bp)) {
    pc +=INSTR_LENGTH;
    return success;
  }
  return failure;	/* fail */
}
