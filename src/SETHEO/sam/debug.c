/**************************************************************
;;;*****************************************
;;;MODULE NAME:   debug.c
;;;
;;;PARENT     :   setheo/mainpart
;;;
;;;PROJECT    :   pppppppppp
;;;
;;;AUTHOR     :   xxxxxxxxxx
;;;
;;;DATE       :   ddddddd
;;;
;;;DESCRIPTION:   xxxxxxxxxxxx
;;;               xxxxxxxxxxxxxx
;;;MODIF      :   xxxxx (when, what, who)
;;;			for atari 5.2.88
;;;			29.7.88 with deb-file
;;;			31.10.88 new loadcode
;;;			11.11.
;;;			22.1.89
;;;			16.2.89
;;;                     11.02.92 CONSTRAINTS added by bschorer
;;;			11.03.93 structure variables Mueller
;;;			16.9.93		johann	disassemble_word//instruction
;;;			17.9.93		jsc		remove of old lemma stuff, tell,told
;;;				copy-binding
;;;
;;;REMARKS    :   xxxxxxx
;;;
;;;COMPILING  :   yn
;;;
;;;BUGS       :   never any !!
;;;****************************************
**************************************************************/


#include <stdio.h>
#include "codedef.h"
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "symbtab.h"
#include "disp.h"
/**/
#include "debug.h"


#define PRTHEAP			

/**********************************/
int dispreg ()
/**********************************/
{
  WORD           *i;
  
  printf ("CPU-Registers\n");
  printf ("PC: %d  SP: %d BP: %d HP: %d GBP: %d\n",
	  pc - code, sp - stack,
	  (WORD *) bp - stack, hp - heap,
	  (WORD *) goal_bp - stack);
  printf ("TR: %d  RA: %d CHP: %d GP: %d DEPTH: %d\n",
	  tr - trail, (ra - code),
	  (WORD *) chp - stack, gp - code, depth);
  printf ("Inf. Count: %d\n", infcount);
  printf ("Sgs. Count: %d\n", sgscount);
  printf ("Sig. Value: %d\n", sigcount);
  printf ("Intermediate free variables: %d\n", intmd_fvars);
#ifdef ANTILEMMATA
  printf ("Generated antilemmata:       %d\n", anlcount);
#endif
  if (single_delay  ||  multi_delay)
      PRINT_LOG1("Number of subgoal-delays: %8d\n", delaycount);
  printf ("Intermediate inferences: %d\n", intmd_inf);
  printf ("Intermediate term complexity: %d\n", intmd_tc);
  printf ("Intermediate open subgoals: %d\n", intmd_sgs);
  printf ("Intermediate signature: %d\n", intmd_sig);
  
#ifdef STATISTICS1
  printf ("Instr. Count: %d\n",
	  cnt);
#endif
  
  printf ("STACK @ %lx HEAP @ %lx CODE @ %lx\n", (unsigned long)stack, (unsigned long)heap, (unsigned long)code);
  
  if (verbose & 4)
    for (i = heap; i < hp; i++) {
#ifndef WORD_64     
      printf ("HEAP: %lx : %lx\n", (unsigned long)i, (unsigned long)*i);
#endif      
    }
  if (verbose & 8)
    for (i = stack; i < sp; i++) {
#ifndef WORD_64     
      printf ("STACK: %lx : %lx\n", (unsigned long)i, (unsigned long)*i);
#endif      
    }
  
  return 0;
}

void dispchoice (ch, bptr)
choice_pt      *ch;
environ        *bptr;
{
  while (ch) {
    printf ("CHOICE: %lx  ", (unsigned long)ch);
    printf ("next-clause %x\n", (ch->next_clause - code));
    ch = ch->ch_link;
  }
  while (bptr) {
    printf ("ENVIRON @ %lx  ", (unsigned long)bptr);
    printf ("retaddr %x\n", (bptr->ret_addr - code));
    bptr = bptr->dyn_link;
  }
}

/**********************************
 * disp_obj                       
 * in a very primitve form
 **********************************/

void disp_obj (op)
WORD           *op;
{
  switch (GETTAG (*op)) {
  case T_CONST:
    printf ("***CONSTANT: "GETVALFORMAT"\n", (GETVAL (*op)));
    break;
  default:
    printf ("***VARIABLE\n");
  }
}

/**********************************
 * disp_word
 **********************************/
void  disp_word (obj)
WORD           *obj;
{
  switch (GETTAG (*obj)) {
  case T_EMPTY:
    printf (" T_EMPTY  %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_CONST:
    printf (" T_CONST %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_GTERM:
    printf (" T_GTERM %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_NGTERM:
    printf (" T_NGTERM %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_CRTERM:
    printf (" T_CRTERM %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_EOSTR:
    printf (" T_EOSTR %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_CREF:
    printf (" T_CREF %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_VAR:
    printf (" T_VAR %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_BVAR:
    printf (" T_BVAR %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_FVAR:
    printf (" T_FVAR %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_GLOBL:
    printf (" T_GLOBL %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_STRVAR:
    printf (" T_STRVAR %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_CSTRVAR:
    printf (" T_CSTRVAR %lx ", (unsigned long)GETVAL (*obj));
    break;
  default:
#ifndef WORD_64     
    printf ("unknown: %lx ", (unsigned long)*obj);
#endif    
    break;
  }
}

/**********************************
 * disp_chp
 **********************************/
void disp_chp (ch)
choice_pt      *ch;
{
  WORD           *co;
  
  co = ch->next_clause - 1; 
}

/**********************************
 * prt_clause
 **********************************/
void prt_clause (a)
WORD            a;
{  
  printf ("--No debugging info--\n");
  return;
}

/******************************************************
 * disassemble_instruction
 ******************************************************/
WORD *disassemble_instruction(fout,cp)
FILE *fout;
WORD *cp;
{
#ifndef WORD_64 
  int i;
   
  if ((*cp < 0) || (*cp >= nr_instr)){
    /* is not an instruction */
    fprintf(fout,"/* not an instruction (%lx) */\n",(unsigned long)*cp);
    return cp+1;
  }
  fprintf(fout,"\t%s",instr_table[*cp].name);
  
  for (i=1;i<instr_table[*cp].length;i++){
    fprintf(fout,"\tlab%lx",(unsigned long)*(cp+i));
    if (i<instr_table[*cp].length-1){
      fprintf(fout,",");
    }
  }
  fprintf(fout,"\n");
  return cp+instr_table[*cp].length;
#endif 
} 

/**********************************
 * disassemble_word
 **********************************/
void disassemble_word(fout,obj)
FILE		   *fout;
WORD           *obj;
{
  
  /* to be changed */
  switch (GETTAG (*obj)) {
  case T_EMPTY:
    printf (" T_EMPTY  %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_CONST:
    printf (" T_CONST %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_GTERM:
    printf (" T_GTERM %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_NGTERM:
    printf (" T_NGTERM %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_CRTERM:
    printf (" T_CRTERM %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_EOSTR:
    printf (" T_EOSTR %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_CREF:
    printf (" T_CREF %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_VAR:
    printf (" T_VAR %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_BVAR:
    printf (" T_BVAR %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_FVAR:
    printf (" T_FVAR %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_GLOBL:
    printf (" T_GLOBL %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_STRVAR:
    printf (" T_STRVAR %lx ", (unsigned long)GETVAL (*obj));
    break;
  case T_CSTRVAR:
    printf (" T_CSTRVAR %lx ", (unsigned long)GETVAL (*obj));
    break;
  default:
#ifndef WORD_64    
    printf ("unknown: %lx ", (unsigned long)*obj);
#endif    
    break;
  }
}
