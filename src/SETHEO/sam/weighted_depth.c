/************************************************************/
/*    S E T H E O                                           */
/*                                                          */
/* FILE: weighted_depth.c                                   */
/* VERSION:                                                 */
/* DATE:                                                    */
/* AUTHOR: Ortrun Ibens                                     */
/* NAME OF FILE:                                            */
/* DESCR: routines for handling of the weighted depth bound */
/* MOD:                                                     */
/* MOD:                                                     */
/* BUGS:                                                    */
/************************************************************/


#include <stdio.h>    
#include <math.h>
#include "tags.h"
#include "machine.h" 

/**/
#include "weighted_depth.h"


int  free_depth(actual_depth,delta_inf)
int    actual_depth;
int    delta_inf;
{
    float   f;

    if (eq_by_ste_mod)
        f =   (float) actual_depth 
	    +   ((float)wd1 - (float)wd2 * (float)delta_inf)
              / (float)100 
            + 0.5;
    else
        f =   (float) actual_depth 
	    +   ((float)wd1 - (float)wd2 * (float)sqrt((double)(2*delta_inf)))
              / (float)100 
            + 0.5;

    return((int) f);
}



int  weighted_depth(inferences)
int    inferences;
{
    int     d,f,ic;
    float   new_depth;

    d   = (float) goal_bp->depth;
    f   = (float) goal_bp->depth_free;
    ic  = (float) goal_bp->infcount;

    new_depth =   (float) f 
	        +   (float) ((d - f) * 100)
		  / (float) (100 + MAXIMUM(0,(inferences - ic) * wd3))
                + 0.5;

    return((int) new_depth);
}



int  delta_depth(delta_inf,delta_depth)
int    delta_inf;
int    delta_depth;
{
    float   delta;
    
    if (wd4_flag)
	delta =     (float) (100 * delta_depth + wd4 * delta_inf)
	          / (float) (100 + wd4)
	        + 0.5;
    else
	delta =   (float) (wd4 * delta_inf) / (float) 100
	        + (float) delta_depth
		+ 0.5;

    return((int) delta);
}
