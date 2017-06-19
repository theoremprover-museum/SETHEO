/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_cgen.c
 * VERSION:
 * DATE:
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
#include "taut_constr.h"

#define INSTR_LENGTH		2


instr_result i_cgen()
{

#ifdef CONSTRAINTS

  /* was:  cnt--; WHY ??? */

  if(ts_constr)
    { 
    /*************************************************************/
    /****   dynamic tautologie                                 ***/
    /*************************************************************/

      if(!ta_gen_constr(ARGPTR(1), ARGPTR(1) + 1))
          return failure;     /* means goto fail */
      else  {
	  pc +=INSTR_LENGTH;
          return success;
		}
    }

  else
   {
	pc +=INSTR_LENGTH;
        return success;	
    }

#endif
}

