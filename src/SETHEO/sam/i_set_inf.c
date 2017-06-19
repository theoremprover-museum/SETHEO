/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_set_inf.c
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
#include "errors.h"

#define INSTR_LENGTH		2


instr_result i_set_inf()
{
    WORD           *ga;

    ga = deref (ARGPTR(1), bp);

    if (ISNUMBER(*ga)) {
        trail_int(&infcount);
	infcount = GETNUMBER(*ga);
	if (infcount < 0) {
	  sam_error("setinf: Warning: infcount set to negative value.", 
		    NULL, 0);
	}
        pc +=INSTR_LENGTH;
	return success;
    }
    sam_error ("setinf: must be a number", ga, 1);
    return failure;
}

