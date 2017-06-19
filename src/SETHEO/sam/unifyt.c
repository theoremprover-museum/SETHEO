
/******************************************************
 *    S E T H E O                                     *
 *                                                    *
 * FILE: unifyt.c
 *       ****new version*****
 * VERSION:
 * DATE:
 *	3.11.88
 * AUTHOR: J. Schumann
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *		27.12.88 signals
 *		16.2.89  globals & terms
 *		10.5.89  error
 *		23.10.89 error
 *		10.11.89 NEW design
 *		11.02.92 CONSTRAINTS added by bschorer
 *		06.03.92 restyled by bschorer
 *		25.02.93 copy_it removed and restyled by C.Mueller
 *	11.3.93	Mueller,Goller neue Constraints, Statistics
 *		1.6.94		johann: include symbtab.h
 * BUGS:
 *
 ******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "symbtab.h"

#ifdef SIGNALS
#include <signal.h>
#endif

/**/
#include "errors.h"
#include "deref.h"
#include "unifyt.h"
#include "full.h"

#define MAKEOCC		


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  + copy_struct
  + - copies the structure obj onto the heap
  +   if var occurs in obj then occur check fail will be detected
  +
  + special remarks:
  + - called by unification and built-ins
  + - Returns NULL in case of occur check, a pointer to copied object,
  +   or pointer to object itself in case of T_GTERM
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

WORD           *copy_struct (obj, bp, var)

WORD           *obj;
environ        *bp;
WORD	       *var;

{ /*=====================================================================*/
  
  WORD           *hp_start,
                 *lhp,
                 *ga;

  int loop_test;

  /*-----------------------------------------------------------------------*/
  
    lhp = hp;
    hp_start = hp;

	/**************************************/
	/* while not reached end of structure */
	/**************************************/
	do {

#ifdef DEBUG
	printf ("copy_it %lx bp=%x,var=%x lhp=%x hp=%x\n", (long) obj, bp, var, lhp, hp);
	printf ("*obj= %x \n", *obj);
	fflush (stdout);
#endif

	switch (GETTAG (*obj)) {

		case T_HREF:
			ga = ref_head (heap + GETVAL (*obj));
			goto deal_var;

          
#ifdef CONSTRAINTS
		case T_CSTRVAR:
#endif
		case T_VAR:
			ga = deref (obj, bp);
deal_var:
			switch (GETTAG (*ga)) {

				case T_CONST:
					*lhp = *ga;
					break;


#ifdef CONSTRAINTS
				case T_STRVAR:
#endif
				case T_FVAR:
					if (ga == var) {
						/********************/
						/* occur check fail */
						/********************/
						return NULL;
						}
#ifndef CONSTRAINTS
					trail_var (ga);
					GENPTR (*ga, lhp);
					GENOBJ (*lhp, 0, T_FVAR);
#else
					/********************************************************************/
					/* With constraints T_FVAR's must exclusively be kept on the stack  */
					/* Let therefore point the copied new var. on the heap to the stack */
					/********************************************************************/
					GENPTR(*lhp, ga); 
#endif
					break;

				case T_GLOBL:
					trail_var (ga);
					GENOBJ (*lhp, 0, T_FVAR);
					GENPTR (*ga, lhp);
					break;
				
				case T_CRTERM:
					if (occ_ck (var, ga, bp)) {
						/********************/
						/* occur check fail */
						/********************/
						return NULL;
						}
					/* ATTENTION: NO break statement */

				case T_GTERM:
					GENPTR (*lhp, ga);
					break;

				default:
					printf("\nTag "GETTAGFORMAT" is dereferenced T_VAR\n", GETTAG(*ga));
					sam_error ("Illegal tag in copy_struct", ga, 3);

				}
			break;

		case T_CREF:
			/********************************************************/
			/* recursive call for whatever function term is         */
			/* meant by T_CREF might be T_NGTERM OR T_GTERM as well */
			/********************************************************/
			if ((ga = copy_struct ((WORD *) (GETVAL (*obj) + code), 
				 bp, var)) == NULL ) {
				/********************/
				/* occur check fail */
				/********************/
				return NULL;
				}
			/**************************************/
			/* create T_BVAR on the heap and      */
			/* point to the result of copy_struct */ 
			/**************************************/
			GENPTR (*lhp, ga);
			break;

		case T_NGTERM:
			/***************************************************/
			/* increase hp by arity of the function symbol and */
			/* reserve one cell for T_CRTERM and T_EOSTR each  */
			/* the arguments will be copied by the do-loop     */
			/***************************************************/
			GENSYMBOL(*lhp, GETVAL(*obj), T_CRTERM);
			hp += (GETARITY (*obj) + 2);
			break;

		case T_GTERM:
			return obj;

		case T_EOSTR:
		case T_CONST:
			*lhp = *obj;
			break;

		default:
			printf("\nTag "GETTAGFORMAT" occurs in T_NGTERM\n", GETTAG(*obj));
			sam_error ("Illegal tag in copy_struct", obj, 3);

		}
	lhp++;
        loop_test = (GETTAG (*obj) != T_EOSTR);
        obj++;
    } while (loop_test);

    return hp_start;
} /*=====================================================================*/

