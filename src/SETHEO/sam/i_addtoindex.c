/*****************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_addtoindex.c
 * VERSION:		@(#)i_addtoindex.c	8.1
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:			addtoindex	term
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
#include "genlemma.h"
#include "unitlemma.h"
#include "errors.h"

#define INSTR_LENGTH		2

/* #define DEBUG(X)	X; fflush(stdout); */
#define DEBUG(X)	
#define VERBOSE(X)	X
#define DEBUG_DISP_MORE_GEN

/******************************************************
 * addtoindex/1
 *
 * add the given complex term into the index without check
 * for more general terms.
 * More instantiated terms are deleted
 *
 * always succeeds
 ******************************************************/
instr_result i_addtoindex()
{
#ifdef UNITLEMMA
  
  WORD 	*ga;
  unit_lemma_storage_ctrl_block *lemma;

  UL_STAT(ul_stat.genlemma_entered++);
  
  ga = deref (ARGPTR(1), bp);
  
  if (!ISCOMPLEX(*ga)){
    sam_error("addtoindex: argument must be a complex term:",ga,0);
    pc+=INSTR_LENGTH;
    return error;
  }
  /* first, however, we must delete all instances of the
   * new lemma */
  delete_instances_in_index(ga);
  
  /*===================================================================
   * Now, enter the lemma
   * we must compile the fact and create an stored_lemma_control block */
  
  lemma = (unit_lemma_storage_ctrl_block *)
          MALLOC(sizeof(unit_lemma_storage_ctrl_block));

  lemma->status = lemma_valid;
  lemma->chp			= NULL;
  lemma->ulemma		= 
    compile_to_fact(*ga, (ga+1));

  DEBUG(printf("addtoindex: compiled_fact:\n"));
  DEBUG(print_compiled_fact(stdout,lemma->ulemma));
  /*
    DEBUG(printf("addtoindex: compiled_fact (disassembled):\n"));
    DEBUG(disassemble_compiled_fact(stdout,lemma->ulemma));
    */
  
  /* now, we enter the lemma into the path-index */
  DEBUG(printf("addtoindex: entering...\n"));
  path_EntryCreate(unitlemma_index,lemma,ga);
  UL_STAT(ul_stat.gl_lemma_stored++);
  DEBUG(printf("addtoindex: lemma entered.\n"));
  
  /* we succeed */
  DEBUG(printf("addtoindex: returning with success...\n"));
  pc +=INSTR_LENGTH;
  return success;
  
#else
  
  fprintf(stderr,"sorry, ``addtoindex'' is not activated\n");
  pc +=INSTR_LENGTH;
  return error;
  
#endif
}
