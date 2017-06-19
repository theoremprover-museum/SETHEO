/************************************************************/
/*    S E T H E O                                           */
/*                                                          */
/* FILE: i_reduct.c                                         */
/* VERSION:                                                 */
/* DATE:                                                    */
/* AUTHOR:                                                  */
/* NAME OF FILE:                                            */
/* DESCR:                                                   */
/* MOD:                                                     */
/*	11.3.93	Mueller,Goller neue Constraints, Statistics */
/*	27.10.92	new proof-tree                      */
/*	09/12/2001	FOLDED_UP -> IS_FOLDED_UP	    */
/* BUGS:                                                    */
/************************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
/**/
#include "deref.h"
#include "unifred.h"

#ifdef STATPART
#include "sps.h"
int no_redsteps_flag = FALSE; /* CS 10.11.94 */
int skipped_ia_redstep_cnt = 0;
int skipped_lr_redstep_cnt = 0; /* CS 19.11.94 */
#endif

#define INSTR_LENGTH		3


instr_result i_reduct()
{
    register environ *pdp;
    register WORD     farg1 = ARG(1);
    register WORD     farg2 = ARG(2);
    register s_trail  *ltr;
    register int	  i;	      /* used for proof-tree only */
    
    /************************************************************/
    /* make a reduction step                                    */
    /************************************************************/
#ifdef STATPART
    if (!recomp_mode) 
#endif
    {
	
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
	    /* set level to beginning                     */
	    /* this is used by the proof-tree generation: */
	    /* -1 == no reduction step yet		      */
	    i = -1;
	}
	else {
	    /* get reduction pointer from choice point */
	    pdp=chp->red_ptr;
#ifdef REPROOF_MODE
	    i = chp->red_lvl;
#endif
	}
	
	/* get beginning of reduction list */
	/* move up list and unify */
	while (pdp) {
	    
	    ltr = tr;
	    
	    if (un_red(pdp,GETSYMBOL(farg1),GETVAL(farg2))) {   /* unification succeeded */
		
		chp->next_clause--; 
		
		/* note this for the proof-tree:		 */
		/* trailing is absolutely necessary whenever the */          
		/* proof-tree-structure is needed during search  */
		trail_var(goal_bp->tp);
#ifndef FOLD_UP
		chp->red_ptr = pdp->dyn_link;
#ifdef REPROOF_MODE
		chp->red_lvl = i + 1;
#endif
		GENOBJ(*goal_bp->tp, (WORD_cast) pdp, REDUCTION); 
#else
		if (IS_FOLDED_UP(pdp->flags)) {
		    GENOBJ(*goal_bp->tp, (WORD_cast) pdp, FACTORIZATION_PESS); 
		} else {
		    GENOBJ(*goal_bp->tp, (WORD_cast) pdp, REDUCTION);
                }
		
		chp->red_ptr = pdp->path_link;
#ifdef REPROOF_MODE
		chp->red_lvl = i + 1;
#endif
		/* pdp was used in a reduct-operation, therefore we */
		/* have to trail and change red_marking             */
		trail_ptr((void**)&(pdp->red_marking));
		pdp->red_marking = goal_bp; 
#endif
		
		pc +=INSTR_LENGTH;
		return success;
	    }
	    
	    /* unification failed */
	    untrail(ltr);
	    tr = ltr;
	    
#ifndef FOLD_UP
	    pdp = pdp->dyn_link;
#else
	    pdp = pdp->path_link;
#endif
#ifdef REPROOF_MODE
	    i++;
#endif
	}
	
	/* no reduction possible */
	chp->red_ptr = NULL;
	return failure;
    }
    
#ifdef STATPART       /* RECOMPUTATION of Task */
    else {
	pdp=goal_bp;
	i = node_ptr->red_lvl;
	
	/* get beginning of reduction list */
	while (i--) pdp = pdp->dyn_link;
	ltr = tr;                      
	if (un_red(pdp,farg1,farg2)) { /* unific. succeeded */
	    pc = code + (++node_ptr)->codeptr;
	    return success;
        }
	/* no reduction possible */
	sam_error("ERROR in RECOMPUTATION",NULL,2);
	return error | stop_cycle;
    }
#endif
}
