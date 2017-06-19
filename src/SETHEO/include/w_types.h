/****************************************************
;;; MODULE NAME : w_types.h
;;;
;;; PARENT      : preproc/reordering
;;;
;;; PROJECT     : MPLOP
;;;
;;; INWASMSCCS  : @(#)w_types.h	14.1 12 Mar 1996
;;; SCCS        : @(#)w_types.h	8.1 12 Mar 1996
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 15.5.91
;;;
;;; DESCRIPTION : type definition for reordering
;;;
;;; REMARKS     :
;;;		2.7.93	jsc	includes,extern.h
;;;		3/12/96		jsc	moved to ~/include
;;;
****************************************************/

#ifndef W_TYPES_H
#define W_TYPES_H

#include "types.h"

/****************************************************************/
/* data types */
/****************************************************************/
typedef float weighttype;		/* weights are real numbers	*/

/****************************************************************/
/* def. of array of weighting functions with their weight*/
/****************************************************************/
typedef struct w_el {
	weighttype (* we)();		/* pointer to weigth function	*/
	weighttype weight;		/* weight factor		*/
	} w_elt;

#endif

/*######################################################################*/
