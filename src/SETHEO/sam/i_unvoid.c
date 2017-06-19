/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_unvoid.c
 * VERSION:
 * DATE: Fri Sep  4 11:23:50 MET DST 1992
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *	18.2.93	unvoid must bind variable (needed for the proof-tree 
 *	11.3.93	Mueller,Goller neue Constraints, Statistics
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
/**/
#include "deref.h"

#define INSTR_LENGTH		2

		/* FAILURE must not occur */
#define FAIL_U {return error;}

#ifdef STATISTICS1
#define UNIF_FAIL(x) {unif_fails++;FAIL_U}
#else
#define UNIF_FAIL(x) FAIL_U
#endif


#include "unification.h"

instr_result i_unvoid()
{
  register WORD *ga;
  register WORD *ha;
  
  /* unify with a void in the head */
  ga = deref(gp,goal_bp);
  
  /* we do not need to dereference, since this variable
   * is unbound for sure */
  ha = (WORD *)bp + SIZE_IN_WORDS(environ)+GETVAL(ARG(1));
  
  UN_FVAR(ha, bp, ga, goal_bp)
  gp++;
  pc += INSTR_LENGTH;
  return success;
}
