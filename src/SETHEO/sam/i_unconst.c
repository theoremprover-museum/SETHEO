/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_unconst.c
 * VERSION:
 * DATE: Fri Sep  4 11:28:19 MET DST 1992
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *	11.3.93	Mueller,Goller neue Constraints, Statistics
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
/**/
#include "deref.h"
#include "univ_constr.h"

/*#   define FAIL_U {return failure;}

#ifdef STATISTICS1
#define UNIF_FAIL {unif_fails++;FAIL_U}
#else
#define UNIF_FAIL FAIL_U
#endif


#include "unification.h"*/


#define INSTR_LENGTH		2


instr_result i_unconst()
{
/******************************************/
/* one possibility to use UN_CONST */
/*register WORD *ga;
    ga = deref(gp,goal_bp);
	GENOBJ(*hp, ARG(1), T_CONST);
        hp++;

	UN_CONST((hp-1), NULL, ga, goal_bp);

	gp++;
	pc += INSTR_LENGTH;
	return success;*/
/******************************************/

/* In the following the macro UN_CONST is expanded and changed, where it was
   necessary */

  register WORD *ga;
  
  ga = deref(gp,goal_bp);
  
  switch (GETTAG(*ga)) {
    
  case T_CONST:
    
    if (GETVAL(*ga) != GETVAL(ARG(1))) {
#ifdef STATISTICS1
      unif_fails++;
#endif
      return failure;
    }
    break;
    
    
  case T_GLOBL:
    trail_var(ga);
    GENCONST(*ga, ARG(1));
    break;
    
    
  case T_FVAR:
#ifdef CONSTRAINTS
    GENCONST(*hp, ARG(1));
    hp++;
    if(!c_check(ga, hp-1)) {
      return failure;
    }
#endif
    trail_var (ga);
    GENCONST(*ga, ARG(1));
    break;
    
    
  default:
#ifdef STATISTICS1
    unif_fails++;
#endif
    return failure;
    break;
    
  }
  

  gp++;
  pc += INSTR_LENGTH;
  return success;
}
