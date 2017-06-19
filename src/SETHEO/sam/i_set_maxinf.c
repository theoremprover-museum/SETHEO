/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_set_maxinf.c
 * VERSION:
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *	25.1.94		johann		assignment must be backtrackable
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


instr_result i_set_maxinf()
{
    WORD           *ga;

    ga = deref (ARGPTR(1), bp);

    if (ISNUMBER(*ga)) {
	trail_int(&maxinf);
	maxinf = GETNUMBER(*ga);
        inf_bound = TRUE;
        pc +=INSTR_LENGTH;
	return success;
    }
    sam_error ("set_maxinf: must be a number", ga, 1);
    return failure;
}
