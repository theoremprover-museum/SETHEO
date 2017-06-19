/*#########################################################################
# File:			reg_constr.c
# SCCS-Info:		8/7/92, @(#)reg_constr.c	1.1
# Author:		M. Bschorer 
# Date:			13.01.92
# Context:		regularity-constraints (SETHEO)
#
# Contents:		functions for generating of 
#                       regularity-constraints.
#
# Modifications:
#	(when)		(who)		(what)
#       14.01.92        M.Bschorer      new function rc_gen_constr();
#       15.01.92        M.Bschorer      new function rc_gen_decide();
#
#	11.3.93	Mueller,Goller neue Constraints, Statistics
#	03.6.94	johann		new macros

# *	09/12/2001	IS_FOLDED_UP
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
#########################################################################*/

#ifdef CONSTRAINTS

#include <stdio.h>
#include "tags.h"
#include "constraints.h"
#include "machine.h"
#include "disp.h"
#include "codedef.h"
#include "symbtab.h"
/**/
#include "reg_constr.h"
#include "univ_constr.h"

#ifdef STATPART
#include "sps.h"
#endif

extern unsigned long debug_level;

/**************************************************************************
 * declarations
 **************************************************************************/

int   rc_gen_constr();
int   rc_gen_decide();


/**************************************************************************
 * code
 **************************************************************************/




/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  + rc_gen_constr
  + - generates regularity-constraints for the predicate rc_predicate
  +
  + special remarks:
  + - rc_predicate is already without a tag
  + - only called from genreg.ins or call
  + - returns 0, if generated constraint is violated
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rc_gen_constr(rc_predicate, goal_p)
WORD    rc_predicate;             /* predicate of the next subgoals */
WORD   *goal_p;                   /* Pointer to argumentvector      */

{ /*=====================================================================*/
	
  environ  *rc_bp;               /* pointer to environment          */
  int       nargs;               /* number of arguments of clause   */

/*-----------------------------------------------------------------------*/

    /******************************/
    /* get arity of the predicate */
    /******************************/
    nargs = GETARITY(rc_predicate);

    if (ts_constr)
	rc_bp = bp->dyn_link;      /* look in the preprevious environment */
    else
	rc_bp = bp;                /* look in the previous environment    */

    while (rc_bp) { /* evtl. bis rc_bp->dyn_link ? */
#ifdef STATPART
         if( iand_task_num != -2 && rc_bp <= saved_bp )   /* CS 2.12.94 */
	   /*if( recomp && iand_task_num != -2 && rc_bp <= saved_bp && linres )*/
	   break;                /* genreg nur bis recomp-area CS 5.9.94 */
#endif
	 /********************************/
	 /* check for predicate equality */
	 /********************************/
	 if (GETSYMBOL(rc_bp->lcbptr->ps_symb) == GETSYMBOL(rc_predicate)) {
	     if (!nargs) {
#ifdef STATISTICS1
		 c_reg_fails++;
#endif
#ifdef STATISTICS2
		 c_immed_reg_fails++;
#endif
		 return 0;
	     }
             
	     if (!rc_gen_decide(code + GETVAL(rc_bp->lcbptr->argv), rc_bp,
			        rc_predicate, goal_p, nargs)) {
               return 0;
             }
	 }

	 rc_bp = rc_bp->dyn_link;    /* next above environment */

     }  /* while (rc_bp) */

     return 1;

} /*=====================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ rc_genx_constr
+ - generates regularity-constraints for the predicate rc_predicate
+ - (extended regularity) in addition to rc_gen_constr, folded-up
+ - are regarded environments
+ special remarks:
+ -
+ - rc_predicate is already without a tag
+ - returns 0, if generated constraint is violated
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifdef FOLD_UP

int rc_genx_constr(rc_predicate, goal_p)
WORD    rc_predicate;             /* predicate of the next subgoals */
WORD   *goal_p;                   /* Pointer to argumentvector      */

{ /*=====================================================================*/
	
  environ  *rc_bp;               /* pointer to environment          */
  int       nargs;               /* number of arguments of clause   */

/*-----------------------------------------------------------------------*/

    /******************************/
    /* get arity of the predicate */
    /******************************/
    nargs = GETARITY(rc_predicate);

    if (ts_constr)
	rc_bp = bp->path_link;      /* look in the preprevious environment */
    else
	rc_bp = bp;                /* look in the previous environment    */


    while (rc_bp) {

        /************************/
	/* is rc_bp folded up ? */
	/************************/
        /********************************/
	/* check for predicate equality */
	/********************************/

      WORD negated_rc_predicate = rc_predicate;
      CHANGE_THE_SIGN(negated_rc_predicate);
      
        if (    (    (IS_FOLDED_UP(rc_bp->flags)) 
                  && (GETSYMBOL(rc_bp->lcbptr->ps_symb) == GETSYMBOL(rc_predicate)) ) 
             || (    !(IS_FOLDED_UP(rc_bp->flags)) 
                  && (    GETSYMBOL(rc_bp->lcbptr->ps_symb) 
                       == GETSYMBOL(negated_rc_predicate)) )) {

	    if (!nargs) {
#ifdef STATISTICS1
		c_reg_fails++;
#endif
#ifdef STATISTICS2
		c_immed_reg_fails++;
#endif
		return 0;
	    }

	    if (!rc_gen_decide(code + GETVAL(rc_bp->lcbptr->argv), rc_bp,
			       rc_predicate, goal_p, nargs))
                return 0;
	}  /* if */

	rc_bp = rc_bp->path_link;    /* next above environment */
    }  /* while */

    return 1;

} /*=====================================================================*/

#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ rc_genu_constr
+ - generates regularity-constraints for the predicate rc_predicate
+ - in addition to rc_gen_constr open subgoals are regarded
+ special remarks:
+ - rc_predicate is already without a tag
+ - only called from genreg.ins or call
+ - returns 0, if generated constraint is violated
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#ifdef FOLD_UP

int rc_genu_constr(rc_predicate, goal_p)
WORD    rc_predicate;             /* predicate of the next subgoals */
WORD   *goal_p;                   /* Pointer to argumentvector      */

{ /*=====================================================================*/
	
  environ  *rc_bp;               /* pointer to environment          */
  int       nargs;               /* number of arguments of clause   */

    /******************************/
    /* get arity of the predicate */
    /******************************/
    nargs = GETARITY(rc_predicate);

    rc_bp = bp->dyn_link;      /* look in the preprevious environment */

    while (rc_bp) {

	/********************************/
	/* check for predicate equality */
	/********************************/
	if (GETSYMBOL(rc_bp->lcbptr->ps_symb) == GETSYMBOL(rc_predicate)) {

            if (!nargs) {
#ifdef STATISTICS1
	        c_reg_fails++;
#endif
#ifdef STATISTICS2
    	        c_immed_reg_fails++;
#endif
	        return 0;

	    }

            if (!rc_gen_decide(code + GETVAL(rc_bp->lcbptr->argv), rc_bp,
		               rc_predicate, goal_p, nargs))
	         return 0;

        }  /* if */

       if (!rc_genu_help(rc_predicate, goal_p, rc_bp))
	   return 0;

       rc_bp = rc_bp->dyn_link;    /* next above environment */
	   
    }  /* while */

    return 1;
} /*=====================================================================*/



/*************************************************************************/
/* generate regularity-constraints fo the open subgoals of rc_bp         */
/*************************************************************************/

int rc_genu_help(rc_predicate, goal_p, rc_bp)
WORD    rc_predicate;             /* predicate of the next subgoals */
WORD   *goal_p;                   /* Pointer to argumentvector      */
environ  *rc_bp;                  /* pointer to environment         */

{ /*=====================================================================*/

  int       nargs;               /* number of arguments of clause   */
  int      open_sgls;
  WORD     *sg_i;
  literal_ctrl_block *lcbptr;

/*-----------------------------------------------------------------------*/

    /******************************/
    /* get arity of the predicate */
    /******************************/
    nargs = GETARITY(rc_predicate);

    sg_i =   (WORD *)rc_bp + SIZE_IN_WORDS(environ) 
	   + GETNUMBER(rc_bp->lcbptr->nr_vars);
    open_sgls = GETNUMBER(rc_bp->lcbptr->nr_sgls);
    while (sg_i <= rc_bp->tp) {
         sg_i++;
         open_sgls--;
    }

    while(open_sgls){
        lcbptr = (literal_ctrl_block*) GETPTR(*sg_i); 

        /********************************/
        /* check for predicate equality */
        /********************************/
        if (GETSYMBOL(lcbptr->ps_symb) == GETSYMBOL(rc_predicate)) {

	    if (!nargs) {
#ifdef STATISTICS1
	        c_reg_fails++;
#endif
#ifdef STATISTICS2
	        c_immed_reg_fails++;
#endif
	        return 0;
	    }

	    if (!rc_gen_decide(code + GETVAL(lcbptr->argv), rc_bp,
			       rc_predicate, goal_p, nargs))
	        return 0;
       
        } /* if */
        open_sgls--;
        sg_i++;
    }

    return 1;

} /*=====================================================================*/

#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ rc_gen_decide
+ - decides, whether the "primitive-rc-constraint" should be generated
+   or not.
+
+ special remarks:
+ - called only from rc_gen_constr
+ - returns 0, if constraint is violated before generated else return 1
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int rc_gen_decide(rc_gp, rc_bp, rc_predicate, goal_p, nargs)

environ    *rc_bp;                             /* environment of upper */
WORD       *rc_gp;                             /* pointer to argument- */
                                               /* vector of upper clau.*/
                                               /* predicate            */
WORD       *goal_p;                            /* pointer to argument- */
                                               /* vector of curr. clau.*/
WORD        rc_predicate;                      /* predicate of clause  */
int         nargs;                             /* # of arguments       */

{ /*=====================================================================*/
	
  c_counter  *rc_counter;                        /* counter for constraint */

  WORD	     *lhp;

  /*-----------------------------------------------------------------------*/

    lhp = hp;
    pu_tr = pu_trail;                          /* start pu_trail         */
			
    if (un_protoco(goal_p, bp, rc_gp, rc_bp, nargs)) {

	/*******************************************************************/
	/* gp1 is unifiable with gp2 - protocol unification was successful */
	/*******************************************************************/
        if (pu_tr == pu_trail) {
	    /************************************************/
	    /* no bindings were made => optimal solution    */
	    /* don't try any other but produce an immediate */
	    /* regularity fail and goto fail afterwards     */
	    /************************************************/
#ifdef STATISTICS1
	    c_reg_fails++;
#endif
#ifdef STATISTICS2
	    c_immed_reg_fails++;
#endif
	    return 0; 
	}

	/********************************************************/
        /* generate a counter for a regularity constraint, take */
	/* the bindings from protocol unification and make an   */
	/* optimized constraint out of those bindings           */
	/********************************************************/
	rc_counter = (c_counter *) c_gencounter(REGULARITY,chp,0);
	c_gen_optimize(rc_counter);
#ifdef STATISTICS1
        gen_reg_constrs++;
#endif
    }
    else {
	/*********************************************************/
	/* protocol unification did not match - untrail bindings */
	/* possibly made during protocol unification             */
	/*********************************************************/
	hp = lhp; /* garbage collection on the heap is important for
		     antilemmata */
	pu_untrail();
    }

    return 1;
} /*=====================================================================*/

#endif
