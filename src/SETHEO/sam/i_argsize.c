/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: i_argsize.c */
/* VERSION: */
/* DATE: 28.12.92 */
/* AUTHOR: Christoph Goller */
/* NAME OF FILE: */
/* DESCR: sums up the size of the arguments of a given literal */
/*        -> literalsize */
/*        ( as a quick hack the max of the sizes of the given */
/*           literal and all path literals is calculated ) */
/* MOD: */
/*		3.6.94	johann	new macros */
/* BUGS: */
/******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "disp.h"
#include "codedef.h"
#include "symbtab.h"
#include "machine.h"

#define INSTR_LENGTH		3

static environ   *lbp;
static int  get_size _ANSI_ARGS_((WORD           *ga));
    
instr_result i_argsize()
{
    WORD           *ga;
    int            i,s,max;

/* calculate the size of the given literal (first argument) */

    lbp = bp;
    lcbptr = (literal_ctrl_block *)(ARGPTR(1));
    ga = code + GETVAL(lcbptr->argv);
    i = GETARITY(lcbptr->ps_symb);
    s = 0;
    while( i > 0 ){
      s += get_size (ga); 
      ga++;
      i--;
    }



/* calculate the sizes of all path literals and look for the max */

    for(max = s; lbp != NULL; lbp = lbp->dyn_link){
       lcbptr = lbp->lcbptr;
       ga = code + GETVAL(lcbptr->argv);
    	i = GETARITY(lcbptr->ps_symb);
       s = 0;
       while( i > 0 ){
         s += get_size (ga);
         ga++;
         i--;
       }
       if(s > max){
          max = s;
       } 
    }   
  

/* compare max with second argument; variables are instantiated with max */

    ga = deref(ARGPTR(2) + 1,bp);

    if (GETTAG(*ga) == T_CONST)
        if (GETVAL(*ga) != max)
                return failure;
        else{
                pc += INSTR_LENGTH;
                return success;
	    }
    else if (ISVAR(*ga)){
           trail_var(ga);
           GENOBJ(*ga,max,T_CONST);
           pc += INSTR_LENGTH;
           return success;
         }
         else
           sam_error("argsize: second arg must be const or var",ga,3);

    return success; /* gcc is too stupid ... */
}


/************************************************************/
/* calculate the size of an expression */

static int      get_size (ga)
    WORD           *ga;
{
    WORD           *gaa;
    int             s;

    gaa = deref (ga, lbp);
    switch (GETTAG (*gaa)) {
      case T_CRTERM:
      case T_GTERM:
      case T_NGTERM:
	gaa++;
	s = 0;
	while (GETTAG (*gaa) != T_EOSTR) {
	    s += get_size (gaa);
	    gaa++;
	}
	return s + 1;
	break;
      default:
	return 1;
    }
}
