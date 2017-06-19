/****************************************************
;;; MODULE NAME : cgarith.c
;;;
;;; PARENT      :
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)cgarith.c	15.1  01 Apr 1996
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 26.6.91
;;;
;;; DESCRIPTION : code generation functions for the arithemetic instructions
;;;
;;; REMARKS     :
;;;		1.3.93	jsc	arithmetic allows DIVISION
;;;		4.6.93	jsc	new structure for built-ins
;;;				removed all built <number>
;;;				marked printf("error: obsolete code generation\n");
;;;		1.7.93	jsc	extern.h
;;;		30.03.94 hamdi  static functions decl.
;;;		19.1.96	jsc	hexout code-generation
;;;
****************************************************/

#include "symb.h"
#include "extern.h"

/******************************************************/
/* statics                                            */
/******************************************************/
static int compute();
int cga_ev();

/******************************************************/
/* cga_ev()                                           */
/******************************************************/
int cga_ev(tp)
termtype *tp;
{
  int offset;
  
  if (tp->type == GVARIABLE) {
    if (tp->symb == DEPT_SYMB){
      fprintf(stderr,"error: obsolete code generation\n");
      /* getdepth */
      /* cgin_1built(15,0); */
    }
    else if (tp->symb == INF_SYMB){
      fprintf(stderr,"error: obsolete code generation\n");
      /* getinf */
      /* cgin_1built(16,0); */
    }
    else if (tp->symb == COPI_SYMB) {
      warning("$Copies not allowed in numexpr", "");
      return (1);
    }
    else
      if (tp->t_list) {
	/* there is an array element */
	if (tp->t_list->grflag == GROUND) {
	  /* constant index */
	  offset = compute(tp->t_list);
	  if (offset<0 || offset>symbtab[tp->symb].arity)
	    warning(NAM(tp->symb),"wrong index !");
	  cgin_glod(offset+symbtab[tp->symb].index);
	}
	else {
	  cga_ev(tp->t_list);
	  cgin_lconst(symbtab[tp->symb].arity);
	  fprintf(stderr,"error: obsolete code generation\n");
	  /* glod-array */
	  /* cgin_1built(29,symbtab[tp->symb].index); */
	}
      }
      else {
	cgin_glod(symbtab[tp->symb].index);
      }
  }
  else if (!tp->t_list) {
    /* we have a number or a variable */
    switch  (tp->type) {
    case  VARIABLE:
      cgin_lod(symbtab[tp->symb].index);
      break;
    case NUMCONST:
      cgin_lconst(tp->value);
      break;
    default:
      fprintf(stderr,"cga_ev: Illegal type in arithmetic");
    }
  }
  else {
    /* we have subterms */
    if (!tp->t_list->next) {
      if (tp->symb != UMI_SYMB)
	nferror("cga_ev: Illegal unary operator",0);
      else { 
	/* -N == 0-N */
	cgin_lconst(0);
	cga_ev(tp->t_list);
	cgin_sub();
      }
      return (0);
    }
    
    /* it is a binary operator */
    /* use postfix order       */
    /* first term */
    cga_ev(tp->t_list);
    /* second term */
    cga_ev(tp->t_list->next);
    /* operation */
    switch (tp->symb) {
    case PLUS_SYMB:
      cgin_add();
      break;
    case MINUS_SYMB:
      cgin_sub();
      break;
    case MULT_SYMB:
      cgin_mul();
      break;
    case DIV_SYMB:
      cgin_div();
      break;
    default:
      fprintf(stderr,"cga_ev: Illegal operator in arithmetic");
    }
  }
  return (0);
}

/******************************************************/
/* compute()                                          */
/******************************************************/
static int compute(tp)
  termtype *tp;
{
  
  if (tp->type == NUMCONST)
    return tp->value;
  else switch (tp->symb) {
  case PLUS_SYMB:
    return (compute(tp->t_list) + compute(tp->t_list->next));
  case MULT_SYMB:
    return (compute(tp->t_list) * compute(tp->t_list->next));
  case MINUS_SYMB:
    return (compute(tp->t_list) - compute(tp->t_list->next));
  case DIV_SYMB:
    return (compute(tp->t_list) / compute(tp->t_list->next));
  default:
    warning(NAM(tp->symb),"Illegal arithmetic");
    return(0);
  }
}

/*######################################################################*/
