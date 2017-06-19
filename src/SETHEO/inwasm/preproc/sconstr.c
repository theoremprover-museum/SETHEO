/*****************************************************************************
;;; MODULE NAME : sconstr.c 
;;;
;;; PARENT      : generator.c 
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)sconstr.c	18.8 06/19/98
;;;
;;; AUTHOR      : Mohamed Salah, Hamdi 
;;;
;;; DATE        : 15.11.92 
;;;
;;; DESCRIPTION : generate subsumption constraints 
;;;
;;; REMARKS     : 30.12.93	Hamdi	extern.h
;;;		  25.02.94	Hamdi	SUBS,RSUBS 
;;;		  02.03.94	Hamdi	myen name: if nr < 0
;;;		  10.03.94	Hamdi	myemakeref #V --> V
;;;               26.01.95      PJ      bvstack init removed
;;;               02.02.95      PJ      merged my_unify into unify
;;;               07.05.95      PJ      map - attempt at bug fix
;;;               12.04.96      RZ      removed constr_list
;;;               15.04.96      RZ      removed bvstack - new trailstackcnt
;;;               16.04.96      RZ      removed new_unify 
;;;                                     shifted add_to_list() --> oconstr.c
;;;                                     external funcions --> extern.h
;;;               29.05.96      RZ      shifted ren_constr()
;;;                                      --> oconstr.c
;;;                             RZ      shifted make_cart_list() & 
;;;                                     find_s_list() & undo_unify()
;;;                                      --> substest.c
;;;               28.02.97      RZ      restructering and deleting of 
;;;                                     obsolete code
;;;                 
******************************************************************************/

#include "extern.h"           
#include "defines.h"


int map_counter = 0;

/*****************************************************************************/
/* functions (preproc/ sconstr.c)                                            */
/*****************************************************************************/
symbollist *app_sym_to_list();
void make_s_list();
void map_and_genconstr();
void map_all_and_genconstr();
void undo_unify();
void print_del_clauses();
void addConstraintList();
predtype *addToList();
int contains_constraint_builtin();

/*****************************************************************************/
/* gen_s_constr()                                                            */
/* Description: generates the subsumption constraints                        */
/*****************************************************************************/
claustype *gen_s_constr(Gesamtstruktur, compareMode)
claustype *Gesamtstruktur;
int compareMode; 
{
  int FirstClauseNr, SecondClauseNr;
  int returnValue;
  claustype *clauseList = Gesamtstruktur;
  symbollist *delList = NULL;
  subs_unif_counter = 0;
  subs_max_counter = 10000;
  nr_gen_constr = 0;                      /* number of generated constraints */

  if (!clauseList)    
    /* Eingabe leer */ 
    return(clauseList);
  
  if (!clauseList->p_list->symb) 
    /* erste Klausel ist default query */
    clauseList = clauseList->next;

  /* jede Klausel mit allen hinteren Klauseln */
  for (FirstClauseNr = 0; 
       ((FirstClauseNr < nr_clauses) &&
	(subs_unif_counter < subs_max_counter)); 
       FirstClauseNr++) {
    if ( ssort[FirstClauseNr].Clause->del != DELETED  && 
	 ssort[FirstClauseNr].Clause->parsed_rule_type != PRULE  &&
	 ssort[FirstClauseNr].Clause->parsed_rule_type != PQUERY &&
	 /* Begin: SEMANTIC */
	 (!contains_constraint_builtin(ssort[FirstClauseNr].Clause)) && 
	 /* End: SEMANTIC */
	 /* Nur Klauseln welche keine geparsten Constraints besitzten sind erlaubt */
	 (!ssort[FirstClauseNr].Clause->flag_constraints)) {
      for (SecondClauseNr = FirstClauseNr + 1;
	   ((SecondClauseNr < nr_clauses) &&
	    (subs_unif_counter < subs_max_counter));
	   SecondClauseNr++) {
	if ( ssort[SecondClauseNr].Clause->del != DELETED  &&
	     ssort[SecondClauseNr].Clause->parsed_rule_type != PRULE  &&
	     ssort[SecondClauseNr].Clause->parsed_rule_type != PQUERY ) { 
	  /* Wenn RSUBS, dann nur Regeln vergleichen  */
	  if (!(compareMode == RSUBS && 
		(ssort[FirstClauseNr].Clause->nr_lits - ssort[FirstClauseNr].Clause->nr_built == 1 || 
		(ssort[FirstClauseNr].Clause->nr_lits - ssort[FirstClauseNr].Clause->nr_built == 2 &&
		!ssort[FirstClauseNr].Clause->p_list->symb)))) {
	    /* falls Schalter -logprogsubs gesetzt, dann sind nur Klausel erlaubt,     */
	    /* welche keine built-ins enthalten. Dies gilt fuer Klausel1 sowie         */
	    /* Klausel2. Falls Schalter nicht gesetzt werden alle Klauseln akzeptiert. */
	    if ((!inwasm_flags.logprogsubs) || 
		((inwasm_flags.logprogsubs) && 
		 (ssort[FirstClauseNr].Clause->nr_built == 0) &&
		 (ssort[SecondClauseNr].Clause->nr_built == 0))) {

	      returnValue = clause_s_constr(ssort[FirstClauseNr].Clause, 
					    ssort[SecondClauseNr].Clause,
					    NORMALC);			
	      if (returnValue == 1) {
		/* Subsumption */ 
		delList = 
		  app_sym_to_list(delList, ssort[SecondClauseNr].Clause->clnr);
		ssort[SecondClauseNr].Clause->del = DELETED;  
	      }
	    }
	  }
	}
      }
    }
  }
  
  /* print Liste der DELETED-Klauseln */ 
  if (delList)
    print_del_clauses(delList);
  
  if (nr_gen_constr)
    printf("        Message: %d subsumption-constraints generated.\n",
	   nr_gen_constr); 
  return(Gesamtstruktur);
}


/*****************************************************************************/
/* clause_s_constr()                                                         */
/* Description: langeClausel kommt (im sortierten Feld) nach kurzeClausel    */
/*              vor.                                                         */
/*              Return-Wert:                                                 */
/*      	1 : langeClausel wird von kurzeClausel subsumiert !          */
/* 	        0 : langeClausel kann nicht von kurzerClausel subsumiert     */
/*                  werden, eventuell Subsumptions-Constraints geneneriert.  */
/*****************************************************************************/
int clause_s_constr(kurzeClausel,langeClausel,mode)
claustype *kurzeClausel, *langeClausel;
int mode;
{
  predtype *kpp = kurzeClausel->p_list;
  predtype *lpp = langeClausel->p_list;    /* zum Durchlaufen der Klauseln */

  no_subsumption = FALSE;
  
  /* default query ueberspringen */
  if ( ! kpp->symb )
    kpp = kpp->next;
  if ( ! lpp->symb )
    lpp = lpp->next;

  if (kurzeClausel->nr_lits > langeClausel->nr_lits) {
    return 0;
  }

  if (!trail)
    init_trail();
  
  /* Erstelle s_list fuer kurze Klausel */
  switch (mode) {
  case CONNOPTC:
  case NORMALC:
    /* verschiedene Klauseln werden verglichen */   
    make_s_list(kpp,lpp,NULL);
    /* Debug-Ausgabe der s_list: print_s_list(kurzeClausel); */
    break;
  case CONTRAC:
    /* Kontrapositive der gleichen Klausel werden verglichen */
    make_sym_s_list(kpp,lpp);
    break;
  default:
    fprintf(stderr, "  Error: Wrong mode.\n");
    exit(44);
  }
  
  /* Wenn ein Literal keine Partner-Literale hat: keine Subsumption */
  if (no_subsumption) {
    while (kpp) {
      /* remove all s_lists*/
      kpp->s_list = NULL;
      kpp->slistnext = NULL;
      kpp = kpp->next;
    } 
    return 0;
  }
  
  /* Initialisierungen */
  rearrange();
  BoundVariables = 0;
  ConstraintList = NULL;
  is_subsumption = FALSE;
  abbruch = FALSE;
  
  /* Zaehle alle Zuordnungsmoeglichkeiten auf */ 
   map_and_genconstr(kpp, langeClausel, mode); 

  while (kpp) {
    /* remove all s_lists */
    kpp->s_list = NULL;
    kpp->slistnext = NULL;
    kpp=kpp->next;
  }

  if (is_subsumption) {
    /* kurzeClausel subsumiert langeClausel */
    return 1; 
  }
  else {
    if (!ConstraintList) 
      return 0;
    else {
      addConstraintList(ConstraintList, langeClausel);
      return 0;
    }
  }
}


/*****************************************************************************/
/* make_s_list()                                                         */
/* Description: Erstellt s_list von kpp mit Umbenennung der Variablen und    */
/*              berechnet dabei die s_list fuer jedes Literal.               */
/*****************************************************************************/
void make_s_list(kpp, lpp, rootkpp)
predtype *kpp;
predtype *lpp;
predtype *rootkpp;
{
  plittype *helplit, *hptr;
  predtype *helplpp = lpp;
  predtype *helpold = kpp;

  int first = TRUE;

  while (kpp) {
    /* fuer alle Literale der kurzen Klausel */
    kpp->s_list = NULL;
    
    if (kpp->symb >= nrbuilt) { 	  
      helplpp = lpp;
      while (helplpp) {
        rearrange(); 
	
	if ((helplpp->symb == kpp->symb) && 
	    (helplpp->sign == kpp->sign) &&
	    (unify(helplpp->t_list, kpp->t_list, UNIFY_SLIST) == SUCCESS)) {
	  
	  if ((helplit = (plittype *) cnt_malloc(sizeof(plittype))) == NULL) {
	    fprintf(stderr, "  Error (sconstr.c - make_s_list()): Memory allocation error.\n");
	    exit(50);
	  }
	  helplit->pr = helplpp;
	  helplit->notunif = FALSE;
	  helplit->next = NULL;
	  
	  /* helplit hinten an kpp->s_list haengen */
	  if (!kpp->s_list) {
	    kpp->s_list = helplit;
	    kpp->s_list->next = NULL;
	  }
	  else {
	    hptr = kpp->s_list;
	    while (hptr->next) {
	      hptr = hptr->next;
	    }
	    hptr->next = helplit;
	    hptr = NULL;
	  }
	}
	helplpp = helplpp->next;
      }
      if (kpp->s_list) {
	if ((first == TRUE) && (rootkpp != NULL)) {
	  rootkpp->slistnext=kpp;
	  kpp->slistnext = NULL;
	} else {
	  helpold->slistnext=kpp; 
	  kpp->slistnext = NULL;
	}
      }
      
      if (!kpp->s_list) {
	/* Wenn ein Literal keine Partnerliterale hat. */
	no_subsumption = TRUE;
	return;
      }      
    } else { /* built-in */
      helpold->slistnext = kpp->next;
      kpp->slistnext = NULL;
    }
    helpold = kpp;
    if (kpp->next == NULL) {
      /* last predicate has null pointer */
      kpp->slistnext = NULL;
    }
    first = FALSE;
    kpp = kpp->next;
  }

  return;
}


/*****************************************************************************/
/* map_and_genconstr()                                                       */
/* Description: Zaehlt alle Zuordnungsmoeglichkeiten baumartig auf.          */
/*	       - is_subsumption = TRUE : Subsumption liegt vor.              */
/*	       - Sonst: Constraintliste in constr_list.                      */
/*****************************************************************************/
void map_and_genconstr(kpp,langeClausel,mode)  /* init flags (usage flag) */
predtype *kpp;
claustype *langeClausel;
int mode;
{
  plittype *plist = kpp->s_list;
  
  while (plist) {
    plist->pr->flag=0;
    plist=plist->next;
  }

  map_all_and_genconstr(kpp,langeClausel,mode,trailcnt);
}


/*****************************************************************************/
/* map_all_and_genconstr()                                                   */
/* Description:                                                              */
/*****************************************************************************/
void map_all_and_genconstr(kpp,langeClausel,mode,trailstackcnt)
predtype *kpp;
claustype *langeClausel;
int mode;
int trailstackcnt;
{
  predtype *constr;
  plittype *plist = kpp->s_list; 

  /* Fuer alle Literale der langen Klausel */
  while (plist) {
    
    if (subs_unif_counter > subs_max_counter) {
      is_subsumption = FALSE;
      abbruch = TRUE;
    }

    if (abbruch) {
      return;
    }
      
    if (kpp->slistnext == NULL) {
      /* Wenn alle Literale der kurzen erfolgreich zugeordnet sind */
      if (plist->pr->flag == 0 &&
	  (unify(plist->pr->t_list, kpp->t_list, UNIFY_SCONSTR) == SUCCESS)) {
	constr = GenerateConstraint(plist->pr->claus, GENSUBCONS, TRUE); 
	if (constr == NULL) {	    	   
	  is_subsumption = TRUE;
	  abbruch = TRUE;
	}
	else {	    	  
	  if (mode == NORMALC)
	    constr->sign = SUB_CONSTR;
	  else
	    constr->sign = SYMM_CONSTR; 	    
	  ConstraintList = addToList(constr, ConstraintList);
	  var_counter++; 	
	}
      } 
    } 
    else if ((plist->pr->flag == 0) &&
	     (unify(plist->pr->t_list, kpp->t_list, UNIFY_SCONSTR) == SUCCESS)) {
      /* naechstes Literal zuordnen */
      plist->pr->flag=1;
      /* map_all_and_genconstr(kpp->next,langeClausel,mode,trailcnt); */
      map_all_and_genconstr(kpp->slistnext,langeClausel,mode,trailcnt);
      plist->pr->flag=0;
    }
    /* Wenn alle Zuordnungen erfolgreich waren,  */
    /* oder die Zuordnung vorzeitig abgebrochen  */
    /* wurde: Dann mache die Wirkung der letzten */
    /* Unifikation im Trail rueckgaengig.        */
    undo_unify(trailstackcnt);/* backtracking */
    plist = plist->next;
    subs_unif_counter += 1;
  } /* while */
}


/*****************************************************************************/
/* undo_unify()                                                              */
/* Description: Loescht Bindungen, die durch das letzte unify gemacht wurden.*/
/*****************************************************************************/
void undo_unify(trailstackcnt)
int trailstackcnt;
{  
  while (trailcnt > trailstackcnt) {   
    trailcnt--;
    /* high- bzw. low-loclist Eintrag loeschen */
    *trail[trailcnt].loc = NULL;    
    /* clause_high bzw. clause_low Eintrag loeschen */
    if (trail[trailcnt].reference == LOW_REFERENCE) {
      symbtab[trail[trailcnt].symb].clause_low = 0;
      symbtab[trail[trailcnt].symb].lowloclist = NULL;
    }
    else if (trail[trailcnt].reference == HIGH_REFERENCE) {
      symbtab[trail[trailcnt].symb].clause_high = 0; 
      symbtab[trail[trailcnt].symb].highloclist = NULL; 
    }
  }  
  return;
}


/*****************************************************************************/
/* app_sym_to_list()                                                         */
/* Description: fuegt symb an list hinten an. Return-Wert: die neue Liste.   */
/*****************************************************************************/
symbollist *app_sym_to_list(list,sym)
symbollist *list;
int sym;
{
  symbollist *hp;
  symbollist *newsym;

  if ((newsym = (symbollist *)  cnt_malloc(sizeof(symbollist))) == NULL) {
    fprintf(stderr, "  Error (sconstr.c - app_sym_to_list()): Memory allocation error.\n");
    exit(50);
  }

  newsym->symbol = sym;
  newsym->next = NULL;
  
  if (!list)
    list = newsym;
  else {
    hp = list;
    while (hp->next) {
      hp = hp->next;
    }
    hp->next = newsym;
  }
  return(list);
}


/*****************************************************************************/
/* print_del_clauses()                                                       */
/* Description:                                                             */
/*****************************************************************************/
void print_del_clauses(list)
symbollist  *list;
{
  int counter = 0;
  symbollist *cntlist = list;

  while (cntlist) {
    counter++;
    cntlist = cntlist->next;   
  }

  if (counter == 1) {
    printf("        Message: Clause ");
    while (list) {
    printf("%d",list->symbol);
    list = list->next;   
    }
  } else {
    printf("        Message: Clauses ");
    while (list) {
      printf("%d",list->symbol);
      list = list->next;   
      counter--;
      if (counter > 1) {
	printf(", ");
      } else if (counter > 0) {
	printf(" and ");
      }
    }
  }
  printf(" deleted.\n");
}


/*****************************************************************************/
/* addConstraintList()                                                       */
/* Description: Fuegt eine Constraintliste zu einer Klausel ein. Die         */
/*              Constraintliste wird vorne eingefuegt.                       */
/*****************************************************************************/
void addConstraintList(clist, clause)
predtype *clist;
claustype *clause;
{
  predtype *hp;
  if (clist) {
    hp = clist;
    while (hp->next)
      hp = hp->next;
    hp->next = clause->constr;
    clause->constr = clist;
  }
}


/*****************************************************************************/
/* addToList()                                                               */
/* Description: Fuegt ein uebergebenes Constraint am Ende einer gegebenen    */
/*              Liste ein, und gibt diese anschliessend zurueck.             */
/*****************************************************************************/
predtype *addToList(constraint, list)
predtype *constraint;
predtype *list;
{
  predtype *hp;
  termtype *tList;

  /* set variable_number and clause_number values for the constraint */
  /* left side of constraint */
  tList = constraint->t_list;
  tList->variable_number = VariableEnum;
  tList->clause_number   = 1000;
  VariableEnum++;
  if (tList->t_list)
    VariableEnum = 
      generate_var_enum_predsym(1000, VariableEnum, tList->t_list);
  /* right side of constraint */
  tList = constraint->t_list->next;
  tList->variable_number = VariableEnum;
  tList->clause_number   = 1000;
  VariableEnum++;
  if (tList->t_list)
    VariableEnum = 
      generate_var_enum_predsym(1000, VariableEnum, tList->t_list);

  if(!list)
    list = constraint;
  else {
    hp = list;
    while (hp->next)
      hp = hp->next;
    hp->next = constraint;
  }

  return list;
}

/* Begin: SEMANTIC */
/*------------------ contains_constraint_builtin ----------------------*/

/* Returns TRUE, if clause contains a built-in $disconstr or a
   built-in $equconstr. 
   Returns FALSE otherwise.
   */

/*****************************************************************************/
/* contains_constraint_builtin()                                             */
/* Description:                                                              */
/*****************************************************************************/

int contains_constraint_builtin(clause)
     claustype *clause;
{
  predtype*  predicate;
  
  predicate = clause->p_list;
  while (predicate) {
    if (predicate->symb == DISCONSTR  ||  predicate->symb == EQUCONSTR)
      return TRUE;
    predicate = predicate->next;
  }
  
  return FALSE;
}
/* End: SEMANTIC */


int print_s_list(clause)
     claustype *clause;
{
  
  predtype *pp = clause->p_list;
  plittype *plt = clause->p_list->s_list;
  int pred_nr = 1;
  int plt_nr = 0;
 

  printf("S-LIST fuer Klausel-Nr.: %d\n", clause->clnr);
  while (pp) {
    plt = pp->s_list;
    printf("  Pred_nr: %d\n", pred_nr);
    if (!plt) {
      printf("    >> slist leer.\n");
    } else {
      printf("    >> printing s_list:\n");
      plt_nr = 1;
      while(plt) {
	printf("    %d--", plt_nr);
	print_lop_pred(plt->pr);
	printf("\n");
	plt = plt->next;
	plt_nr++;
      }
    }
    pp = pp->next;
    pred_nr++;
  }
  
  return 0;
}

