/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: bounds.c                                     */
/* VERSION:                                           */
/* DATE:                                              */
/* AUTHOR: Ortrun Ibens                               */
/* NAME OF FILE:                                      */
/* DESCR: routines for handling of bounds             */
/* MOD:                                               */
/* MOD:                                               */
/* BUGS:                                              */
/******************************************************/


#include <stdio.h>
#include "tags.h"
#include "machine.h"
#include "defaults.h"
#include "symbtab.h"
/**/
#include "bounds.h"
#include "weighted_depth.h"


/*******************************************************************/
/* The look_ahead_for_bound_fail() routine checks, if a bound fail */
/* can be predicted. The look_ahead_for_bound_fail() routine has   */
/* side effects on the inference lookahead counter inf_lookahead   */
/* and on the current choicepoint: inf_lookahead is set according  */
/* to the next clause and chp->inf_lookahead is set to             */
/* inf_lookahead.                                                  */
/* The possible return values are                                  */
/* 0 <==> no bound fail can be predicted                           */
/* 1 <==> a bound fail can be predicted                            */
/*                                                                 */
/* WARNING: If inwasm -noclreord is called, sam -nolookahead MUST  */
/*          be called (otherwise wrong behaviour) !                */
/*                                                                 */
/*******************************************************************/
int  look_ahead_for_bound_fail()
{
    int                 dinf_thcl;  /* dinf of this clause */
    int                 dinf_ncl;   /* dinf of next clause */
    int                 ddepth;     /* ddepth of next clause */
    int                 new_depth;
    alt_types           type_of_this_clause,type_of_next_clause;
    
    /*****************************************************************/
    /* Get dinf of this clause, dinf of next clause, ddepth of next  */
    /* clause:                                                       */
    /*****************************************************************/
    type_of_this_clause = GETNUMBER(((literal_ctrl_block*) 
				     (code + GETVAL(*(chp->this_clause))))->type);

    type_of_next_clause = GETNUMBER(((literal_ctrl_block*) 
				     (code + GETVAL(*(chp->next_clause))))->type);

    if (type_of_this_clause == reduct)
	dinf_thcl = 0;
    else
	dinf_thcl = GETNUMBER(((literal_ctrl_block*)
			       (code + GETVAL(*(chp->this_clause))))->dinf);
    
    if (type_of_next_clause == reduct) {
	dinf_ncl = 0;
	ddepth = 0;
    }
    else {
	dinf_ncl = GETNUMBER(((literal_ctrl_block*)
			      (code + GETVAL(*(chp->next_clause))))->dinf);
	ddepth = GETNUMBER(((literal_ctrl_block*)
			    (code + GETVAL(*(chp->next_clause))))->ddepth);
    }

    /*****************************************************************/
    /* If next clause is a reduct or a fact, no bounds are checked. */
    /*****************************************************************/
    if (dinf_ncl == 0)
        return 0;
	
    /*****************************************************************/
    /* Set inference lookahead value:                                */
    /*****************************************************************/
    inf_lookahead += (dinf_ncl - dinf_thcl);
    chp->inf_lookahead = inf_lookahead;

    /*****************************************************************/
    /* Look ahead for new depth:                                     */
    /*****************************************************************/
    new_depth = depth;
    if (wd_bound) {
        if (eq_by_ste_mod)
	    new_depth =   weighted_depth(infcount + inf_lookahead - dinf_ncl)
	                - delta_depth(inf_lookahead,ddepth);
	else
	    new_depth -= delta_depth(inf_lookahead,ddepth);
    }
    else
	new_depth -= ddepth;
 
    /*****************************************************************/
    /* Look ahead for bound fail:                                    */
    /*****************************************************************/
    if (bound_fail(new_depth,
		   infcount + inf_lookahead,
		   infcount,
		   local_inf_bound - inf_lookahead,
		   sgscount + inf_lookahead)) {
	return 1;
    }
    
    return 0;
}



int  bound_fail(depth_value,inf_value,old_inf_value,local_inf_value,sgs_value)
int   depth_value; 
int   inf_value; 
int   old_inf_value; 
int   local_inf_value; 
int   sgs_value;
{
    register int   i;
    c_list         *ptr;

    if (depth_value < 0) {
	boundflag = TRUE;
#ifdef STATISTICS1
	depth_fails++;
#endif
        return(1);
    }

    if (local_inf_value < 0) {
        boundflag = TRUE;
#ifdef STATISTICS1
        local_inf_fails++;
#endif
        return(1);
    }

    if (sgs_bound  &&  sgs_value > maxsgs) {
	boundflag = TRUE;
#ifdef STATISTICS1
        sgs_fails++;
#endif
      	return(1);
    }

    if (inf_value > maxinf) {
        boundflag = TRUE;
#ifdef STATISTICS1
        inf_fails++;
#endif
        return(1);
    }
    if (anl_constr  &&  inf_bound) {
#ifdef ANTILEMMATA
        for ( i = MAXIMUM(old_inf_value + 1, 0) ; i <= inf_value ; i++ ) {
            if (i <= DEF_DELAYMAX) {
                for ( ptr = DELAYED_BY_INF_BOUND(i); 
                      ptr ; 
                      ptr = ptr->next ) {
                    if ( ptr->zaehler->length == 0 ) {
#ifdef STATISTICS1
                        c_anl_fails++;
#endif
                        return(1);
		    }
                }
	    }
        }
#endif
    }

    return(0);
}



int  fvars_bound_fail(fvars_value)
int   fvars_value;
{
    if (fvars_bound  &&  fvars_value > maxfvars) { 
        boundflag = TRUE;
#ifdef STATISTICS1
        fvars_fails++;
#endif
	return(1);
    }

    return(0);
}



int  tc_bound_fail(tc_value)
int   tc_value;
{
    if (tc_value > maxtc) { 
        boundflag = TRUE;
#ifdef STATISTICS1
        tc_fails++;
#endif
	return(1);
    }

    return(0);
}



int  sig_bound_fail(sig_value)
int   sig_value;
{
    if (sig_value > maxsig) { 
        boundflag = TRUE;
#ifdef STATISTICS1
        sig_fails++;
#endif
	return(1);
    }

    return(0);
}



void  init_bounds(tc_init,sgs_init,fvars_init,sig_init) 
int  tc_init;
int  sgs_init;
int  fvars_init;
int  sig_init;
{
    register int  i;

    maxtc = tc_init;
    maxsgs = sgs_init;
    maxfvars = fvars_init;
    maxsig = sig_init;
    
    if (depth_bound) {

	if (tcd_flag)
	    maxtc = MINIMUM(tc_init, 
			    (int) (  (float) tcd1 / (float) 100 
				   + (float) (depth*tcd2) / (float) 100
				   + 0.5));
	
	if (sigd_flag)
	    maxsig = MINIMUM(sig_init, 
			     (int) (  (float) sigd1 / (float) 100 
				    + (float) (depth*sigd2) / (float) 100
				    + 0.5));
	
	if (sgsd_flag)
	    maxsgs = MINIMUM(sgs_init, 
			     (int) (  (float) sgsd1 / (float) 100 
				    + (float) (depth*sgsd2) / (float) 100
				    + 0.5));
	    
	if (fvarsd_flag)
	    maxfvars = MINIMUM(fvars_init, 
			       (int) (  (float) fvarsd1 / (float) 100 
				      + (float) (depth*fvarsd2) / (float) 100
				      + 0.5));

    }

    for ( i = 0 ; i < nr_symbols ; i++ )
	sig_occurrence[i] = 1;
    
    return;
}
