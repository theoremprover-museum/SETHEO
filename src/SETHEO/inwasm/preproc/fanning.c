/*****************************************************************************
;;; MODULE NAME : fanning.c
;;;
;;; PARENT      : generator.c
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)fanning.c	15.1  01 Apr 1996
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 7.6.91
;;;
;;; DESCRIPTION : fan clauses 
;;;
;;; REMARKS     :  2. 7.93	jsc	extern.h
;;;		   2. 7.93	jsc	extern.h
;;;               22.12.93      Hamdi   copy_constr_list()  
;;;
******************************************************************************/

#include "extern.h"

/* muss noch ins extern.h verschoben werden */
extern termtype *copy_terms();          

/*****************************************************************************/
/* methods                                                                   */
/*****************************************************************************/
static claustype *gen_fan_clause();
static predtype *copy_pred();
static predtype *copy_tail_pred();
static predtype *copy_constr_list();
static void gen_fan_module();
static int semiisolated();
void del_conn();

/*****************************************************************************/
/* negative_goals()                                                          */
/* Description:                                                              */
/*****************************************************************************/
int negative_goals(cll)
claustype *cll;
{
  claustype *cl;
  predtype *pp;

  for (cl = cll; cl; cl = cl->next) {
    if (!cl->p_list->symb) {
      for (pp = cl->p_list; pp; pp = pp->next) {
	if (pp->symb && pp->sign == '+')
          return 0;
      }
    }
  }
  return 1;
}


/*****************************************************************************/
/* genfanning()                                                              */
/* Description:                                                              */
/*****************************************************************************/
void genfanning(module)
modtype *module;
{
  while (module) {
    if (module->m_list)
      genfanning(module->m_list);
    if (do_it(module,"nofan",COMPILE)) {
      del_conn(module);
    }
    else
      gen_fan_module(module->c_list);
    module = module->next;
  }
}


/*****************************************************************************/
/* gen_fan_module()                                                          */
/* Description: fan clauses of module                                        */
/*****************************************************************************/
static void gen_fan_module(cp)
claustype *cp;
{
 
  
  while (cp) {

   


    if ((cp->del == DELETED) ||
	(cp->delpos < 0) || 
	((cp->nr_lits - cp->nr_built) <= 1)) {
      /* prolog style clauses and facts are not fanned */
      cp = cp->next;
      continue;
    }
    if ((cp->p_list->symb) && (cp->delpos != 1)) {
      /* after fanning the original clause has different type */
      cp->delpos = PRULE; /* prolog rule */
    }
    else {
      cp->delpos = PQUERY; /* prolog query */
    }    
    cp->fann = gen_fan_clause(cp);
    cp = cp->next;    
  }
}


/*****************************************************************************/
/* gen_fan_clause()                                                          */
/* Description: fan clause                                                   */
/*****************************************************************************/
static claustype *gen_fan_clause(cp)
claustype *cp;
{
  int i = 1;
  claustype *newclause;
  claustype *llast = NULL;
  claustype *firstclause = NULL;
  predtype *akthead;

  akthead = cp->p_list;
  akthead->kp = cp;

  /*
      THIS IS DEBUG CODE (11.06.97)
  while(myptr) {
      printf("Fanning of Clause <<%d>>\n", cp->clnr);
      printf("  : Symbol-Nr                --> %d\n", myptr->symb);
      printf("  : Literal Nr               --> %d\n", myptr->litnr);
      printf("  : Complexity               --> %f\n", myptr->complexity);
      printf("  : ------------------------------------\n");
      myptr = myptr->next;
    }
    */

  /* begin with the second literal */
  akthead = akthead->next;

  if (!akthead)
    return NULL; 

  /* for queries we have to begin earlier */  
  if (cp->delpos == 1) { 
    i = 0; 
    akthead = cp->p_list;
  }

  /* initialize */
  while (akthead) {
    i++;
    if (akthead->symb < nrbuilt) {
      /* built-in can not be head of clause */
      akthead = akthead->next;
      continue;
    }
    if (semiisolated(akthead)) {
      akthead = akthead->next;
      continue;
    }

    if ((newclause = (claustype *)cnt_malloc(sizeof(claustype))) == NULL) {
      fprintf(stderr, "  Error (fanning.c - gen_fan_clause()): Memory allocation error.\n");
      exit(50);
    }

    if (!firstclause) 
      firstclause = newclause;
    else
      llast->next = newclause;
    newclause->clnr	= -i;
    newclause->modul	= cp->modul;
    newclause->lineno	= cp->lineno;
    newclause->order	= cp->order;
    newclause->constr	= cp->constr;
    newclause->nr_lits	= (!cp->p_list->symb)? cp->nr_lits-1 : cp->nr_lits;
    newclause->nr_built	= cp->nr_built;
    newclause->fann	= NULL;
    newclause->next	= NULL;
    newclause->p_list	= copy_pred(akthead);
    newclause->p_list->next = copy_tail_pred(cp->p_list,i);
    newclause->complexity = cp->complexity;
    newclause->delpos	= (newclause->p_list->next)? 
      PRULE : PFACT; /* prolog style */
    newclause->parsed_rule_type	= cp->parsed_rule_type;
    newclause->ddepth   = (newclause->delpos == PRULE)? 1 : 0;
    newclause->dinf     = newclause->nr_lits - newclause->nr_built - 1;
    newclause->connections = cp->connections;
    newclause->flag_constraints = cp->flag_constraints;
   
    llast = newclause;
    akthead->kp = newclause;
    akthead = akthead->next;
  }
  return firstclause;
}


/*****************************************************************************/
/* copy_tail_pred()                                                          */
/* Description: copy literal list except literal i                           */
/*****************************************************************************/
static predtype *copy_tail_pred(orgpred,i)
predtype *orgpred;
int i;
{
  predtype *newpred;

  if (!orgpred) 
    return orgpred;

  /* skip i. literal or defaultquery */
  if ((orgpred->litnr == i) || (!orgpred->symb)) 
    return copy_tail_pred(orgpred->next, i);
  
  newpred	= copy_pred(orgpred);
  newpred->next	= copy_tail_pred(orgpred->next, i);

  return newpred;
}


/*****************************************************************************/
/* copy_constr_list()                                                        */
/* Description: copy constraint list                                         */
/*****************************************************************************/
static predtype *copy_constr_list(orgcl)
predtype *orgcl;
{
  predtype *newcl;

  if (!orgcl) 
    return orgcl;

  newcl           = copy_pred(orgcl);
  newcl->next     = copy_constr_list(orgcl->next);
  return newcl;
}


/*****************************************************************************/
/* copy_pred()                                                               */
/* Description: copy predicate                                               */
/*****************************************************************************/
static predtype *copy_pred(orgpred)
predtype *orgpred;
{
  register predtype *newpred = (predtype *)cnt_malloc(sizeof(predtype));

  if (newpred == NULL) {
    fprintf(stderr, "  Error (fanning.c - copy_pred()): Memory allocation error.\n");
    exit(50);
  }

  newpred->symb            = orgpred->symb;
  newpred->sign	           = orgpred->sign;
  newpred->arity           = orgpred->arity;
  newpred->litnr	   = orgpred->litnr;
  newpred->nr_connections  = orgpred->nr_connections;
  newpred->missing_link    = orgpred->missing_link;
  newpred->complexity      = orgpred->complexity;
  newpred->t_list          = copy_terms(orgpred->t_list,ALL_TERM);
  newpred->claus           = orgpred->claus;
  newpred->wu              = orgpred->wu;
  newpred->literal_flag    = orgpred->literal_flag;
  newpred->kp              = NULL;
  newpred->subgoal_flag    = orgpred->subgoal_flag;

  return newpred;
}


/*****************************************************************************/
/* semiisolated()                                                            */
/* Description: look in weak unification list of -pp                         */
/*              if all literals  are facts                                   */
/*              if is semiisolated return -1 else 0                          */
/*****************************************************************************/
static int semiisolated(pp)
predtype *pp;
{
  predlist *w;

  for (w=pp->wu; w; w=w->next)
    if (w->pr->claus->nr_lits != 1) 
      return 0; 

  return -1; /* only weak unification with unit clauses */
}


/*****************************************************************************/
/* del_conn()                                                                */
/* Description: delete all connections to literals nr > 1                    */
/*              no calls in tail of nofan clause   !!                        */
/*****************************************************************************/
void del_conn(module)
modtype *module;
{
  claustype *clauses = module->c_list;
  predtype  *pred;
  predlist   *wulist;
  
  while (clauses) {
    /* formerly :
       if (pred = clauses->p_list->next)
       */
    pred = clauses->p_list->next;
    if (pred)
      clauses->delpos = (clauses->p_list->sign=='+')? PRULE : PQUERY;
    
    for ( ; pred; pred=pred->next) 
      for (wulist=pred->wu; wulist; wulist=wulist->next) {
	delete_p(pred,wulist->pr);
	if ((wulist->pr->litnr>1) &&
	    (pred->claus->modul == wulist->pr->claus->modul))
	  delete_p(wulist->pr, pred);
      }

    if (!clauses->p_list->symb) 
      clauses->delpos = PQUERY;
    clauses = clauses->next;
  }
}
