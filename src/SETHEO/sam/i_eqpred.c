/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_eqpred.c
 * VERSION:
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "tags.h"
#include "machine.h"
/**/
#include "unifeq.h"

#define INSTR_LENGTH		4


instr_result i_eqpred()
{
  register environ *pdp;
  register WORD *llpr;

  goal_bp = bp; /* un_eq is used by porbranch, where goal_bp has already been
		   modified by call (we need the current environment in
		   goal_bp */
  pdp = goal_bp;
  llpr = gp;			/* save gp */
  gp = ARGPTR(1);		/* use farg as goal structure */
  /* get beginning of predecessor list */
  while (pdp)
    {
      /* move up list and unify */
      if (un_eq (pdp, GETSYMBOL(ARG(2)), GETVAL(ARG(3))))
	{
	  /* equality. succeeded */
	  /* make a fail	       */
	  gp = llpr;
	  return failure;
	}
      /* equality failed */
      pdp = pdp->dyn_link;
    }
  
  gp = llpr;
  pc +=INSTR_LENGTH;
  return success;
}
