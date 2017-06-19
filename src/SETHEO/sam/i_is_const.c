/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_is_const.c
 * VERSION:
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *		26.3.93	jsc	av is the first argument
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
/**/
#include "deref.h"

#define INSTR_LENGTH		2


instr_result i_is_const()
{
    register WORD           *ga;

    ga = deref (ARGPTR(1), bp);

    if (ISCONSTANT(*ga)) {
        pc +=INSTR_LENGTH;
	return success;
    }
    return failure;
}
