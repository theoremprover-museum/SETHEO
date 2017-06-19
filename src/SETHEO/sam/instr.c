/******************************************************
 *    S E T H E O                                     *
 *                                                    *
 * FILE: instr.c
 * VERSION:
 * DATE: 23.12.87
 *	8.2.88
 * 	10.3.88
 * AUTHOR: J. Schumann
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *	27.7. check for end of proof-tree
 *	31.10. remove of symbtab.h,
 *	20.2.89 gen-purp built ins
 *	10.5.89 error
 *	06.6.89 partheo
 *	19.6.89      "
 *	31.7.89 symbtab.h
 *      26.6.91 default-query-ck   by Goller
 *      3.7.91 trail & heap-overflow added by Goller
 *      23.7.91 pt_area overflow added by Goller
 * BUGS:
 ******************************************************/
/* with new commands orbranch and new fail */
/* depth-binding only in alloc statements, not in facts */

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
/**/
#include "instr.h"
#include "debug.h"
#include "errors.h"
#include "i_fail.h"
#ifdef STATPART
#include "sps.h"
#endif

/***************************************************************
 * DEBUGGING MACROS
 ***************************************************************/
#define CODE_CONTROL		0	/* check bounds if set */
#define FIRST_TRACE		0
#define SECOND_TRACE            0       /* by Lutz Albers */

extern unsigned long debug_level;

/***************************************************************
 * instr_cycle
 ***************************************************************
 * start the setheo-machine
 * returns: SUCCESS etc
 ***************************************************************/
instr_result instr_cycle ()
{
  register int res;

/*****************************************************/
/*         instruction cycle                         */ 
/*****************************************************/

#ifdef STATPART
if( recomp ){
    init_inf_step_area();
    recomp_mode = TRUE;
    }
#endif

    for (;;) {

#	ifdef STATISTICS1
		cnt++;
#	endif

/***************************************/
/* overflow tests for storage areas    */
/***************************************/
	if (sp > up_stack - 500) {
	    sam_error("stack overflow", NULL, 2);
	    return mem_overflow;
	}

#if (CODE_CONTROL == 1)
       /* I have added these 2 additional test; they may be omitted, cause 
          they are rather costly        Goller */

	If (hp > hplast) {
	    sam_error("heap overflow", NULL, 2);
	    return mem_overflow;
	}
	if (tr > trlast) {
	    sam_error("trail overflow", NULL, 2);
	    return mem_overflow;
	}
#ifdef REPROOF_MODE
	if (pt_ptr > pt_area_last) {
	    sam_error("proof_tree_area overflow", NULL, 2);
	    return mem_overflow;
	}
#endif
#endif


#if (CODE_CONTROL == 1)
	if ((GETVAL(*pc) < 0) || (GETVAL(*pc) >= nr_instr)){
		sam_error("illegal instruction",pc,3);
		return error;
		}
#endif
	
#if (FIRST_TRACE == 1)
	dispreg();
	fprintf(stderr,"%lx: %s	",pc-code,instr_table[GETVAL(*pc)].name);
	
	for (res=1; res< instr_table[GETVAL(*pc)].length;res++){
		fprintf(stderr,"%lx	",*(pc+res));
		}
	fprintf(stderr,"\n");
#endif

	/* execute instruction	*/
#if (SECOND_TRACE == 1)
        if (debug_level & 0x1)
          fprintf(stderr,
		  " exec %s (%d) => ",
		  instr_table[GETVAL(*pc)].name,
		  instr_table[GETVAL(*pc)].length);
#endif        
	if ((res = (*instr_table[GETVAL(*pc)].instruction)()) & stop_cycle){
		/* we break the instruction cycle and 	*/
		/* return the cause of the break	*/
		return res & ~stop_cycle;
	        }
#if (SECOND_TRACE == 1)
        if (debug_level & 0x1)
          fprintf(stderr,"%d\n",res);
#endif        

	if (res == failure){
#if (FIRST_TRACE == 1)
	fprintf(stderr,"instruction returned ``failure''\n");
#endif
	if ((res = i_fail()) & stop_cycle){
		/* we have to break the stuff */
		return res & ~stop_cycle;
		}
	}

    }	/* for */
}
