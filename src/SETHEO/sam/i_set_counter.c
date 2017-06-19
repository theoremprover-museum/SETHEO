/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_set_counter.c
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


instr_result i_set_counter()
{
    WORD  *ga1,*ga2;
    int    num,val;

    ga1 = deref(ARGV(0),bp);    /* get 1st argument of set_counter */
    num = GETNUMBER(*ga1);     
    if (!ISNUMBER(*ga1)  ||  num<1  ||  num>num_of_counters) {
        sam_error("getcounter: wrong counter index",ga1, 1);
        pc += INSTR_LENGTH;
        return error;
    }

    ga2 = deref(ARGV(1),bp);    /* get 2nd argument of set_counter */
    val = GETNUMBER(*ga2);     
    if (!ISNUMBER(*ga1)) {
        sam_error("getcounter: value must be a number",ga1, 1);
        pc += INSTR_LENGTH;
        return error;
    }

    GENNUMBER(*(counter_start+num-1),val);          /* set counter */

    pc +=INSTR_LENGTH;
    return success;
}
