/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_glod.c
 * VERSION:
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
/**/
#include "deref.h"
#include "errors.h"


#define INSTR_LENGTH		2


instr_result i_glod()
{
    register WORD *ga;

    ga = ref_head(heap+GETVAL(ARG(1)));

    switch (GETTAG(*ga)){

        case T_CONST:
	    if (CONSTCELLISSYMBOL(*ga)){
	        sam_error("load-global-number: no number 0 taken",NULL,0);
	        GENOBJ(*sp,0,T_CONST);
            }
	    else
	        *sp = *ga;
            break;

    	default:
	    sam_error("load-global-number: no number 0 taken",NULL,0);
		        GENOBJ(*sp,0,T_CONST);
    }

    sp++;
    pc +=INSTR_LENGTH;
    return success;
}
