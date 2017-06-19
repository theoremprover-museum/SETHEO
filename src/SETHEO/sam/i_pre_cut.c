/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_pre_cut.c
 * VERSION:
 * DATE: 25.5.93
 * AUTHOR: C. Goller
 * NAME OF FILE:
 * DESCR: the chp, to which setheo has to backtrack after a cut, is pushed on 
 *        the stack; the only correct position is at the beginning of a clause, 
 *        directly after the alloc-instruction;
 * MOD:
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"

#define DEBUG(X)

#define INSTR_LENGTH		1


instr_result i_pre_cut()
{

   DEBUG(printf("pre_cut \n");)
   DEBUG(printf("stack:%p\n", stack);)
   DEBUG(printf("chp:%p\n", chp);)
   DEBUG(printf("sp:%p\n", sp);)
   DEBUG(printf("bp:%p\n", bp);)

   GENPTR(*sp, (WORD_cast)chp->ch_link);
   sp++;

 pc += INSTR_LENGTH;
 return success;

}



