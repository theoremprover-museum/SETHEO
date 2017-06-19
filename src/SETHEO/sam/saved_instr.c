/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: i_unvoid.c
/* VERSION:
/* DATE: Fri Sep  4 11:23:50 MET DST 1992
/* AUTHOR:
/* NAME OF FILE:
/* DESCR:
/* MOD:
/* BUGS:
/******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"

#define INSTR_LENGTH		2


instr_result i_unvoid()
{
        /* unify with a void in the head */
gp++;
pc += INSTR_LENGTH;
return success;
}
