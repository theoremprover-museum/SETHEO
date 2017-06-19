/************************************************************/
/*    S E T H E O                                           */
/*                                                          */
/* FILE: i_genreg.c                                         */
/* VERSION:                                                 */
/* DATE:                                                    */
/* AUTHOR:                                                  */
/* NAME OF FILE:                                            */
/* DESCR:                                                   */
/* MOD:                                                     */
/*	11.3.93	Mueller,Goller neue Constraints, Statistics */
/*	24.3.94	Christoph	new arguments               */
/* BUGS:                                                    */
/************************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "disp.h"
/**/
#include "reg_constr.h"

#define INSTR_LENGTH		2


instr_result i_genreg()
{
#ifdef CONSTRAINTS
    
    /* the argument of genreg is a literalcontrolblock-pointer       */ 
    /* generate regularity constraints to the predicate ...->ps_symb */
    /* and the argumentvektor ....->argv                             */    
    
    if(reg_constr) {
       literal_ctrl_block * ptr = ((literal_ctrl_block *)ARGPTR(1));
       WORD negated_ps_symb = ptr->ps_symb;
       CHANGE_THE_SIGN(negated_ps_symb);
       
       if(!rc_gen_constr(negated_ps_symb,code + GETVAL(ptr->argv))) {
	    /* immediate regularity constraint fail */
	    return failure;                 
	}                         
    }  
    
#endif
    
    pc +=INSTR_LENGTH;
    return success;
}
