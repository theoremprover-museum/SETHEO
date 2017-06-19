/******************************************************
 *    S E T H E O                                     
 *                                                   
 * FILE: i_functor.c
 * VERSION:
 * DATE:
 * AUTHOR:			Chr. Mueller
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *			30.5.94		johann
 *			9.3.95		johann
 *			22.3.95		johann
 *				new spec
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

/*****************************************************/


#define INSTR_LENGTH		2

#define FAIL_U {return failure;}
#define UN_WITHGLOBAL

#ifdef STATISTICS1
#define UNIF_FAIL(x) {unif_fails++;FAIL_U}
#else
#define UNIF_FAIL(x) FAIL_U
#endif


#define DEBUG(X) 


static WORD *new_crterm();


/************************************************
 *
 ************************************************/
instr_result i_functor()
/* Argumentvektor T, F, A */
{
  WORD     *farg1, *farg2, *farg3;
  WORD 	 *tmp;
  int 	 arity;
  int	ar;
  
  farg1 = deref (ARGPTR(1), bp);
  farg2 = deref (ARGPTR(1)+1, bp);
  farg3 = deref (ARGPTR(1)+2, bp);
  
  DEBUG(printf("functor:\n"));
  DEBUG(disp_(stdout,farg1,bp));
  DEBUG(printf("\n"));
  DEBUG(disp_(stdout,farg2,bp));
  DEBUG(printf("\n"));
  DEBUG(disp_(stdout,farg3,bp));
  DEBUG(printf("\n"));

  switch (GETTAG(*farg1)) {
  case T_CONST:
  case T_GTERM:
  case T_NGTERM:
  case T_CRTERM:
    /*
     * first argument is a term
     * then:
     *   unify 2nd argument with the function symbol 
     *   and the 3rd argument with its arity
     */
    /*     GENOBJ(*hp, INDEXTOSYMBOL(*farg1), T_CONST); */
    GENSYMBOL(*hp, GETSYMBOL(*farg1), T_CONST);
    hp++;
    DEBUG(printf("term-before UN_CONST: GETSYMBOL(*farg1)=%lx\n",GETSYMBOL(*farg1)));
    UN_CONST((hp-1), (environ *)NULL, farg2, bp);
    DEBUG(printf("term-after UN_CONST\n"));
    DEBUG(printf("term-GETARITY(*farg1)=%lx\n",GETARITY(*farg1)));
    GENOBJ(*hp, GETARITY(*farg1), T_CONST);
    hp++;
    UN_CONST((hp-1), (environ *)NULL, farg3, bp);
    break;
    
  case T_FVAR:
    /*
     * first argument is a variable
     * in that case, the second argument must be
     * a symbol
     * if GETARITY(2nd-arg) == 0:
     *	i.e., it has been used as a constant only,
     *  any arity is possible.
     *	In that case, if the 3rd argument is a number,
     *  generate a term f(X1,...,Xn), and set the arity
     *  in the symbol table. 
     *  unify arity with 3rd argument, and, if OK,
     *  generate a term f(X1,...,Xn)
     */
    
    DEBUG(printf("var-before ISSYMBOL\n"));
    if (!(ISCONSTANT(*farg2) || ISCOMPLEX(*farg2))){
      DEBUG(printf("var-farg2 is not a symbol (! ISSYMBOL)\n"));
      sam_error("functor: 2nd argument type mismatch", farg2, 0);
      return failure;
    }
    if (!ISNUMBER(*farg3)){
      /*
       * 3rd argument must be a number
       */
      sam_error("functor: 3rd argument type mismatch", farg3, 0);
      return failure;
    }
    
    arity = GETARITY(*farg2);
    ar=GETNUMBER(*farg3);
    if (!ar){
      /*
       * just use the symbol as a constant
       */
      UN_CONST(farg2, bp, farg1, bp);
      break;
    }
    if (!arity){
      /*
       * const used as a functor for the first time
       */
      symbtab[GETSYMBOLINDEX(*farg2)].arity = ar;
      DEBUG(printf("var-arity unknown: new arity=%d\n",ar));
      arity=ar;
    }
    if (arity != ar){
      sam_error("functor: argument arity mismatch", farg2, 0);
      return failure;
    }
    tmp = new_crterm(*farg2,arity);
    UN_FVAR(farg1, bp, tmp, NULL);
    break;
    
  default:
    sam_error("functor: first argument type mismatch", farg1, 0);
    return failure;
  }

  DEBUG(printf("end of functor-success\n"));
  pc +=INSTR_LENGTH;
  return success;
}

/************************************************
 * static new_crterm
 *
 * generate a new term with T_CRTERM or T_CONST
 *  and new
 * variables on the heap, given the functor
 * and the arity
 * The begin of the term is returned
 *
 * if the arity is 0, only a constant (T_CONST)
 * is generated
 ************************************************/
static WORD *new_crterm(funct,arity)
WORD funct;
int arity;
{
  WORD *tmp;
  
  DEBUG(printf("new_crterm: arity=%d\n",arity));
  tmp=hp;
  
  if (!arity){
    GENSYMBOL(*hp,GETSYMBOL(funct),T_CONST);
    hp++;
    return tmp;
  }
  
  DEBUG(printf("new_crterm: IN-to_sy=%lx\n",INDEXTOSYMBOL(GETSYMBOL(funct))));
  GENSYMBOL(*hp,GETSYMBOL(funct),T_CRTERM);
  hp++;
  while(arity--){
#ifdef CONSTRAINTS
    GENOBJ(*hp, (WORD_cast)NULL, T_FVAR);
    hp++;
#else
    GENOBJ(*hp, 0, T_FVAR);
    hp++;
#endif
  }
  GENOBJ(*hp, (WORD_cast)NULL, T_EOSTR);
  hp++;
  return tmp;
}
