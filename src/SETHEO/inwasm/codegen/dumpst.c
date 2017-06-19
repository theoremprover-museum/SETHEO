/****************************************************
;;; MODULE NAME :
;;;
;;; PARENT      :
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)dumpst.c	15.1 01 Apr 1996
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 15.5.91
;;;
;;; DESCRIPTION : dump symboltable in codefile
;;;
;;; REMARKS     :
;;;		1.7.93		jsc	extern.h
;;;		23.1.96		jsc	hex-out
;;;
****************************************************/

/****************************************************/
/* includes                                         */
/****************************************************/
#include "symb.h"
#include "extern.h"
#include "codedef.h"

/****************************************************/
/* functions (codegen/dumpst.c)                     */
/****************************************************/
void cg_symbtab();

/****************************************************/
/* cg_symbtab()                                     */
/****************************************************/
void cg_symbtab()
{
  int i;
  int nr_symbols = 1;
  
  if (!inwasm_flags.hex_out){
    fprintf(code_file,
	    "/* * * * * *  S Y M B O L T A B L E * * * * * * * * * * * * * */\n");
    /*defaultquery */
    fprintf(code_file,"	.symb	\"%s\",\tpred, %d\n", 
	    NAM(0), (symbtab[0].arity < 0) ? 0 : symbtab[0].arity); 
  }
  else {
    hex_out_start_put_symbol();
    hex_out_put_symbol(NAM(0),SYMB_P,
		       (symbtab[0].arity < 0) ? 0 : symbtab[0].arity);
  }
  
  /* constants */
  for (i=1; i< nump; i++) {
    if (symbtab[i].type==CONSTANT || symbtab[i].type==STRINGCONST
	|| symbtab[i].type==CONSTSYM) {
      nr_symbols++;
      if (!inwasm_flags.hex_out){
	fprintf(code_file,"	.symb	\"%s\",\tconst, %d\n", 
		NAM(i), 
	   	(symbtab[i].arity < 0) ? 0 : symbtab[i].arity); 
      }
      else {
	hex_out_put_symbol(NAM(i),SYMB_C,
			   (symbtab[i].arity < 0) ? 0 : symbtab[i].arity); 
      }
    }
  }
  
  /* predicates */
  for (i=1; i< nump; i++) {
    if (symbtab[i].type==PREDSYM || symbtab[i].type==BUILTIN) {
      nr_symbols++;
      if (!inwasm_flags.hex_out){
	fprintf(code_file,"	.symb	\"%s\",\tpred, %d\n", 
		NAM(i), 
	   	(symbtab[i].arity < 0) ? 0 : symbtab[i].arity); 
      }
      else {
	hex_out_put_symbol(NAM(i),SYMB_P,
			   (symbtab[i].arity < 0) ? 0 : symbtab[i].arity);
      }
    }
  }
  if (inwasm_flags.hex_out){
    hex_out_finish_put_symbol(nr_symbols);
  }
}

