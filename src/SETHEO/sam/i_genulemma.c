/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_genulemma.c
 * VERSION:		@(#)i_genulemma.c	8.1 05 Jan 1996
 * DATE:
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:			genulemma	lcbptr, 01
 * MOD:
 *	18.6.94		with additional data (number)
 *	7.11.94		jsc	proof-tree for lemmata
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "path/path.h"
#include "unitlemma.h"
#include "symbtab.h"
#include "disp.h"
/**/
#include "deref.h"
#include "disp.h"
#include "errors.h"
#include "genlemma.h"
#include "unitlemma.h"

#define INSTR_LENGTH		3

/* #define DEBUG(X)	X; fflush(stdout); */
#define DEBUG(X)	
#define DEBUG1(X)	
#define VERBOSE(X)	X
#define DEBUG_DISP_MORE_GEN

/******************************************************
 * genulemma/3
 *
 * consider the generation of a lemma
 * for the given subgoal (i.e., its lcb), if the 2nd argument
 * is a number not equal to 0
 *
 * annotate teh generated lemma with the 3rd argument
 * (a number currently)
 *
 * Note: No check is performed, if the given lemma
 *		is unit or not
 * Note: The predicate symbol is NEGATED
 *		e.g., H <- p(X),genulemma(2,1).
 *		generates p(X).
 ******************************************************/
instr_result i_genulemma()
{
#ifdef UNITLEMMA
  
  WORD 	*ga;
  literal_ctrl_block	*lcbptr;
  int 	arity;
  unit_lemma_storage_ctrl_block *lemma;
  int		i;
  WORD	*annotate;
  WORD    *lbp;
  
  UL_STAT(ul_stat.genlemma_entered++);
  
  /*
   * is the given 2nd argument <> 0 ?
   * if no, succeed without
   * generating a lemma
   * Note:
   *	- argument-vector pointer is in *(pc+2) !
   */
  ga = deref (ARGPTR(2)+1, bp);
  
  if (ISNUMBER(*ga) && (GETVAL(*ga) == 0)) {
    /* do not generate a lemma */
    UL_STAT(ul_stat.genlemma_disabled++);
    pc +=INSTR_LENGTH;
    return success;
  }
  
  
  /* is the 3nd argument a number ?*/
  annotate = deref (ARGPTR(2) +2 , bp);
  
  if (!ISNUMBER(*annotate)){
    /* do not generate a lemma */
    sam_error("genulemma: 3nd argument must be a number",annotate,0);
    pc +=INSTR_LENGTH;
    return error;
  }
  
  
  /* get the literal control block for the lemma to generate
   */
  lcbptr = (literal_ctrl_block *)(ARGPTR(1)); 
  
  /* temporarily generate the ''predicate term'' on the stack */
  ga = sp;
  
  /* push the predicate symbol and NEGATED sign */
  GENSYMBOL(*sp, GETSYMBOL(lcbptr->ps_symb), T_CRTERM); /* write symbol */
  CHANGE_THE_SIGN(*sp);                                 /* negate it */
  sp++;                                                 /* and push */
  
  /* get the arity */
  arity=GETARITY(lcbptr->ps_symb);
  
  DEBUG(printf("genulemma: arity = %d\n",arity));
  for (i=0; i<arity;i++){
    GENOBJ(*sp,(WORD_cast)deref(code + GETVAL(lcbptr->argv) +i,bp),T_BVAR);
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
  {
    WORD negated_ps_symb = lcbptr->ps_symb;
    CHANGE_THE_SIGN(negated_ps_symb);

    lemma->ulemma		= 
    compile_to_fact(negated_ps_symb,code + GETVAL(lcbptr->argv));
  }
  
  /* annotate the lemma */
  lemma->data = (*annotate);
  DEBUG1(printf("genulemma: annotate with %lx (&%lx)\n",*annotate,annotate));
  
  DEBUG(printf("genulemma: compiled_fact:\n"));
  DEBUG(print_compiled_fact(stdout,lemma->ulemma));
  /*
    DEBUG(printf("genulemma: compiled_fact (disassembled):\n"));
    DEBUG(disassemble_compiled_fact(stdout,lemma->ulemma));
    */
  
  /* now, we enter the lemma into the path-index */
  DEBUG(printf("genulemma: entering...\n"));
  path_EntryCreate(unitlemma_index,lemma,ga);
  UL_STAT(ul_stat.gl_lemma_stored++);
  DEBUG(printf("genulemma: lemma entered.\n"));
  
  /* do we need to print the lemma's proof-tree ?*/
  if (unitlemma_tree){
    lbp =  (WORD *)bp + SIZE_IN_WORDS(environ) 
      + GETNUMBER(bp->lcbptr->nr_vars) + GETVAL(ARG(1));
    ul_print_tree(ftree,lemma, (environ*) GETPTR(*lbp));
  }
  
  /* clean up the temporary term */
  sp = ga;
  
  /* we succeed */
  DEBUG(printf("genulemma: returning with success...\n"));
  pc +=INSTR_LENGTH;
  return success;
  
#else
  
  fprintf(stderr,"sorry, ``genulemma'' is not activated\n");
  pc +=INSTR_LENGTH;
  return error;
  
#endif
}
