/******************************************************
 *    S E T H E O                                     *
 *                                                    *
 * FILE: unifeq.c
 * VERSION:
 * DATE: 7.1.88
 *	11.1.88
 * AUTHOR: J. Schumann
 * NAME OF FILE:
 * DESCR:
 *	check for equality of subgoals
 * MOD:
 *	check for REAL equality
 *	with terms 16.5.88
 *      bug there 1.6.88
 *      20.6. bug
 *	08.05.89  modify chk_arg arguments for eq/neq
 *      27.6.91  changes in chk_arg     Goller
 *	4.9.92   LCB
 * BUGS:
 ******************************************************
 
 Algorithm:
 Move thru all parameters and dereference head and goal
 check for == and return on !=
 ******************************************************/
#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
/**/
#include "unifeq.h"
#include "deref.h"

/******************************************************
 * un_eq
 ******************************************************/
int             un_eq (lbp, predsymb, nargs)
    environ        *lbp;
    int             predsymb,
                    nargs;
{
    WORD           *lgp,
                   *lhp;
         
    int             i;

/* check if predsymbols are equal */
    if (predsymb != GETSYMBOL((lbp->lcbptr)->ps_symb)) {
	/* is a fail */

	return 0;
    }

/* if we have a prop. formula: return success */
    if (!nargs)
	return 1;
/* head is here the current subgoal */
/* pointer from hi to low	    */
/* goal is at *lbp                  */
    for (i = 0, lgp = code + GETVAL(lbp->lcbptr->argv), lhp = gp;
	    i < nargs;
	    lhp++, lgp++, i++) {
	if (!chk_arg (lhp, goal_bp, lgp, lbp))
	    return 0;
    }
    return 1;	/* success of eq-pred */
}

/**************************************************
 * check one argument
 *************************************************/
int chk_arg (lhp, gbp, lgp, lbp)
    WORD           *lhp,
                   *lgp;
    environ        *lbp,
                   *gbp;
{
    WORD           *lha,
                   *lga;

    lha = deref (lhp, gbp);
    lga = deref (lgp, lbp);

#ifdef DEBUG

    printf ("check-equal-on reduction arg#");
    printf ("curr_goal: %lx curr_goal *: %lx\n", (long) lhp, (long) *lhp);
    printf ("deref:curr_goal: %lx curr_goal *: %lx\n", (long) lha, (long) *lha);
    printf ("goal-up: %lx *goal-up: %lx\n", (long) lgp, (long) *lgp);
    printf ("deref: goal-up: %lx *goal-up: %lx\n", (long) lga, (long) *lga);
#endif

/* if (lga == lha) return 1; // if structures are equal, return ok */

    /* a really twisted 2-dimensional switch-case structure ... */
    switch ((GETTAG (*lga) << TAGSIZE) | (GETTAG (*lha))) {
      case (T_CONST << TAGSIZE) | T_CONST:
	/* const-const */
	if (GETVAL (*lga) != GETVAL (*lha))
	    /* fail */
	    return 0;
	return 1;
/*********************************************************
 * difference to unifred:  C-V and V-C not possible
 *********************************************************/
      case (T_FVAR << TAGSIZE) | T_FVAR:
	/* var -var */
	if (lha != lga)
	    return 0;
	return 1;
      case (T_GTERM << TAGSIZE) | T_GTERM:
      case (T_GTERM << TAGSIZE) | T_CRTERM:
      case (T_CRTERM << TAGSIZE) | T_GTERM:
      case (T_CRTERM << TAGSIZE) | T_CRTERM:
/* these cases are necessary only for the eq/neq predicate */
/* as NGTERM only occurrs in CODE but never in heap        */
      case (T_NGTERM << TAGSIZE) | T_CRTERM:
      case (T_NGTERM << TAGSIZE) | T_GTERM:
/* added by Goller */
/* erasing bug prun.lop */
      case (T_CRTERM << TAGSIZE) | T_NGTERM:
      case (T_GTERM << TAGSIZE) | T_NGTERM:
/*------------------------------------*/
      case (T_NGTERM << TAGSIZE) | T_NGTERM:
	if (GETVAL (*lha) != GETVAL (*lga))
	    return 0;
	lha++; lga++;
	while (GETTAG (*lha) != T_EOSTR) {
	    /* proceed thru arguments */
	    if (!chk_arg (lha++, gbp, lga++, lbp))
		return 0;
	}
	return 1;
      default:
	return 0;
    }
}
