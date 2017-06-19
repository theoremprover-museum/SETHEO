/******************************************************************************
;;;MODULE NAME:   cl_reordering.c
;;;
;;;PARENT     :   generator.c
;;;
;;;VERSION    :	  @(#)cl_reordering.c	15.1 01 Apr 1996
;;;
;;;PROJECT    :   setheo
;;;
;;;AUTHOR     :   Martin Ziegenaus, TUM
;;;
;;;DATE       :   02.10.91
;;;
;;;DESCRIPTION:   purity preprocessing step
;;;
;;;MODIF      :   12.11.92 sort.c M. Salah Hamdi                                   
;;;               28.10.94 K. Mayr: weigt of consts and vars exchanged,     
;;;                        clause complexity is the sum of lit complexity,
;;;                        less complex clauses get smaller indices.    
;;;                        Intialisierung von erg korrigiert: meist 1.0   
;;;                        Neue Sortierfunktion hinzugefuegt             
;;;               16.06.95 PJ: weight function    
;;;               19.02.97 RNZ: erased obsolete code, structuring
;;;	
******************************************************************************/

#include "extern.h"

/*****************************************************************************/
/* functions (preproc/cl_reordering.c)                                       */
/*****************************************************************************/
float BerechneSortierkriterium();
void SortiereNachKriterium();
int nr_var_in_lit();
void count_connections();

sortarray *ssort = NULL;
sortarray *hsort = NULL;


/*****************************************************************************/
/* inspect_clauses()                                                         */
/* Description:                                                              */
/*****************************************************************************/
void inspect_clauses(cll)
claustype *cll;
{
  claustype *cl;
  int plits;

  minlits = 10000;
  for (cl = cll; cl; cl = cl->next) {
    plits = pos_lits(cl);
    if (plits == 0)
      nr_negated_clauses++;
    if (plits == 1)
      nr_definite_clauses++;
    if (plits > 1)
      nr_indefinite_clauses++;
    if (cl->nr_lits == 1) /*RZ: old(06.02.98): (cl-nr_lits = 1) */
      nr_facts++;
    if (cl->nr_lits > maxlits) 
      maxlits = cl->nr_lits; 
    if (cl->nr_lits < minlits) 
      minlits = cl->nr_lits; 
    nr_literals += cl->nr_lits; 
    nr_connections += cl->connections; 
    complexity += cl->complexity; 
  }
}


/*****************************************************************************/
/* KriteriumEinesTerms()                                                     */
/* Description:                                                              */
/*****************************************************************************/
float KriteriumEinesTerms(term)
termtype *term;
{
  int argnr;	       /* zum Feststellen der Aritaet bei Funktionen         */
  termtype *hpt;       /* zum Durchlaufen der Argumentenliste bei Funktionen */
  float erg;

  if (term->type == VARIABLE)
    return( 0.0 );
  if (term->type == GVARIABLE)
    return( 0.0 );
  if (term->type == BUILTIN)
    return( 0.0 );

  if (term->type == CONSTANT || term->type == NUMCONST || 
      term->type == STRINGCONST) {
    if (term->t_list == NULL) {
      /* Konstante haben Gewicht 1 */
      return( 1.0 );
    }
    else {
      /* Funktionen haben Gewicht                 */ 
      /* 1 / Summe der Argumentkomplexitaeten + 1 */
      argnr = 1;
      hpt = term->t_list;
      while (hpt != NULL) {
	argnr++;
	hpt = hpt->next;
      }
		
      /* durchlaufe Unterstruktur und rufe diese Funktion rekursiv auf */
      hpt = term->t_list;
      erg = 1.0;
      while (hpt != NULL) {
	erg += KriteriumEinesTerms( hpt );
	hpt = hpt->next;
      }

      return(erg / (float) (argnr));
    }
  }
  else {
    return(-1.0);
  }
}			


/*****************************************************************************/
/* KriteriumEinesPraedikates()                                               */
/* Description:                                                              */
/*****************************************************************************/
float KriteriumEinesPraedikates(praedikat)
predtype *praedikat;
{
  termtype *term =  praedikat->t_list;
  float erg = 1.0;

  /* do not count builtins:  if predicate is a builtin -> return 0.0 */
  /* do not count ^predicates, if Option -ignorelits is set          */
  /* --> if predicate is a ^pred -> return 0.0                       */
  if ((symbtab[praedikat->symb].name[0] == '$') || 
      (symbtab[praedikat->symb].name[0] == '@' &&
       symbtab[praedikat->symb].name[1] == '$')){
    praedikat->complexity = 0.0;
    return( 0.0 );
  } else if ((inwasm_flags.ignorelits) && (praedikat->literal_flag == 1)) {
    praedikat->complexity = 0.0;
    return( 0.0 );
  }

  /* durchlaufe Termliste des Praedikates */
  while (term != NULL) {
    erg += KriteriumEinesTerms(term) / (float)(praedikat->arity + 1);
    term = term->next;
  }

  /*
    THIS IS DEBUG CODE (11.06.97) 
  printf("  : Praedikat %d hat Complexity: %f\n", praedikat->litnr, erg);
  */

  if (inwasm_flags.oldclausreord == 0) {
    /* neue methode des clause_reorderings zaehlt keine hinzugefuegten
       query-praedikate */
    if (!strcmp(NAM(praedikat->symb), "query__")) {
      erg = erg - 1.0;
    }
    praedikat->complexity = erg ;
    return (erg);
  } 
  else {
    /* flag fuer altes clause_reodering gesetzt. Hinzugefuegte Query-Praedikate
       werden nicht speziell behandelt. */
    praedikat->complexity = erg ;
    return (erg);
  }
}


/* Begin: SEMANTIC */				
/*****************************************************************************/
/* KriteriumEinerTermListe()                                                 */
/* Description:                                                              */
/*****************************************************************************/
float KriteriumEinerTermListe( List )
     termtype*  List;
{
  termtype*  HeadOfList;
  termtype*  TailOfList;
  
  if ( List->symb == NIL_SYMB )
    return 0.0;
  
  HeadOfList = List->t_list;
  TailOfList = List->t_list->next;
  
#ifdef DEBUG
  printf("KriteriumEinesTerms(HeadOfList) = %f\n",
	 KriteriumEinesTerms(HeadOfList));  
  printf("KriteriumEinerTermListe(TailOfList) = %f\n",
	 KriteriumEinerTermListe(TailOfList));
  printf("KriteriumEinerTermListe = %f\n", 
	 (KriteriumEinesTerms(HeadOfList)
	  + KriteriumEinerTermListe(TailOfList)));   
  fflush(stdout);
#endif
  
  return (KriteriumEinesTerms(HeadOfList)
	  + KriteriumEinerTermListe(TailOfList));
}


/*****************************************************************************/
/* KriteriumEinesListenEqConstraints()                                       */
/* Description:                                                              */
/*****************************************************************************/
float KriteriumEinesListenEqConstraints( ListEqConstr )
     termtype*  ListEqConstr;
{
  termtype*  RightList;
  
  if (ListEqConstr->t_list == NULL  ||  
      ListEqConstr->t_list->next == NULL) {
    fprintf(stderr,"  Syntax error: list equality constraint is unparsable.\n");
    exit(70);
  }
  
  RightList = ListEqConstr->t_list->next;
  
#ifdef DEBUG
  printf("KriteriumEinesListenEqConstraints = %f\n",
	 KriteriumEinerTermListe(RightList));   
  fflush(stdout);
#endif
  
  return KriteriumEinerTermListe(RightList);
}  	


/*****************************************************************************/
/* KriteriumEinesDisConstrs()                                                */
/* Description: The disjunctive constraint must have the following structure:*/
/*              - The top-level junctor is a disjunction. The elements of    */
/*                this disjunction are elements of a list.                   */
/*              - The elements of the disjunction are equalities between     */
/*                termlists. The left sides of these equalities are lists of */
/*                variables, the right sides are arbitrary terms.            */
/*****************************************************************************/
float KriteriumEinesDisConstrs( DisconstrList )
     termtype*  DisconstrList;
{
  termtype*  HeadOfDisconstrList;
  termtype*  TailOfDisconstrList;
  
  if ( DisconstrList->symb == NIL_SYMB )
    return 0.0;
  
  HeadOfDisconstrList = DisconstrList->t_list;
  TailOfDisconstrList = DisconstrList->t_list->next;
  
  if ( HeadOfDisconstrList->symb != HEAD_SYMB ) {
    fprintf(stderr,"  Syntax error: disjunctive constraint has an unparsable element.\n");
    exit(71);
  }
  
#ifdef DEBUG
  printf("KriteriumEinesDisConstrs = %f\n",
	 (KriteriumEinesListenEqConstraints(HeadOfDisconstrList)
	  + KriteriumEinesDisConstrs(TailOfDisconstrList)));  
  fflush(stdout);
#endif
  
  return (KriteriumEinesListenEqConstraints(HeadOfDisconstrList)
	  + KriteriumEinesDisConstrs(TailOfDisconstrList));
}


/*****************************************************************************/
/* KriteriumDesBuiltInsDisconstr()                                           */
/* Description: Das erste Argument des Built-Ins $disconstr/2 ist das dis-   */
/*              junktive Constraint, das zweite ist ein Mass fuer den Dis-   */
/*              junktivitaetsgrad des Constraints, z.B. die Anzahl der       */
/*              Klauseln, die durch dieses Constraint zu einer zusammenge-   */
/*              fasst wurden.                                                */
/*****************************************************************************/
float KriteriumDesBuiltInsDisconstr(built_in)
     predtype*  built_in;	
{
  termtype*  disjunction   = built_in->t_list;
  int        disjunctivity = built_in->t_list->next->value;
  float      erg           = 0.0;
  
  if ( !(disjunction->symb == HEAD_SYMB
	 ||  disjunction->symb == NIL_SYMB)) {
    fprintf(stderr,"  Syntax error: first argument of $disconstr/2 must be a list.\n");
    exit(72);
  }
  if ( disjunctivity == 0 ) {
    printf("Warning: disjunctivity measure of $disconstr/2 is zero.\n");
    erg = 0.0;
  }
  else {
    erg = (KriteriumEinesDisConstrs(disjunction)
	   / (float) (disjunctivity*disjunctivity));
  }
  
  built_in->complexity = erg;
  
#ifdef DEBUG
  printf("KriteriumDesBuiltInsDisconstr = %f\n", erg);  
  fflush(stdout);
#endif
  
  return erg;
}


/*****************************************************************************/
/* KriteriumEinesEquConstrs()                                                */
/* Description:                                                              */
/*****************************************************************************/
float KriteriumEinesEquConstrs( EquconstrList )
     termtype*  EquconstrList;
{
  termtype*  firstArg  = EquconstrList->t_list;
  termtype*  secondArg = EquconstrList->t_list->next;
  float      firstErg;
  float      secondErg;
  float      erg;
  
  switch (firstArg->symb) {
  case HEAD_SYMB:
    firstErg = KriteriumEinesListenEqConstraints(firstArg);
    break;
  case AND_SYMB:
  case OR_SYMB:
    firstErg = KriteriumEinesEquConstrs(firstArg);
    break;
  default:
    fprintf(stderr,"  Syntax error: con/disjunction or list expected.\n");
    fflush(stdout);
    exit(73);
  }
  
  switch (secondArg->symb) {
  case HEAD_SYMB:
    secondErg = KriteriumEinesListenEqConstraints(secondArg);
    break;
  case AND_SYMB:
  case OR_SYMB:
    secondErg = KriteriumEinesEquConstrs(secondArg);
    break;
  default:
    fprintf(stderr," Syntax error: con/disjunction or list expected.\n");
    fflush(stdout);
    exit(74);
  }
  
  switch (EquconstrList->symb) {
  case AND_SYMB:
    erg = firstErg + secondErg;
    break;
  case OR_SYMB:
    erg = (firstErg + secondErg) / 2.0;
    break;
  default:
    fprintf(stderr," Syntax error: con/disjunction expected.\n");
    fflush(stdout);
    exit(75);
  }
  
#ifdef DEBUG
  printf("KriteriumEinesEquConstrs = %f\n", erg);  
  fflush(stdout);
#endif
  
  return erg;
}

/*****************************************************************************/
/* KriteriumDesBuiltInsEquconstr()                                           */
/* Description: Das erste Argument des Built-Ins $Equconstr/2 ist das        */
/*              Constraint, das zweite ist ein Mass fuer den Disjunkt-       */
/*              ivitaetsgrad des Constraints, z.B. die Anzahl der Klauseln,  */
/*              die durch dieses Constraint zu einer zusammengefasst wurden. */
/*****************************************************************************/
float KriteriumDesBuiltInsEquconstr(built_in)
     predtype*  built_in;	
{
  termtype*  constraint_term = built_in->t_list;
  int        disjunctivity   = built_in->t_list->next->value;
  float      erg             = 0.0;
  
  if ( !(constraint_term->symb == AND_SYMB
	 ||  constraint_term->symb == OR_SYMB)) {
    fprintf(stderr," Syntax error: argument of $equconstr/2 must be a con/disjunction.\n");
    exit(76);
  }
  if ( disjunctivity == 0 ) {
    printf("Warning: disjunctivity measure of $equconstr/2 is zero.\n");
    erg = 0.0;
  }
  else {
    erg = (KriteriumEinesEquConstrs(constraint_term)
	   / (float) disjunctivity);
  }
  
  built_in->complexity = erg;
  
#ifdef DEBUG
  printf("KriteriumDesBuiltInsEquconstr = %f\n", erg);  
  fflush(stdout);
#endif
  
  return erg;
}
/* End: SEMANTIC */


/*****************************************************************************/
/* BerechneSortierkriterium()                                                */
/* Description: Berechnet ein float-Kriterium, nach dem die Klauseln sortiert*/
/*              werden koennen um bei der Subsumption moeglichst starke      */
/*              Constraints zu erhalten.                                     */
/*****************************************************************************/
float BerechneSortierkriterium(Clausel)
claustype *Clausel;
{
  predtype *praedikat;
  float erg;

  erg = 0.0;

  /* THIS IS DEBUG CODE (11.06.97) 
  printf("CL: Complexity of Clause ++>%d\n",Clausel->clnr);
  */
  
    
  /* durchlaufe Praedikatenliste der Klausel */
  praedikat = Clausel->p_list;

  while (praedikat != NULL) {
    /* old(06.02.98): erg += KriteriumEinesPraedikates(praedikat); */
    /* Begin: SEMANTIC */
    
    if (praedikat->symb == DISCONSTR) {
      erg += KriteriumDesBuiltInsDisconstr(praedikat);
    } else if (praedikat->symb == EQUCONSTR) {
      erg += KriteriumDesBuiltInsEquconstr(praedikat);
    } else {
      erg += KriteriumEinesPraedikates(praedikat);
    }
    /* End: SEMANTIC */
    praedikat = praedikat->next;
  }
  Clausel->complexity = erg;

  /* Begin: SEMANTIC */
#ifdef DEBUG
  printf("Berechnetes Sortierkriterium = %f\n\n", erg);  
  fflush(stdout);
#endif
  /* End: SEMANTIC */

  return( erg );
}


/*****************************************************************************/
/* tausche()                                                                 */
/* Description:                                                              */
/*****************************************************************************/
static void tausche(sa,i,j)
sortarray *sa;
int i,j;
{
  sortarray tausch;

  tausch.kriterium = sa[i].kriterium;
  tausch.varanz = sa[i].varanz;
  tausch.Clause = sa[i].Clause;
  
  sa[i].kriterium = sa[j].kriterium;
  sa[i].varanz = sa[j].varanz;
  sa[i].Clause = sa[j].Clause;
  
  sa[j].kriterium = tausch.kriterium;
  sa[j].varanz = tausch.varanz;
  sa[j].Clause = tausch.Clause;
  
  /* Ordnung in der Klausel sichern */
  sa[j].Clause->order = j;
  sa[i].Clause->order = i;
}


/*****************************************************************************/
/* SortiereNachKriterium()                                                   */
/* Description:                                                              */
/*****************************************************************************/	
void SortiereNachKriterium(ssort,Clausanz)
sortarray *ssort;
int Clausanz;
{
  int i,j,length_i,length_j;

  /* korrekten Startpunkt holen */
  if (inwasm_flags.clstart == TRUE) {
    cl_reord_start = (cl_reord_start_commline);
  } else {
    if (inwasm_flags.ignoreclstart == TRUE) {
      cl_reord_start = 1;
    } 
  }
  if ((cl_reord_start >= Clausanz) ||
      (cl_reord_start < 1)) {
    fprintf(stderr," Clause reordering error: Wrong starting clause number: %d (max. %d)\n", 
	    cl_reord_start, (Clausanz-1));
    exit(100);
  }
  if (cl_reord_start > 1) {
    printf("        Message: Clause reordering starting clause: %d\n", cl_reord_start);
  }
 
  /* strukturelle Klauseln weiter nach hinten! */
  for (i = 0; i < Clausanz; i++) {
    if ((float) (ssort[i].Clause->nr_lits) == ssort[i].kriterium)
       ssort[i].kriterium += 1; 
    if ((ssort[i].Clause->parsed_rule_type == PQUERY) ||
	(ssort[i].Clause->parsed_rule_type == PRULE) ||
	(ssort[i].Clause->parsed_rule_type == PFACT)) 
      ssort[i].kriterium -= 1000;
  }

  /* lexicographisch Ordnung der Klauseln nach Laenge und Komplexitaet */
  for (i = (cl_reord_start-1); i < Clausanz; i++) {
    for (j = i; j < Clausanz; j++) {
      length_i = ssort[i].Clause->clr_nr_lits;
      length_j = ssort[j].Clause->clr_nr_lits;
      if (length_j < length_i) {
	tausche(ssort,i,j);
      } else if ((length_j == length_i) &&
		 (ssort[j].kriterium < ssort[i].kriterium)) {
	tausche(ssort,i,j);      
      }
    }
  }
}


/*****************************************************************************/
/* make_sortarray()                                                          */
/* Description: allocate and fill an array with pointer in every clause      */
/*              and add two static criteria: ->kriterium ->varanz            */
/*****************************************************************************/
int make_sortarray(Gesamtstruktur)
claustype *Gesamtstruktur;
{
  int i;
  claustype *hpClause;

  if (matrix->m_list) {
    nferror("no sortarray for modules",0);
    return (1);
  }
  if (!ssort) {
    /* Speicher fuer Sortierarray allocieren */
    if((ssort = (sortarray *)cnt_malloc(sizeof(sortarray) * nr_clauses))==NULL) {
      nferror("no memory for sortarray",0);
      return (1);
    }
  }
  else
    /* array already built up 				      */
    /* may be second call by main.c before orbranch_reorder() */
    return (0);

  count_connections(Gesamtstruktur);

  hsort = ssort;
  hpClause = Gesamtstruktur;
	
  while (hpClause) {
    hsort->Clause = hpClause;
    hsort->kriterium = BerechneSortierkriterium(hpClause);
    hpClause = hpClause->next;	
    hsort++;
  }

  /* Sortieren des Feldes */
  SortiereNachKriterium( ssort, nr_clauses );	

  /* die Reihenfolge im lop File wird nach der hier berechneten */
  /* Ordnung umsortiert JB				        */
  if (inwasm_flags.clausreord) {
    if (VERB(9)) {
      printf("\n  i clnr   nrlits  nrbuilt  reordlits  kriterium  ordnung\n");
      printf("           +builts          -builts \n");
      for ( i = 0; i < nr_clauses; i++) 
	printf("%3d %3d    %3d     %3d       %3d        %f    %d\n",
	       i,
	       ssort[i].Clause->clnr,
	       ssort[i].Clause->nr_lits,
	       ssort[i].Clause->nr_built,
	       ssort[i].Clause->clr_nr_lits,
	       ssort[i].kriterium,
	       ssort[i].Clause->order);
      printf("\n");
    } 
    
    for ( i = 0; i < nr_clauses-1; i++) 
      ssort[i].Clause->next = ssort[i+1].Clause;
    if (nr_clauses > 0)
      ssort[nr_clauses-1].Clause->next = NULL;
    matrix->c_list->next = ssort[0].Clause; 
  }

  return (0);
}


/*****************************************************************************/
/* orbranch_reorder()                                                        */
/* Description: reorder the connections of every predicate                   */
/*              use clause->order                                            */
/*****************************************************************************/
void orbranch_reorder(clause)
claustype * clause;
{
  predtype * aktpred;
  predtype * help;
  predlist * connections;
  predlist * hconnect;

  /* go through all clauses, all literals, all connections lists */
  while (clause) {
    aktpred = clause->p_list;

    while (aktpred) {
      connections = aktpred->wu;

      while (connections) {
	hconnect = connections->next;

	while (hconnect) {
	  /* now compare */
	  if (connections->pr->claus->order > hconnect->pr->claus->order) {
	    /* change order */
	    help = connections->pr;
	    connections->pr = hconnect->pr;
	    hconnect->pr = help;
	  }
	  hconnect = hconnect->next;
	}
	connections = connections->next;
      }
      aktpred = aktpred->next;
    }
    clause = clause->next;
  }
}


/*****************************************************************************/
/* KriteriumEinesKontrapositivs()                                            */
/* Description:                                                              */
/*****************************************************************************/
float KriteriumEinesKontrapositivs(cp)
claustype *cp;
{
  predtype *head;
  predtype *pred;
  symbollist *vlist;
  float erg;
  float old;

  /* print_lop_clause(cp,cp->p_list,cp->delpos,1,0); 
     printf("\n"); */

  head = cp->p_list;
  erg = 0.0;

  /* PJ: get local and global var_dependency (= #var) */
  pred = head->next;
  head->var_dependency=0;
  while (pred != NULL) {

    


    vlist = get_all_var(pred->t_list);
    pred->var_dependency  = nr_var_in_lit(head->t_list,vlist);	
    head->var_dependency += pred->var_dependency;
    pred = pred->next;
  }

  /* weight formulae for reordering subgoals (most restrictive tail pred first) 
     complexity' = at * complexity +
     av * relative local v_dep +
     ac * (1 - relative no of connections) 
     */

  pred=head->next;
  while ( pred != NULL ) {
    
    /*
      THIS IS DEBUG CODE (11.06.97)
    printf("cl_reord: KP -> Clausel: %d / Praedikat : %d\n", cp->clnr, pred->litnr);
	  printf("  : Old Complexity       : %f\n", pred->complexity);
	  printf("  : sgreord_at           : %f\n", sgreord_at);
	  printf("  : pred->var_dependency : %d\n", pred->var_dependency);
	  printf("  : head->var_dependency : %d\n", head->var_dependency);
	  printf("  : sgreord_av           : %f\n", sgreord_av);
	  printf("  : sgreord_ac           : %f\n", sgreord_ac);
	  printf("  : nr_connections       : %d\n", nr_connections);
	  printf("  : cp->connections      : %d\n", cp->connections);
	  */

    old=pred->complexity;
    pred->complexity = 
      sgreord_at * pred->complexity                            /* alpha tc */
      + sgreord_av * (float) pred->var_dependency /
      (float) (head->var_dependency + 1)
      /* alpha v */
      + sgreord_ac * ( 1.0 - (float) pred->nr_connections /
		       (float) (cp->connections + 1));
      /* alpha conn */
    erg+=pred->complexity;


    /*
      THIS IS DEBUG CODE (11.06.97)
    printf("  : New Complexity         : %f\n", pred->complexity);
    printf("cl_reordering: KRITERIUM EINES KP: clause %d : lit %d : old %f : new %f\n",
	   cp->clnr, pred->litnr, old, pred->complexity);*/


    /*
      printf("erg %f pc %f (pc %f pvd %f hvd %f pc %d cc %d)
      \nat-t %f av-t %f ac-t %f\n",\
      erg, pred->complexity,old,
      (float) pred->var_dependency,(float) head->var_dependency,\
      pred->nr_connections,cp->connections,sgreord_at*old,\
      sgreord_av * (float) pred->var_dependency /
      (float) (head->var_dependency + 1),\
      sgreord_ac * ( 1.0 - (float) pred->nr_connections /
      (float) (cp->connections + 1))\
      );
      */

    /*
      printf("pc %f (pc %f pvd %f conn %d)\n",\
      pred->complexity,old,(float) pred->var_dependency,\
      pred->nr_connections\
      );
      */
	
    pred = pred->next;
  }
  cp->complexity = erg;
  
  return ( erg );
}


/*****************************************************************************/
/* gen_subgoal_weights()                                                     */
/* Description:                                                              */
/*****************************************************************************/
void gen_subgoal_weights(cl)
claustype *cl;
{
  claustype *hcl;
  claustype *hcp;
  float erg;

  hcl = cl;

  while (hcl) {
    erg += KriteriumEinesKontrapositivs(hcl);
    hcp = hcl->fann;
    while(hcp) {
      erg += KriteriumEinesKontrapositivs(hcp);
      hcp = hcp->next;
    }
    hcl = hcl->next;
  }
}


/******************************************************************************/
/* count_connections()                                                        */
/* Description:                                                               */
/******************************************************************************/
void count_connections (cll)
claustype *cll;
{
  predtype *pp;
  predlist *wu;

  /* 11.06.97 
  if (cll)
     cll->connections = 0; */

  while (cll) {
    cll->connections = 0;   /*  11.06.97 */
    for (pp = cll->p_list; pp; pp = pp->next) {
      pp->nr_connections = 0;
      for (wu = pp->wu; wu; wu = wu->next)
	pp->nr_connections += 1;
      cll->connections += pp->nr_connections;
      /* printf("\n\tpp%d",pp->nr_connections); */
    }
    /* printf("\n\tclause%d : %d",cll->clnr, cll->connections); */
    cll = cll->next;
  }
}


/*****************************************************************************/
/* nr_var_in_lit()                                                           */
/* Description: Liefert f"ur tlist, die Anzahl von Vorkommen von Variablen   */
/*              aus vlist.                                                   */
/*****************************************************************************/
int nr_var_in_lit(tlist,vlist)
termtype *tlist;
symbollist *vlist;
{
  symbollist *hlpvlist;

  if (! tlist || ! vlist) return(0);
  else if (tlist->type == VARIABLE || tlist->type == SVARIABLE) {
    hlpvlist = vlist;
    while (hlpvlist) {
      if (hlpvlist->symbol == tlist->symb) /*hlpvlist->value == tlist->value)*/
	return(1 + nr_var_in_lit(tlist->next,vlist));

      hlpvlist = hlpvlist->next;
    }
    return(nr_var_in_lit(tlist->next,vlist));
  }
  else  return(nr_var_in_lit(tlist->next,vlist) +
	       nr_var_in_lit(tlist->t_list,vlist));
}

