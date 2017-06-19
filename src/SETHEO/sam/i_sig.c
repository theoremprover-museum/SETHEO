/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: i_sig.c                                      */
/* VERSION:                                           */
/* DATE:                                              */
/* AUTHOR:                                            */
/* NAME OF FILE:                                      */
/* DESCR:                                             */
/* MOD:                                               */
/* BUGS:                                              */
/******************************************************/

#include <stdio.h>
#include "tags.h"
#include "machine.h"
#include "symbtab.h"
#include "deref.h"
#include "errors.h"
#include "bounds.h"

#define INSTR_LENGTH		2


instr_result i_sig()
{
    WORD            *list,*element;
    int             index;
    
    list = deref(ARGV(0),bp);
    
    if (!(    ISSYMBOL(*list)  
	  &&  (    GETSYMBOLINDEX(*list) == LISTFUNCT  
	       ||  GETSYMBOLINDEX(*list) == EMPTYLIST)))
	sam_error("sig: Argument of sig must be a list.",list,3);
    
    /******************************************************************/
    /* Process elements of list:                                      */
    /******************************************************************/
    while (GETSYMBOLINDEX(*list) == LISTFUNCT) { 
	
	element = deref(list+1,bp);
	
	if (ISSYMBOL(*element)) {
	    index = GETSYMBOLINDEX(*element);
	    if (sig_occurrence[index]) {
		if (   GETSYMBOLINDEX(bp->dyn_link->lcbptr->ps_symb) 
		    != DEFAULT_QUERY)
		    sigcount += sig_occurrence[index];
		trail_int(&(sig_occurrence[index]));
		sig_occurrence[index] = 0;
	    }
	}
	else
	    sam_error("sig: Elements of list must be symbols.",element,3);
	
	list = deref(list+2,bp);
	if (!(    ISSYMBOL(*list)  
	      &&  (    GETSYMBOLINDEX(*list) == LISTFUNCT  
		   ||  GETSYMBOLINDEX(*list) == EMPTYLIST)))
	    sam_error("sig: Internal error in list handling.",list,3);
    }
    
    /******************************************************************/
    /* Check for signature bound fail:                                */
    /******************************************************************/
    if (sig_bound  &&  sig_bound_fail(sigcount))
	return failure;
    if (sigcount > intmd_sig)
	intmd_sig = sigcount;
    
    pc +=INSTR_LENGTH;
    return success;
}
