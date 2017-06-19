/*****************************************************************************/
/*    S E T H E O                                                            */
/*                                                                           */
/* FILE: i_fail.c                                                            */
/* VERSION:                                                                  */
/* DATE:                                                                     */
/* AUTHOR:                                                                   */
/* NAME OF FILE:                                                             */
/* DESCR:                                                                    */
/* MOD:                                                                      */
/*		16.3.89 bug with chp                                         */
/*		25.4.89 statistics                                           */
/*		31.8.   backtr                                               */
/*              26.02.92 Local-Inf-Bounding added by bschorer                */
/*	11.3.93	Mueller,Goller neue Constraints, Statistics                  */
/*	17.9.93		jsc		remove of old lemma stuff, tell,told */
/*					copy-binding                         */
/* BUGS:                                                                     */
/*****************************************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "symbtab.h"
#include "defaults.h"
/**/
#include "i_fail.h"
#include "antilemma.h"
#include "deref.h"
#include "dynsgreord.h"
#include "bounds.h"

#ifdef STATPART
#include "sps.h"
extern int ff_check_mode, ff_saved_maxinf, ff_bound_cnt;
#endif

#define INSTR_LENGTH	1

instr_result i_fail()
{
    choice_pt           *free_chp, *cp;
    environ             *old_bp;

#ifdef STATPART
    s_boundfail = FALSE; /* definitives Ruecksetzen, falls immer noch gesetzt */
    /*   if( ff_check_mode && chp->infcount <= ff_saved_maxinf ){
	   maxinf = ff_saved_maxinf;
	   ff_check_mode = FALSE;
	   ff_saved_maxinf = 0;
	   if( ff_bound_cnt ){
             cp_or_task();
	     printf(">");
	     }
         }*/
#endif


#ifdef ANTILEMMATA
    free_chp = chp;
#endif

fail_1:

    while (chp >= (choice_pt*) stack  &&  !GETVAL(*(chp->next_clause))) {
        chp = chp->ch_link;
    }
    if (chp < (choice_pt *)stack ) {
#ifdef ANTILEMMATA
        if (anl_constr) {
            for ( cp = free_chp ; cp >= (choice_pt*)stack  ; cp = cp->ch_link )
                free_anl_list(cp->anl_list);
	}
#endif
        if (boundflag)
            return boundfail | stop_cycle;
        return totfail | stop_cycle;
    }

#ifdef ANTILEMMATA
    if ((cp = immediate_antilemma_fail())) {
	chp = cp;
	goto fail_1;
    }
#endif
	    
	    
fail_2:

    untrail(chp->top_trail);
    tr = chp->top_trail;
#ifdef ANTILEMMATA
    if (anl_constr) {
        for ( cp = free_chp ; cp != chp ; cp = cp->ch_link )
            free_anl_list(cp->anl_list);
        free_chp = chp;
    }
#endif
    hp = chp->top_heap;
    old_bp = bp;      /* for delay switch */
    goal_bp = bp = chp->goal_bp;
    ra = chp->ret_addr;
    depth = chp->depth;
    infcount = chp->infcount;
    sgscount = chp->sgscount;
    sigcount = chp->sigcount;
    inf_lookahead = chp->inf_lookahead;
    local_inf_bound = chp->local_inf_bound;

    /* sp adjust to end of choice-point */
    sp = chp->sp;

    /* reset gp after failed unification */
    gp = chp->gp_save;

#ifdef CONSTRAINTS
    c_sp = chp->c_sp; 
    c_counter_sp = chp->c_counter_sp;
#endif

#ifdef ANTILEMMATA
    if (anl_constr) {
        chp->head_bp = NULL;
        chp->end_trail = NULL;
        chp->end_infcount = 0;
        chp->end_total_inf = 0;

        /*******************************************************************/
        /* try to generate antilemma constraints                           */
        /* and handle immediate antilemma fail                             */
        /*******************************************************************/
        if (!anl_gen_constr()) {    /* immediate antilemma constraint fail */
#ifdef STATISTICS1
            c_anl_fails++;
#endif
#ifdef STATISTICS2
            c_immed_anl_fails++;
#endif
            chp = chp->ch_link;
            goto fail_2;
        }

        if (!GETVAL(*(chp->next_clause)))
            goto fail_1;    

        chp->c_sp = c_sp;
        chp->c_counter_sp = c_counter_sp;
        chp->top_trail = tr;
    } 

    /*******************************************************************/
    /* The look_ahead_for_bound_fail() routine checks, if a bound fail */
    /* can be predicted. The look_ahead_for_bound_fail() routine has   */
    /* side effects on the inference lookahead counter inf_lookahead   */
    /* and on the current choicepoint: inf_lookahead is set according  */
    /* to the next clause and chp->inf_lookahead is set to             */
    /* inf_lookahead.                                                  */
    /*******************************************************************/
    if (lookahead_flag  &&  look_ahead_for_bound_fail()) {
	chp = chp->ch_link;
	goto fail_1;
    }

    /***************************************************************/
    /* Set delay option if backtracking has led to another clause: */
    /***************************************************************/
    if (delay_switch  &&  bp != old_bp) {
        if (GETNUMBER(bp->lcbptr->dinf) < switch_length) {
	    set_delay_option(option_for_shortcl);
	}
	else {
	    set_delay_option(option_for_longcl);
	}
    } 

    if (single_delay  ||  multi_delay) {
	/*******************************************************************/
	/*   The delay_subgoal() routine checks, if a subgoal must be      */
	/*   delayed. The delay_subgoal() routine has side effects on the  */
	/*   actual choicepoint chp and, since select_subgoal() is called, */
	/*   on the order of subgoal pointers on the stack.                */
	/*******************************************************************/
	delay_subgoal();
    }

    /* go and try next clause */
    /* set pc */
    pc = code + GETVAL(((literal_ctrl_block*)(code + GETVAL(*(chp->next_clause))))->clauselbl);
    chp->this_clause = chp->next_clause;
    (chp->next_clause)++;
#endif
    
    return success;
}
