/*****************************************************************************
;;; MODULE NAME : constrsu.c
;;;
;;; PARENT      : main.c
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)constrsu.c	13.2 07 Apr 1995
;;;
;;; AUTHOR      : Mohamed Salah, Hamdi
;;;
;;; DATE        : 15.11.92
;;;
;;; DESCRIPTION : generate constraint subsumption 
;;;
;;; REMARKS     :
;;;
******************************************************************************/

#include "extern.h"           

int CountDeletedConstraints = FALSE;

/*****************************************************************************/
/* functions (preproc/ constrsu.c)                                           */
/*****************************************************************************/
void gen_constr_subs();
predtype *constr_subsume();
int subsumes_c();
int c_subs();
void free_term();
int testsubs();
void renameStructureVariables();


/*****************************************************************************/
/* gen_constr_subs()                                                         */
/* Description: Loescht subsumierbare Constr. der Klauselliste cll.          */
/*              Hauptprogramm fuer Constr.-Subsumption.                      */
/*****************************************************************************/
void gen_constr_subs(cll)
claustype *cll;
{
  claustype *fann;

  Generated = 0;
  Olddeleted = Deleted;
  Deleted = 0;                           /* Number of deleted constraints */
  
  while (cll) {

    if (cll->del == DELETED) CountDeletedConstraints = FALSE;
    else 
      CountDeletedConstraints = TRUE;
    /* optimiere Constr. der original Klausel */
    cll->constr = constr_subsume(cll->constr);
      
    if (cll->fann) {
      /* Klausel besitzt fann-Komponeneten     */
      /* optimiere Constr. der fann-Komponente */
      fann = cll->fann;
      while (fann) {
	if (fann->del == DELETED) CountDeletedConstraints = FALSE;
	else 
	  CountDeletedConstraints = TRUE;
	fann->constr = constr_subsume(fann->constr);
	fann = fann->next;
      }
    }
    /* naechste Klausel */
    cll = cll->next;
  }
  printf("        Message: %d constraints deleted.\n", Deleted);
}


/*****************************************************************************/
/* constr_subsusme()                                                         */
/* Description: Entfernt subsumierbare Constr. aus cl.                       */
/*              Jedes Constr. mit all seine Nachfolger.                      */
/*              Return-Wert: optimierte Liste von Constr.                    */
/*              Subsumierbare Constr. markieren: ->sign = DELETED            */
/*****************************************************************************/
predtype *constr_subsume(cl)
predtype *cl;
{
  predtype *hp1 = cl;
  predtype *hp2;

  while (hp1) {
    /* zaehle vorhandene Constraints */
    if (hp1->litnr > Generated)
      Generated = hp1->litnr;

    if (hp1->sign != DELETED) {
      /* vergleiche hp1 mit allen hinteren */
      hp2 = hp1->next;
      while (hp2  && hp1->sign != DELETED) {
	if (hp2->sign != DELETED) {
	  switch(subsumes_c(hp1, hp2)) {
	  case 1: /* hp1 wird von hp2 subsumiert: loesche hp1 */
	    hp1->sign = DELETED;	   
	    break;
	  case 2: /* hp2 wird von hp1 subsumiert: loesche hp2 */
	    hp2->sign = DELETED;	 
	    break;
	  default: /* -1: Keine Subsumption */
	    break;
	  }
	}
	hp2 = hp2->next;
      }
    }
    hp1 = hp1->next;
  }
  
  /* DELETED-Constr. loeschen */
  hp1 = cl;
  hp2 = NULL;
  while (hp1) {
    if (hp1->sign == DELETED) {
      /* increment counter, only if clause is not deleted */
      if (CountDeletedConstraints) Deleted++;
      if (hp1 == cl)
	hp1 = cl = hp1->next;
      else {
	hp2->next = hp1->next;
	hp1 = hp1->next;
      }
    }
    else {
      hp2 = hp1;
      hp1 = hp1->next;
    }
  }
  return(cl);
}


/*****************************************************************************/
/* subsumes_c()                                                              */
/* Description: Untersucht c1 und c2 auf Subsumption.                        */
/*              Return-Wert: 1: c1 wird von c2 subsumiert.                   */
/*                           2: c2 wird von c1 subsumiert.                   */
/*                          -1: keine Subsumption zwischen c1 und c2.        */
/*****************************************************************************/
int subsumes_c(c1, c2)
predtype *c1, *c2;
{
  int returnValue = 0;
  
  switch(c_subs(c1, c2)) {
    /* c1 wird von c2 subsumiert */
  case 1: 
    returnValue = 1;
    break;
    /* Moeglichkeit der Subs. von c2 durch c1 */
  case 0: 
    if ( c_subs(c2, c1) == 1 ) {     
      returnValue = 2;
    }
    else {     
      returnValue = -1;
    }
    break;
  case -1:    
    returnValue = -1;
    break;
  }
  if (returnValue == 0) {
    fprintf(stderr, "  Error: Wrong return value : %d.\n", returnValue);
    exit(43);
  }
  return returnValue;
}


/*****************************************************************************/
/* c_subs()                                                                  */
/* Description: Prueft ob das Constr. c1 vom Constr. c2 subsumiert wird.     */
/*              Return-Wert: 1: c1 wird von c2 subsumiert.                   */
/*                           0: c1 kann NICHT von c2 subsumiert werden aber  */
/*                              es ist moeglich,dass c2 von c1 subsum. wird. */
/*                          -1: Keine Subsumption zwischen c1 und c2 moeglich*/
/*                              (in beiden Richtungen!)                      */
/*****************************************************************************/
int c_subs(constraint1, constraint2)
predtype *constraint1, *constraint2;
{
  termtype *left1, *right1;
  termtype *left2, *right2;
  int tempReturnValue1, tempReturnValue2;
  int boundVariables1, boundVariables2;
  int returnValue = -1;

  /* Es werden nur nicht leere Constr. betrachtet */
  if (!constraint2) {   
    return 1;
  }
  if (!constraint1) {   
    return (-1);
  }
  
  if ((constraint2->sign == TAUT_CONSTR) || 
      (constraint2->sign == SUB_CONSTR) ||
      (constraint1->sign == SYMM_CONSTR) || 
      (constraint1->sign == OVER_CONSTR) ||
      ((constraint1->sign == TAUT_CONSTR) && (!constraint1->claus->fann)) ||
      ((constraint1->sign == SUB_CONSTR) && (!constraint1->claus->fann))) {

    /* left side of first constraint  */
    left1  = constraint1->t_list->t_list;   
    /* right side of first constraint */      
    right1 = copy_terms(constraint1->t_list->next->t_list, ALL_TERM);  
    /* #V... ---> V...: damit werden Bind. von SV von c1 gezaehlt */
    renameStructureVariables(right1);
  
    /* left side of second constraint  */
    left2  = constraint2->t_list->t_list;  
    /* right side of second constraint */
    right2 = constraint2->t_list->next->t_list;   

    rearrange();
    BoundVariables = 0;

    tempReturnValue1 = unify(left1, right1, UNIFY_CONSTRSU);
    boundVariables1 = BoundVariables;   /* Anz. der geb. Var. (ohne SV) */
  
    tempReturnValue2 = unify(left2, right2, UNIFY_CONSTRSU);
    boundVariables2 = BoundVariables;   /* Anz. der geb. Var. (ohne SV) */

    
    
    if (tempReturnValue1 == SUCCESS && tempReturnValue2 == SUCCESS) {
      if (boundVariables2 != boundVariables1) {
	/* constraint1 NICHT subsumierbar aber andere Richtg moeglich */
	returnValue = 0;
      }
      else {
	/* boundVariables2 == boundVariables1 : */
	/* constraint1 wird von constraint2 subsumiert */
	returnValue = 1;
      }
    }
    else           /* Keine Subs. in beiden Richtgen */
      returnValue = -1;
  }
  else 
    returnValue = 0;

  if (inwasm_flags.freememory)
    free_termtype(right1);
  
  return returnValue;
}


/*****************************************************************************/
/* test_subs()                                                               */
/* Description:                                                              */
/*****************************************************************************/
int testsubs(oconstr,clist)
predtype *oconstr, *clist;
{
  while (clist) {
    if (clist->sign != DELETED && c_subs(oconstr,clist) == 1) {      
      return(1);
    }
    clist = clist->next;
  }
  return (0);
}


/*****************************************************************************/
/* renameStructureVariables()                                                */
/* Description: Changes structure-variables in tlist to variables            */
/*              e.g. name:  #X22 --> X_22                                    */
/*                   type:  SVARIABLE --> VARIABLE                           */
/*              no possibility to re-rename to the old values                */
/*****************************************************************************/
void renameStructureVariables(tlist) 
termtype *tlist;
{
  char name[80];
  int head = 0;
  
  if (!tlist) {
    return;
  }
  if (tlist->next) {
    renameStructureVariables(tlist->next);
  }
  if (tlist->t_list) {
    renameStructureVariables(tlist->t_list);
  }
  if (tlist->type == VARIABLE || tlist->type == SVARIABLE) {
    if (symbtab[tlist->symb].name[0] == '#')  {
      sprintf(name,"%s",&(symbtab[tlist->symb].name[1]));
      head = enter_token(name);
      tlist->symb = enter_name(head,VARIABLE,0,0);
      tlist->type = symbtab[tlist->symb].type;
    }     
  }
  return;
}

