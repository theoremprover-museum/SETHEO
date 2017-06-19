/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_ungterm.c
 * VERSION:
 * DATE: Fri Sep  4 11:35:07 MET DST 1992
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *	18.2.93		MACROS added
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

#define FAIL_U {return failure;}

#ifdef STATISTICS1
#define UNIF_FAIL(x) {unif_fails++;FAIL_U}
#else
#define UNIF_FAIL(x) FAIL_U
#endif


#include "unification.h"

instr_result i_ungterm()
{
  register WORD *ga;
  register WORD *ha;

  /* unify with a ground term */
  /* no copy is needed	    */
  
  ga = deref(gp,goal_bp);
  ha = ARGPTR(1);
  UN_GTERM(ha, bp, ga, goal_bp)
    
    /* success */
  gp++;
  pc+= INSTR_LENGTH;
  return success;
}
