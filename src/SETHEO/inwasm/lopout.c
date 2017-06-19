/******************************************************************************
;;; MODULE NAME : lopout.c
;;;
;;; PARENT      :
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)lopout.c	15.1 01 Apr 1996
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 15.5.91
;;;
;;; DESCRIPTION : print matrix in mplop style
;;;
;;; REMARKS     :
;;;		3.3.93	jsc	adapted from hamdi, new syntax
;;;				without OVL-CONSTRAINTS
;;;		4.3.93	jsc	mode: after/before fanning
;;;				add'l parameter of print_wunif
;;;		26.3.93	jsc	Req: add'tl empty line
;;;		2.7.93	jsc	extern.h
;;;		30.12.93 hamdi	OVL_CONSTR removed
;;;		30.03.94 hamdi  static functions decl.
;;;				no: head <-.  
;;;
******************************************************************************/

#include "symb.h"
#include "extern.h"

/*****************************************************************************/
/* functions (inwasm/lopout.c)                                               */
/*****************************************************************************/  
void lop_output();
void print_lop_formel();
void print_lop_module();
void print_lop_clause();
void print_lop_tail();
void print_lop_pred();
void print_lop_term();
void print_lop_built();
void print_lop_numex();
void print_lop_glvar();
void print_lop_list();
void print_lop_constr();
void print_lop_connection();

/*****************************************************************************/
/* local macros                                                              */
/*****************************************************************************/
#define IND i,"" 	/* indent */
#define DELETED (-9999)

/*****************************************************************************/
/* statics                                                                   */
/*****************************************************************************/
static int i = 0;
static FILE *fout = NULL; 	    /* default */
static int defaultindex; 	    /* symbnr "default_" */


/*****************************************************************************/
/* lop_output()                                                              */
/* Description:                                                              */
/*****************************************************************************/
void lop_output(matrix,fp)
modtype *matrix;
FILE *fp;
{
  /* print clause and contrapositive lists  */
  fprintf(fp,"# List of Contrapositives\n");
  print_lop_formel(matrix,fp,1,1);
  fprintf(fp,"# End of Contrapositives\n\n");
  
  /* print the weak unification list */
  fprintf(fp,"# Weak-Unification List\n");
  print_wunif(matrix,fp,1);
  fprintf(fp,"# End Weak-Unification List\n");
}


/*****************************************************************************/
/* print_lop_formel()                                                        */
/* Description: print the intern repraesentation to fout                     */
/*              if outmode = 1 do not print the default query                */
/*              if fanned =1 print contrapositives only                      */
/*****************************************************************************/
void print_lop_formel(matrix,file,outmode,fanned)
modtype *matrix;
FILE *file;
int outmode;
int fanned;
{
  fout = file;
  defaultindex = 0;
  print_lop_module(matrix->c_list,outmode,fanned);
}


/*****************************************************************************/
/* print_lop_module()                                                        */
/* Description: print module in standard logic notation                      */
/*****************************************************************************/
void print_lop_module(cp,outmode,fanned)
claustype * cp;
int outmode;
int fanned;
{
  while (cp) {
    if (cp->del != DELETED) {
      print_lop_clause(cp,cp->p_list,cp->delpos,outmode,fanned);
      if (cp->constr) 
	print_lop_constr(cp->constr);

      if (!outmode || cp->clnr ) 
	fprintf(fout,".\n");
    }
    if (cp->fann) {
      i +=2;
      print_lop_module(cp->fann,outmode,fanned);
      i -=2;
    }
    cp = cp->next;
  }
}


/*****************************************************************************/
/* print_lop_clause()                                                        */
/* Description: print clause in logic/prolog notation                        */
/*              negated predicates confuse and error                         */
/*****************************************************************************/
void print_lop_clause(cp,pp,pos,outmode,fanned)
claustype *cp; 
predtype * pp;
int pos;
int outmode;
int fanned;
{
  int firsthead;
  int type_of_delimiter;
  claustype *clause = pp->claus;

  if (pp->claus->clnr == 0 && outmode == 1)
    return;
  /* fprintf(fout,"[Comp:%f/Nrbuilts:%d]",cp->complexity, cp->nr_built); */

  fprintf(fout,"\n%*s/*-%d.%d-*/  ",IND,clause->clnr,pp->litnr);

  if (pos<0) 
    switch (pos) {
    case PQUERY : 
      /* prolog query */
      fprintf(fout,"?-\n\t\t%*s",IND);
      print_lop_tail(pp->next);
      break;
    case PFACT :
      /* one literal fact */
      if (pp->sign == '-') 
	fprintf(fout,"~");
      print_lop_pred(pp);
      break;
    case PRULE :
      /* prolog rule  */
      if (pp->sign == '-') 
	fprintf(fout,"~");
      print_lop_pred(pp);
      pp=pp->next;
      if (pp) 
	fprintf(fout," :-\n\t\t%*s",IND);
      print_lop_tail(pp);
      break;
    default :
      fprintf(stderr,"undefined clausetype (Line_Nr. %d)",clause->lineno);
    }
  else {
    type_of_delimiter = 0;
    
    /* goal clauses   */
    if (!pp->symb) {
      /* skip default query */
      type_of_delimiter = 1;
      pp = pp->next;
    }
    else {
      firsthead = 1; 
      while (--pos) {
	if (!firsthead) 
	  fprintf(fout,"/*-%d.%d-*/ ",clause->clnr,pp->litnr);
	if (pp->sign == '-') 
	  fprintf(fout,"~");
	print_lop_pred(pp);
	if ((pp=pp->next) && pos>1)
	  fprintf(fout,";\n ");
	firsthead = 0;
      }
    }
    
    /* after fanning unit clauses are printed without "<-" */
    if (!fanned || pp) {
      /* if the formula is horn we use PROLOG-style */
      if (ishorn) {
	if (type_of_delimiter) {
	  if (pp) 
	    fprintf(fout," ?-\n\t\t%*s",IND);
	}
	else {
	  if (pp) 
	    fprintf(fout," :-\n\t\t%*s",IND);
	}
      }
      else {
	if (pp) 
	  fprintf(fout," <-\n\t\t%*s",IND);
      }
    }
    print_lop_tail(pp);
  }
}


/*****************************************************************************/
/* print_lop_tail()                                                          */
/* Description: print tail of clause in standard logic notation              */
/*****************************************************************************/
void print_lop_tail(pp)
predtype * pp;
{
  while(pp) {
    fprintf(fout,"/*-%d.%d-*/ ",pp->claus->clnr,pp->litnr);
    
    if (pp->sign == '+') 
      fprintf(fout,"~");
    print_lop_pred(pp);
    /*  print_lop_connection(pp);  */
    pp = pp->next; 
    if (pp) 
      fprintf(fout,",\n\t\t");
    if (pp) 
      fprintf(fout,"%*s",IND);
  }
}


/*****************************************************************************/
/* print_lop_pred()                                                          */
/* Description: print pred in standard logic notation                        */
/*****************************************************************************/
void print_lop_pred(pp)
predtype * pp;
{

  if (pp->symb < nrbuilt) {
    print_lop_built(pp);
    return;
  }
  fprintf(fout,"%s",NAM(pp->symb));
  /* fprintf(fout,"[L%d/S:%d/Litnr:%d/Compl:%f]",pp->literal_flag,
	pp->subgoal_flag, pp->litnr, pp->complexity); 
  */
 
  if (pp->t_list) {
    fprintf(fout,"(");
    print_lop_term(pp->t_list);
    fprintf(fout,")");
    /* fprintf(fout,"\t \% %f",pp->complexity); */
  }
}


/*****************************************************************************/
/* print_lop_term()                                                          */
/* Description: print term in prolog like manner on fout                     */
/*****************************************************************************/
void print_lop_term(tp)
termtype * tp;
{
  /* print term in a prolog like manner */
  while(tp) {
    switch (tp->type) 
      {	
      case GVARIABLE:
	print_lop_glvar(tp);
	break;
      case NUMCONST:
      case SVARIABLE:
      case VARIABLE:
	fprintf(fout,"%s",NAM(tp->symb)); 	
	fprintf(fout,"[%d]",tp->clause_number);
	break;
      case STRINGCONST:
	fprintf(fout,"\"%s\"",NAM(tp->symb));
	break;
      case CONSTANT:
	if (tp->symb == HEAD_SYMB) {
	  fprintf(fout,"[");	  
	  print_lop_list(tp->t_list);	    
	  fprintf(fout,"]");
	}
	else {
	  fprintf(fout,"%s",NAM(tp->symb));
	  if (tp->t_list) {
	    fprintf(fout,"(");		
	    print_lop_term(tp->t_list);	    
	    fprintf(fout,")");
	  }
	}
	break;
      default:
	fprintf(stderr,"wrong type in print_lop_term\n");
	break;
      }
    if (tp->next)
      fprintf(fout,",");
    tp=tp->next;
  }
}


/*****************************************************************************/
/* print_lop_built()                                                         */
/* Description: special handling for infix builtin := :is is < >             */
/*****************************************************************************/
void print_lop_built(pp)
predtype *pp;
{
  termtype *t1,*t2;

  if (symbtab[pp->symb].arity == 2) {
    /* if infix operator then seperate t_list in t1 t2     */
    if (!(t1 = pp->t_list)) {
      warning(NAM(pp->symb)," incomplete built-in");
      return;
    }
    if (!(t2=t1->next)) {
      warning(NAM(pp->symb)," incomplete built-in");
      return;
    }
    /* separate t1 t2 */
    t1->next = NULL;
  }
  else { /* builtin arity !=2: query__, write, dfs, isnumber, ..  */
    fprintf(fout,"%s",NAM(pp->symb));
    if (pp->t_list) {
      fprintf(fout,"(");
      print_lop_term(pp->t_list);
      fprintf(fout,")");
    }
    return;
  }
  
  switch (pp->symb) {
  case ASSI_SYMB:
    print_lop_glvar(t1);
    fprintf(fout," := ");
    print_lop_term(t2);
    break;
  case EVAS_SYMB:
    print_lop_glvar(t1);
    fprintf(fout," :is ");
    print_lop_numex(t2,1);
    break;
  case IS_SYMB:
    fprintf(fout,"%s is ",NAM(t1->symb));
    print_lop_numex(t2,1);
    break;
  case UNIF_SYMB:
    print_lop_term(t1);
    fprintf(fout," = ");
    print_lop_term(t2);
    break;
  case EQAL_SYMB:
    print_lop_term(t1);
    fprintf(fout," == ");
    print_lop_term(t2);
    break;
  case NQAL_SYMB:
    print_lop_term(t1);
    fprintf(fout," =/= ");
    print_lop_term(t2);
    break;
  case GREA_SYMB:
    print_lop_numex(t1,1);
    fprintf(fout," > ");
    print_lop_numex(t2,1);
    break;
  case LESS_SYMB:
    print_lop_numex(t1,1);
    fprintf(fout," < ");
    print_lop_numex(t2,1);
    break;
  case GRQU_SYMB:
    print_lop_numex(t1,1);
    fprintf(fout," >= ");
    print_lop_numex(t2,1);
    break;
  case LEQU_SYMB:
    print_lop_numex(t1,1);
    fprintf(fout," =< ");
    print_lop_numex(t2,1);
    break;
  default:
    fprintf(fout,"%s",NAM(pp->symb));
    pp->t_list->next = t2;
    if (pp->t_list) {
      fprintf(fout,"(");
      print_lop_term(pp->t_list);
      fprintf(fout,")");
    }
    break;
  } /* switch */
  
  /* join t1 t2 again */
  t1->next = t2;
}


/*****************************************************************************/
/* print_lop_numex()                                                         */
/* Description:                                                              */
/*****************************************************************************/
void print_lop_numex(tp,first)
termtype *tp;
int first;		/* reduce parenthesises */
{
  termtype *t1,*t2;

  if (tp->type == GVARIABLE)
    print_lop_glvar(tp);
  else if (tp->symb > nrbuilt) {
    /* there is a numeric constant or variable */
    fprintf(fout,"%s",NAM(tp->symb));
  }
  else if (!(t1 = tp->t_list)) {
    warning(NAM(tp->symb)," incomplete arihmetik");
    return;
  }
  else if (!(t2 = t1->next)) {
    if (tp->symb == UMI_SYMB) {
      fprintf(fout,"-");
      print_lop_numex(t1,0);
    }
    else
      warning(NAM(tp->symb)," incomplete arihmetik");
  }
  else {
    t1->next = NULL;
    if (!first) fprintf(fout,"(");
    print_lop_numex(t1,0);
    fprintf(fout,"%c",NAM(tp->symb)[0]);
    print_lop_numex(t2,0);
    if (!first) fprintf(fout,")");
    /* join t1 t2 again */
    t1->next = t2;
  }
}	


/*****************************************************************************/
/* print_lop_glvar()                                                         */
/* Description:                                                              */
/*****************************************************************************/
void print_lop_glvar(tp)
termtype *tp;
{
  if (tp->t_list) {
    /* there is a global array */
    fprintf(fout,"%s[",symbtab[tp->symb].name);
    print_lop_numex(tp->t_list,1);
    fprintf(fout,"]");
  }
  else
    fprintf(fout,"%s",symbtab[tp->symb].name);
}


/*****************************************************************************/
/* print_lop_list()                                                          */
/* Description:                                                              */
/*****************************************************************************/
void print_lop_list(tp)
termtype *tp;
{
  termtype *t2;
  
  if (!(t2=tp->next)) {
    warning("","incomplete list");
    return;
  }
  tp->next = NULL;
  if (t2->symb == HEAD_SYMB) {
    print_lop_term(tp);
    fprintf(fout,",");
    print_lop_list(t2->t_list);
  }
  else if (t2->symb == NIL_SYMB) 
    print_lop_term(tp);
  else {
    print_lop_term(tp);
    fprintf(fout,"|");
    print_lop_term(t2);
  }
  tp->next = t2;
}


/*****************************************************************************/
/* print_lop_constr()                                                        */
/* Description:                                                              */
/*****************************************************************************/
void print_lop_constr(pp)
predtype * pp;
{
  if (pp == NULL) 
    return;

  fprintf(fout,":\n\t\t");
  while (pp) {
    switch (pp->sign) {
    case TAUT_CONSTR:
      fprintf(fout,"/* tau %d */ ",pp->litnr);
      break;
    case SUB_CONSTR:
      fprintf(fout,"/* sub %d */ ",pp->litnr);
      break;
    case SYMM_CONSTR:
      fprintf(fout,"/* sym %d */ ",pp->litnr);
      break;
    case OVER_CONSTR:
      fprintf(fout,"/* ovl %d */ ",pp->litnr);
      break;
    default:
      /* user defined constr sign == '-' */
      break;
    }

    fprintf(fout,"[");
    print_lop_term(pp->t_list->t_list);
    fprintf(fout,"] =/= [");
    print_lop_term(pp->t_list->next->t_list);
    fprintf(fout,"]");
    pp = pp->next;
    if (pp)
      fprintf(fout,",\n\t\t");    
  }
}


/*****************************************************************************/
/* print_lop_connection()                                                    */
/* Description:                                                              */
/*****************************************************************************/
void print_lop_connection(pp)
predtype * pp;
{
  predlist *hcp;

  if (!(hcp = pp->wu))
   return;

  fprintf(fout,"{");
  while (hcp) {
    fprintf(fout,"%d.%d",hcp->pr->claus->order,hcp->pr->litnr);
    hcp = hcp->next; 
    if (hcp) 
      fprintf(fout,",");
  }
  fprintf(fout,"}\n");
}


