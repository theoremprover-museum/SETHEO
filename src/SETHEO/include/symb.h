/****************************************************
;;; MODULE NAME : symb.h
;;;
;;; PARENT      :
;;;
;;; PROJECT     : MPLOP
;;;
;;; INWASMSCCS  : @(#)symb.h	14.1 12 Mar 1996
;;; SCCS        : @(#)symb.h	10.1 02 Apr 1996
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 15.5.91
;;;
;;; DESCRIPTION : header file with typedefinition for symbol table
;;;
;;; REMARKS     :
;;;		4.6.93	jsc	new structure for built-ins
;;;		2.7.93	jsc	includes,extern.h
;;;		3/12/96		jsc	moved to ~/include
;;;		3/12/96		jsc		wasm-less inwasm
;;;
****************************************************/

#ifndef SYMB_H
#define SYMB_H

#include "types.h"


/*****************************************************/
/* definition of structures for symbol table         */
/*****************************************************/
typedef struct sytabel {
	struct preli *loclist;  /* list of occurrencies 	*/   /*later obsolete */
        struct preli *lowloclist; /* list of unrenamed occurencies */
        struct preli *highloclist;/* list of renamed occurencies*/
        int clause_low;         /* clause_nr with lowloc reference  */
        int clause_high;        /* clause_nr with highloc reference */
	char name[NAMELENGTH];	/* symbol name (string) 	*/
	int symbnr;             /* index in symboltable 	*/
	int modunr;             /* number of modul		*/
	int type;               /* type of symbol 		*/
	int arity;              /* store arity  		*/
	int index;		/* index  different usage 	*/
	int codenr;		/* index in setheo symbtab, code*/
	int (*bfun)();		/* pt to built-in function 	*/
	char *b_instr_name;	/* name of sam-instruction for  */
				/* built-in			*/
	built_in_type	b_type;	/* type of built-in		*/
	int b_opcode;		/* opcode for the built-in	*/
        struct sytabel * next;  /* pt to next elem 		*/
	} syel;


#endif
/*######################################################################*/
