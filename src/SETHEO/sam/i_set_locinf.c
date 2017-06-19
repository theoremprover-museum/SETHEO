/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_set_locinf.c
 * VERSION:
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 * 	Mon Jan 24 15:03:05 MET 1994 johann trailing of set_*
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
/**/
#include "deref.h"
#include "errors.h"

#define INSTR_LENGTH		2


instr_result i_set_locinf()
{
    WORD           *ga;

    ga = deref (ARGPTR(1), bp);

    if (ISNUMBER(*ga)) {
	trail_int(&local_inf_bound);
	local_inf_bound = GETNUMBER(*ga);
        pc +=INSTR_LENGTH;
	return success;
    }
    sam_error ("setdepth: must be a number", ga, 1);
    return failure;
}
