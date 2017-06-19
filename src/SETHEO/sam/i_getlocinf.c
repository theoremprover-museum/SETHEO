/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_getlocinf.c
 * VERSION:	@(#)i_getlocinf.c	8.1 05 Jan 1996
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *	13.1.95		jsc	check for range
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



instr_result i_getlocinf()
{
    WORD s;
    WORD *ga;

    if (local_inf_bound < MINNUMBER  ||  local_inf_bound > MAXNUMBER){
	/* The format for numbers is short. */
	sam_error("getlocinf: local inf bound too large - truncated to short",
		  NULL,0);
    }
    GENNUMBER(s,local_inf_bound);
    ga = deref(ARGPTR(1),bp);

    /* unify the argument with local_inf_bound*/
    trail_var(ga);
    UN_CONST((&s), bp, ga, bp);

    pc +=INSTR_LENGTH;
    return success;
}
