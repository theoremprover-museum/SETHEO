/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_unvar.c
 * VERSION:
 * DATE: Fri Sep  4 11:31:45 MET DST 1992
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *	18.2.93	MACROS von Ch. Mueller
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
#define UNIF_FAIL(x) {unif_fails++; FAIL_U}
#else
#define UNIF_FAIL(x) FAIL_U
#endif


#include "unification.h"


instr_result i_unvar()
{
  register WORD *ga, *ha;
  
  ga = deref(gp,goal_bp);
  ha = ref_head((WORD *)bp + SIZE_IN_WORDS(environ)+GETVAL(ARG(1)));
  
  
  switch(GETTAG(*ha)) {
  case T_CONST:
    UN_CONST(ha, bp, ga, goal_bp)
    break;
  case T_FVAR:
    UN_FVAR(ha, bp, ga, goal_bp)
    break;
  case T_CRTERM:
    UN_CRTERM(ha, bp, ga, goal_bp)
    break;
  case T_GTERM:
    UN_GTERM(ha, bp, ga, goal_bp)
    break;
  default:
    UNIF_FAIL("default")
      }
  
  gp++;
  pc+=INSTR_LENGTH;
  return success;
}
