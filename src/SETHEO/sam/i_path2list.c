/****************************************************************/
/*    S E T H E O                                               */
/*                                                              */
/* FILE: i_path2list.c                                          */
/* VERSION:                                                     */
/* DATE:                                                        */
/* AUTHOR:                                                      */
/* NAME OF FILE:                                                */
/* DESCR:                                                       */
/*		path2list(X) with X must be an unbound variable */
/* MOD:                                                         */
/* BUGS:                                                        */
/****************************************************************/

#include <stdio.h>
#include "tags.h"
#include "machine.h"
#include "symbtab.h"
/**/
#include "deref.h"
#include "errors.h"
#include "disp.h"


#define FAIL_U {sam_error("path2list:internal error",0,0); return error;}

#ifdef STATISTICS1
#define UNIF_FAIL(x) {unif_fails++;FAIL_U}
#else
#define UNIF_FAIL(x) FAIL_U
#endif

#include "unification.h"

/******************************************************/
#define INSTR_LENGTH		2

#define DEBUG(X) 

instr_result i_path2list()
{
    environ *lbp;
    WORD    *cdr, *curr_pred, *ga, *argument;
    int     i;
    int     arity;

    argument = deref(ARGPTR(1),bp);
    if (!ISVAR(*argument)) {
	disp_(stdout,argument,bp);
	sam_error("path2list: argument must be a variable",argument,2);
	return failure;
    }

    lbp = bp;

    /* generate the final [] */
    /*            vvv What does this magic cookie mean ???? */
    GENSYMBOL(*hp, 2 ,T_CONST);
    cdr = hp++;
	
    while (lbp->dyn_link  &&  (lbp->dyn_link)->dyn_link){
	DEBUG(printf("lbp=%lx\n",lbp));

        /* dump current predictate */
	curr_pred = hp;
	DEBUG(printf("arity of predicate=%d\n",GETARITY(lbp->lcbptr->ps_symb)));
	if ((arity = GETARITY(lbp->lcbptr->ps_symb))) {
	    GENSYMBOL(*hp,GETSYMBOL(lbp->lcbptr->ps_symb),T_CRTERM);
            hp++;
	    hp += arity+1;
	    for ( i=0 ; i<arity ; i++ ) {
		ga = deref((code + GETVAL(lbp->lcbptr->argv) + i), lbp);
	        DEBUG(printf("argument %d:\n",i));
	        DEBUG(disp_(stdout,ga,lbp));
	        DEBUG(printf("\n"));
                GENOBJ(*(curr_pred+i+1),0,T_FVAR);
		UN_FVAR_NO_CONSTR((curr_pred+i+1),bp,ga,lbp);
	    }
	    GENOBJ(*(curr_pred+arity+1), 0, T_EOSTR);
	}
	else {
	    GENSYMBOL(*hp,GETSYMBOL(lbp->lcbptr->ps_symb), T_CONST);
            hp++;
	}

	DEBUG(printf("predicate dumped\n"));
	DEBUG(disp_(stdout,curr_pred,lbp));
	DEBUG(printf("\n"));
        /* END dump current predictate */

        /* now, generate the list element */
	GENSYMBOL(*hp,LISTFUNCT,T_CRTERM);
	GENPTR(*(hp+1),curr_pred);
	GENPTR(*(hp+2),cdr);
	GENOBJ(*(hp+3),0,T_EOSTR);
	cdr = hp;
	hp+=4;
	DEBUG(printf("actual list: "));
	DEBUG(disp_(stdout,cdr,lbp));
        DEBUG(printf("\n"));
        /********************************************************/
	lbp=lbp->dyn_link;
    }

    /* assign the list to the variable */
    trail_var(argument);
    GENPTR(*argument,cdr);

    pc +=INSTR_LENGTH;
    return success;
}
