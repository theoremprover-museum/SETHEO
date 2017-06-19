/*########################################################################
# Include File:		tags.h
# SCCS-Info:		@(#)unitlemma.c	8.1 05 Jan 1996
# Author:		J. Schumann
# Date:			8.7.93
# Context:		sam
# 
# Contents:		generation of unit-lemmata
#
# Modifications:	
#	(when)		(who)		(what)
#	24.1.94		jsc			only, ifunitlemma is defined
#	06.6.94		jsc			new macros
#	06.6.94		jsc			~sign = +10000
#	7.11.94		jsc			count unit-lemmata
#	9.3.95		jsc		new entries in LCB
#					print_compiled_term
#
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
########################################################################*/

#ifdef UNITLEMMA

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "symbtab.h"
#include "disp.h"
#include "unitlemma.h"
#include "opcodes.h"
#include "defaults.h"
/**/
#include "unitlemma.h"
#include "debug.h"
#include "deref.h"
#include "errors.h"

/***********************************************************************
 * local MACROS
 ***********************************************************************/
#define DEBUG
#define PRT(X) 

/*------------------------------------------------------------------------
  README:
  ------------------------------------------------------------------------*/
WORD *disassemble_instruction();


/***********************************************************************
 * statics
 ***********************************************************************/
static WORD *process_term();
static int get_variable_offset();

static int unit_lemma_cnt = CLNU_LEMMA;

/***********************************************************************
 * declarations
 ***********************************************************************/

int compile_argument();

/***********************************************************************
 * compile_to_fact
 ***********************************************************************/
/*	compile a literal into a dynamically allocated fact
 *	input: 	-predsymb, sign
 *		-argument pointer
 *	output:	pointer to compiled fact or NULL
 */
unit_lemma_ctrl_block  *compile_to_fact(predsymb,argptr)
WORD predsymb;
WORD *argptr;
{
  unit_lemma_ctrl_block  *code_ptr;	/* into this we compile the fact
					 * Note: we actually allocate more space for
					 * that pointer, since we append the
					 * code and argument vector directly afterwards */
	
  WORD *cp;		/* points to start of code */
  WORD *av;		/* points to start of argument vectors */
  int pred_arity;
  int i;


  pred_arity = GETARITY(predsymb);

  /****************************************
   *  allocate space for the code,
   *	i.e. for
   *	- literal control block
   *	- code (nalloc, unif*, dealloc)
   *	- argvector
   *	- internal information
   *  NOTE: no space is allocated yet for complex terms */

  if (!(code_ptr = (unit_lemma_ctrl_block *)MALLOC(FACTSIZE(pred_arity)))){
    /* we could not allocate memory */
    return NULL;
  }


  /****************************************
   * fill this space with information
   */
  code_ptr->term_block_ptr = NULL;			

  /* fill the literal control block      */
  cp = (WORD *)code_ptr + CODE_OFFSET;
  
  GENOBJ(code_ptr->lcb.clauselbl, cp - code,T_EMPTY);	
  /* Note: clauselabels are code-relative */

  GENSYMBOL(code_ptr->lcb.ps_symb,GETSYMBOL(predsymb),T_CONST);
  GENOBJ(code_ptr->lcb.clause_nr, unit_lemma_cnt,T_CONST);
  GENOBJ(code_ptr->lcb.lit_nr, 0,T_CONST);		/* has number 0 */
  GENOBJ(code_ptr->lcb.nr_vars, 0,T_CONST);		/* will be adjusted */
  GENOBJ(code_ptr->lcb.nr_strvars, 0,T_CONST);
  GENOBJ(code_ptr->lcb.nr_sgls, 0,T_CONST);
  GENOBJ(code_ptr->lcb.subgoal_list,0,T_CONST);
  unit_lemma_cnt++;

  av = (WORD *)code_ptr + ARGV_OFFSET(pred_arity);
  
  GENOBJ(code_ptr->lcb.argv,av - code,T_EMPTY);
  /* Note: arg-vectors are code-relative */
		

  /****************************************
   * generate an ``nalloc lcb'' instruction
   */
  GENCODE(*cp,OPCODE_nalloc); 
  cp++;
  GENOBJ (*cp,(WORD*)((&(code_ptr->lcb))) - code, T_EMPTY); 
  cp++;
  /* references are relative to code */

  /****************************************
   * generate the unify-instructions
   * and the argument vectors
   */
  for (i=0; i< pred_arity;i++){
    if(!compile_argument(argptr+i,&cp,&av,code_ptr)){
      /* there was an error */
      /* free(code_ptr);    */
      return NULL;
    }
  }

  /****************************************
   * generate an ``ndealloc'' instruction
   */
  GENCODE(*cp,OPCODE_ndealloc);


/****************************************
 * we are done
 */
  return code_ptr;
}


/***********************************************************************
 * compile_argument
 ***********************************************************************/
/*	compile an argument, i.e., generate the appropriate
 *	unify instruction and the appropriate argument vector.
 *	If a complex term is bound to the argument, compile this term
 *	too.
 *
 *	a temporary variable symbol table is allocated on the stack.
 *	These cells contain the addresses of the resp. variables
 *	input:	- argument pointer
 *		- pointer to pointer to code area where to place the
 *		  unify instruction
 *		- pointer to pointer to code area where to place the
 *		  argument vector
 *		- code_ptr: pointer to fact control block for adjusting
 *		  several data, e.g. nr_vars, term_block_ptr;
 *	output: 0 if not OK
 *		1 otherwise
 *	Side-effects:
 *		*cpp,*avp and certain fields in code_ptr are modified
 */
int compile_argument(argptr,cpp,avp,code_ptr)
WORD *argptr;
WORD **cpp;
WORD **avp;
unit_lemma_ctrl_block *code_ptr;
{
  WORD *ga;
  int offset;
  int ground;
  
  ga=deref(argptr,bp);
  
  switch (GETTAG(*ga)){
    
  case T_CONST:
    /****************************************/
    /* we have a constant*/
    /* therefore, generate:   unconst  const */
    GENCODE(**cpp,OPCODE_unconst);
    (*cpp)++;
    /* note, that the argument of unconst is NOT tagged */
    GENOBJ(**cpp,GETVAL(*ga),T_EMPTY);
    /* generate the appropriate argument vector entry */
    **avp = *ga;
    break;

  case T_FVAR:
    /****************************************/
    /* we have a variable */
    /* get the offset for it */
    offset = get_variable_offset(ga,code_ptr);
    /* now, we have the offset of this variable */
    GENCODE(**cpp,OPCODE_unvar);
    (*cpp)++;
    GENOBJ(**cpp,offset,T_EMPTY);
    /* generate the appropriate argument vector entry */
    GENOBJ(**avp,offset,T_VAR); 
    break;

  case T_GTERM:
    /****************************************/
    /* we have a ground term. */
    /* therefore, there is no need to copy it */
    GENCODE(**cpp,OPCODE_ungterm);
    (*cpp)++;
    GENOBJ(**cpp, (WORD *)(ga) -code,T_EMPTY);
    /* generate the appropriate argument vector entry */
    GENOBJ(**avp,(WORD *)(ga) -code,T_CREF); 
    break;
    
  case T_NGTERM:
  case T_CRTERM:
    /****************************************
     * we have a complex term		*
     * The following steps must be performed:
     * 1) compile the term
     * 2) if we find, that the term is ground,
     *	  generate a ``ungterm termlab'',
     * 3) otherwise, a ``unngterm termlab''
     * 4) ``termlab'' is the code-relative label
     *    of the address where the term resides
     */
    /* now we must copy and process the entire term */
    /* its starting address is in ga afterwards     */
    ga = process_term(ga,&ground,code_ptr);
    GENCODE(**cpp, (ground)?OPCODE_ungterm:OPCODE_unngterm);
    (*cpp)++;
    GENOBJ(**cpp,ga - code,T_EMPTY);
    
    /* and generate the appropriate argument vector entry */
    GENOBJ(**avp,ga - code,T_CREF); 
    break;

  default:
    printf("unitlemma:compile_term: illegal tag: "GETTAGFORMAT"\n",
	   GETTAG(*ga));
    sam_error("unitlemma: illegal tag",ga,3);
    
  }
  (*cpp)++;
  (*avp)++;

  /* we are done successfully */
  return 1;
}

/***********************************************************************
 * free_compiled_fact
 ***********************************************************************/
/*	free the space for the compiled fact
 *	move thru the entire list: fact->term_block_ptr
 *	and free this stuff, then delete fact
 *	Note: all sizes of the blocks are recalculated
 *	Input:	- compiled fact
 *	Output:	-
 */
void free_compiled_fact(fact)
unit_lemma_ctrl_block *fact;
{
  WORD *block_ptr;
  WORD *tbp;
  int size;
  
  block_ptr = fact->term_block_ptr;
  
  /* free the list of term-blocks */
  while (block_ptr){
    size = TERMSIZE(*block_ptr);
    /* get next element in the list */
    tbp = GETPTR(*(block_ptr + GETARITY(*block_ptr)+2));
    /* free that chunk of memory */
    FREE(block_ptr,size);
    block_ptr = tbp;
  }

  /* free the fact itself */
  FREE(fact,FACTSIZE(GETARITY(fact->lcb.ps_symb)));
  
}

#if defined(DEBUG) || defined(VERBOSE)
/***********************************************************************
 * disassemble_compiled_fact
 ***********************************************************************/
/* print the compiled fact in a way
 * which ``wasm'' could understand
 */
void disassemble_compiled_fact(fout,fact)
FILE *fout;
unit_lemma_ctrl_block *fact;
{
  WORD *wp;
  int i,arity;

  /* print literal control block */
  fprintf(fout,"/* Compiled fact at %lx */\n",(long)fact);
  fprintf(fout,"lcb%lx:\n",(long)fact);
  fprintf(fout,"\t.dw\tlab"GETVALFORMAT"\n",GETVAL(fact->lcb.clauselbl));
  fprintf(fout,"\t.dw\tconst "GETSYMBOLFORMAT"\n",GETSYMBOL(fact->lcb.ps_symb));
  fprintf(fout,"\t.dw\tconst %d\n",GETNUMBER(fact->lcb.clause_nr));
  fprintf(fout,"\t.dw\tconst %d\n",GETNUMBER(fact->lcb.lit_nr));
  fprintf(fout,"\t.dw\tconst %d\n",GETNUMBER(fact->lcb.nr_vars));
  fprintf(fout,"\t.dw\tconst %d\n",GETNUMBER(fact->lcb.nr_strvars));
  fprintf(fout,"\t.dw\tconst %d\n",GETNUMBER(fact->lcb.nr_sgls));
  fprintf(fout,"\t.dw\tav"GETVALFORMAT"\n",GETVAL(fact->lcb.argv));
  fprintf(fout,"\t.dw\tconst "GETVALFORMAT"\n",GETVAL(fact->lcb.subgoal_list));
  
  /* print code of compiled fact */
  fprintf(fout,"/* compiled fact */\n");
  fprintf(fout,"lab"GETVALFORMAT":\n",GETVAL(fact->lcb.clauselbl));
  
  wp = (WORD *)fact + CODE_OFFSET;
  
  arity= GETARITY(fact->lcb.ps_symb);
  
  wp=disassemble_instruction(fout,wp);
  for (i=0; i< arity; i++){
    wp=disassemble_instruction(fout,wp);
  }
  disassemble_instruction(fout,wp);
  
  /* print argument vector */
  fprintf(fout,"/* argument vector */\n");
  fprintf(fout,"av"GETVALFORMAT":\n",GETVAL(fact->lcb.argv));
  wp = (WORD *)fact + ARGV_OFFSET(arity);
  
  for (i=0; i< arity; i++){
    fprintf(fout,"/* argument # %d */\n",i+1);
    disassemble_word(fout,wp+i);
  }
  
  fprintf(fout,"\n/* print complex terms */\n");
  wp=fact->term_block_ptr;
  while (wp){
	fprintf(fout,"/* print complex term at %lx*/\n",(unsigned long)wp);
	fprintf(fout,"lab%x*/\n",wp-code);
	for (i=0; i<GETARITY(*wp) + 2;i++){
	  disassemble_word(fout,wp+i);
	  fprintf(fout,"\n");
	}
	wp = GETPTR(*(wp + (GETARITY(*wp) + 2)));
  }
  
  fprintf(fout,"/* end of compiled fact */\n");
}

#endif

/***********************************************************************
 * print_compiled_fact
 ***********************************************************************/
/* print the compiled fact in lop-like notation
 * NOTE: the sign of the literal in the LCB
 *
 */
void print_compiled_fact(fout,fact)
FILE *fout;
unit_lemma_ctrl_block *fact;
{
  print_compiled_term(fout,fact,0);
  fprintf(fout,"<-.\n");
}

/***********************************************************************
 * print_compiled_term
 ***********************************************************************/
/* print the compiled fact as a term.
 * 3rd argument:
 *	if 0: do not negate
 *
 */
void print_compiled_term(fout,fact,negated)
FILE *fout;
unit_lemma_ctrl_block *fact;
int negated;
{
  WORD *wp;
  int i,arity;
  
  if (ISNEGATED(fact->lcb.ps_symb)){
    if (!negated)fprintf(fout,"~");
  }
  else {
    if (negated)fprintf(fout,"~");
  }
  /*
    printf("lcb.ps_symb: %d G_P(GID)= %d arity=%d Name=%s\n",
    fact->lcb.ps_symb,
    GETSYMBOL(fact->lcb.ps_symb),
    GETARITY(fact->lcb.ps_symb),
    GETPRINTNAME(fact->lcb.ps_symb));
    */
  fprintf(fout,"%s",GETPRINTNAME(fact->lcb.ps_symb));
  arity = GETARITY(fact->lcb.ps_symb);

  if (arity){
    fprintf(fout,"(");
  }
  
  wp = (WORD *)fact + ARGV_OFFSET(arity);
  
  for(i=0;i<arity;i++){
    disp_(fout,wp+i,NULL);
    if (i < arity -1){
      fprintf(fout,",");
    }
  }
  if (arity){
    fprintf(fout,")");
  }
}

/***********************************************************************
 *    S T A T I C  F U N C T I O N S
 ***********************************************************************/
/***********************************************************************
 * process term
 ***********************************************************************/
/* process a complex term, ie. copy it into a newly allocated
 * memory space, and process all variables and subterms in the same
 * way. 
 *	Input:	- ga: address of complex term (T_CRTERM, T_NGTERM)
 *		  NOTE: This is never a T_GTERM, and ga is
 *		 	dereferenced already.
 *		- code_ptr
 *	Output:	- starting address of the compiled term
 *		  NULL, if anything went wrong
 *		- ptr to ground: set to 1, if the term is ground
 *	Side-effect:
 *		- the items lcb.nr_vars and term_block_address of
 *		  the code_ptr are modified, such that all newly 
 *		  allocated memory blocks are linked together
 *		  in a linear list.
 */
static WORD *process_term(ga,ground,code_ptr)
WORD *ga;
int  *ground;
unit_lemma_ctrl_block *code_ptr;
{
  WORD *term;
  WORD *functor;
  WORD *tp;
  WORD *gaa;
  WORD *subtermp;
  int subterm_ground;
  
  
  PRT(printf("process_term: ga=%lx\n",ga);disp_(stdout,ga,bp););
  /****************************************
   * get required memory:
   * size = arity + term_block_ptr + T_EOSTR + 1
   */
  PRT(printf("process_term: size=%d\n",TERMSIZE(*ga)));


  if (!(term = (WORD *)MALLOC(TERMSIZE(*ga)))){
    /* error: could not allocate space */
    return NULL;
  }

  /****************************************
   * proceed through the arguments of the
   * term *ga
   * Note: - the functor will be set afterwards,
   *	   because we have to decide, if the
   *	   term is ground or not
   */
  functor = ga++;
  /* ga now points to the first argument (source) */
  tp = term+1;
  /* tp points to the first argument (destination)*/
  *ground = 1;
  
  while (GETTAG(*ga) != T_EOSTR){
    gaa = deref(ga,bp);
    PRT(printf("process_term:while-loop ga=%lx[%lx] gaa=%lx[%lx]\n",
	       ga,*ga,gaa,*gaa));
    PRT(printf("process_term:while-loop tp=%lx\n",tp));
    
    switch (GETTAG(*gaa)){
    case T_CONST:
      /****************************************/
      /* we enter the constant */
      
      *tp = *gaa;
      break;
    case T_FVAR:
      /****************************************
       * enter the variable with offset
       */
      GENOBJ(*tp,get_variable_offset(gaa,code_ptr),T_VAR);
      *ground = 0;
      break;
    case T_GTERM:
      /****************************************
       * for the ground-term, we only note
       * its address (as cref)
       */
      GENOBJ(*tp,(WORD *)gaa -code,T_CREF); 
      break;
    case T_NGTERM:
    case T_CRTERM:
      /****************************************
      * we have a complex term
      * and we have to call ``process_term''
      * recursively
      */
      if(!(subtermp = process_term(gaa,&subterm_ground,code_ptr))){
	/* error */
	/* clean up XXX */
	PRT(printf("process_term: process subterm returned an error\n"));
	return NULL;
      }
      /* are we still gound or not ? */
      *ground = *ground & subterm_ground;
      /* we must generate a CREF cell */
      GENOBJ(*tp,subtermp - code,T_CREF);
      break;
    default:
      printf("unilemma:process_term: illegal tag: "GETTAGFORMAT"\n",
	     GETTAG(*gaa));
      sam_error("unitlemma: illegal tag",gaa,3);
    }
    ga++;
    tp++;
  }
  
  /****************************************
   * add the functor and the T_EOSTR
   */
  GENOBJ(*term,GETVAL(*functor),((*ground)?T_GTERM:T_NGTERM));
  GENOBJ(*tp,0,T_EOSTR);
  tp++;

  /****************************************
   * link the space to the list of used blocks
   * (at the beginning)
   */
  GENOBJ(*tp, (WORD_cast) code_ptr->term_block_ptr,T_EMPTY);
  code_ptr->term_block_ptr = term;
  
  return term;
}

/***********************************************************************
 * get variable offset
 ***********************************************************************/
/* get the variable offset for a variable occuring in the current
 * term. a table containing the addresses of all variables
 * already encountered, is temporarily allocated on the stack
 *	input:	- address of variable cell
 *		- code_ptr
 *	output: offset for that variable
 */
static int get_variable_offset(ga,code_ptr)
WORD *ga;
unit_lemma_ctrl_block *code_ptr;
{
  int found;
  
  /* search, if the variable has been encountered
   * already or not */
  for (found = 0; found< GETNUMBER(code_ptr->lcb.nr_vars);found++){
    if (GETPTR(*(sp + found)) == ga){
      /* we found the variable */
      return found;
    }
  }
  /* the variable occurred for the first time */
  /* so keep its address                      */
  found = GETNUMBER(code_ptr->lcb.nr_vars);
  GENNUMBER(code_ptr->lcb.nr_vars,GETNUMBER(code_ptr->lcb.nr_vars)+1);
  GENOBJ(*(sp + found), (WORD_cast) ga, GETTAG(*ga));
  
  return found;
}


#endif
