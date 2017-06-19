/**************************************************************************
;;; MODULE NAME : stypes.h 
;;;
;;; PARENT      : preproc/sconstr.c 
;;;
;;; PROJECT     : MPLOP
;;;
;;; INWASMSCCS  : @(#)stypes.h	14.1 12 Mar 1996
;;; SCCS        : @(#)stypes.h	8.1 12 Mar 1996
;;;
;;; AUTHOR      : Mohamed Salah, Hamdi 
;;;
;;; DATE        : 
;;;
;;; DESCRIPTION : type definition 
;;;
;;; REMARKS     :
;;;		3/12/96		jsc	moved to ~/include
;;;
***************************************************************************/

#ifndef STYPES_H
#define STYPES_H


/* Struktur fuer Partner-Literale eines Literals aus kurzeClausel */
typedef struct plit  {
			 predtype *pr;        /*  Partner-Literal */
			 int notunif;         /* = 1 falls Partner-Literal
						 mit zugh. Literal nicht
						 unifizierbar. sonst 0 */
			 struct plit *next;   /* next Partner-Literal */
		     } plittype;


/* Struktur fuer aus kurzeClausel kopierte und umbenannte Literale */
typedef struct lit  {
			 int symb;            /* Praedikatsymbol */
			 int sign;
			 struct term *t_list;
			 plittype *s_list;    /* Partner-Literale */
			 struct lit *next;    /* naechstes Literal */
		    } littype;


#endif

/*########################################################################*/
