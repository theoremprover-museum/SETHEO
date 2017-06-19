/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_cut.c
 * VERSION:
 * DATE: 25.5.93
 * AUTHOR: C. Goller
 * NAME OF FILE:
 * DESCR: chp is loaded with the value, found directly below the current
 *        environment
 * MOD:
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
/**/
#include "errors.h"

#define DEBUG(X)
#define INSTR_LENGTH		1


instr_result i_cut()
{
  WORD obj;
  
  DEBUG(printf("cut \n"));
  DEBUG(printf("chp:%p\n", chp));
  DEBUG(printf("bp:%p\n", bp));
  DEBUG(printf("sp:%p\n", sp));
  
  obj =  *( (WORD *) bp + SIZE_IN_WORDS(environ) 
	    + GETNUMBER(bp->lcbptr->nr_vars) + GETNUMBER(bp->lcbptr->nr_sgls) );

  DEBUG(printf("obj:%p\n", obj));
  DEBUG(fflush(stdout));
  
  if ( GETTAG(obj) != T_BVAR )
    sam_error ("wrong tag in cut", &obj, 3);
  else
    chp = (choice_pt *) GETPTR (obj);
  
  pc += INSTR_LENGTH;
  return success;
}


