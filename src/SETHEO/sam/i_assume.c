/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: i_assume.c                                   */
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
/**/
#include "deref.h"

#define INSTR_LENGTH		1


instr_result i_assume()
{

    environ *pdp;

    pdp = bp->dyn_link;


    trail_ptr((void**)&(bp->path_link));
    trail_ptr((void**)&(pdp->path_link));
    bp->path_link = pdp->path_link;
    pdp->path_link = bp;

    trail_var(&(bp->flags));
    /*    bp->flags |= FOLDED_UP; */
    GENOBJ(bp->flags,GETVAL(bp->flags)|FOLDED_UP,T_EMPTY);


    pc +=INSTR_LENGTH;
    return success;
}
