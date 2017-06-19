/*****************************************************************************/
/*    S E T H E O                                                            */
/*                                                                           */
/* FILE: i_orbranch.c                                                        */
/* VERSION:                                                                  */
/* DATE: Thu Sep  3 12:51:15 MET DST 1992                                    */
/* AUTHOR:                                                                   */
/* NAME OF FILE:                                                             */
/* DESCR:                                                                    */
/* MOD:                                                                      */
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
/**/
#include "i_orbranch.h"
#include "bounds.h"
#include "fvars.h"

#define  INSTR_LENGTH		2


instr_result i_orbranch()
{
    register choice_pt 	*lcp;
    int                 nr_new_free_vars;


    sgscount--;

    lcp = (choice_pt *)sp;
    sp += SIZE_IN_WORDS(choice_pt);
    lcp->ch_link = chp;
    lcp->top_trail = tr;
    lcp->top_heap = hp;
    lcp->goal_bp = goal_bp;
    lcp->ret_addr = ra;
    lcp->gp_save = gp;
    lcp->depth = depth;
    lcp->infcount = infcount;
    lcp->sgscount = sgscount;
    lcp->sigcount = sigcount;
    lcp->inf_lookahead = inf_lookahead;
    lcp->local_inf_bound = local_inf_bound;

    /***************************************************/
    /* Read lcp->red_ptr from reduction pointer block: */
    /***************************************************/
    lcp->red_ptr = (environ*) GET_REDPTR_OF_SG(goal_bp);

#ifdef REPROOF_MODE
    lcp->ptree_save = pt_ptr;
#endif
#ifdef FOLD_UP
    lcp->red_tp = NULL;
    lcp->open_sgls = 0;
#endif

#ifdef CONSTRAINTS
    /***********************************************************************/
    /* Save constraint_sp and pointer to last generated constraint counter */
    /***********************************************************************/
    lcp->c_sp = c_sp;
    lcp->c_counter_sp = c_counter_sp;
    lcp->deact_c_counter_sp = c_counter_sp;
#endif

#ifdef ANTILEMMATA
    lcp->head_bp = NULL;
    lcp->block_fvars = get_free_variables(lcp);
    if (stronganl)
        mark_irrelevant_fvars(lcp);
    lcp->end_trail = NULL;
    lcp->end_infcount = 0;
    lcp->end_total_inf = 0;
    lcp->anl_list = NULL;
#endif

    lcp->sp = sp;

    /*************************************************/
    /* Read lcp->next_clause from next_clause block: */
    /*************************************************/
    lcp->next_clause = GET_NEXT_CL_OF_SG(goal_bp);

    /*******************************************************************/
    /* Set pc to the clause indicated by lcp->next_clause, set         */
    /* lcp->this_clause and increase lcp->next_clause:                 */
    /*******************************************************************/
    pc = code + GETVAL(((literal_ctrl_block*)(code + GETVAL(*(lcp->next_clause))))->clauselbl);
    lcp->this_clause = lcp->next_clause;
    (lcp->next_clause)++;


#ifdef FVARS_BOUND
    /*****************************************************************/
    /* Check for variable bound fail:                                */
    /*****************************************************************/
    nr_new_free_vars = lcp->nr_fvars;

    if (fvars_bound_fail(nr_new_free_vars)) {
	return failure;
    }

    if (nr_new_free_vars > intmd_fvars)
        intmd_fvars = nr_new_free_vars;
#endif

    chp = lcp;

    return success;
}
