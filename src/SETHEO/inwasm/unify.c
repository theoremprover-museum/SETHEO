/*****************************************************************************
;;; MODULE NAME : unify.c
;;;
;;; PARENT      : wunif.c
;;;
;;; PROJECT     : MPLOP
;;;
;;; AUTHOR      : Rudolf Zeilhofer
;;;
;;; DATE        : 18.02.97
;;;
;;; DESCRIPTION : unify recursively 2 termlists
;;;
;;; REMARKS     : 18.02.97     RNZ            changed method of unification
;;;                                           renaming of variables is now 
;;;                                           obsolete
;;;
******************************************************************************/

#include "symb.h"
#include "extern.h"

/*****************************************************************************/
/* functions (inwasm/unify.c)                                                */
/*****************************************************************************/
void init_trail();
void rearrange();
termtype *deref();
int unify();
int unify_all();
int makeref();
int occ_check();


/*****************************************************************************/
/* init_trail()                                                              */
/* Description: initialise the trail, allocate memory for it                 */
/*****************************************************************************/
void init_trail()
{
  if (!trail) {
    if ((trail = (trailelt *)cnt_malloc(MAXTRAIL * sizeof(trailelt))) == NULL) {
      fprintf(stderr, "  Error (unify.c - init_trail()): Memory allocation error.\n");
      exit(50);
    }      
  }
  trailcnt=0;  
}


/*****************************************************************************/
/* rearrange()                                                               */
/* Description: unwind the trail, i.e. reset the pointers to the trail in    */
/*              the symbol table and make the trail empty (trailcnt=0)       */
/*****************************************************************************/
void rearrange()
{
  int trail_entry_nr;
  
  for (trail_entry_nr = 0; trail_entry_nr < trailcnt; trail_entry_nr++) {
    symbtab[trail[trail_entry_nr].symb].lowloclist = NULL;
    symbtab[trail[trail_entry_nr].symb].highloclist = NULL;
    symbtab[trail[trail_entry_nr].symb].clause_low = 0;
    symbtab[trail[trail_entry_nr].symb].clause_high = 0;
  }
  trailcnt = 0;  
}


/*****************************************************************************/
/* deref()                                                                   */
/* Description: dereference a variable cell                                  */
/*              if it unbound (i.e. there is no trail-entry in the symbol    */
/*              table, return the orig. term. Otherwise look at the term,    */
/*              the trail points to and follow up.                           */
/*              The term_nr specifies if it is the first or second term in   */
/*              in a unification. (first term uses lowloclist, second highl.)*/
/*****************************************************************************/
termtype *deref(term)
termtype *term;
{   
  trailelt *trailpointer;
  
  /* dereference variables/svariables only */
  while ((term->type == VARIABLE) || 
	 (term->type == SVARIABLE)) {
    if (symbtab[term->symb].clause_low == term->clause_number) {
      trailpointer = (trailelt *)symbtab[term->symb].lowloclist;
      term = trailpointer->term;            
    }
    else if (symbtab[term->symb].clause_high == term->clause_number) {
      trailpointer = (trailelt *)symbtab[term->symb].highloclist;
      term = trailpointer->term;      
    }
    else {
      return term;
    }
  }
  return term;
}


/*****************************************************************************/
/* unify()                                                                   */
/* Description: unify 2 terms. Returns: SUCESS or FAIL.                      */
/*              Needs an empty trail, does no rearrange.                     */
/*****************************************************************************/
int unify(term1, term2, mode)
termtype *term1, *term2;
int mode;
{
  /* initialize unification mode */
  UnificationMode = mode;

  /* initialize High- and LowClausenr */
  if (term1) 
    LowClauseNr  = term1->clause_number;
  else 
    LowClauseNr = 0;
  if (term2) 
    HighClauseNr = term2->clause_number;
  else 
    HighClauseNr = 0;   
  
  return (unify_all(term1, term2));
}

/*****************************************************************************/

int unify_all(term1, term2)
termtype *term1, *term2;
{
  termtype *literal1, *literal2;
  int return_value;
  
  /* move through term_lists of term1 and term2 */
  while (term1) {

    /* dereference the two terms */
    literal1 = deref(term1);
    literal2 = deref(term2);

    switch (literal1->type) {
    case STRINGCONST:
    case NUMCONST:
    case CONSTANT:
      switch (literal2->type) {
      case STRINGCONST:
      case NUMCONST:
      case CONSTANT:
	if (literal1->symb != literal2->symb) {
	  return FAIL; 
	}
	/* there are function symbols */
	if (literal1->t_list) {
	  return_value = unify_all(literal1->t_list, literal2->t_list);	  	  
	  if (return_value == FAIL)
	    return FAIL;	 
	}
	break;
      case GVARIABLE:
	/* there is a globvar array */
	if (literal2->t_list)
	  break;
      case SVARIABLE:
      case VARIABLE :	
	/* literal2 is a (unbound) variable          */
	/* bind literal1 to literal2 variable        */
	if (makeref(literal1, literal2)) {	 
	  return FAIL;
	}
	break;
      default :
	warning(NAM(literal1->symb)," new weak-unif error ");
	break;
      }
      break;
    case GVARIABLE:
      /* there is a globvar array */
      if (literal1->t_list)
	break;
    case SVARIABLE:
    case VARIABLE :
      /* literal1 is a (unbound) variable       */
      /* bind literal2 to literal1, whatever literal2 is  */
      if (makeref(literal2, literal1)) {
	return FAIL;
      }
      break;
    default :
      warning(NAM(literal1->symb)," new weak-unif error ");
      break;
    }      
    term1 = term1->next;
    term2 = term2->next;
  }
  return SUCCESS;
}
		

/*****************************************************************************/
/* makeref()                                                                 */
/* Description: generate a new entry on the trail and make an assignment     */
/*              - pointer in the symbol-table for symb                       */
/*              - new trail entry with l/r-assignment                        */
/*              - there is at least one variable                             */
/*              - if necessary do the occur check                            */
/*              - returns: 0 if ok, -1 if occur check                        */
/*****************************************************************************/
int makeref(term1, term2)
termtype *term1, *term2;
{
  termtype *term;
  termtype *symb;

  if ((term1->type == CONSTANT) || 
      (term1->type == NUMCONST) || 
      (term1->type==STRINGCONST)) {
    /* bind term1 on symbol(term2) */
    term = term1;
    symb = term2;    
    if ((term1->t_list) && 
	(occ_check(term2, term1->t_list)))
      return (-1);
  }
  else {
    /* two variables - bind lower to higher - no occur check needed 
       dont bind variable to itself -  no self reference !!         */
    if (((term1->symb == term2->symb) &&
	 (term1->clause_number == term2->clause_number)))
      return (0);
    
    /* variable symb  is bound to variable term; use fixed order to avoid 
       cycles; this order is necessary - by changing it you change; the number
       and look of the generated constraints. */
    if ((term1->clause_number == LowClauseNr) &&
	(term2->clause_number == HighClauseNr)) {
      term = term1;
      symb = term2;
    }
    else if ((((term1->clause_number == LowClauseNr) &&
	       (term2->clause_number == LowClauseNr)) ||
	      ((term1->clause_number == HighClauseNr) &&
	       (term2->clause_number == HighClauseNr))) &&
	     (term1 < term2)) {
	term = term1;
	symb = term2;
      }
    else {
      term = term2;
      symb = term1;
    }
  }

  if (UnificationMode == UNIFY_CONSTRSU) {
    if ((term1->type != SVARIABLE) &&
        (term2->type == SVARIABLE)) {
         term = term1;
	 symb = term2;
    }
    else if ((term1->type == SVARIABLE) &&
	     (term2->type != SVARIABLE)) {
	 term = term2;
	 symb = term1;
    }
    if (symb->type != SVARIABLE) {
      BoundVariables++;   
    }
  } /* else if (UnificationMode == UNIFY_SCONSTR) {
    if (term1->clause_number == HighClauseNr) {
      term = term2;
      symb = term1;
    } else {
      term = term1;
      symb = term2;
    }
    if (symb->clause_number != HighClauseNr) {
    BoundVariables++;   
    }
  }*/
  
  /* unbound variable make new trail cell */
  if (trailcnt == MAXTRAIL-1) {
    nferror("trail too small",0);
    return (0);
  }

  /* generating trail-entries */
  trail[trailcnt].symb = symb->symb;
  trail[trailcnt].term = term;
  trail[trailcnt].highClauseNr = HighClauseNr;
 
  if (LowClauseNr == symb->clause_number) {
    trail[trailcnt].loc  = &symbtab[symb->symb].lowloclist;
    trail[trailcnt].reference = LOW_REFERENCE;
    symbtab[symb->symb].lowloclist = (predlist *)(trail + trailcnt); 
    symbtab[symb->symb].clause_low = symb->clause_number;       
  }
  else if (HighClauseNr == symb->clause_number) {
    trail[trailcnt].loc  = &symbtab[symb->symb].highloclist;
    trail[trailcnt].reference = HIGH_REFERENCE; 
    symbtab[symb->symb].highloclist = (predlist *)(trail + trailcnt);
    symbtab[symb->symb].clause_high = symb->clause_number;        
  }
  else {
    fprintf(stderr, "  Error: Wrong trail entry.\n");
    exit(42);
  }

  trailcnt++; 
  return (0);
}


/*****************************************************************************/
/* occ_check()                                                               */
/* Description: do the occur check :                                         */
/*              a functor is bound to a variable                             */
/*              test if the variable occurs in functor                       */
/*              TRUE : variable occurs in the functor                        */
/*              FALSE: no occurency                                          */
/*****************************************************************************/
int occ_check(var_term, func_term)
termtype *var_term, *func_term;
{
  termtype *deref_term; 

  while (func_term) { 
      deref_term = deref(func_term);
      
    if ((deref_term->type == VARIABLE) ||
	(deref_term->type == SVARIABLE)) {      
      if ((deref_term->symb == var_term->symb) && 
	  (deref_term->clause_number == var_term->clause_number)) {
	return TRUE;
      }      
    }
    if (deref_term->t_list) {      
      if (occ_check(var_term, deref_term->t_list))
	return TRUE;
    }
    func_term = func_term->next;
  }
  return FALSE;
}
