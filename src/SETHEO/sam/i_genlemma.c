/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_genlemma.c
 * VERSION:		@(#)i_genlemma.c	8.1 05 Jan 1996
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *	24.1.94		jsc	only if UNITLEMMA
 *	17.5.94		jsc	with matching
 *	7.6.94		new layout
 *	18.6.94		with additional data (number)
 *	7.11.94		jsc	proof-tree for unitlemmata
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
#include "disp.h"
#include "unitlemma.h"


#define INSTR_LENGTH		2

/* #define DEBUG(X)	X; fflush(stdout); */
#define DEBUG(X)	
#define VERBOSE(X)	X
#define DEBUG_DISP_MORE_GEN

/******************************************************
 * genlemma/2
 *
 * consider the generation of a lemma
 * for the current head, if the 1st argument
 * is a number not equal to 0
 *
 * annotate the generated lemma with the given 2nd argument
 * (in current version, a number only)
 *
 * succeeds always
 ******************************************************/
instr_result i_genlemma()
{
#ifdef UNITLEMMA

  WORD 	*ga;
  WORD	*annotate;
  int 	arity;
  unit_lemma_storage_ctrl_block *lemma;
  int		i;
  
  UL_STAT(ul_stat.genlemma_entered++);

  /*
   * is the given argument <> 0 ?
   * if no, succeed without
   * generating a lemma
   */
  ga = deref (ARGPTR(1), bp);
  
  if (ISNUMBER(*ga) && (GETVAL(*ga) == 0)) {
    /* do not generate a lemma */
    UL_STAT(ul_stat.genlemma_disabled++);
    pc +=INSTR_LENGTH;
    return success;
  }
  
  /* is the 2nd argument a number ?*/
  annotate = deref (ARGPTR(1)+1, bp);
  
  if (!ISNUMBER(*annotate)){
    /* do not generate a lemma */
    sam_error("genlemma: 2nd argument must be a number",annotate,0);
    pc +=INSTR_LENGTH;
    return error;
  }
  
  /* do we have a unit-lemma ? */
  if(!head_is_unit_lemma()){
    /* no, it is no unit lemma */
    /* do not generate a lemma */
    UL_STAT(ul_stat.genlemma_no_unit_lemma++);
    pc +=INSTR_LENGTH;
    return success;
  }
  
  /* temporarily generate the ''predicate term'' on the stack */
  ga = sp;
  
  /* push the predicate symbol and sign */
  GENSYMBOL(*sp, GETSYMBOL(bp->lcbptr->ps_symb), T_CRTERM);
  sp++;
  
  /* get the arity */
  arity=GETARITY(bp->lcbptr->ps_symb);
  
  DEBUG(printf("genlemma: arity = %d\n",arity));
  for (i=0; i<arity;i++){
    GENOBJ(*sp,(WORD_cast)deref(code + GETVAL(bp->lcbptr->argv) +i,bp),T_BVAR);
    sp++;
  }
  GENOBJ(*sp,0,T_EOSTR);
  sp++;
  
  DEBUG(disp_(stdout,ga,bp));
  
  /*are there already more general terms in the path-index ? */
  if (more_general_in_index(ga)){
    /*
     * yes, in that case, we do not enter the
     * lemma
     */
    sp=ga;
    pc +=INSTR_LENGTH;
    return success;
  }
  
  /* else: enter the new lemma */
  
  /* first, however, we must delete all instances of the
   * new lemma */
  delete_instances_in_index(ga);
  
  /*===================================================================
   * Now: all checks are finished, enter the lemma
   * we must compile the fact and create an stored_lemma_control block */

  unitlemma_nr_valid++;
  
  lemma = (unit_lemma_storage_ctrl_block *)
    MALLOC(sizeof(unit_lemma_storage_ctrl_block));
  
  lemma->status = lemma_valid;
  lemma->chp			= NULL;
  lemma->ulemma		= 
    compile_to_fact(bp->lcbptr->ps_symb,
		    code + GETVAL(bp->lcbptr->argv));
  
  /* an store the 2nd argument in the lemma-control-block */
  lemma->data = *annotate;
  
  DEBUG(printf("genlemma: compiled_fact:\n"));
  DEBUG(print_compiled_fact(stdout,lemma->ulemma));
  /*
    DEBUG(printf("genlemma: compiled_fact (disassembled):\n"));
    DEBUG(disassemble_compiled_fact(stdout,lemma->ulemma));
    */
  
  /* now, we enter the lemma into the path-index */
  DEBUG(printf("genlemma: entering...\n"));
  path_EntryCreate(unitlemma_index,lemma,ga);
  UL_STAT(ul_stat.gl_lemma_stored++);
  DEBUG(printf("genlemma: lemma entered.\n"));
  
  /* do we print the proof tree for that lemma into the tree file? */
  if (unitlemma_tree){
    ul_print_tree(ftree,lemma,bp);
  }
  
  /* clean up the temporary term */
  sp = ga;
  
  /* we succeed */
  DEBUG(printf("genlemma: returning with success...\n"));
  pc +=INSTR_LENGTH;
  return success;
  
#else
  
  fprintf(stderr,"sorry, ``genlemma'' is not activated\n");
  pc +=INSTR_LENGTH;
  return error;
  
#endif
}
