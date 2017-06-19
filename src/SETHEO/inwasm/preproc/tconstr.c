/******************************************************************************
;;; MODULE NAME : preproc/tconstr.c
;;;
;;; PARENT      : generator.c
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)tconstr.c	13.2 07 Apr 1995 
;;;
;;; AUTHOR      : Martin Ziegenaus 
;;;
;;; DATE        : 13.09.91
;;;
;;; DESCRIPTION : generate Tautology-Constraints
;;;
;;; REMARKS     : 12.11.92  tconstr.c             Hamdi
;;;               12.01.94  bug,extern.h          Hamdi
******************************************************************************/

#include "extern.h"

/*****************************************************************************/
/* functions (inwasm/preproc/tconstr.c)                                      */
/*****************************************************************************/
claustype *gen_taut_constr();
int FindeTautologie();


/*****************************************************************************/
/* gen_taut_constr()                                                         */
/* Description: generates tautology-constraints for all clauses              */
/*****************************************************************************/
claustype *gen_taut_constr(all_clauses)
claustype *all_clauses;    
{
  claustype *pointerToClause;
  claustype *firstClause = all_clauses; 
  int flag_printDeleted = FALSE;

  nr_gen_constr = 0;

  /* test, if there is a data structure */
  if (firstClause == NULL)
    return(NULL);

  /* test, if the first clause is the default-query */
  if (!(firstClause->p_list->symb))
    firstClause= firstClause->next;
  
  /* generate tautologie-constraints */
  pointerToClause = firstClause;

  while (pointerToClause != NULL) {
    /* Klausel tautologisch */ 
    if (FindeTautologie(pointerToClause) == TAUTOLOGY) {
      if (!flag_printDeleted) {
	printf("        Message: Deleted clauses: ");
	flag_printDeleted = TRUE;
      }
      printf("%d ", pointerToClause->clnr);
      pointerToClause->del = DELETED; 
    }
    pointerToClause = pointerToClause->next;
  }  
  if (flag_printDeleted) {
    printf("\n");
  }
  /* print message about tautology constraint generation to screen */
  if (nr_gen_constr)
    printf("        Message: %d tautology-constraints generated.\n",
	   nr_gen_constr);  
  return (all_clauses); 
}

/*****************************************************************************/
/* Finde Tautologie()                                                        */
/* Description: generates tautology-constraints for a special clause         */
/*              Clausel wird auf tautologische Instanzen hin untersucht      */
/*              Return-Wert:  NO_CONSTR   : kein Constraint, keine Tautologie*/
/*                            TAUTOLOGY   : Tautologie                       */
/*                            TAUTOLOGY_CONSTR: Constraint, da pot.Tautologie*/
/*****************************************************************************/
int FindeTautologie(Clausel)
claustype *Clausel;
{
  predtype *pp = Clausel->p_list;
  predtype *hpp;	
  predtype *constr = NULL;         /* das generierte Constraint             */
  int flag_constraintGen = FALSE;  /* TRUE falls Constraint generiert wurde */

  while (pp != NULL) {
    hpp = pp->next;
    /* suche in aktueller Klausel komplementaeres Literal */	
    while (hpp != NULL) {
      if ((pp->symb == hpp->symb) &&
	  (pp->sign != hpp->sign)) {	
	/* und versuche, zu unifizieren. */
	rearrange();	
	switch (unify(hpp->t_list, pp->t_list, UNIFY_TCONSTR)) {
	case SUCCESS:
	  /* v4debug: printf("TAUTO: ClauselNr: %d \n", Clausel->clnr); */
	  constr = GenerateConstraint(Clausel, GENTAUTOCONS, FALSE);
	  if (constr) {
	    /* printf("Constraint wird hinzugefuegt.\n"); */
	    FuegeConstraintHinzu(constr, Clausel);
	    constr->sign = TAUT_CONSTR;
	    flag_constraintGen = TRUE; 
	  }
	  /* Klausel ist tautologisch */
	  else {		 
	    return TAUTOLOGY;
	  }
	  break;
	case FAIL:	   
	  /* no constraint */	     
	  break;
	default:
	  fprintf(stderr," weak unify: return error");
	}	
      } 
      hpp = hpp->next;		
    }	
    pp = pp->next;
  }  

  rearrange();
  if (flag_constraintGen) 
    return TAUTOLOGY_CONSTR;
  else
    return NO_CONSTR;
}
