/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_sto.c
 * VERSION:
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"

#define INSTR_LENGTH		2

#define FAIL_U {return failure;}


#ifdef STATISTICS1
#define UNIF_FAIL(x) {unif_fails++;FAIL_U}
#else
#define UNIF_FAIL(x) FAIL_U
#endif

#define COPY_CONST

#include "unification.h"



instr_result i_sto()
{
  register WORD *ga;
  
  ga = ref_head((WORD *)bp + SIZE_IN_WORDS(environ)+GETVAL(ARG(1)));
  
  
  UN_CONST((sp-1), bp, ga, bp);
  
  sp--;
  
  pc +=INSTR_LENGTH;
  return success;
}
