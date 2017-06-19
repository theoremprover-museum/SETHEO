/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_checkindex.c
 * VERSION:		@(#)i_checkindex.c	8.1
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:			checkindex term
 * MOD:
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "path/path.h"
#include "unitlemma.h"
#include "symbtab.h"
#include "disp.h"
/**/
#include "deref.h"
#include "errors.h"
#include "genlemma.h"

#define INSTR_LENGTH		2

/* #define DEBUG(X)	X; fflush(stdout);*/
#define DEBUG(X)	
#define VERBOSE(X)	X
#define DEBUG_DISP_MORE_GEN

/******************************************************
 * checkindex/1
 *
 * check, if for the given complex term, more general
 * ones are already present.
 *
 * Succeeds, if NO ``more general'' terms
 *	exists in the index (i.e.,the condition to enter a lemma)
 * Fails: otherwise
 ******************************************************/
instr_result i_checkindex()
{
#ifdef UNITLEMMA

  WORD 	*ga;

  ga = deref (ARGPTR(1), bp);

  if (!ISCOMPLEX(*ga)){
    sam_error("addtoindex: argument must be a complex term:",ga,0);
    pc+=INSTR_LENGTH;
    return error;
  }

  /*are there already more general terms in the path-index ? */
  if (more_general_in_index(ga)){
    /*
     * yes, in that case, we do not enter the
     * lemma
     */
    return failure;
  }
  
  
  
  /* we succeed */
  DEBUG(printf("checkindex: returning with success...\n"));
  pc +=INSTR_LENGTH;
  return success;
  
#else
  
  fprintf(stderr,"sorry, ``checkindex'' is not activated\n");
  pc +=INSTR_LENGTH;
  return error;
  
#endif
}
