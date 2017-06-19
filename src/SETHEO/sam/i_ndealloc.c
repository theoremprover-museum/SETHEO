/************************************************************/
/*    S E T H E O                                           */
/*                                                          */
/* FILE: i_ndealloc.c                                       */
/* VERSION:                                                 */
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
/*	11.3.93	Mueller,Goller neue Constraints, Statistics */
/* BUGS:                                                    */
/************************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
/**/
#include "i_ndealloc.h"
#include "bounds.h"
#include "deref.h"
#include "termcomplexity.h"
#include "dynsgreord.h"

#ifdef STATPART
#include "sps.h"
#endif

#define INSTR_LENGTH	


instr_result i_ndealloc() {
#ifdef CONSTRAINTS
    c_counter *lptr;
#endif
    int       tc;


#ifdef ANTILEMMATA
    /********************************************************/
    /* use local variable lptr for deactivating constraints */
    /********************************************************/
    lptr = c_counter_sp;
    while (lptr > bp->chp->deact_c_counter_sp) {
        /*********************************************************************/
        /* If (      constraint is still active                              */
        /*      and (    constr is not an anl constr                         */
        /*             or constr is generated from a younger chp than bp->chp*/
        /*             or (     inf bound flag is switched on                */
        /*                  and constr is generated from bp->chp             */
        /*                  and constr allows less rest-resources than are   */
        /*                      still available                              */
        /*                )                                                  */
        /*           )                                                       */
        /*    )                                                              */
        /* then trail and deactivate constr.                                 */
        /*********************************************************************/
        if (    lptr->length != -1                  
             && (    GETTAG(lptr->next) != ANTILEMMA 
                  || lptr->chp > bp->chp            
                  || (    inf_bound         
                       && lptr->chp == bp->chp      
                       && lptr->infcount > infcount))) {
                              trail_int(&lptr->length);
                              lptr->length = -1;
        }
        lptr = (c_counter *) GETPTR(lptr->next);
    }

    if (anl_constr  &&  bp->chp >= (choice_pt*) stack) {
        if (inf_bound) {
            bp->chp->end_infcount = infcount;
        }
        bp->chp->end_total_inf = total_inf;

        /*******************************************************************/
        /* Whenever a subgoal has been solved we have to store the current */
        /* trail pointer for further use in antilemma constraints          */
        /*******************************************************************/
        trail_ptr((void**)&(bp->chp->end_trail));
        bp->chp->end_trail = tr;
    }
#endif

#ifdef TC_BOUND
    if (tc_bound  &&  GETVAL(bp->lcbptr->nr_sgls) == 0   &&   sgscount) {
	tc = tc_of_all_open_subgoals();
	if (tc_bound_fail(tc))
	    return failure;
        if (tc > intmd_tc)
            intmd_tc = tc;
    }
#endif

    /**********************************/
    /* to go on with the next subgoal */
    /**********************************/
    pc = ra = bp->ret_addr;
    bp = bp->dyn_link;

    /*****************************************************/
    /* Set delay option according to the current clause: */
    /*****************************************************/
    if (delay_switch) {
        if (GETNUMBER(bp->lcbptr->dinf) < switch_length) {
	    set_delay_option(option_for_shortcl);
	}
	else {
	    set_delay_option(option_for_longcl);
	}
    } 

#ifdef STATPART
    /* IAbegin */ /* nach unterhalb von bp = bp->dyn_link versetzt, CS 4.9.94 */
    if (recomp && iand_task_num != -2) /* IA-Task vorhanden */
	if( bp == saved_bp ){
	    /*
            printf("i_ndealloc: now_next_subg = TRUE; next?= ");
	    print_subgoal(stdout,bp->lcbptr,bp,0);
	    printf("\n");
	    */
	    trail_var(&now_next_subg);
            now_next_subg = TRUE;
	    }
    /* IAend */
#endif

    return success;
}


