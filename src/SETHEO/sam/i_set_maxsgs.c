/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_set_maxsgs.c
 * VERSION:
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "tags.h"
#include "machine.h"
/**/
#include "deref.h"
#include "errors.h"

#define INSTR_LENGTH		2


instr_result i_set_maxsgs()
{
    WORD           *ga;

    ga = deref(ARGPTR(1),bp);

    if (ISNUMBER(*ga)) {
        trail_int(&maxsgs);
	maxsgs = GETNUMBER(*ga);
        sgs_bound = TRUE;
        pc +=INSTR_LENGTH;
        return success;
    }
    sam_error ("set_maxsgs: must be a number", ga, 1);
    return failure;
}
