/************************************************************/
/*    S E T H E O                                           */
/*                                                          */
/* FILE: i_proceed.c                                        */
/* VERSION:                                                 */
/* DATE: Fri Sep  4 11:13:37 MET DST 1992                   */
/* AUTHOR:                                                  */
/* NAME OF FILE:                                            */
/* DESCR:                                                   */
/* MOD:                                                     */
/*	11.3.93	Mueller,Goller neue Constraints, Statistics */
/* BUGS:                                                    */
/************************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
/**/
#include "bounds.h"
#include "deref.h"
#include "termcomplexity.h"

#ifdef STATPART
#include "sps.h"
#endif

#define INSTR_LENGTH		1


instr_result i_proceed()
{
    int       tc;

#ifdef ANTILEMMATA
    if (anl_constr) {
        if (inf_bound) {
            chp->end_infcount = infcount;
        }
        chp->end_total_inf = total_inf;
        trail_ptr((void**)&(chp->end_trail));
        chp->end_trail = tr;
    }
#endif

#ifdef TC_BOUND
    if (tc_bound  &&  sgscount) {
	tc = tc_of_all_open_subgoals();
	if (tc_bound_fail(tc))
	    return failure;
        if (tc > intmd_tc)
            intmd_tc = tc;
    }
#endif

    pc = ra;

#ifdef STATPART
        /* IAbegin */
	if( recomp && iand_task_num != -2) /* IA-Task vorhanden */
	if( bp == saved_bp )
	  {
	  /*
	    printf("i_proceed: now_next_subg = TRUE; next?= ");
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
