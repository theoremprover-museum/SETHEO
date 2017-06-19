/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: i_fold_up.c                                  */
/* VERSION:                                           */
/* DATE:                                              */
/* AUTHOR:                                            */
/* NAME OF FILE:                                      */
/* DESCR:                                             */
/* MOD:                                               */
/* BUGS:                                              */
/******************************************************/

#include <stdio.h>
#include "tags.h"
#include "machine.h"
/**/
#include "deref.h"

#define INSTR_LENGTH		1


instr_result i_fold_up()
{
#ifdef FOLD_UP

    environ *pdp;


#ifdef STATISTICS1
    folding_statistics.foldings++;
#endif

    /* search for minimal context of lemma */
    pdp = bp->dyn_link;
    while (pdp && pdp != (environ *) stack && pdp->red_marking < bp)
	pdp = pdp->path_link;

    /* we have a unit lemma */
    if (!pdp || pdp == (environ *) stack ){
#ifdef STATISTICS1
	folding_statistics.possible_lemmas++;
#endif
        pdp = (environ *) stack;
    }
	
    /* we can only fold up by one level */
    if (pdp == bp->dyn_link){
#ifdef STATISTICS1
	folding_statistics.one_level_foldings++;
#endif
    }

    trail_ptr((void**)&(bp->path_link));
    bp->path_link = pdp->path_link;

    trail_ptr((void**)&(pdp->path_link));
    pdp->path_link = bp;

    trail_var(&(bp->flags));
    /*    bp->flags |= FOLDED_UP; */

/*
printf("before: %lx GETVAL=%lx\n",bp->flags,GETVAL(bp->flags));
*/
    GENOBJ(bp->flags,GETVAL(bp->flags)|FOLDED_UP,T_EMPTY);
/*
printf("after: %lx GETVAL=%lx\n",bp->flags,GETVAL(bp->flags));
*/

	
   
#endif

    pc +=INSTR_LENGTH;
    return success;
}
