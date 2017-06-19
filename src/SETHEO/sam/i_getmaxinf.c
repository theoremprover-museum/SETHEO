/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_getmaxinf.c
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

#define FAIL_U                  {return failure;}

#ifdef STATISTICS1
#define UNIF_FAIL(x) {unif_fails++;FAIL_U}
#else
#define UNIF_FAIL(x) FAIL_U
#endif

#define COPY_CONST

#include "unification.h"



instr_result i_getmaxinf()
{
    WORD s;
    WORD *ga;

    if (maxinf < MINNUMBER  ||  maxinf > MAXNUMBER){
	/* The format for numbers is short. */
	sam_error("getmaxinf: maxinf too large - truncated to short",NULL,0);
    }

    GENNUMBER(s,maxinf);
    ga = deref(ARGPTR(1),bp);

    /* unify the argument with maxinf*/
    trail_var(ga);
    UN_CONST((&s), bp, ga, bp);

    pc +=INSTR_LENGTH;
    return success;
}
