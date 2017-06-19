/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: i_tdepth3.c                                  */
/* VERSION:                                           */
/* DATE:                                              */
/* AUTHOR:                                            */
/* NAME OF FILE:                                      */
/* DESCR:                                             */
/*	termdepth which counts only a certain function*/
/*	symbol                                        */
/*	tdepth3(term,fu-symb,depth)                   */
/* MOD:                                               */
/* BUGS:                                              */
/******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
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



/************************************************************************/
static int      get_tdepth3 (ga,symbol)
    WORD           *ga;
    WORD	     symbol;
{
    WORD           *gaa;
    int             s,
                    s1;
    int found = 0;

    gaa = deref (ga, bp);
    switch (GETTAG (*gaa)) {
      case T_CRTERM:
      case T_GTERM:
      case T_NGTERM:
	    if (GETSYMBOL(*gaa) == GETSYMBOL(symbol)){
		found = 1;
		}
	    gaa++;
	    s = 0;
	    while (GETTAG (*gaa) != T_EOSTR) {
	       if ((s1 = get_tdepth3 (gaa,symbol)) > s) {
		      s = s1;
	       }
	       gaa++;
	    }
	    return (found)?(s + 1):s;
	    break;
      default:
	    return 0;
    }
}


instr_result i_tdepth3()
{
    WORD           *ga,*ga2;
    WORD             s;

    ga = ARGV (0);
    ga2 = deref(ARGV(1),bp);

/*
	disp_(stdout,ga,bp);
	disp_(stdout,ga2,bp);
*/

    GENNUMBER(s,get_tdepth3(ga,*ga2));

/*
	disp_(stdout,&s,bp);
*/

    /* unify the calculated depth with the second argument */

    ga = deref(ARGV(2),bp);

    UN_CONST((&s), bp, ga, bp);

    pc += INSTR_LENGTH;
    return success;

}

