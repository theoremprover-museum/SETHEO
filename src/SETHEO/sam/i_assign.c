/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_assign.c
 * VERSION:
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *	11.3.93	Mueller,Goller neue Constraints, Statistics
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
/**/
#include "deref.h"
#include "errors.h"
#include "unifyt.h"

#define INSTR_LENGTH		3


instr_result i_assign()
{
  register WORD *ga, *ha;

	/* assign farg2 to global variable farg */
	ha = heap + GETVAL(ARG(1));
	/* trail old value of global variable   */
	trail_var(ha);

	ga = deref(ARGPTR(2), bp);
	switch (GETTAG(*ga)){
	case T_CONST:
		*ha = *ga;
		break;
	case T_GTERM:
		GENOBJ(*ha,(WORD_cast)ga,T_BVAR);
		break;
	case T_FVAR:
		/* generate new variable on heap	*/
		GENOBJ(*hp,0,T_FVAR);
		/* let old var. point to it		*/
		trail_var(ga);
		GENOBJ(*ga,(WORD_cast)hp,T_BVAR);
		/* let global variable point to it	*/
		GENOBJ(*ha,(WORD_cast)hp,T_BVAR);
                hp++;
		break;
	case T_CRTERM:
		/* term is already on heap, nothing	*/
		/* has to be done			*/
		GENOBJ(*ha,(WORD_cast)ga,T_BVAR);
		break;
	case T_NGTERM:
  		/* assign term to variable */
                /* No occurs check necessary 		*/
                ga = copy_struct(ga,bp,NULL);
                GENOBJ(*ha,(WORD_cast)ga ,T_BVAR);
                break;

	case T_GLOBL:
		GENOBJ(*hp,0,T_FVAR);
		trail_var(ga);
		GENOBJ(*ga,(WORD_cast)hp,T_BVAR);
		GENOBJ(*ha,(WORD_cast)hp,T_BVAR);
                hp++;
		break;
	default:
		sam_error("Illegal tag in assign",ga,3);
		return error;
		}
	pc +=INSTR_LENGTH;
	return success;
}
