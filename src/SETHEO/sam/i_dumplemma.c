/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_dumplemma.c
 * VERSION:		@(#)i_dumplemma.c	8.1
 * DATE:		17.9.93
 * AUTHOR:		J. Schumann
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *	24.1.94		jsc	only if UNITLEMMA
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "unitlemma.h"
#include "symbtab.h"
#include "disp.h"
/**/
#include "unitlemma.h"

#define INSTR_LENGTH		1


/******************************************************
 * print all lemmata in the lemma store
 * in a lop-like notation
 *
 ******************************************************/
instr_result i_dumplemma()
{
#ifdef UNITLEMMA
  
  path_QUERY	root;
  POINTER		ptr;
  unit_lemma_storage_ctrl_block *lemma;
  
  root = path_QueryCreate(unitlemma_index,ALL_MIN,NULL);
  
  while ( (ptr = path_QueryApply(root)) ) {
    lemma = (unit_lemma_storage_ctrl_block *)ptr;
    /* print the lemma */
    if ((lemma->status == lemma_deleted) ||
	(lemma->status == lemma_deleted_not_used)){
      fprintf(outputfile,"# ");
    }
    print_compiled_fact(outputfile, lemma->ulemma);
    
  }
  
  pc += INSTR_LENGTH;
  return success;
#else
  return error;
#endif
}
