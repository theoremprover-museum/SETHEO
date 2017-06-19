/*#########################################################################
  # File:			taut_constr.c
  # SCCS-Info:		8/7/92, @(#)taut_constr.c	1.1
  # Author:		M. Bschorer 
  # Date:			13.01.92
  # Context:		Tautologie- and Subsumptions- constraints (SETHEO)
  #
  # Contents:		functions for generating of 
  #                       tautologie- and subsumptions - constraints.
  #
  # Modifications:
  #       (when)	  (who)	          (what)
  #       12.02.92        M.Bschorer      new function ta_gen_constr();
  #
  #	  11.3.93         Mueller,Goller  neue Constraints, Statistics

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
#include "taut_constr.h"
#include "univ_constr.h"

/**************************************************************************
* declarations
**************************************************************************/

int   ta_gen_constr();

extern unsigned long debug_level;

/**************************************************************************
* code
**************************************************************************/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ ta_gen_constr
+ - generates tautologie-constraints for the arguments in the gp1, und gp2
+
+ special remarks:
+ - called from i_cgen und i_neq_built
+ - returns 0, if generated constraint is violated
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int ta_gen_constr(gp1, gp2)
WORD   *gp1;                      /* Pointer to argumentlist 1     */
WORD   *gp2;                      /* Pointer to argumentlist 2     */

{ /*=====================================================================*/
	
  c_counter *ta_counter;

  WORD	    *lhp;

  /*-----------------------------------------------------------------------*/

    lhp = hp;
    pu_tr = pu_trail;                          /* reset pu_trail */

    if (un_protoco(gp1, bp, gp2, bp, 1)) {
	/*******************************************************************/
	/* gp1 is unifiable with gp2 - protocol unification was successful */
	/*******************************************************************/

	if (pu_tr == pu_trail) {
	    /************************************************/
	    /* no bindings were made => optimal solution    */
	    /* don't try any other but produce an immediate */
	    /* tautology fail and goto fail afterwards      */
	    /************************************************/
#ifdef STATISTICS1
	    c_ts_fails++;
#endif                                       
#ifdef STATISTICS2
	    c_immed_ts_fails++;
#endif                                       
	    return 0; 
	}

	/*******************************************************/
	/* generate a counter for a tautology constraint, take */
	/* the bindings from protocol unification and make an  */
	/* optimized constraint out of those bindings          */
	/*******************************************************/
        ta_counter = (c_counter *) c_gencounter(TAUTOLOGIE,chp,0);
	c_gen_optimize(ta_counter);

#ifdef STATISTICS1
	gen_ts_constrs++;
#endif 
    }
    else {
	/*********************************************************/
	/* protocol unification did not match - untrail bindings */
        /* possibly made during protocol unification             */
	/*********************************************************/
	hp = lhp; /* garbage collection on the heap is important
		     for antilemmata */
	pu_untrail();
    }  /* else */

    return 1;
} /*=====================================================================*/


#endif
