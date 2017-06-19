/************************************************************/
/*    S E T H E O                                           */
/*                                                          */
/* FILE: i_ureduct.c                                        */
/* VERSION:                                                 */
/* DATE:                                                    */
/* AUTHOR:                                                  */
/* NAME OF FILE:                                            */
/* DESCR:                                                   */
/* MOD:                                                     */
/*	11.3.93	Mueller,Goller neue Constraints, Statistics */
/*	27.10.92	new proof-tree                      */
/* BUGS:                                                    */
/************************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
/**/
#include "deref.h"
#include "unifred.h"

#define INSTR_LENGTH		3


#ifdef FOLD_UP
instr_result i_ureduct()
{
    register environ *pdp;
    register WORD     farg1 = ARG(1);
    register WORD     farg2 = ARG(2);
    register s_trail  *ltr;
    register WORD     *sg_i;
    register int open_sgls;

    /************************************************************/
    /* make a reduction step                                    */
    /************************************************************/


    /* look in the choice point, and get the reduction pointer */
    /* was there a reduction already ? */
    if (chp->red_ptr == NVAL) {
        /* no, start with current gbp->dyn_link */
        if (!(pdp=goal_bp)) {
            /* if it does not exist, fail */
            chp->red_ptr = NULL;
            /* no retry */
            return failure;
        }
    }
    else {
        /* get reduction pointer from choice point */
        pdp=chp->red_ptr;
    }

    sg_i = chp->red_tp;
    open_sgls = chp->open_sgls;

    /* get beginning of reduction list */
    /* move up list and unify */
    while (pdp) {

        if (!sg_i) {

            sg_i =   (WORD *)pdp + SIZE_IN_WORDS(environ)
                   + GETNUMBER(pdp->lcbptr->nr_vars);
            open_sgls = GETNUMBER(pdp->lcbptr->nr_sgls);
            while (sg_i <= pdp->tp) {
                sg_i++;
                open_sgls--;
	    }

            ltr = tr;

            if (un_red(pdp,GETSYMBOL(farg1),GETVAL(farg2))) {   /* unification succeeded */

                chp->next_clause--; 

                /* note this for the proof-tree:		 */
	        /* trailing is absolutely necessary whenever the */
	        /* proof-tree-structure is needed during search  */
                trail_var(goal_bp->tp); 
                GENOBJ(*goal_bp->tp,(WORD_cast)pdp,REDUCTION); 

                if (open_sgls){
	            chp->open_sgls = open_sgls;
	            chp->red_tp = sg_i;
                    chp->red_ptr = pdp;
	        }
	        else {
	            chp->red_ptr = pdp->dyn_link;
	            chp->open_sgls = 0;
	            chp->red_tp = NULL;
                }

                pc +=INSTR_LENGTH;
                return success;

	    }   /* if (un_red(...)) */

            else {   /* unification failed */

                untrail(ltr);
                tr = ltr;
	        if (!open_sgls) {
                    pdp = pdp->dyn_link;
       	            sg_i = NULL;
                }

	    }

	} /* if (!sg_i) */

        else {

            open_sgls--;
            ltr = tr;

            if (un_factor(pdp,
                          (literal_ctrl_block*)GETPTR(*sg_i),GETSYMBOL(farg1),GETVAL(farg2))) {
                /* unification succeeded */

                chp->next_clause--;

                /* note this for the proof-tree:                 */
                /* trailing is absolutely necessary whenever the */
                /* proof-tree-structure is needed during search  */
                trail_var(goal_bp->tp);
	        GENOBJ(*goal_bp->tp,(WORD_cast)sg_i,FACTORIZATION_OPT);

	        if (open_sgls){
	            chp->open_sgls = open_sgls;
	            chp->red_tp = sg_i + 1;
	            chp->red_ptr = pdp;
	        }
	        else {
	            chp->red_ptr = pdp->dyn_link;
	            chp->open_sgls = 0;
	            chp->red_tp = NULL;
                }
                pc +=INSTR_LENGTH;
                return success;

	    } /* if (un_factor(...)) */

            else {   /* unification failed */

	        untrail(ltr);
	        tr = ltr;
	        if (!open_sgls) {
	            pdp = pdp->dyn_link;
	            sg_i = NULL;
                }
	        else
	            sg_i++;

	    }

        }

    } /* while (pdp) */

    /* no reduction possible */
    chp->red_ptr = NULL;
    return failure;
 
}
#endif

