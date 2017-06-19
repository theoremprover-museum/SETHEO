/*****************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_arg.c
 * VERSION:
 * DATE:			1.6.94
 * AUTHOR:			J. Schumann
 * NAME OF FILE:
 * DESCR:
 *			arg(N,T,A)
 * MOD:
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "tags.h"
#include "machine.h"
#include "unification.h"
#include "symbtab.h"
#include "disp.h"
/**/
#include "deref.h"
#include "disp.h"
#include "errors.h"


/************************************************/

#define INSTR_LENGTH		2

#define FAIL_U {return failure;}
#define UN_WITHGLOBAL

#ifdef STATISTICS1
#define UNIF_FAIL(x) {unif_fails++;FAIL_U}
#else
#define UNIF_FAIL(x) FAIL_U
#endif


#define DEBUG(X) 




/************************************************
 *
 ************************************************/
instr_result i_arg()
{
  WORD     *farg1, *farg2, *farg3;
  int 	 arity;
  int		whicharg;

  farg1 = deref (ARGPTR(1), bp);
  farg2 = deref (ARGPTR(1)+1, bp);
  farg3 = deref (ARGPTR(1)+2, bp);
  
  DEBUG(printf("arg:\n"));
  DEBUG(disp_(stdout,farg1,bp));
  DEBUG(printf("\n"));
  DEBUG(disp_(stdout,farg2,bp));
  DEBUG(printf("\n"));
  DEBUG(disp_(stdout,farg3,bp));
  DEBUG(printf("\n"));
  
  if (!ISNUMBER(*farg1)){
    sam_error("First argument of arg must be a number", farg1, 0);
    pc +=INSTR_LENGTH;
    return error;
  }
  
  if (!ISCOMPLEX(*farg2)){
    sam_error("Second argument of arg must be a complex term", farg1, 0);
    pc +=INSTR_LENGTH;
    return error;
  }
  
  arity = GETARITY(*farg2);
  
  whicharg = GETNUMBER(*farg1);
  
  if ((whicharg <= 0 ) || (whicharg > GETARITY(*farg2))){
    sam_error("First argument of arg out of range", farg1, 0);
    pc +=INSTR_LENGTH;
    return error;
  }
  
  /*
   * unify the argument with the 3rd argument
   */
  farg1 = deref(farg2 + whicharg,bp);
  
  DEBUG(printf("arg: argument ="));
  DEBUG(disp(stdout,farg1,bp));
  DEBUG(printf("\n"));
  UNIFY(farg1,bp,farg3,bp);
  
  pc +=INSTR_LENGTH;
  return success;
}
