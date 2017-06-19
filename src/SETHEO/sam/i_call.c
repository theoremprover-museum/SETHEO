/*************************************************************/
/*    S E T H E O                                            */
/*                                                           */
/* FILE: i_call.c                                            */
/* VERSION:                                                  */
/* DATE:                                                     */
/* AUTHOR:                                                   */
/* NAME OF FILE:                                             */
/* DESCR:                                                    */
/* MOD:                                                      */
/*	27.10.92: new proof-tree                             */
/*	11.3.93	Mueller,Goller neue Constraints, Statistics  */
/* BUGS:                                                     */
/*************************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "disp.h"
#include "symbtab.h"
/**/
#include "deref.h"
#include "reg_constr.h"
#include "dynsgreord.h"
#include "weighted_depth.h"


#ifdef STATPART
#include "sps.h"
#include "unification.h"
#endif



#define INSTR_LENGTH	1

#define DEBUG(X)



instr_result i_call()
{
    literal_ctrl_block  *lcb;
    alt_types            type_of_clause;
    
    /*************************************************************************/
    /* for the proof-tree: store the current subgoal                         */
    /*************************************************************************/
    trail_ptr((void**)&(bp->tp));
    DEBUG(printf("nalloc: bp=%lx bp->tp=%lx\n",bp,bp->tp););
    bp->tp++;
    DEBUG(printf("nalloc: bp=%lx bp->tp=%lx\n",bp,bp->tp););
    
    /*******************************************************/
    /* Select literal control block of next subgoal:       */
    /* (The select_subgoal routine has side effects on the */
    /* order of subgoal pointers on the stack.)            */
    /*******************************************************/
    type_of_clause = GETNUMBER(bp->lcbptr->type);
    if (type_of_clause == declarative) {
        if (force_delay  &&  (single_delay  ||  multi_delay))
	    select_subgoal(1);
	else
	    if (dynsgreord)
		select_subgoal(0);
    }

    lcb = (literal_ctrl_block*) GETPTR(*(bp->tp));

    /*************************************************************************/
    /* call a new procedure                                                  */
    /*************************************************************************/
    ra = pc + INSTR_LENGTH;                   /* save return-adress          */
    goal_bp = bp;          		      /* environment of calling goal */
    gp = code + GETVAL(lcb->argv);            /* goal pointer                */

    /***********************************************************/
    /* Set depth according to chosen subgoal:                  */
    /***********************************************************/
    if (   wd_bound  
	&& !eq_by_ste_mod  
	&& GETSYMBOLINDEX(lcb->ps_symb) != DEFAULT_QUERY)
        depth = weighted_depth(infcount + inf_lookahead);
    
#ifdef STATPART
    if( recomp_mode ){  /* deterministischer Call */
        if( !depth ){   /* no depth-fail during recompetition! */
	    depth++;    /* set new depth bound for next iteration */
	    t_depth++;          
	    }
        if( node_ptr->red_lvl >= 0 ){ /* perform Reduction step here! */
            perform_red_step(lcb);
	    node_ptr++;
	    pc = ra;
            }
        else          
            pc = code + (node_ptr++)->codeptr;
	return success;
       }
    else  /* !recomp_mode */ /* IAbegin */ /* subgoal aus IA-Task bearbeiten */
         if( recomp && iand_task_num != -2 && now_next_subg ){
             trail_var(&now_next_subg);
             now_next_subg = FALSE;    /* subgoals nicht ber"ucksichtigen */
	     trail_var(&node_ptr);
	     node_ptr++;               /* n"achstes subgoal bearbeiten */
             pc = code + GETVAL(lcb->orlabel); /* aktuelles subgoal bearbeiten */
	     trail_var(&saved_bp);
	     saved_bp = bp;
	     return success;
       	     }
 if( recomp && !recomp_mode && iand_task_num != -2 )          /* IAbegin */
   if( now_next_subg ){ /* subgoal ist in OR; nun pruefe, ob auch in iAND */
      if( node_ptr->codeptr == -2){        /* IAnd-Task beendet */
          i_ptree(); 
          return success | stop_cycle;     /* Beweisbaumausgabe */
          }
      else{  
	  trail_var(&num_of_subgs);
          if( (node_ptr)->codeptr != num_of_subgs++){/*subgoal not in AND task*/
              pc += INSTR_LENGTH; /* Call "uberspringen */
	      return success;
	      }
          }
    }
#endif /* END STATPART */

    /*************************************************************************/
    /* set pc                                                                */
    /*************************************************************************/
    pc = code + GETVAL(lcb->orlabel);

    return success;
}
