/******************************************************
 *    S E T H E O                                     *
 *                                                    *
 * FILE: 		uselemma.c
 * VERSION:		@(#)i_uselemma.c	5.3
 * DATE:		16.9.93
 * AUTHOR:		J. Schumann
 * NAME OF FILE:
 * DESCR:
 *			uselemma - now used as a A-inference rule
 *			p(_,_) :- uselemma(TERM)
 *			
 V1.0: The term must be a number, and it is checked against the
 internal number, stored together with the lemma on genlemma
 
 When uselemma is entered, all lemmata which are unifiable with the current
 head are extracted, and, if their condition is met (ARG(1) > lemma.condition,
 V1.0), their code-address are pushed onto the stack. The status of
 the lemma is set to : used.
 After the addresses of all lemmata are pushed onto the stack, the rest
 of the current choice-point list is appended, chp->next_clause is adjusted,
 
 Note:
 this instruction always succeeds
 
 Note: if this built-in is used on the LOP-level, we leave the
 enviroment in the stack (as garbage).
 Possibly, we should use:
 p(_,_) :- precut,cut,...,uselemma(T),fail.
 
 * MOD:
 *	24.1.94		jsc	only if UNITLEMMA
 *	15.6.94		jsc	new setup for uselemma
 *	18.6.94		jsc	use a lemma only, if its annotation
 *					(number) is SMALLER than the given argument
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "path/path.h"
#include "symbtab.h"
#include "unitlemma.h"
#include "disp.h"
/**/
#include "deref.h"
#include "errors.h"
#include "unitlemma.h"

#define INSTR_LENGTH		2


/**************************************************************************
 * definitions --- do not change
 **************************************************************************/

/**************************************************************************
 * declarations
 **************************************************************************/

#define DEBUG(X) 
#define DEBUG1(X) 

/**************************************************************************
 * code
 **************************************************************************/



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ uselemma/1
+
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

instr_result i_uselemma()

{ /*=====================================================================*/
#ifdef UNITLEMMA

  path_QUERY root;
  POINTER path_ptr;
  
  WORD 	*ga;
  WORD	*arg1;
  WORD	*tmp;
  unit_lemma_storage_ctrl_block *lemma;
  int 	arity;
  int		i;
	
	/****************************************************************
	 *
	 ****************************************************************/

	UL_STAT(ul_stat.uselemma_entered++);

	/* 
	 * get the argument for uselemma
	 */
	arg1 = deref(ARGPTR(1),bp);
	
	DEBUG1(printf("arg1=%lx *=%lx\n",arg1,*arg1));
	
	if (!ISNUMBER(*arg1)){
	  /*
	   * in this version, the argument MUST be a number
	   */
	  sam_error("uselemma: argument must be a number",arg1,0);
	  pc += INSTR_LENGTH;
	  return error;
	}

	/*
	 * now, we extract all unifiable lemmata from the 
	 * and obtain a list of possible lemmata
	 */
	

	/* temporarily generate the ''predicate term'' on the stack */
	ga = sp;
	
	
	/* push the predicate symbol and sign */
	GENSYMBOL(*sp, GETSYMBOL(bp->lcbptr->ps_symb), T_CRTERM);
        sp++;
	
	/* get the arity */
	arity=GETARITY(bp->lcbptr->ps_symb);
	
	DEBUG(printf("uselemma: arity = %d\n",arity));
	for (i=0; i<arity;i++){
	  GENOBJ(*sp,(WORD_cast)deref(code + GETVAL(bp->lcbptr->argv) +i,bp),T_BVAR);
          sp++;
	}
	GENOBJ(*sp,0,T_EOSTR);
        sp++;
	
	DEBUG(printf("uselemma:...>>\n"));
	DEBUG(disp_(stdout,ga,bp));
	DEBUG(printf("<<\n"));

	/*
	 * set up the query-tree
	 */
	root = path_QueryCreate(unitlemma_index,UNIFIER,ga);
	
	sp = ga;
	
	if(!(path_ptr= path_QueryApply(root))){
	  /*
	   * no, there are no unifiable lemmas in the store
	   */
	  UL_STAT(ul_stat.ul_no_unifiable++);
	  path_QueryDelete(root);
	  pc += INSTR_LENGTH;
	  return success;
	}
	
	/*
	 * yes, there are unifiable ones
	 */
	DEBUG(printf("uselemma: unifiable lemma(s) present\n"));
	
	/*
	 * now, proceed thru the list
	 * check the conditions, and, if the condition holds
	 * push the lcb-label of the lemma
	 */
	while (path_ptr){
 

	lemma= (unit_lemma_storage_ctrl_block*)(path_ptr);
	DEBUG(print_compiled_fact(stdout,lemma->ulemma));

	/*
	 * is the lemma valid ?
	 */
	if ((lemma->status != lemma_deleted) && 
            (lemma->status != lemma_deleted_not_used)) {
	  /*
	   * yes, it's OK
	   */
	  
	  /* check the condition */
	  DEBUG1(printf("uselemma: ARG1=%lx (%d) > le->data=%lx (%d)\n",*arg1,GETNUMBER((*arg1)),lemma->data,GETNUMBER((lemma->data))));
	  if ((GETNUMBER((lemma->data))) < (GETNUMBER((*arg1)))) {
	    /*
	     * yes, we really can use this lemma
	     *	1. set the status
	     *	2. push the lemma's lcb-address onto the stack
	     *		(code relatative)
	     */
	    UL_STAT(ul_stat.ul_lemma_pushed++);
	    lemma->status = lemma_used;
	    GENOBJ(*sp, (WORD *) (&(lemma->ulemma->lcb)) - code, T_EMPTY);
            sp++;
	    DEBUG(printf("uselemma: &lcb=%lx\n",lemma->ulemma->lcb));
	    DEBUG(printf("uselemma: code-rel=%lx\n",*(sp-1)));
	  }
	}
	
	/*
	 * we have to try the others
	 */
	path_ptr = path_QueryApply(root);
	}
	
	/*
	 * we are done with the lemmata
	 * delete the query-tree
	 */
	path_QueryDelete(root);

	/*
	 * now, copy the rest of the choice_pt->next-clause list onto the
	 * stack (including the trailing 0)
	 */
	DEBUG(printf("uselemma: copy rest of WU-list: "));
	tmp=chp->next_clause;
	while (GETVAL(*tmp)) {
          GENOBJ(*sp, GETVAL(*tmp), T_EMPTY);
          sp++; tmp++;
	  DEBUG(printf("*"));
	}
	GENOBJ(*sp, GETVAL(*tmp), T_EMPTY);
	sp++; tmp++;
	DEBUG(printf("\n"));
	
	/*
	 * set sp and next_clause in the current choice_pt
	 */
	chp->sp = sp;
	chp->next_clause = ga;
	
#endif
	/*
	 * we are done now
	 */
	pc += INSTR_LENGTH;
	return success;
}

/*#######################################################################*/












