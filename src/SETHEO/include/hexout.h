/****************************************************
;;; MODULE NAME : hexout.h
;;;
;;; PARENT      :
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)hexout.h	10.1 02 Apr 1996
;;;
;;; AUTHOR      : J. Schumann
;;;
;;; DATE        : 3.1.95
;;;
;;; DESCRIPTION : header file with all defines
;;;
;;; REMARKS     :
;;;
****************************************************/

#ifndef HEXOUT_H
#define HEXOUT_H

#include <stdio.h>

/*****************************************************/
/* symbol table for unresolved addresses             */
/*****************************************************/
typedef enum {
	none = 0,
	label_type_cl_label_pos,		/* arity: 2 */
	label_type_cl_label_neg,		/* arity: 2 */
	label_type_lcb_label,			/* arity: 2 */
	label_type_av_label,			/* arity: 2 */
	label_type_cv_label,			/* arity: 2 */
	label_type_or_label,			/* arity: 2 */
	label_type_reduct_label_pos,		/* arity: 1 */
	label_type_reduct_label_neg,		/* arity: 1 */
	label_type_reduct2_label_pos,		/* arity: 1 */
	label_type_reduct2_label_neg,		/* arity: 1 */
	label_type_tl_label,			/* arity: 2 */
	label_type_lab_label,			/* arity: 1 */
	label_type_blab_label,			/* arity: 1 */
	label_type_qv_label,			/* arity: 2 */
	label_type_sl_label,			/* arity: 2 */
	label_type_lcbfailure,			/* arity: 0 */
	label_type_lcbreduct,			/* arity: 0 */
	label_type_lcbrreord,			/* arity: 0 */
	label_type_rrord,			/* arity: 0 */
	label_type_lcbexec,			/* arity: 0 */
	label_type_lblexec,			/* arity: 0 */
	label_type_lcbtrymode,			/* arity: 0 */
	label_type_lbltrymode,			/* arity: 0 */
	label_type_fcb,				/* arity: 0 */
	label_type_failure 			/* arity: 0 */
	} t_hexout_label_type;

typedef struct hexcode_lablist {
	long	where;
	int	pc;
	int	tag;
	struct code_symbtab_element *symbol;
	struct hexcode_lablist	*next;
	} t_lablist;
	
typedef struct code_symbtab_element {
		/*
		 * "name"
		  */
	t_hexout_label_type hexout_label_type;
	int	arg1;
	int	arg2;
		/*
		 * defined?
		 *  and value
		 **/
	int	defined;
	int	value;

	} t_code_symbtab_element;
	


/*****************************************************/
/* access macros for code-symbol-table               */
/*****************************************************/
#define CODE_SYMB_EQUAL(TYPE,A1,A2,ELEMENT) \
	((TYPE == ELEMENT->hexout_label_type) && \
	 (A1 == ELEMENT->arg1) && \
	 (A2 == ELEMENT->arg2))

#define CODE_SYMB_HASH(TYPE,A1,A2,A3) \
	((TYPE | (A1 << 20) | A2 << 4 ) % code_symbtab_size)

/*****************************************************/
/* other macros                                      */
/*****************************************************/
#define hex_out_use_label(LT,A1,A2) hex_out_use_label_tagged(LT,A1,A2,0)

/*****************************************************/
/* macros for optimizer                              */
/*	Since the optimizer always takes only a pair */
/*	(value,tag), we have to distinguish between  */
/*	labels and values.                           */
/*	Therefore, we have the following macros:     */
/*****************************************************/
#define OPT_IDFLAG		0x10000000
#define OPT_IDMASK		0x0fffffff
#define	OPT_IS_LABEL(tag)	(tag & OPT_IDFLAG)
#define	OPT_IS_VALUE(tag)	((tag & OPT_IDFLAG) == 0)
#define	OPT_TAG_LABELTAG(tag)   (tag & OPT_IDMASK)
#define	OPT_TAG_VALUETAG(tag)   (tag)
#define	OPT_VALUETAG_TAG(tag)   (tag)
#define	OPT_LABELTAG_TAG(tag)   (tag | OPT_IDFLAG)

#ifdef ANSI
		/* initialize everything */
void 	hex_out_init();
		/* define a label */
void	hex_out_enter_label();
		/* use a label (defined or not) */
void	hex_out_use_label_tagged();
		/* resolve any undefineds */
int	resolve_undefined();
		/* close everything */
void 	hex_out_close();
		/* put-code-word */
void 	hex_out_put_word();

	/*
	lcb25012:			ho_enter_label(l_t_lcb_l,25,12)
		.dw	fvar	5       ho_enter_tagged(T_FVAR,5)
		.dw	7		ho_enter_word(7)
		call lcbxx		ho_enter_word(OPCODE_call);
					ho_use_label(l_t_lcb_l,xx);
	*/

#endif

#endif
