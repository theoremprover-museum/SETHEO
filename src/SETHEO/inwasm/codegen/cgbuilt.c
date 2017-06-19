/*******************************************************
;;; MODULE NAME : codegen/cgbuilt.c
;;;
;;; PARENT      :
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)cgbuilt.c	15.1 01 Apr 1996
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 27.5.91
;;;
;;; DESCRIPTION : codegeneration for built-ins
;;;
;;; REMARKS     :
;;;		* 28.12.: argsize
;;;		26.3.93		jsc	cg_b_[n]eq, cg_is*
;;;		4.6.93	jsc	new structure for built-ins
;;;		2.7.93		jsc	extern.h, nondet. built-ins
;;;		21.7.93		jsc	debugging 
;;;		7.6.94		jsc	b_gen_lcb_t function
;;;		19.1.96		jsc	clean codegen
;;;
*******************************************************/

/******************************************************/
/* includes                                           */
/******************************************************/
#include "symb.h"
#include "extern.h"

/******************************************************/
/* statics                                            */
/******************************************************/
static int built_lab = 0;

/******************************************************/
/* functions (codegen/cgbuilt.c)                      */
/******************************************************/
void b_assign();
void b_greater();
void b_greateq();
void b_less();
void b_lesseq();
void b_globeval();
void b_eval();
void b_eqpred();
void b_generic();
void b_error();
void b_gen_lcb_t();

/******************************************************/
/* b_assign()                                         */
/******************************************************/
void b_assign(pr)
  predtype *pr;
{
  if (pr->t_list->symb<nrbuilt)
    warning(NAM(pr->t_list->symb)," := not implemented");
  else {
    if (pr->t_list->t_list) {
      /* arrayelement */
      cga_ev(pr->t_list->t_list);
      cgin_lconst(symbtab[pr->t_list->symb].arity);
      cgin_lconst(symbtab[pr->t_list->symb].index);
      /* array_gsto */
      fprintf(stderr,"error: obsolete code-gen:	built	28, av%3.3d%3.3d\n",
	      pr->claus->clnr,pr->litnr);
    }
    else
      cgin_assign(symbtab[pr->t_list->symb].index,pr->claus->clnr,pr->litnr);
  }
}

/******************************************************/
/* b_greater();                                       */
/******************************************************/
void b_greater(pr)
  predtype *pr;
{
  /* numterm '>' numterm */
  cga_ev(pr->t_list);
  cga_ev(pr->t_list->next);
  cgin_sub();
  cgin_jmpg_blab(built_lab);
  cgin_fail();
  cgin_blab(built_lab++);
}

/******************************************************/
/* b_greateq();                                       */
/******************************************************/
/* a >= b : a-b >= 0	*/
void b_greateq(pr)
  predtype *pr;
{
  /* numterm '>=' numterm */
  cga_ev(pr->t_list);
  cga_ev(pr->t_list->next);
  cgin_sub();
  cgin_jmpz_blab(built_lab);
  cgin_jmpg_blab(built_lab);
  cgin_fail();
  cgin_blab(built_lab++);
}

/******************************************************/
/* b_less()                                           */
/******************************************************/
/* a < b :  b-a > 0	*/
void b_less(pr)
  predtype *pr;
{
  /* numterm '<' numterm */
  cga_ev(pr->t_list->next);
  cga_ev(pr->t_list);
  cgin_sub();
  cgin_jmpg_blab(built_lab);
  cgin_fail();
  cgin_blab(built_lab++);
}

/******************************************************/
/* b_lesseq()                                         */
/******************************************************/
/* a =< b : b-a >= 0	*/
void b_lesseq(pr)
  predtype *pr;
{
  /* numterm '=<' numterm */
  cga_ev(pr->t_list->next);
  cga_ev(pr->t_list);
  cgin_sub();
  cgin_jmpz_blab(built_lab);
  cgin_jmpg_blab(built_lab);
  cgin_fail();
  cgin_blab(built_lab++);
}

/******************************************************/
/* b_globeval()                                       */
/******************************************************/
void b_globeval(pr)
  predtype *pr;
{
  /* GVariable :IS numterm */
  if (pr->t_list->symb == COPI_SYMB) {
    warning("$Copies :is not defined ","");
    return;
  }
  cga_ev(pr->t_list->next);
  if (pr->t_list->symb == DEPT_SYMB) 
    cgin_setdepth(0);
  else {
    if (pr->t_list->t_list) {
      /* arrayelement */
      cga_ev(pr->t_list->t_list);
      cgin_lconst(symbtab[pr->t_list->symb].arity);
      fprintf(stderr,"error obsolete:cgin_1built(30...\n");
      /* ,symbtab[pr->t_list->symb].index); */
    }
    else
      cgin_gsto(symbtab[pr->t_list->symb].index);
  }
}


/******************************************************/
/* b_eval()                                           */
/******************************************************/
void b_eval(pr)
  predtype *pr;
{
  /* Variable IS numterm */
  cga_ev(pr->t_list->next); 
  cgin_sto(symbtab[pr->t_list->symb].index);
}


/******************************************************/
/* b_eqpred()                                         */
/******************************************************/
/* generate     eqpred    argvect , prednu , arity   */
void b_eqpred(pr)
  predtype *pr;
{
  predtype *p;
  int litnr;
  
  if ( ! pr->t_list->type == NUMCONST) {
    nferror("eqpred: must be a number",pr->claus->lineno);
    return;
  }
  litnr = pr->t_list->value;
  p = pr->claus->p_list;
  while (p->litnr != litnr) {
    if (!(p = p->next)) {
      nferror("eqpred: illegal number",pr->claus->lineno);
      return ;
    }
  }
  cgin_eqpred(pr->claus->clnr,litnr,p->sign,p->symb,symbtab[p->symb].arity);
}

/******************************************************/
/* b_generic()                                        */
/******************************************************/
/* this is the generic code generation function for   */
/* a built-in                                         */
/******************************************************/
void b_generic(pr)
  predtype *pr;
{
  
  /* determine the type of the built-in */
  if (symbtab[pr->symb].b_type == NONDETERMINISTIC){
    /* we have a non-deterministic built-in        */
    /* which requires the preparation instruction  */
    /* ``prepare_ndbuilt	lcbfailure''       */
    /* Note: If we have encountered such a         */
    /* built-in, we need the failure clause        */
    
    cgin_prepare_ndbuilt();
    failure_clause_needed = 1;
  }

  /* in any case, we must generate the built-in instruction */
  if (!symbtab[pr->symb].arity){
    /* built-in with no arguments */
    cgin_gen_built_in_noargs(symbtab[pr->symb].b_instr_name,
			     symbtab[pr->symb].b_opcode);
  }
  else {
    cgin_gen_built_in_args(symbtab[pr->symb].b_instr_name,
			   symbtab[pr->symb].b_opcode,
			   pr->claus->clnr, pr->litnr);
  }
}

/******************************************************/
/* b_error()                                          */
/******************************************************/
void b_error(pr)
  predtype *pr;
{
  char warn[40];
  
  sprintf(warn,"Built-in %s codegen",NAM(pr->symb));
  nferror(warn,pr->claus->lineno);
}

/******************************************************/
/* b_gen_lcb_t()                                      */
/******************************************************/
/* this is the generic code generation function for   */
/* a DETERMINISTIC built-in with:                     */
/*	* first argument is LCB  (i.e. literal-number)*/
/*	* 2nd,... argument(s) as usual                */
/******************************************************/
void b_gen_lcb_t(pr)
  predtype *pr;
{
  predtype *p;
  int litnr;
  
  if ( pr->t_list->type != NUMCONST) {
    nferror("built-in: must be a number",pr->claus->lineno);
    return;
  }
  litnr = pr->t_list->value;
  p = pr->claus->p_list;
  while (p->litnr != litnr) {
    if (!(p = p->next)) {
      nferror("built-in: illegal number",pr->claus->lineno);
      return;
    }
  }
  
  cgin_b_gen_lcb_t(symbtab[pr->symb].b_instr_name,
		   symbtab[pr->symb].b_opcode,
		   p->litnr, pr->claus->clnr, pr->litnr);
}




