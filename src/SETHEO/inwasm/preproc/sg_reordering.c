/*******************************************************************************
;;; MODULE NAME : sg_reordering.c
;;;
;;; PARENT      : generator.c
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)sg_reordering.c	15.1 01 Apr 1996
;;;
;;; AUTHOR      : Mohamed Salah, Hamdi
;;;
;;; DATE        : 27.01.94 
;;;
;;; DESCRIPTION : process subgoal reordering 
;;;
;;; REMARKS     :
;;;
*******************************************************************************/

#include "extern.h" 

/******************************************************************************/
/* methods                                                                    */
/******************************************************************************/
void sg_reordering();
void clause_sg_reordering();
void place_at_end();
int conn_in_fact();


/******************************************************************************/
/* sg_reordering()                                                            */
/* Description: F"uhrt subgoal reordering f"ur die ganze Formel durch.        */
/*              wird unmittelbar vor der Codegenerierung aufgerufen.          */
/******************************************************************************/
void sg_reordering(cll)
claustype *cll;
{
  claustype *fann;

  while (cll) {	
    /* sg reordering f"ur die deklarativen Klauseln und 
       eventuel	l auch fuer die prozeduralen Klauseln */
	  if (cll->del != DELETED &&			
	  ((inwasm_flags.sgreord == 1 &&		
	    cll->parsed_rule_type != PRULE && 		
	    cll->parsed_rule_type != PQUERY) ||		
	   inwasm_flags.sgreord == 2))			
      clause_sg_reordering(cll);	
	
    if (cll->fann) {
      fann = cll->fann;
      while (fann) {
	/* sg reordering f"ur Kontrapositive */
	if (fann->del != DELETED) 
	  clause_sg_reordering(fann);
	fann = fann->next;
      }
    }   
    cll = cll->next;
  }
}


/******************************************************************************/
/* clause_sg_reordering()                                                     */
/* Description: F"uhrt subgoalreordering f"ur eine Klausel durch:             */
/*              Literale nach 'Termkomplexit"at' 'Variablenabhaengigkeit' und */
/*           	Konnektionen.                                                 */
/******************************************************************************/
void clause_sg_reordering(clause)
claustype *clause;
{
  predtype * aktpr;
  predtype * hlppr,*hpr;
  predtype * prev_aktpr;
  predtype * best;

  
  if ((clause->nr_lits > 2) && 
      (clause->p_list->next->next->litnr < 100)) {
    /* mindestens 2 Lit. im Tail */
    aktpr = clause->p_list->next;
    prev_aktpr = clause->p_list;
    while (aktpr && aktpr->next) {
      hlppr = aktpr->next;
      best = aktpr;

      while (hlppr) {
	if (!hlppr->subgoal_flag) { 
	  if ((hlppr->symb < nrbuilt) || (best->symb < nrbuilt)) {
	    if ((hlppr->symb < nrbuilt) && 
		(best->symb < nrbuilt) &&
		(hlppr->litnr < best->litnr))
	      best = hlppr;
	    if ((hlppr->symb < nrbuilt) &&
		(!(best->symb < nrbuilt)))
	      best = hlppr;
	  }
	  else if (hlppr->complexity > best->complexity) { 
	    best = hlppr;
	  }
	} 
	hlppr = hlppr->next;
      }
     
    
      /* das beste steht schon vorn */ 
      if (best == aktpr) {
	prev_aktpr = prev_aktpr->next;
	aktpr = aktpr->next;
      }
      /* vertauschen */
      else {
	hpr = aktpr;
	while (hpr && (hpr->next != best)) 
	  hpr = hpr->next; 

	/* best aushaengen */
	hpr->next = best->next;
	/* best vorne einhaengen */
	prev_aktpr->next = best;
	best->next = aktpr;  

	/* akt_pr und prev_aktpr weiterschalten */
	prev_aktpr = best;
	aktpr = best->next;	    
      }
    }

    /* @Literale and @built-ins are placed at the end of the clause */
    place_at_end(clause); 
  }
}


/******************************************************************************/
/* place_at_end()                                                             */
/* Description: Special treatment of @literals. They will be positioned at the*/
/*              end of the clause. @built-ins in front of @literals.          */
/*              @Literals and @built-ins are collected in two seperate lists  */
/*              and eliminated in the original clause. At the end of this     */
/*              function, first the @built-ins-list and second the @literals- */
/*              list are put at the end of the original clause.               */
/*              The effect: @literals are sorted at the end of the clause.    */
/*              @built-ins are between normal literals and @literals.         */
/******************************************************************************/
void place_at_end(clause) 
claustype *clause;
{
  predtype *helpPtr = clause->p_list->next;
  predtype *prevHelpPtr = clause->p_list;
  predtype *movedPredAktPtr = NULL;
  predtype *movedPredStartPtr = NULL;
  predtype *movedBuiltInStartPtr = NULL;
  predtype *movedBuiltInAktPtr = NULL;
  int start = FALSE;
  int startBuiltIn = FALSE;

  while(helpPtr != NULL) {
    if (helpPtr->subgoal_flag == 1) {
      if (helpPtr->symb >= nrbuilt) {
	prevHelpPtr->next = helpPtr->next;
	if (start == FALSE) {
	  movedPredAktPtr = helpPtr;
	} else {
	  movedPredAktPtr->next = helpPtr;
	  movedPredAktPtr = movedPredAktPtr->next;
	}
	movedPredAktPtr->next = NULL;
	if (start == FALSE) {
	  movedPredStartPtr = movedPredAktPtr;
	  start = TRUE;
	}
	helpPtr = prevHelpPtr->next;
      } else { /* built-in */
	prevHelpPtr->next = helpPtr->next;
	if (startBuiltIn == FALSE) {
	  movedBuiltInAktPtr = helpPtr;
	} else {
	  movedBuiltInAktPtr->next = helpPtr;
	  movedBuiltInAktPtr = movedBuiltInAktPtr->next;
	}
	movedBuiltInAktPtr->next = NULL;
	if (startBuiltIn == FALSE) {
	  movedBuiltInStartPtr = movedBuiltInAktPtr;
	  startBuiltIn = TRUE;
	}
	helpPtr = prevHelpPtr->next;
      }
    } else {
      prevHelpPtr = helpPtr;
      helpPtr = helpPtr->next;
    }
  }

  /* place moved-preds at end */
  helpPtr = clause->p_list;
  while (helpPtr->next != NULL) {
    helpPtr = helpPtr->next;
  }
  if (movedBuiltInStartPtr != NULL) {
    helpPtr->next = movedBuiltInStartPtr;
    while (helpPtr->next != NULL) {
      helpPtr = helpPtr->next;
    }
    helpPtr->next = movedPredStartPtr;
  } else {
    helpPtr->next = movedPredStartPtr;
  }
}

/******************************************************************************/
/* conn_in_fact()                                                             */
/* Description: Berechnet die Anzahl der Konnektionen in Fakten f"ur ein      */
/*              gegebenes Literal                                             */
/******************************************************************************/
int conn_in_fact(lit)
predtype *lit;
{
  int erg = 0;
  predlist *wul = lit->wu;

  for (; wul; wul = wul->next)
    if (wul->pr->claus->nr_lits == 1) 
      erg++;

  return(erg);
}
