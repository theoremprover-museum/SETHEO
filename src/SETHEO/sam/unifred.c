
/******************************************************
 *    S E T H E O                                     *
 *                                                    *
 * FILE: unifred.c
 * VERSION:
 * DATE: 7.1.88
 *	11.1.88
 *	08.2.88
 * AUTHOR: J. Schumann
 * NAME OF FILE:
 * DESCR:
 * MOD:
 * 2.5.   modular.
 * 30.11. no MACRO
 * 25.6.91 including special unifyred.h macro      Goller
 * 4.9.92 Lit-cntrl-blck
 * 18.2.93 Einau MACROS von Ch. Mueller
 *	11.3.93	Mueller,Goller neue Constraints, Statistics
 *	09/12/2001	IS_FOLDED_UP
 * BUGS:
 *     no (long) in DEBUG
 ******************************************************/


#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "disp.h"
/**/
#include "unifred.h"
#include "deref.h"

#define FAIL_U {return 0;}

#ifdef STATISTICS1
#define UNIF_FAIL(x) {unif_fails++;FAIL_U}
#else
#define UNIF_FAIL(x) FAIL_U
#endif

#include "unification.h"

/******************************************************
 * int un_red(lbp,predsymb,nargs)
 ******************************************************/
int             un_red (lbp, predsymb, nargs)
environ         *lbp;
long            predsymb;
int             nargs;
{
    WORD           *lgp,
                   *lhp,
                   *lga,
                   *lha;
    WORD           negated_ps_symb;
    int             i;

#ifdef DEBUG
    printf ("un_red lbp %d predsymb %d nrargs %d\n", lbp, predsymb, nargs);
#endif

#ifndef FOLD_UP

    if (predsymb != GETSYMBOL((lbp->lcbptr)->ps_symb)) {
			/* check for equal predicates       */
	return 0;
    }

#ifdef STATISTICS1
    total_inf++;
    total_reduct++;                     /* increment the counter for tried  */
                                        /* reduction-steps                  */
#endif

#else

    negated_ps_symb = (lbp->lcbptr)->ps_symb;
    CHANGE_THE_SIGN(negated_ps_symb);
    if (predsymb != 
	    ( (IS_FOLDED_UP(lbp->flags)) ?
		  GETSYMBOL(negated_ps_symb) :
		  GETSYMBOL((lbp->lcbptr)->ps_symb)
		) 
	   ){
			/* check for equal predicates       */
	return 0;
    }

#ifdef STATISTICS1
    total_inf++;
	if(IS_FOLDED_UP(lbp->flags))
	   folding_statistics.total_factorizations++;
    else 
       total_reduct++;                  /* increment the counter for tried  */
                                        /* reduction-steps                  */
#endif

#endif


    for (i = 0,lgp = code + GETVAL(lbp->lcbptr->argv),lhp = gp; i < nargs; lhp++,lgp++,i++) {

	lha = deref (lhp, goal_bp);
	lga = deref (lgp, lbp);


#ifdef DEBUG

	printf ("unif-on reduction arg#: %d ", i + 1);
	printf ("curr_goal: %x curr_goal *: %x\n", lhp, *lhp);
	printf ("deref:curr_goal: %x curr_goal *: %x\n", lha, *lha);
	printf ("goal-up: %x *goal-up: %x\n", lgp, *lgp);
	printf ("deref: goal-up: %x *goal-up: %x\n", lga, *lga);
	fflush (stdout);
#endif



	switch(GETTAG(*lha)) {
	case T_CONST:
	  UN_CONST(lha, goal_bp, lga, lbp)
	    break;
	case T_FVAR:
	  UN_FVAR(lha, goal_bp, lga, lbp)
	    break;
	case T_CRTERM:
	  UN_CRTERM(lha, goal_bp, lga, lbp)
	    break;
	case T_GTERM:
	  UN_GTERM(lha, goal_bp, lga, lbp)
	    break;
	case T_NGTERM:
	  UN_NGTERM(lha, goal_bp, lga, lbp)
	    break;
	case T_GLOBL:
	  UN_GLOBL(lha, goal_bp, lga, lbp)
	    break;
	default:
	  UNIF_FAIL("default")
	    break;
	}


    }	/* for */
    /* success */
    
    return 1;
}

/******************************************************
 * int un_factor(lbp,lcbptr,predsymb,nargs)
 ******************************************************/
int un_factor (lbp, lcbptr, predsymb, nargs)
environ            *lbp;
literal_ctrl_block *lcbptr;
long               predsymb;
int                nargs;
{
    WORD           *lgp,
                   *lhp,
                   *lga,
                   *lha;
    int             i;

#ifdef DEBUG
    printf ("un_factor lbp %d predsymb %d nrargs %d\n", lbp, predsymb, nargs);
#endif


    if (predsymb != GETSYMBOL(lcbptr->ps_symb)) {
			/* check for equal predicates       */
	return 0;
    }

#ifdef STATISTICS1
    total_inf++;
	folding_statistics.total_factorizations++;
#endif



    for (i = 0, lgp = code + GETVAL(lcbptr->argv), lhp = gp; i < nargs; lhp++, lgp++, i++) {

	lha = deref (lhp, goal_bp);
	lga = deref (lgp, lbp);


#ifdef DEBUG

	printf ("unif-on reduction arg#: %d ", i + 1);
	printf ("curr_goal: %x curr_goal *: %x\n", lhp, *lhp);
	printf ("deref:curr_goal: %x curr_goal *: %x\n", lha, *lha);
	printf ("goal-up: %x *goal-up: %x\n", lgp, *lgp);
	printf ("deref: goal-up: %x *goal-up: %x\n", lga, *lga);
	fflush (stdout);
#endif



	switch(GETTAG(*lha)) {
	case T_CONST:
	  UN_CONST(lha, goal_bp, lga, lbp)
	    break;
	case T_FVAR:
	  UN_FVAR(lha, goal_bp, lga, lbp)
	    break;
	case T_CRTERM:
	  UN_CRTERM(lha, goal_bp, lga, lbp)
	    break;
	case T_GTERM:
	  UN_GTERM(lha, goal_bp, lga, lbp)
	    break;
	case T_NGTERM:
	  UN_NGTERM(lha, goal_bp, lga, lbp)
	    break;
	case T_GLOBL:
	  UN_GLOBL(lha, goal_bp, lga, lbp)
	    break;
	default:
	  UNIF_FAIL("default")
	    break;
	}
	
	
    }	/* for */
    /* success */
    
    return 1;
}


