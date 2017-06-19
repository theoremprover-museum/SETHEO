/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_size.c
 * VERSION:
 * DATE: 28.12.92
 * AUTHOR: Christoph Goller
 * NAME OF FILE:
 * DESCR: calculate the size of an expression
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

#define COPY_CONST

#include "unification.h"




/************************************************************/
static int      get_size (ga)
    WORD           *ga;
{
    WORD           *gaa;
    int             s;

    gaa = deref (ga, bp);
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


instr_result i_size()
{
   WORD *ga;
   WORD s;

   ga = ARGV(0);
   GENNUMBER (s, get_size(ga));

   /* unify the calculated size with the second argument */
   
   ga = deref(ARGV(1),bp);

   UN_CONST((&s), bp, ga, bp);

   pc += INSTR_LENGTH;
   return success;
}



