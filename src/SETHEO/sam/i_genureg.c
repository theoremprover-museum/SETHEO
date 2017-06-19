/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_genureg.c
 * VERSION:
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *	24.3.94	Christoph	new arguments
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "tags.h"
#include "machine.h"
#include "disp.h"
/**/
#include "reg_constr.h"

#define INSTR_LENGTH		2


instr_result i_genureg()
{

#ifdef CONSTRAINTS
   /* the argument of genureg is a literalcontrolblock-pointer       */
   /* generate regularity constraints to the predicate ...->ps_symb */
   /* and the argumentvektor ....->argv                             */

  if(reg_constr)
	   /* REPROOF: &&!pmode*/
  {
#ifdef FOLD_UP
    WORD negated_ps_symb = ((literal_ctrl_block *)ARGPTR(1))->ps_symb;
    CHANGE_THE_SIGN(negated_ps_symb);
    
    if(!rc_genu_constr(negated_ps_symb,
                       code + GETVAL(((literal_ctrl_block *)(ARGPTR(1)))->argv)))
     {
                                        /* if constraint is violated,      */
      return failure;                   /* before it is generated, we have */
     }                                 /* a case of irregularity -> FAIL */
#endif
  } 

#endif

  pc +=INSTR_LENGTH;
  return error;
}
