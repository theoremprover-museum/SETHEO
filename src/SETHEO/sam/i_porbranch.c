/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_porbranch.c
 * VERSION:
 * DATE: Thu Sep  3 13:02:45 MET DST 1992
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
#include "i_orbranch.h"
#include "unifeq.h"

#define INSTR_LENGTH		4


instr_result i_porbranch()
{
  register WORD farg2 = ARG(2);
  register WORD farg3 = ARG(3);
  register environ* pdp;
  
  /**********************************/
  /* dp predfail                    */
  /**********************************/
  farg2 = *(pc+2);
  farg3 = *(pc+3);
  pdp= bp ; /*  goal_bp; */
  /* get beginning of predecessor list */
  while (pdp) {
    /* move up list and unify */
    if (un_eq(pdp,GETSYMBOL(farg2),GETVAL(farg3))) {
      /* equality. succeeded */
      /* make a fail	       */
      return failure;
    }
    /* equality failed */
    pdp = pdp->dyn_link;
  }
  /* equality check didnt succeed ,
   * so built up a normal choice point and proceed */
  
  return i_orbranch();
  
}
