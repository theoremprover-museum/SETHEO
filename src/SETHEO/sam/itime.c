/*******************************************
;;;*****************************************
;;;MODULE NAME:   itime.c  
;;;
;;;PARENT     :   src
;;;
;;;PROJECT    :   setheo
;;;
;;;AUTHOR     :   J.Schumann
;;;
;;;DATE       :   @(#)itime.c	5.2 27 Sep 1994
;;;
;;;DESCRIPTION:   test the speed of the theorem prover
;;;               xxxxxxxxxxxxxx
;;;MODIF      :   xxxxx (when, what, who)
;;;
;;;REMARKS    :   xxxxxxx
;;;		UNIX-systems ONLY
;;;		transputer-system (27.12.88)
;;;		12.6.89 iterative deepening
;;;             11.02.91 CONSTRAINTS added by bschorer
;;;             26.2.92 local-inf-bounding added by bschorer
;;;             10.3.93 time units changed from msec to sec
;;;                     statistic counters restyled and corrected C.Mueller
;;;		11.3.93 V3.0
;;;		16.9.93		johann	unitlemma-statistics
;;;		25.2.94		johann	CLK_TCKs
;;;
;;;#****************************************
********************************************/
#include <stdio.h>
#ifdef UNITLEMMA
#include "unitlemma.h"
#endif
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "defaults.h"
#include "iteration.h"

#include <sys/types.h>
#include <sys/times.h>

#include <time.h>
/**/
#include "itime.h"
#include "bounds.h"
#include "errors.h"
#include "genlemma.h"
#include "init.h"
#include "instr.h"

#ifndef CLK_TCK
#define CLK_TCK 60
#endif

#ifdef STATPART
#include "sps.h"

int  ti;        /* STATPART ! for SPS signal handling required globally !*/
float actime;   /* STATPART ! for SPS signal handling required globally !*/
extern int   task_bnd_reached;
extern int     cputimelimit;  /* CS 21.9.84 */
extern int     inf_step_cnt;  /* CS 15.10.94 */
extern int skipped_ia_redstep_cnt;
extern int skipped_lr_redstep_cnt;
int  t_depth;
int      lbfails = 0, bfails = 0;
#endif

#ifdef STATISTICS1
#ifndef STATPART
int             ti;
#endif
float           run_time;
static	int     lfails;
#endif

#undef PVM

/****************************************************************************/
/* instr-time                                                             ***/
/****************************************************************************/

int  instr_time (iterative_bounding, min_d, d_inc, min_loc_inf, 
                 loc_inf_inc, min_inf, inf_inc)

    int             iterative_bounding,
                    min_d,
                    d_inc,
                    min_loc_inf,
                    loc_inf_inc,
                    min_inf, 
                    inf_inc;

{
    int             save_depth, save_local_inf_bound, save_maxinf, 
                    save_maxfvars, save_maxtc, save_maxsgs, save_maxsig, 
                    iter; 
    int             min_iter = 1;
    int             max_iter = 0; 
    int             iter_inc = 1; 
    int            *iter_bound = NULL;
    char           *print_string = NULL;
    instr_result    result = error;
#ifdef STATPART
    char            iter_char = ' ';
    int             i;
    int             j;         /* IAnd */
    int             pt_reduction_cnt = m_tasks/8;
#endif



#ifdef STATISTICS1
    /* Save start time: */
    times (&buffer);
    ti = buffer.tms_utime;
    run_time = 0.0;
    evaltime = ((float) ti / (float)CLK_TCK);
#endif

    switch (iterative_bounding) {
 
        case NONE:     result = instr_cycle();
#ifdef STATISTICS1
                       times (&buffer);
	               evaltime =   ((float)buffer.tms_utime / (float)CLK_TCK) 
                                  - evaltime;
                       GET_INFERENCES_AND_FAILS;
                       fails = lfails;
#endif
	               return result;

        case DEPTH:    min_iter = min_d;
                       max_iter = DEF_DEPTH;
                       iter_inc = d_inc;
                       iter_bound = &depth;
                       if (wd_bound)
                           print_string = "-wd: %4d  ";
                       else
                           print_string = "-d: %4d  ";
#ifdef STATPART
                       iter_char = 'd';
#endif
                       break;

        case INF:      min_iter = min_inf;
                       max_iter = DEF_INFCOUNT;
                       iter_inc = inf_inc;
                       iter_bound = &maxinf;
                       print_string = "-i %6d  ";
#ifdef STATPART
		       iter_char = 'i';
#endif
                       break;

        case LOCINF:   min_iter = min_loc_inf;
                       max_iter = DEF_LOC_INF_BOUND;
                       iter_inc = loc_inf_inc;
                       iter_bound = &local_inf_bound;
                       print_string = "-loci %8d  ";
                       break;

        case FVARS:    sam_error("itime.c:instr_time: Iterative fvars bound\
                                  is not yet available !",NULL,2);
                       break;

        case TC:       sam_error("itime.c:instr_time: Iterative tc bound is\
                                  not yet available !",NULL,2);
                       break;

        case SGS:      sam_error("itime.c:instr_time: Iterative sgs bound is\
                                  not yet available !",NULL,2);
                       break;

        case SIG:      sam_error("itime.c:instr_time: Iterative signature\
                                  bound is not yet available !",NULL,2);
                       break;

        default:       sam_error("itime.c:instr_time: No such bound !",NULL,2);

    }

    /* Save counters and bounds: */
    save_depth = depth;
    save_local_inf_bound = local_inf_bound;
    save_maxinf = maxinf;
    save_maxfvars = maxfvars;
    save_maxtc = maxtc;
    save_maxsgs = maxsgs;
    save_maxsig = maxsig;

    for ( iter = min_iter ; iter <= max_iter ; iter += iter_inc ) {

        /* Reset counters and bounds: */
        infcount = 0;
        sgscount = 1;
        sigcount = 0;
        inf_lookahead = 0;
        depth = save_depth;
        local_inf_bound = save_local_inf_bound;
        maxinf = save_maxinf;

        /* Set and print iteration bound: */
        *iter_bound = iter;
#ifdef STATISTICS1
        PRINT_LOG1(print_string,*iter_bound);
#endif

#ifdef STATPART
        if (recomp)
           PRINT_RES2("-%c:  %3d", iter_char, iter_bound);
#endif

        /* Init and run: */
	init_bounds(save_maxtc,save_maxsgs,save_maxfvars,save_maxsig);
        init_reg();
        if (((result = instr_cycle ()) == success) || (result == error)
                                                   || (result == totfail)) {
	    break;
            }

#ifdef STATPART
    if( iterative_bounding == INF && create_task ){
        if( ornode_counter >=(m_tasks-m_tasks/15.0)){/* 15% deviation allowed!*/
            if( ! task_bnd_reached )
                printf("  ");
            printf("%3d ornodes ", ornode_counter);
            break;   /* already enough tasks generated; no further deepening! */
            }
        if( task_bnd_reached ){
            iter -= inf_inc;
            if( ornode_counter > pt_reduction_cnt ){
                pt = ornode_counter - pt_reduction_cnt;
                pt_reduction_cnt = (int) (2 * (pt_reduction_cnt)+1) ;
                }
            else
                pt = 0;
            task_bnd_reached = FALSE;
            }
        else{
            printf("  ");
            pt = ornode_counter;
            }
        printf("%3d ornodes ", ornode_counter); fflush(stdout);
        init_spt_it_deep();
        }
#endif

#ifdef STATISTICS1
        /* Get and print intermediate statistics: */
        ITERATION_STATISTICS;
#endif
    }
#ifdef PVM
    if( recomp && pvm && result == success ) pvm_task_success();
#endif

    prepare_statistics();
      
    return result;
}




/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * prepare_statistics
 *	-called after iterative deepening
 *	        or if a signal occurred
 *+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void  prepare_statistics()
{
#ifdef STATISTICS1
    ITERATION_STATISTICS;
    evaltime = ((float)buffer.tms_utime / (float)CLK_TCK) - evaltime;
#endif
}
      



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ disp_statistics()
+ - Prints out the statistical information about proof. 
+
+ special remarks:
+ - When using PRINT_LOG macros in if-statements do consider that they
+   are already put into parentheses by definition (see machine.h).
+   Therefore no semicolon is needed after those macros.
+ - <side effects, return values>
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void  disp_statistics(print_time) 
int print_time;

{ /*=====================================================================*/

#ifdef STATISTICS1

    int successful_inferences;
    
    /* evaltime changed to programm-global float variable - C. Mueller */
    times (&buffer);
    
    PRINT_LOG1("Intermediate free variables        : %8d\n", intmd_fvars);
    if (tc_bound)
        PRINT_LOG1("Intermediate term complexity       : %8d\n", intmd_tc);
    PRINT_LOG1("Intermediate inferences            : %8d\n", intmd_inf);
    PRINT_LOG1("Intermediate open subgoals         : %8d\n", intmd_sgs);
    if (sig_bound  ||  intmd_sig)
	PRINT_LOG1("Intermediate signature             : %8d\n", intmd_sig);
    if (sig_bound  ||  sigcount)
	PRINT_LOG1("Signature in proof                 : %8d\n", sigcount);
#ifdef ANTILEMMATA
    PRINT_LOG1("Generated antilemmata              : %8d\n", anlcount);
#endif
	
    if (single_delay  ||  multi_delay)
	PRINT_LOG1("Number of subgoal-delays           : %8d\n", delaycount);

    if (print_time) {
        PRINT_LOG1("Elapsed Time                       : %8.2f sec\n",
                   (float) (buffer.tms_utime - ti) / (float)CLK_TCK);
        PRINT_LOG1("Number of Inferences               : %8d\n", infcount);
    }

    PRINT_LOG1("Number of unifications             : %8d\n", global_tot_inf);
    PRINT_LOG4("    - E/R/F/L                      : %8d/%8d/%8d/%8d\n", 
               (global_tot_inf - total_reduct 
                - folding_statistics.total_factorizations),
               total_reduct, 
               folding_statistics.total_factorizations,
               0);

#ifdef CONSTRAINTS
    if (gen_reg_constrs + gen_ts_constrs + gen_anl_constrs) {
        PRINT_LOG1("Number of generated constraints    : %8d\n",
	           (gen_reg_constrs + gen_ts_constrs + gen_anl_constrs));
        PRINT_LOG3("    - anl/reg/ts                   : %8d/%8d/%8d\n",
	            gen_anl_constrs, gen_reg_constrs, gen_ts_constrs);
    }
#endif
  
    PRINT_LOG1("Number of fails                    : %8d\n", fails);
    if (unif_fails)
        PRINT_LOG1("    - unification                  : %8d\n", unif_fails);

    if (depth_fails)
        PRINT_LOG1("    - depth bound                  : %8d\n", depth_fails)

    if (inf_fails)
        PRINT_LOG1("    - inference bound              : %8d\n", inf_fails)

    if (fvars_fails)
        PRINT_LOG1("    - variable bound               : %8d\n", fvars_fails);

    if (tc_fails)
        PRINT_LOG1("    - term complexity bound        : %8d\n", tc_fails);

    if (sgs_fails)
        PRINT_LOG1("    - open subgoals bound          : %8d\n", sgs_fails);

    if (sig_fails)
        PRINT_LOG1("    - signature bound              : %8d\n", sig_fails);

    if (local_inf_fails)
        PRINT_LOG1("    - local-inf bound              : %8d\n", 
                   local_inf_fails)

#ifdef CONSTRAINTS
    if (c_anl_fails + c_reg_fails + c_ts_fails) {
        PRINT_LOG1("    - constraints                  : %8d\n",
                   (c_anl_fails + c_reg_fails + c_ts_fails));
        PRINT_LOG3("        - anl/reg/ts               : %8d/%8d/%8d\n",
                   c_anl_fails, c_reg_fails, c_ts_fails);
#ifdef STATISTICS2
        PRINT_LOG1("    - imm. constr. fails           : %8d\n",
                   (c_immed_reg_fails + c_immed_ts_fails + c_immed_anl_fails));
        PRINT_LOG3("        - anl/reg/ts               : %8d/%8d/%8d\n",
                   c_immed_anl_fails, c_immed_reg_fails, c_immed_ts_fails);
#endif
    }
#endif

    if (folding_statistics.foldings) {
        PRINT_LOG1("Number of folding operations       : %8d\n",
                   folding_statistics.foldings); 
        PRINT_LOG1("    - one level                    : %8d\n", 
                   folding_statistics.one_level_foldings);
        PRINT_LOG1("    - root                         : %8d\n", 
                   folding_statistics.possible_lemmas);
    }
    PRINT_LOG1("Instructions executed              : %8d\n", cnt);

    if (evaltime >= 0.01)
        PRINT_LOG1("Abstract machine time (seconds)    : %8.2f\n", evaltime)
    else
       PRINT_LOG0("Abstract machine time (seconds)    :   < 0.01\n")

    /* global_tot_inf - fails except bound-fails */
    successful_inferences = global_tot_inf -
                            (fails - depth_fails - inf_fails 
                             - local_inf_fails);

/*  Wegkommentiert, weil es nicht verstaendlich ist. */
/*
    if (successful_inferences && (evaltime > 0.01))
        PRINT_LOG1 (" (%3.1f Klips)\n",
                    (float)successful_inferences/(1000.0 * evaltime))
    else
        PRINT_LOG0 (" (--- Klips)\n")
*/

    times(&buffer);
    /* overall_time is first taken at very begin of main */
    overall_time = ((float)buffer.tms_utime / (float)CLK_TCK) - overall_time;
    if (overall_time >= 0.01)
        PRINT_LOG1("Overall time          (seconds)    : %8.2f\n", 
                   overall_time)
    else
        PRINT_LOG0 ("Overall time          (seconds)    :   < 0.01\n")

#ifdef UNITLEMMA
#ifdef UNITLEMMA_STATISTICS
    unitlemma_statistics_print();
#endif
#endif

#endif

} /*=====================================================================*/
