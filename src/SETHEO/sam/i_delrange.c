/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_delrange.c
 * VERSION:		@(#)i_delrange.c	8.1 05 Jan 1996
 * DATE:		30.9.94
 * AUTHOR:		j. Schumann
 * NAME OF FILE:
 * DESCR:
 *		delrange(I,J):
 *		delete all lemmata in the index with
 *		annotation I <= n <= J
 *		and adjust number of lemmata
 * MOD:
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "unitlemma.h"
#include "symbtab.h"
#include "disp.h"
/**/
#include "deref.h"
#include "errors.h"
#include "unitlemma.h"

#define INSTR_LENGTH		2

#define DEBUG(X) 

/******************************************************
 * delete all lemmata in the index with a given
 * range of annotation numbers
 *
 ******************************************************/
instr_result i_delrange()
{
#ifdef UNITLEMMA

  path_QUERY	root;
  POINTER		ptr;
  unit_lemma_storage_ctrl_block *lemma;
  WORD *lb, *ub;

  /*
   * get the lower and upper bounds
   */
  lb = deref (ARGPTR(1), bp);
  ub = deref (ARGPTR(1)+1, bp);

  if (!ISNUMBER(*lb) || !ISNUMBER(*ub) ||
      (GETNUMBER(*lb) > GETNUMBER(*ub))){
    sam_error("delrange: arguments must be numbers and arg1 <= arg2",NULL,0);
    pc += INSTR_LENGTH;
    return error;
  }

  DEBUG(printf("lb=%d ub=%d\n",GETNUMBER(*lb),GETNUMBER(*ub)));
  
  root = path_QueryCreate(unitlemma_index,ALL_MIN,NULL);
  
  while ( (ptr = path_QueryApply(root)) ) {
    lemma = (unit_lemma_storage_ctrl_block *)ptr;
    /*
     * if the lemma is not marked "deleted"...
     */
    if ((lemma->status == lemma_deleted) ||
	(lemma->status == lemma_deleted_not_used)){
      continue;
    }
    /*
     * are we in the range to delete ?
     */
    DEBUG(printf("considered: "));
    DEBUG(print_compiled_fact(stdout, lemma->ulemma));
    DEBUG(printf("annot: %d\n",lemma->data));
    if ((GETNUMBER(lemma->data) >= GETNUMBER(*lb)) && (GETNUMBER(lemma->data) <= GETNUMBER(*ub))){
      /*
       * yes, we can delete the lemma
       * or, at least mark it deleted
       * and update the number of entries
       */
      DEBUG(printf("deleted: "));
      DEBUG(print_compiled_fact(stdout, lemma->ulemma));
      DEBUG(printf("\n"));
      
      unitlemma_nr_valid--;
      
      if (lemma->status == lemma_used){
	/*
	 * mark the lemma as deleted
	 * and remember its choice_point
	 * if it is smaller than the previous ones
	 */
	lemma->status = lemma_deleted;
      }
      if (lemma->status == lemma_valid)
	lemma->status = lemma_deleted_not_used;
    }
    
  }
  
  pc += INSTR_LENGTH;
  return success;
#else
  return error;
#endif
}
