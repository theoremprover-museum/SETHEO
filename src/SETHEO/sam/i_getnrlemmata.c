/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_getnrlemmata.c
 * VERSION:
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 *		getnrlemmata	argv
 *	get the number of lemmata in the lemma store
 * MOD:
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "tags.h"
#include "machine.h"
#include "unitlemma.h"
/**/
#include "deref.h"
#include "errors.h"


#define INSTR_LENGTH		2


#       define FAIL_U {return failure;}


#ifdef STATISTICS1
#define UNIF_FAIL(x) {unif_fails++;FAIL_U}
#else
#define UNIF_FAIL(x) FAIL_U
#endif

#define COPY_CONST

#include "unification.h"



/******************************************************
 * getnrlemmata/1
 *
 * get the number of valid, i.e., non-deleted untilemmata
 * in the index
 ******************************************************/
instr_result i_getnrlemmata()
{
#ifdef UNIT_LEMMATA
 WORD s;
 WORD *ga;

    if (unitlemma_nr_valid < MINNUMBER  ||  unitlemma_nr_valid > MAXNUMBER){
	/* The format for numbers is short. */
	sam_error("getnrlemmata: nrlemmata too large - truncated to short",
                  NULL,0);
    }

    GENNUMBER(s,unitlemma_nr_valid);
    ga = deref(ARGPTR(1),bp);

    /* unify the argument with depth*/
    trail_var(ga);
    UN_CONST((&s), bp, ga, bp);

#endif
    pc +=INSTR_LENGTH;
    return success;
}


