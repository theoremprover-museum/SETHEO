/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_neq_built.c
 * VERSION:
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *	11.3.93	Mueller,Goller neue Constraints, Statistics
 *		old meaning: check for syntactical inequality
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "unifeq.h"

#define INSTR_LENGTH		2


instr_result i_neq_built()
{
    if (!chk_arg (ARGPTR(1), bp, ARGPTR(1) + 1, bp)) {
	pc +=INSTR_LENGTH;
	return success;
    }
    return failure;	/* fail */
}
