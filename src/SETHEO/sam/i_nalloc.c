/*************************************************************/
/*    S E T H E O                                            */
/*                                                           */
/* FILE: i_nalloc.c                                          */
/* VERSION:	@(#)i_nalloc.c	8.1 05 Jan 1996                                      */
/* DATE:                                                     */
/* AUTHOR:                                                   */
/* NAME OF FILE:                                             */
/* DESCR:                                                    */
/* MOD:                                                      */
/*	12.02.88                                             */
/*	25.4.89  statistics                                  */
/*	17.7.89  nalloc                                      */
/*      26.6.91 small changes on bound-cks   by Goller       */
/*      13.01.92 REG_CONSTR added by bschorer                */
/*      11.02.92 CONSTRAINTS added by bschorer               */
/*      26.2.92  Local-Inf-Bounding added by bschorer        */
/*	26.10.92 ONE argument ONLY (lcb-ptr)                 */
/*	27.10.92 New_proof_tree                              */
/*	11.3.93	Mueller,Goller neue Constraints, Statistics  */
/*	9.3.95	set-subgoals is now in LCB                   */
/*	7.4.95	Linux-bug                                    */
/* BUGS:                                                     */
/*************************************************************/

/* allocate without affecting infcount and depth */
/* for dfs non-bound rules */

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
/**/
#include "i_nalloc.h"
#include "deref.h"
#include "weighted_depth.h"

#define INSTR_LENGTH		2

#define DEBUG(X)


instr_result i_nalloc()
{
    register int                i;
    WORD		       *lvec,*next_clause;
    int		                nr_sgls,fa_dinf,dinf,
                                ncl_offset,dl_offset,redptr_offset;
    literal_ctrl_block         *lcb;

#ifdef STATISTICS1
    /******************************************/
    /***** increment the inference-counter  ***/
    /******************************************/
    total_inf++;
#endif

    /********************************************************/
    /* make a link from that bp to the current environment  */
    /********************************************************/
    bp = (environ *)sp;
    bp->dyn_link = goal_bp;

#ifdef ANTILEMMATA
    if (anl_constr  &&  chp >= (choice_pt*) stack) {
        chp->head_bp=bp;
    }
#endif

    /**************/
    /* proof tree */
    /**************/
    if (goal_bp){
#ifdef IS_DEBUG
	printf("nalloc: goal_bp=%lx goal_bp->tp=%lx GETPTR(*goal_bp->tp)=%lx\n",
	       goal_bp,goal_bp->tp,GETPTR(*goal_bp->tp));

	if (GETNUMBER(goal_bp->lcbptr->nr_sgls) == 0){
	    printf("nalloc: EEEEEEEEE\n");
	}
#endif
        /* trailing is absolutely necessary whenever the */         
        /* proof-tree-structure is needed during search  */
	trail_var(goal_bp->tp);

        GENOBJ(*goal_bp->tp,(WORD_cast)bp,EXTENSION);
    }

    /*****************************************/
    /******* set environment    **************/
    /*****************************************/

#ifdef FOLD_UP
    /*     bp->flags &= NOT_FOLDED_UP; */
    GENOBJ(bp->flags,GETVAL(bp->flags)&NOT_FOLDED_UP,T_EMPTY);
    bp->path_link = goal_bp;
    bp->red_marking = (environ *) stack;
#endif

    bp->ret_addr = ra;
    bp->lcbptr = (literal_ctrl_block *)ARGPTR(1);
    sp += SIZE_IN_WORDS(environ);

#ifdef CONSTRAINTS
#ifdef ANTILEMMATA
     bp->chp = chp;
#endif

    /*******************************************************/
    /* save a pointer to the last generated counter        */
    /* after the clause is solved all tableau  constraints */
    /* belonging to that clause will be deactivated        */
    /*******************************************************/
    bp->c_counter_sp = c_counter_sp;
#endif

    /************************************************************************/
    /* Save depth, infcount and free depth for weighted depth bound search: */ 
    /************************************************************************/
    if (wd_bound) {
	dinf = GETNUMBER((bp->lcbptr)->dinf);
	bp->depth = depth;
	if (eq_by_ste_mod) {
	    bp->infcount = infcount;
	    bp->depth_free = free_depth(depth, dinf + inf_lookahead);
	}
	else {
	    bp->infcount = infcount + inf_lookahead;
	    bp->depth_free = free_depth(depth, dinf);
	}
    }

    /********************************/
    /* generate variables on stack */
    /********************************/
    i = GETNUMBER((bp->lcbptr)->nr_vars) - GETNUMBER((bp->lcbptr)->nr_strvars);
    while(i--){
#ifdef CONSTRAINTS
        GENOBJ(*sp, (WORD_cast)NULL, T_FVAR);
        sp++;
#else
        GENOBJ(*sp, 0, T_FVAR);
        sp++;
#endif
    }

#ifdef CONSTRAINTS
    /*****************************************/
    /* generate structure variables on stack */
    /*****************************************/
    i = GETNUMBER((bp->lcbptr)->nr_strvars);
    while(i--){
       GENOBJ(*sp, 0, T_STRVAR);
       sp++;
    }
#endif

    /************************************************************/
    /* Push the information for each subgoal onto the stack:    */
    /* Generate a block containing the pointers to the literal  */
    /* control blocks of the subgoals, a block containing the   */
    /* next alternative for each subgoal and a block containing */
    /* the delay level of each subgoal.                         */
    /************************************************************/

    /* set tp to beginning of that area -1 */
    bp->tp = sp - 1;

    nr_sgls = GETNUMBER((bp->lcbptr)->nr_sgls);
    ncl_offset = NEXT_CL_OFFSET(bp);
    redptr_offset = REDPTR_OFFSET(bp);
    dl_offset = DL_OFFSET(bp);
    lvec = code + GETVAL((bp->lcbptr)->subgoal_list);

    for ( i=0 ; i<nr_sgls ; i++ ) {

        /****************************************************************/
        /* get pointer to literal control block and initialize entry in */
        /* subgoal-pointer block:                                       */
        /****************************************************************/
	GENOBJ(*sp, (WORD_cast)(code+GETVAL(*lvec)), NOT_SOLVED);

	/************************************************************/
	/* Get pointer to first alternative and initialize entry in */
	/* next-clause block:                                       */
	/************************************************************/
	lcb = (literal_ctrl_block*) GETPTR(*sp);
	next_clause = code + GETVAL(*(code + GETVAL(lcb->orlabel) + 1));
	GENOBJ(*(sp+ncl_offset), (WORD_cast) next_clause, T_EMPTY);

	/************************************************/
	/* Initialize entry in reduction pointer block: */
	/************************************************/
	GENPTR(*(sp+redptr_offset), NVAL);

	/***************************************************************/
	/* Get dinf of first alternative:                              */
	/* If first alternative is a reduction step:                   */
	/*     If spread_reducts and multi- or single-delay are on:    */
	/*         If there is a second alternative: fa_dinf is number */
	/*             of subgoals in second alternative.              */
	/*         Else fa_dinf is 0.                                  */
	/*     Else fa_dinf is 0.                                      */
	/* Else fa_dinf is number of subgoals in first alternative.    */
	/*                                                             */
	/* If first alternative is not a reduction step and if the     */
	/* lookahead_flag is on, use fa_dinf to compute inf_lookahead. */
	/***************************************************************/
	if (GETNUMBER(((literal_ctrl_block*)
		       (code + GETVAL(*next_clause)))->type) == reduct) {

	    if (spread_reducts  &&  (single_delay  ||  multi_delay)) {
	        next_clause++;
		if (GETVAL(*next_clause))
		    fa_dinf = GETNUMBER(((literal_ctrl_block*) 
					 (code + GETVAL(*next_clause)))->dinf);
		else
		    fa_dinf = 0;
	    }
	    else
		fa_dinf = 0;
	}
	else {
	    fa_dinf = GETNUMBER(((literal_ctrl_block*) 
				 (code + GETVAL(*next_clause)))->dinf);
	    if (lookahead_flag)
		inf_lookahead += fa_dinf; 
	}
	
	/*********************************************************/
	/* If single_delay or multi_delay is on,                 */
	/* use fa_dinf to initialize entry in delay level block: */
	/*********************************************************/
	if (single_delay  ||  multi_delay  ||  delay_switch) {
	    GENNUMBER(*(sp+dl_offset), fa_dinf);
	}
	
	sp++;
	lvec++;
    }

    if (single_delay  ||  multi_delay  ||  delay_switch)
	sp += dl_offset;
    else
	sp += redptr_offset;

    DEBUG(printf("nalloc: sp=%lx, tp=%lx\n",sp,bp->tp);)
    DEBUG(printf("nalloc: nr_sgls=%d\n",GETNUMBER(bp->lcbptr->nr_sgls));)

    pc += INSTR_LENGTH;
    return success;
}




