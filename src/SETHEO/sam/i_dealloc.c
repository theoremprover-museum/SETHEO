/*********************************************************/
/*    S E T H E O                                        */
/*                                                       */
/* FILE: i_dealloc.c                                     */
/* VERSION:	@(#)i_dealloc.c	8.1 05 Jan 1996          */
/* DATE:                                                 */
/* AUTHOR:                                               */
/* NAME OF FILE:                                         */
/* DESCR:                                                */
/* MOD:                                                  */
/*	12.02.88                                         */
/*	25.4.89  statistics                              */
/*	17.7.89  nalloc                                  */
/*      26.6.91 small changes on bound-cks   by Goller   */ 
/*      13.01.92 REG_CONSTR added by bschorer            */
/*      11.02.92 CONSTRAINTS added by bschorer           */
/*      26.2.92  Local-Inf-Bounding added by bschorer    */
/*	13.1.95	jsc	depth incremented by lcb->ddepth */
/* BUGS:                                                 */
/*********************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
/**/
#include "i_ndealloc.h"
#include "weighted_depth.h"

instr_result i_dealloc()
{
    int           ddepth;
    int           dinf;

    /* deallocate environment */


    ddepth = GETNUMBER(bp->lcbptr->ddepth);
    dinf = GETNUMBER(bp->lcbptr->dinf);

    /***********************************************/
    /* reset local inference bound:                */
    /***********************************************/
    local_inf_bound += dinf;

    /***********************************************/
    /* reset depth:                                */
    /* (Dependence on inf_lookahead leads to       */
    /*  buggyness for wd4!=0 combined with delay.) */
    /***********************************************/
    if (wd_bound) {
        depth =   bp->depth
	        + delta_depth(dinf + bp->chp->inf_lookahead, ddepth);
    }
    else
        depth += ddepth;


    /* call the ndalloc	*/
     return i_ndealloc();
}
