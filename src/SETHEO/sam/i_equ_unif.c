/*****************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_equ_unif.c
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
/**/
#include "deref.h"

#define INSTR_LENGTH		2


#define FAIL_U {return failure;}
#ifdef STATISTICS1
#define UNIF_FAIL(x) {unif_fails++;FAIL_U}
#else
#define UNIF_FAIL(x) FAIL_U
#endif


#include "unification.h"

/**************************************************************************
 * code
 **************************************************************************/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  + i_equ_unif
  + - function for built-in predicate unify(X, Y)
  +
  + special remarks:
  + - <e.g. called from>
  + - <side effects, return values>
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

instr_result i_equ_unif()

/*=====================================================================*/

{ /*=====================================================================*/
  
  register WORD     *farg1, *farg2;
  
/*-----------------------------------------------------------------------*/
  
  farg1 = deref(ARGPTR(1), bp);
  farg2 = deref(ARGPTR(1)+1, bp);

  switch(GETTAG(*farg1)) {
  case T_CONST:
    UN_CONST(farg1, bp, farg2, bp)
    break;
  case T_FVAR:
    UN_FVAR(farg1, bp, farg2, bp)
    break;
  case T_CRTERM:
    UN_CRTERM(farg1, bp, farg2, bp)
    break;
  case T_GTERM:
    UN_GTERM(farg1, bp, farg2, bp)
    break;
  case T_NGTERM:
    UN_NGTERM(farg1, bp, farg2, bp)
    break;
  case T_GLOBL:
    UN_GLOBL(farg1, bp, farg2, bp)
    break;
  default:
    FAIL_U;
  }

  pc +=INSTR_LENGTH;
  return success;
} /*=====================================================================*/


/*#######################################################################*/

