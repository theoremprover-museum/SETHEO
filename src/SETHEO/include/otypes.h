/**************************************************************************
;;; MODULE NAME : otypes.h 
;;;
;;; PARENT      : preproc/oconstr.c 
;;;
;;; PROJECT     : MPLOP
;;;
;;; INWASMSCCS  : @(#)otypes.h	14.1 12 Mar 1996
;;; SCCS        : @(#)otypes.h	8.1 12 Mar 1996
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

#ifndef OTYPES_H
#define OTYPES_H


/* type fuer DELETED-Kontrap. */
typedef struct dlist {
			int clnr;
			int litnr;
			struct dlist *next;
		      } deltype;


#endif

/*########################################################################*/
