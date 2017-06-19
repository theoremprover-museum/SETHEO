/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_getcounter.c
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

#define INSTR_LENGTH	 2
#define FAIL_U           {return failure;}

#ifdef STATISTICS1
#define UNIF_FAIL(x)        {unif_fails++;FAIL_U}
#else
#define UNIF_FAIL(x)        FAIL_U
#endif

#define COPY_CONST

#include "unification.h"


instr_result i_getcounter()
{
    WORD  *ga1,*ga2,s;
    int    num,val;

    ga1 = deref(ARGV(0),bp);             /* 1st argument of getcounter */
    num = GETNUMBER(*ga1);               /* get number of counter      */
    if (!ISNUMBER(*ga1)  ||  num<1  ||  num>num_of_counters) {
        sam_error("getcounter: wrong counter index",ga1, 1);
        pc += INSTR_LENGTH;
        return error;
    }
   
    ga2 = deref(ARGV(1),bp);
    val = GETNUMBER(*(counter_start+num-1));

    GENNUMBER(s,val); 
    UN_CONST((&s), bp, ga2, bp);

    pc += INSTR_LENGTH;
    return success;
}
