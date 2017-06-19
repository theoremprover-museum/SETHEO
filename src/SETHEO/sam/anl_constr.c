/*#########################################################################
# File:			anl_constr.c
# SCCS-Info:		8/7/92, @(#)anl_constr.c	1.1
# Author:		M. Bschorer 
# Date:			09.04.92
# Context:		antilemma-constraints (SETHEO)
#
# Contents:		functions for generating of 
#                       antilemma-constraints.
#
# Modifications:
#	(when)		(who)		(what)
#       09.04.92        M.Bschorer      new function ac_gen_constr();
#	11.3.93	Mueller,Goller neue Constraints, Statistics
#
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
#########################################################################*/


#ifdef ANL_CONSTR

#include <stdio.h>
#include "tags.h"
#include "constraints.h"
#include "machine.h"
#include "disp.h"
#include "codedef.h"
#include "symbtab.h"

/**************************************************************************
* declarations
**************************************************************************/

int      ac_gen_constr();
int      ac_create_anl_constr();


/**************************************************************************
* code
**************************************************************************/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ ac_gen_constr
+ - generates antilemma-constraints 
+
+ special remarks:
+ - only called from fail.ins 
+ - returns 0, if generated constraint is violated, else returns 1
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int ac_gen_constr()                    

{ /*=====================================================================*/
	
WORD      *lhp;                /* pointer-variable for scanning heap*/
WORD      *ref;                /* local-help-variable for scanning  */
                               /* the heap                          */
int        retval;             /* return value                      */
WORD      *strvar;             /* pointer to generated structure-var*/

/*-----------------------------------------------------------------------*/


	/**********************************************************************/
	/* If the subgoal belonging to the chp was not solved, anl_tr is NULL */ 
	/**********************************************************************/
	if (!chp->anl_tr)
		return 1;   
                             
	/**************************************************************/
	/* If the proof search after solving the subgoal didn't cost  */
	/* more than ac_risk, redundancies in the search are accepted */
	/**************************************************************/
#ifdef STATISTICS1
	if( (total_inf - chp->anl_tot_inf) <= ac_risk )
		return 1;
#endif

	/**************************************************************/
	/* Untrail all bindings made after solving the subgoal of chp */
	/**************************************************************/
	untrail(chp->anl_tr);      
	tr = chp->anl_tr;
	hp = chp->anl_hp;         
                          
	/****************************************************/
	/* If no bindings were made in the solution of the  */
	/* subgoal, return with an immediate antilemma fail */
	/****************************************************/
	if (tr <= chp->top_trail)
		return 0;

	lhp  = chp->top_heap;     

	/**************************************************/
	/* compress the terms which have been copied onto */
	/* the heap during the solution of the subgoal    */
	/**************************************************/
	while (lhp < chp->anl_hp) {                       

		if (GETTAG(*lhp) == T_BVAR)  {                           

			ref = (WORD *)ref_head(lhp);
			if ((ref > (WORD *) chp) && (ref <= sp)) { 

				switch(GETTAG(*ref)) {

					/**************************************************/
					/* if ref will be deleted by backtracking to chp  */
					/* create a structure variable for it on the heap */
					/**************************************************/
					case T_FVAR:
						GENOBJ(*lhp, 0, T_STRVAR);
						GENOBJ(*ref, (WORD) lhp, T_BVAR);      
						break;

					/***********************************************/
					/* if ref is a constant which would be deleted */
					/* save it on the heap and point to it         */
					/***********************************************/
					case T_CONST:
						*lhp = *ref;
						GENOBJ(*ref, (WORD) lhp, T_BVAR);      
						break;
					}
				}
			else {
				/************************************************/
				/* avoid further deref chains through the stack */
				/************************************************/
				GENOBJ(*lhp, (WORD) ref, T_BVAR);
				}
			}

		lhp ++;
		}

	c_sp = chp->c_sp;		/* reset c_sp for creation of new constraints */
	c_counter_sp = chp->c_counter_sp;

	retval = ac_create_anl_constr();

	/**********************************************************/
	/* Due to constraint generations heap, c_stack, and trail */
	/* might have grown. Save the machine state after this.   */
	/**********************************************************/
	chp->c_sp = c_sp;     
	chp->c_counter_sp = c_counter_sp;
	chp->top_trail = tr;
	chp->top_heap = hp;  

/*	chp->anl_hp = NULL;
	chp->anl_tr = NULL;*/
	return retval;  


} /*=====================================================================*/



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ ac_create_anl_constr
+ - untrailing variable-bindings, and preparing these bindings for
+   generating a antilemma-subconstraint.
+
+ special remarks:
+ - only called from ac_gen_constr
+ - returns number of created subconstraints.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int ac_create_anl_constr()        

{ /*=====================================================================*/
	
s_trail        *lpt;
WORD           *left;
WORD           *right;
c_counter      *anl_counter;         /* pointer to counter of subconstraints */

/*-----------------------------------------------------------------------*/

pu_tr = pu_trail;                   /* reset pu_trail     */

/* Buffer all bindings of former free variables on the pu_trail */
for (lpt = tr - 1; lpt >= chp->top_trail; lpt--) {

	/* Do not consider changes on subconstraint pointers and counters */
	if ((GETTAG(lpt->oval) == T_FVAR) &&
		(GETTAG(*lpt->ptr) != T_FVAR)) {

		left = lpt->ptr;                     
		right = (WORD *) ref_head(left);

		if (GETTAG(*right) == T_CONST) {
			if (hp + 1 >= hplast)
				sam_error("Heap overflow", NULL, 2);
			*hp = *right;
			right = hp++;
			} 
        
		if(left < (WORD *) chp) {
			(pu_tr++)->ptr = left;  
			(pu_tr++)->ptr = right;
			}
		}
	}    /* end for */

/*********************************************************************/
/* Only now untrail the bindings made in the subproof of the subgoal */
/*********************************************************************/
untrail(chp->top_trail); 
tr = chp->top_trail;

if(pu_tr > pu_trail) {

		anl_counter = (c_counter *) c_gencounter(ANTILEMMA);

		/**************************************************/
		/* Create subconstraints from the buffer pu_trail */
		/**************************************************/
		while (pu_tr > pu_trail) {
			right = (--pu_tr)->ptr;
			left = (--pu_tr)->ptr;
			c_create(left, right, anl_counter);
			}

#ifdef STATISTICS1
		gen_anl_constrs++;
#endif
	return 1;
	}
else
	return 0;
} /*=====================================================================*/
#endif
