/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_get_maxtc.c
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
#include "unification.h"
/**/
#include "deref.h"
#include "errors.h"

#ifdef STATISTICS1
#define UNIF_FAIL(x) {unif_fails++;FAIL_U}
#else
#define UNIF_FAIL(x) FAIL_U
#endif

#define COPY_CONST
#define FAIL_U {return failure;}


#define INSTR_LENGTH		2


instr_result i_get_maxtc()
{
    WORD s;
    WORD *ga;

    if (maxtc < MINNUMBER  ||  maxtc > MAXNUMBER){
	/* The format for numbers is short. */
	sam_error("getmaxtc: maxtc too large - truncated to short",NULL,0);
    }
	
    GENNUMBER(s,maxtc);
    ga = deref(ARGPTR(1),bp);

    /* unify the argument with maxtc */
    trail_var(ga);
    UN_CONST((&s), bp, ga, bp);

    pc +=INSTR_LENGTH;
    return success;
}
