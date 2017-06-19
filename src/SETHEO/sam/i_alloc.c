/************************************************************/
/*    S E T H E O                                           */
/*                                                          */
/* FILE: i_alloc.c                                          */
/* VERSION:	@(#)i_alloc.c	8.1 05 Jan 1996             */
/* DATE:                                                    */
/* AUTHOR:                                                  */
/* NAME OF FILE:                                            */
/* DESCR:                                                   */
/* MOD:                                                     */
/*	12.02.88                                            */
/*	25.4.89  statistics                                 */
/*	17.7.89  nalloc                                     */
/*      26.6.91 small changes on bound-cks   by Goller      */
/*      13.01.92 REG_CONSTR added by bschorer               */
/*      11.02.92 CONSTRAINTS added by bschorer              */
/*      26.2.92  Local-Inf-Bounding added by bschorer       */
/*	26.10.92 ONE argument ONLY (lcb-ptr)                */
/*	11.3.93	Mueller,Goller neue Constraints, Statistics */
/*	13.1.95	jsc	ddepth and dinf from lcb            */
/* BUGS:                                                    */
/************************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "defaults.h"
/**/
#include "bounds.h"
#include "i_nalloc.h"
#include "weighted_depth.h"
#include "dynsgreord.h"

#ifdef STATPART
#include "sps.h"
#endif /* STATPART */



#define INSTR_LENGTH	2

instr_result i_alloc()
{
    int           dinf;     /* value to increment the inference counter */
    int           ddepth;   /* value to decrement the depth */
    int           old_infcount;
    
    /* allocate an environment */
    

    /******************************************************/
    /* Check for bound fail:                             */
    /******************************************************/
    lcbptr = (literal_ctrl_block *)(ARGPTR(1));
    dinf = GETNUMBER(lcbptr->dinf);
    ddepth = GETNUMBER(lcbptr->ddepth);

    if (lookahead_flag)
	inf_lookahead -= dinf;

    if (wd_bound) {
        if (eq_by_ste_mod)
	    depth =   weighted_depth(infcount + inf_lookahead)
	            - delta_depth(dinf + inf_lookahead, ddepth);
	else
	    depth -= delta_depth(dinf + inf_lookahead, ddepth);
    }
    else
        depth -= ddepth;

    old_infcount = infcount;
    infcount += dinf;
    local_inf_bound -= dinf;
    sgscount += dinf;

    if (bound_fail(depth,
		   infcount + inf_lookahead,
		   old_infcount,
		   local_inf_bound - inf_lookahead,
		   sgscount + inf_lookahead)) {
#ifdef STATPART
        if (create_task){
	    s_boundfail = TRUE;
	}
        else
#endif
	    return failure;
    }

    /******************************************************/
    /* Set intermediate values:                           */
    /******************************************************/
    if (infcount > intmd_inf)
        intmd_inf = infcount;

    if (sgscount > intmd_sgs)
        intmd_sgs = sgscount;

    /******************************************************/
    /* Set delay option according to the new clause:      */
    /******************************************************/
    if (delay_switch) {
        if (dinf < switch_length) {
	    set_delay_option(option_for_shortcl);
	}
        else {
	    set_delay_option(option_for_longcl);
	}
    } 

    /* do the alloc without any control */
    return i_nalloc();
}
