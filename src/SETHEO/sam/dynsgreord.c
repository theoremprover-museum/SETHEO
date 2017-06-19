/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: dynsgreord.c                                 */
/* VERSION:                                           */
/* DATE:                                              */
/* AUTHOR: Ortrun Ibens                               */
/* NAME OF FILE:                                      */
/* DESCR: routines for subgoal reordering             */
/* MOD:                                               */
/* MOD:                                               */
/* BUGS:                                              */
/******************************************************/


#include <stdio.h>
#include "tags.h"
#include "machine.h"
#include "symbtab.h"
/**/
#include "dynsgreord.h"
#include "bounds.h"
#include "deref.h"
#include "errors.h"
#include "termcomplexity.h"
#include "weighted_depth.h"

static sg_result  subgoal_check();
static void       mark_fvars_of_subgoal();


int  select_subgoal(delay_mode)
int  delay_mode;
{
    register WORD          *sgls,*sgi;
    WORD                   tmp;
    register int           nr_sgls,ncl_offset,dl_offset,redptr_offset,
                           first_open_sg,selected_sg,sg_index;
    register sg_result     chk;     /* results of recently checked sg */
    register sg_result     sel;     /* results of selected sg         */
    s_trail                *save_tr;
    int                    subgoals_exchanged;

    subgoals_exchanged = 0;

    /********************************************************/
    /* Get pointer to subgoal block and number of subgoals: */
    /********************************************************/
    sgls =   (WORD*)bp + SIZE_IN_WORDS(environ)
           + GETNUMBER(bp->lcbptr->nr_vars); 
    nr_sgls = GETNUMBER(bp->lcbptr->nr_sgls);

    /********************************************************************/
    /* Get offset for next clause pointers, offset for reduction        */
    /* pointers and offset for delay levels:                            */
    /********************************************************************/
    ncl_offset = NEXT_CL_OFFSET(bp);
    redptr_offset = REDPTR_OFFSET(bp);
    dl_offset = DL_OFFSET(bp);

    /************************************/
    /* Get index of first open subgoal: */
    /************************************/
    first_open_sg = bp->tp - sgls;

    /************************************************************/
    /* Select one subgoal only if there are more than one open: */
    /************************************************************/
    if (first_open_sg < nr_sgls-1) {

	sgi = sgls + first_open_sg;
	sel = subgoal_check(sgi,dl_offset,0,0,0,MAXNUMBER);
	selected_sg = first_open_sg;
	chk = sel;

	if (delay_mode  &&  force_ground  && sel.ground)
	    return subgoals_exchanged;

	/* Save trail pointer before marking fvars of subgoal: */
	save_tr = tr;

	if (delay_mode  &&  force_fvar)
	    mark_fvars_of_subgoal(sgi);
	
	SELECT_SUBGOAL

	/* Untrail: */
	untrail(save_tr);
	tr = save_tr;
	
	/****************************************************/
	/* put selected subgoal at place of first_open_sg   */
	/****************************************************/
	if (first_open_sg != selected_sg) {
	    
	    /* Exchange subgoal pointers: */
	    trail_var(sgls+first_open_sg);
	    trail_var(sgls+selected_sg);
            tmp = *(sgls+first_open_sg);
	    *(sgls+first_open_sg) = *(sgls+selected_sg);
	    *(sgls+selected_sg) = tmp;
          
	    /* Exchange next-clause pointers: */
	    trail_var(sgls+ncl_offset+first_open_sg);
	    trail_var(sgls+ncl_offset+selected_sg);
            tmp = *(sgls+ncl_offset+first_open_sg);
	    *(sgls+ncl_offset+first_open_sg) = *(sgls+ncl_offset+selected_sg);
	    *(sgls+ncl_offset+selected_sg) = tmp;

	    /* Exchange reduction pointers: */
	    trail_var(sgls+redptr_offset+first_open_sg);
	    trail_var(sgls+redptr_offset+selected_sg);
            tmp = *(sgls+redptr_offset+first_open_sg);
	    *(sgls+redptr_offset+first_open_sg) = *( sgls+redptr_offset
						    +selected_sg);
	    *(sgls+redptr_offset+selected_sg) = tmp;
	    
	    if (single_delay  ||  multi_delay  ||  delay_switch) {
		/* Exchange delay-levels: */
		trail_var(sgls+dl_offset+first_open_sg);
		trail_var(sgls+dl_offset+selected_sg);
                tmp = *(sgls+dl_offset+first_open_sg);
		*(sgls+dl_offset+first_open_sg) = *( sgls+dl_offset
						    +selected_sg);
		*(sgls+dl_offset+selected_sg) = tmp;
#ifdef STATISTICS1
		if (delay_mode)
		    delaycount++;
#endif
	    }

	    subgoals_exchanged = 1;
	}
    }

    return subgoals_exchanged;
}




static sg_result  subgoal_check(sg,dl_offset,ground_required,
				ps_required,ps,delay_level) 
WORD      *sg;
int        dl_offset;
int        ground_required;
int        ps_required;
int        ps;
int        delay_level;
{
    WORD                 *args;
    int                  arity;
    s_trail              *save_tr;
    sg_result            res;
    literal_ctrl_block   *lcb;

    lcb = (literal_ctrl_block*) GETPTR(*sg);

    res.tc = 0;
    res.fvars = 0;
    res.ground = 1;
    res.ps = GETSYMBOL(lcb->ps_symb);
    res.var_common = 0;

    if (single_delay  ||  multi_delay) { 
	/* otherwise information does not exist */
	res.dl = GETNUMBER(*(sg + dl_offset));
	if (delay_level < res.dl)
	    return(res);
    }
    else
	res.dl = 0;

    if (ps_required  &&  ps != res.ps)
        return(res);

    /* Save trail pointer: */
    save_tr = tr;

    args = code + GETVAL(lcb->argv);
    arity = GETARITY(lcb->ps_symb);
    dag_term_check(args,arity,ground_required,&res);

    /* Untrail: */
    untrail(save_tr);
    tr = save_tr;

    return(res);
}



static void  mark_fvars(args,arity)
WORD                 *args;
int                  arity;
{
    WORD                 *argi;
    int                  i;

    for ( i=0 ; i<arity ; i++ ) {
	argi = deref(args+i,bp);
	switch (GETTAG(*argi)) {

	case T_FVAR:
	    trail_var(argi);
	    GENOBJ(*argi,0,T_MARKED_FVAR);
	    break;

	case T_MARKED_FVAR:
	case T_CONST:
	case T_GTERM:  
	    break;

	case T_NGTERM: 
	case T_CRTERM:
	    mark_fvars(argi+1,GETARITY(*argi));
	    break;
	}
    }
}



static void  mark_fvars_of_subgoal(sg)
WORD   *sg;
{
    literal_ctrl_block   *lcb;
    WORD                 *args;
    int                  arity;

    lcb = (literal_ctrl_block*) GETPTR(*sg);
    args = code + GETVAL(lcb->argv);
    arity = GETARITY(lcb->ps_symb);

    mark_fvars(args,arity);
}




/*******************************************************************/
/*   The delay_subgoal() routine checks, if a subgoal must be      */
/*   delayed. The delay_subgoal() routine has side effects on the  */
/*   actual choicepoint chp and, since select_subgoal() is called, */
/*   on the order of subgoal pointers on the stack.                */
/*******************************************************************/
void  delay_subgoal() 
{
    int                 delay_level,dinf,subgoals_exchanged;
    alt_types           type_of_clause,type_of_next_clause;
    
    /*********************************************************************/
    /* Get type of current clause, type of 1st alternative at current    */
    /* subgoal, dinf of 1st alternative at current subgoal and delay     */
    /* level of current subgoal:                                         */
    /*********************************************************************/
    type_of_clause = GETNUMBER(bp->lcbptr->type);
    type_of_next_clause = GETNUMBER(((literal_ctrl_block*) 
				     (code + GETVAL(*(chp->next_clause))))->type);
   
    delay_level = GET_DL_OF_SG(bp);
    
    /***************************************************************/
    /* Get dinf of next alternative:                               */
    /* If next alternative is a reduction step:                    */
    /*     If spread_reducts and multi- or single-delay are on:    */
    /*         If there is a following alternative: dinf is number */
    /*             of subgoals in following alternative.           */
    /*         Else dinf is delay_level.                           */
    /*     Else dinf is 0.                                         */
    /* Else dinf is number of subgoals in next alternative.        */
    /***************************************************************/
    if (type_of_next_clause == reduct) {
	if (spread_reducts  &&  (single_delay  ||  multi_delay)) {
	    if (GETVAL(*((chp->next_clause)+1)))
		dinf = GETNUMBER(((literal_ctrl_block*) 
				  (code + GETVAL(*((chp->next_clause)+1))))->dinf);
	    else
		dinf = delay_level;
	}
	else
	    dinf = 0;
    }
    else
	dinf = GETNUMBER(((literal_ctrl_block*)
        		  (code + GETVAL(*(chp->next_clause))))->dinf);

    /********************************************************************/
    /* Try to delay subgoal, if                                         */
    /*    (   (    single_delay is switched on                          */
    /*         and next alternative of current subgoal has at least     */
    /*             1 subgoal                                            */
    /*         and at current subgoals so far only redution steps and   */ 
    /*             extension steps with facts have been tried)          */
    /*     or (    multi_delay is switched on                           */
    /*         and the next alternative of current subgoal has more     */
    /*             subgoals than the preceding alternative))            */
    /* and thee current clause is declarative                           */
    /* and the available depth-resource is greater than or equal to the */ 
    /*     value specified by delay_resource                            */
    /********************************************************************/
    if (    (    (single_delay  &&  dinf >= 1  &&  delay_level < 1)
	     ||  (multi_delay  &&  delay_level < dinf))
	&&  type_of_clause == declarative
	&&  depth >= delay_resource) {

	/***********************************************************/
	/* Save chp->next_clause as next-clause pointer of current */
	/* subgoal (trail before):                                 */
	/***********************************************************/
	trail_var(bp->tp + NEXT_CL_OFFSET(bp));
	GEN_NEXT_CL_OF_SG(chp->next_clause, bp);
	
	/***********************************************************/
	/* Save chp->red_ptr as reduction pointer of current       */
	/* subgoal (trail before):                                 */
	/***********************************************************/
	trail_var(bp->tp + REDPTR_OFFSET(bp));
	GEN_REDPTR_OF_SG(chp->red_ptr, bp);
	
	/***********************************************************/
	/* Store dinf of next_clause as delay-level of current     */
	/* subgoal (trail before):                                 */
	/***********************************************************/
	trail_var(bp->tp + DL_OFFSET(bp));
        GEN_DL_OF_SG(dinf, bp);
	
	/***********************************************************/
	/* Select subgoal:                                         */
	/* (The select_subgoal routine has side effects on the     */
	/* order of subgoal pointers on the stack.)                */
	/***********************************************************/
	subgoals_exchanged = select_subgoal(1);

	/***********************************************************/
	/* Do not repeat alternatives before the choicepoint is    */
	/* removed:                                                */
	/***********************************************************/
	chp->top_trail = tr;
	
	/***********************************************************/
	/* Get saved next-clause pointer of chosen subgoal,        */
	/* chp->red_ptr of chosen subgoal, set gp and save gp as   */
	/* chp->gp_save:                                           */
	/***********************************************************/
	gp = code + GETVAL(((literal_ctrl_block*)GETPTR(*(bp->tp)))->argv);
	chp->gp_save = gp;
	chp->next_clause = GET_NEXT_CL_OF_SG(bp);
	chp->red_ptr = (environ*) GET_REDPTR_OF_SG(bp);

	/***********************************************************/
	/* Set depth and chp->depth according to chosen subgoal:   */
	/* If subgoals are exchanged, weighted depth is newly      */
	/* computed; otherwise not.                                */
	/***********************************************************/
	if (subgoals_exchanged  &&  wd_bound  &&  !eq_by_ste_mod) {
	    depth = weighted_depth(infcount + inf_lookahead);
	    chp->depth = depth;
	}
    }

    return;
}



void  dag_term_check(ga,arity,ground_required,res)
WORD           *ga;
int            arity;
int            ground_required;
sg_result      *res;                 /* result */
{
    WORD           *gai;
    register int   i;
    int            sub_arity;       /* arity of subterm */


    for ( i = 0 ; i < arity ; i++ ) {

        gai = deref(ga+i,bp);
        switch (GETTAG(*gai)) {

	    case T_CONST:  res->tc += COMPL_CONST;
                           break;

	    case T_MARKED_FVAR:
		           res->var_common = 1;
            case T_FVAR:   res->tc += COMPL_FVAR;
                           res->fvars++;
                           res->ground = 0;
                           if (ground_required)
                               return;
                           /* Mark seen variables: */
                           trail_var(gai);
                           GENOBJ(*gai,COMPL_DAG,T_MARK);
                           break;

            case T_MARK:   res->tc += COMPL_DAG; /* dag complexity of seen */
		                                 /* terms/variables        */
                           break;

	    case T_GTERM:  
            case T_NGTERM:
            case T_CRTERM: sub_arity = GETARITY(*gai);
		           dag_term_check(gai+1,sub_arity,ground_required,res);
                           res->tc += COMPL_FSYMB;
                           if (ground_required  &&  !res->ground)
                               return;
                           /* Mark seen terms: */
                           trail_var(gai);
                           GENOBJ(*gai,COMPL_DAG,T_MARK);
                           break;

	    default:       sam_error("dag_term_check: illegal tag",gai,3);

        }
    }
}



void  set_delay_option(option)
int  option;
{
        single_delay = multi_delay = FALSE;
	delay_resource = 0;
	force_delay = FALSE;
	spread_reducts = FALSE;
        switch (option) {
	    case 0:  break;
	    case 1:  single_delay = TRUE;
	             break;
	    case 2:  single_delay = TRUE;
             	     delay_resource = 2;
	             break;
	    case 3:  single_delay = TRUE;
             	     force_delay = TRUE;
	             break;
	    case 4:  single_delay = TRUE;
             	     spread_reducts = TRUE;
	             break;
	    case 5:  multi_delay = TRUE;
	             break;
	    case 6:  multi_delay = TRUE;
             	     delay_resource = 2;
	             break;
	    case 7:  multi_delay = TRUE;
             	     force_delay = TRUE;
	             break;
	    case 8:  multi_delay = TRUE;
             	     spread_reducts = TRUE;
	             break;
	    default: sam_error("Delay option out of range.",NULL,0);
	             break;
	}
}

