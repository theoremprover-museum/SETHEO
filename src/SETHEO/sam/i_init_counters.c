/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_init_counters.c
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

#define INSTR_LENGTH		2


instr_result i_init_counters()
{
    WORD          *ga;

    ga = deref(ARGV(0),bp);              /* get argument of init_counters    */
    num_of_counters = GETNUMBER(*ga);    /* get total number of counters     */
    counter_start = hp;                  /* save heap address of 1st counter */
    hp += num_of_counters;

    pc +=INSTR_LENGTH;
    return success;
}







