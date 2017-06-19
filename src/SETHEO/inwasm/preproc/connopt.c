/******************************************************************************
;;; MODULE NAME : connopt.c
;;;
;;; PARENT      : generator.c
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)connopt.c	15.1 01 Apr 1996
;;;
;;; AUTHOR      : Mohamed Salah, Hamdi
;;;
;;; DATE        : 10.2.93 
;;;
;;; DESCRIPTION : delete connections that produce taut. or subs. resolvents 
;;;
;;; REMARKS     : 30.12.93	Hamdi	extern.h
;;;		  02.03.94	Hamdi	DELCONN,RDELCONN
;;;               14.06.95      PJ      do_res: reject some subsumptions
;;;               07.04.97      RNZ     recode code
;;;
******************************************************************************/

#include "extern.h"             

/*****************************************************************************/
/* global variables                                                          */
/*****************************************************************************/
int nr_del_conn = 0;   /* Anzahl durch Optimierung gel"oschten Konnektionen. */
int nr_resolvents;     /* >= nr_clauses!                                     */
claustype *rll = NULL;
int clnr_offset = 1000;          /* offset for clnr_remapping */
int mapping_of_clnr = FALSE;

/*****************************************************************************/
/* functions (preproc/ connopt.c)                                            */
/*****************************************************************************/
void opt_connections();
predtype *ren_get_red_pred();
claustype *do_resolution();
predtype *get_other_conn_lit();
predtype *get_red_pred2();
int opt_clause_conn();
int is_subsumable();
termtype *replace_in_term2();
claustype *copy_clause();
predtype *copy_predicate();
termtype *copy_termlist();
void rename_terms();
void rename_variables();

/*****************************************************************************/
/* opt_connections()                                                         */
/* Description: Hauptprogramm zur Elimination von Konnectionen, die tauto-   */
/*              logische und subsumierbare Resolventen erzeugen.             */
/*              Return-Wert: gesamte Anzahl der durchgef"uhrten Resolutionen.*/
/*              Nebeneffekt: die globale Var. `nr_del_conn' enth"alt die     */
/*              Anzahl der durch diese Optimierung gel"oschten Konnektionen. */
/*****************************************************************************/
void opt_connections(cll,compare_mode)
claustype *cll;
int compare_mode;
{
  claustype *hp1,*hp2;
  int erg = 0;
  int maxlength, reslength, i,j;
  
  subs_unif_counter = 0;
  subs_max_counter = 20000;
  
  /* Print-out the deleted connections */
  printf("        Message: Deleted connections :\n                 ");
  
  /* Initialize */
  nr_del_conn = 0;
  nr_resolvents = nr_clauses;
  
  /* jump over default query */
  hp1 = cll->next;
  
  maxlength = 0;
  while(hp1) {
    if (hp1->nr_lits > maxlength)
      maxlength = hp1->nr_lits;
    hp1 = hp1->next;
  }

  for (reslength = 0; reslength <= ((maxlength * 2) - 2); reslength++) {
    /* resolviere alle Tail-Literale mit allen Headliteralen */     
    for (i = 0; 
	 ((i < nr_clauses) && (subs_unif_counter <= subs_max_counter));
	 i++) {
      hp1 = ssort[i].Clause;
      for (j = 0; 
	   ((j < nr_clauses) && (subs_unif_counter <= subs_max_counter));
	   j++) { 
	hp2 = ssort[j].Clause;
	if ((hp1->nr_lits + hp2->nr_lits - 2) == reslength) {
	  erg += opt_clause_conn(hp1, hp2, cll, compare_mode);
	}
      }
    }
  }

  printf("\n");
  nr_links_tested = erg;
}


/*****************************************************************************/
/* opt_clause_conn()                                                         */
/* Description: Att: If cl1 and cl2 are the same clauses then copying of     */
/*              the clause2 and a remapping of clnr in this clause2 is       */
/*              necessary to distinguish the predicates of cl1 and cl2       */
/*****************************************************************************/
int opt_clause_conn(clause1, clause2, cll, compare_mode)
claustype *clause1, *clause2;
claustype *cll; 	 
int compare_mode; 
{
  predtype *pp, *conn_pp;
  claustype *resolvente = NULL; 
  claustype *copied_clause2 = NULL;
  int returnValue = 0;
  
  /* RDELCONN: nur Konnektionen zwischen Regeln "uberpr"ufen */ 

  if ((compare_mode == RDELCONN) &&
      (clause1->nr_lits == 1 || clause2->nr_lits == 1)) {
    return returnValue;
  }
  
  if (pos_lits(clause1) > 1 || pos_lits(clause2) > 1) {
    return returnValue;
  }

  if (clause1->clnr == clause2->clnr) {
    /* copy the second clause, if both clauses are the same */
    /* no copying of fanned clauses , just this clause, no recursion */
    copied_clause2 = copy_clause(clause2, FALSE, FALSE); 
    map_clause_number(copied_clause2, (clnr_offset+clause2->clnr));
  }
  
  /* Optimiere Konnektionen clause1 <--> clause2 */
  pp = clause1->p_list;
  
  while (pp) {
    conn_pp = clause2->p_list;
    while (conn_pp) {	  
      /* Resolvente bilden */

      if ((conn_pp->sign == '+') &&
	  (pp->sign == '-') && 
	  (pp->symb == conn_pp->symb) &&
	  (subs_unif_counter < subs_max_counter)) {

	  resolvente = do_resolution(clause1,clause2,copied_clause2, pp, conn_pp);	
      }
      else {
	resolvente = NULL;
      } 
      
      if (resolvente) {
	returnValue++;

	/*
	  printf("\n############## (%d.%d -> %d.%d) #############\n",
	  clause1->clnr,pp->litnr,clause2->clnr,conn_pp->litnr); 
	  print_lop_clause(clause1,clause1->p_list,
	  clause1->delpos,0,0);
	  print_lop_clause(clause2,clause2->p_list,
	  clause2->delpos,0,0);
	  printf("\n<---------- RESOLVENTE ---------->");
	  print_lop_clause(resolvente,resolvente->p_list,
	  resolvente->delpos,0,0);
	*/
	
	/* Resolvente testen */
	if  ((FindeTautologie(resolvente) == TAUTOLOGY) ||
	     is_subsumable(cll,resolvente) || 
	     is_subsumable(rll,resolvente)) {
	  /* Konnektion (pp -- conn_pp) l"oschen */		  
	  delete_p(conn_pp, pp);
	  delete_p(pp, conn_pp);
	  pp->missing_link = 1;
	  conn_pp->missing_link = 1;
	  nr_del_conn++ ;
	  printf("(%d.%d %d.%d) ", 
		 clause1->clnr,pp->litnr,clause2->clnr,conn_pp->litnr);
	  
 	  /*
	    fprintf(del_file,"%d %d %d %d\n", 
	    clause1->clnr,pp->litnr,clause2->clnr,conn_pp->litnr);
	    fprintf(del_file,"%d %d %d %d\n", 
	    clause2->clnr,conn_pp->litnr,clause1->clnr,pp->litnr);	
	    printf("\n================================================\n ");
	  */
		  
	  if (nr_del_conn % 5 == 0) 
	    printf("\n                 ");                           
	}
	else {	 
	  /* Resolvente einfuegen */		  
	  resolvente->next = rll;
	  rll = resolvente;
	}
      }
      conn_pp = conn_pp->next;
    } 
    pp = pp->next;
  }
  return returnValue;
}


/*****************************************************************************/
/* do_resolution()                                                           */
/* Description: f"uhrt die Resolution von cl1 und cl2 "uber die Literale     */
/*              lit1 (aus cl1) und lit2 (aus cl2) durch.                     */
/*               Return-Wert: - Resolvente.                                  */
/*                            - NULL: falls Resolution nicht m"oglich.       */
/*****************************************************************************/
claustype *do_resolution(cl1,cl2,copied_cl2,literal1,literal2)
claustype *cl1,*cl2, *copied_cl2;
predtype *literal1,*literal2;
{
  predtype *orglit1, *orglit2, *hpp, *newlit; 
  predtype *sigma; 		/* Mgu */
  claustype *erg = NULL; 	/* Ergebnis */
  predtype *litlist = NULL; 	/* Liste der Literale von erg */
  int i = 1; 			/* f"ur Numerierung der Literale in erg */
  claustype *clause1 = cl1;     /* new pointers for clause1 and clause2 */
  claustype *clause2 = cl2;     /* because of clnr-mapping */
  predtype *lit1 = literal1;    /* new pointers for lit1 and lit2 */
  predtype *lit2 = literal2;    /* because of literal copying */
  predtype *res_plist = NULL;
  termtype *res_tlist = NULL;

  /* Initialisierung */
  mapping_of_clnr = FALSE;
  if (((!lit1) || (!lit2)) ||
      ((lit1->symb != lit2->symb) || (lit1->sign == lit2->sign)) ||
      /* lit1 ist Literal von clause1 / lit2 ist Literal von clause2 */
      ((lit1->claus != clause1) || (lit2->claus != clause2))) {
    return(NULL);
  }

  /* Falls Klausel 1 und 2 identisch, so wird die 2.Klausel
     kopiert, und deren Klauselnummer rekursiv (d.h. auch in Pred und Term) umbenannt. */
  if (copied_cl2 != NULL) {
    clause2 = copied_cl2;
    mapping_of_clnr = TRUE;
 
    /* Klausel2 is now copied. now copy conn_pp (alias lit2) */
    /* res_plist as help pointer */
    res_plist = clause2->p_list;
    while (res_plist) {
      if (res_plist->litnr == lit2->litnr) {
	lit2 = res_plist;
      }
      res_plist = res_plist->next;
    }
  }
  

  /* Allgemeinsten Unifikator berechnen */
  rearrange();
  
  if (unify(lit1->t_list, lit2->t_list, UNIFY_CONNOPT) != SUCCESS) {    
    return(NULL);    
  } else {
    sigma = GenerateConstraint(clause1, CONNOPT, TRUE);
    /* alle variablen aus klausel2 umbenennen (New_) 
      nur falls klausel1 != klausel2 */
    rename_variables(sigma->t_list, (int)clause2->clnr); 
  }

  /* Resolvente konstruieren: sigma( clause1 - lit1) + sigma( clause2 - lit2) */ 
  if ((erg = (claustype *) cnt_malloc(sizeof(claustype))) == NULL) {
    printf("\nERROR (connopt.c - do_resolution() ):");
    printf(" Memory can not be allocated.\n");
    exit(1);
  }
  
  /* sigma( clause1 - lit1 ) berechnen */
  orglit1 = clause1->p_list;
  
  while (orglit1) { 
      /* lit1 und query nicht in Resolvente! */     
    if ((orglit1 != lit1) && (orglit1->symb)) {
      newlit = get_red_pred2(orglit1, sigma, erg);
      
      if (!litlist) 
	litlist = newlit;
      else {
	hpp = litlist;
	while (hpp->next)
	  hpp = hpp->next;
	hpp->next = newlit;
      }	  
    }      
    orglit1 = orglit1->next;
  }
  
  /* sigma( clause2 - lit2 ) berechnen */
  orglit2 = clause2->p_list;
  while (orglit2) {     
    /* lit2 und query nicht in Resolvente! */     
    if (orglit2 != lit2 && orglit2->symb) {
      newlit = ren_get_red_pred(orglit2, i++, sigma, erg, clause2->clnr);

      if (!litlist) 
	litlist = newlit;
      else {
	hpp = litlist;
	while (hpp->next)
	  hpp = hpp->next;
	hpp->next = newlit;
      }	  
    }
    orglit2 = orglit2->next;
  }
  
  /* falls clause1 oder clause2 QUERY, dann Resolvente auch */ 
  if ((clause1->p_list->symb == 0) || (clause2->p_list->symb == 0)) {     
    /* query-Literal */  
    if ((hpp = (predtype *) cnt_malloc(sizeof(predtype))) == NULL) {
      printf("\nERROR (connopt.c - do_resolution() ):");
      printf(" Memory can not be allocated.\n");
      exit(1);
    }
    hpp->symb  = 0;
    hpp->sign  = 0;
    hpp->arity = 0;
    hpp->litnr = 1;
    hpp->claus = erg;
    hpp->wu    = NULL;
    hpp->next  = litlist;
    hpp->t_list = NULL;

    litlist = hpp;
    /* litnr in litlist verschieben sich */      
    hpp = litlist->next;
    while (hpp) {
      hpp->litnr++; 
      hpp = hpp->next;
    }
    i += 1;
  }
  
  /* leere Resolvente */
  if (!litlist) {
    warning("","Preprocessing derived empty clause!"); 
    return(NULL);
  }
  
  /* Ergebnis */          /* ATTN: Klaus - reduziere Size - eigene struct */
  erg->clnr     = ++nr_resolvents; /* PJ - darf ich ? */
  erg->rescl1   = clause1->clnr;       /* PJ: speichere clnr der Clauses fuer */
  erg->rescl2   = clause2->clnr;       /*     diese Resolvente */
  erg->modul    = clause1->modul;
  erg->lineno   = 0;
  erg->constr   = NULL; 
  erg->fann     = NULL;
  erg->next     = NULL;
  erg->p_list   = litlist;
  erg->delpos   = PRULE;
  erg->ddepth   = 2;
  erg->dinf     = clause1->nr_lits  + clause2->nr_lits - 2;
  erg->nr_built = clause1->nr_built + clause2->nr_built;
  erg->nr_lits  = clause1->nr_lits  + clause2->nr_lits - 2;


  /* Variablen aus Klausel 2 (in Resolvente = erg) umbenennen. */
  res_plist = erg->p_list;

  /* skip default query */
  if (res_plist->symb == 0) {
    res_plist = res_plist->next;
  }

  while(res_plist) {
    res_tlist = res_plist->t_list;
    rename_terms(res_tlist, (int)erg->clnr); 
    res_plist = res_plist->next;
  }	      
  /* resolvente->rescl2 is not correct if clnr of clause2 was changed */
  if (mapping_of_clnr) {
    erg->rescl2 = erg->rescl2 - clnr_offset;
  }

  return erg;
}


/*****************************************************************************/
/* ren_get_red_pred()                                                        */
/* Description: liefert eine Kopie von pr mit:                               */
/*              - Umbenennung der Variablen: V --> V<ren_nr> und             */
/*              - Anwendung von sigma = constr auf die Argumentliste.        */
/*                Wie get_red_pred() aber mit renaming!                      */
/*****************************************************************************/
predtype *ren_get_red_pred(pr,nr,constr,clause,ren_nr)
predtype *pr;
int nr;
predtype *constr;
claustype *clause;
int ren_nr;
{
  predtype *erg;
  termtype *term,*left,*right;

  if ((erg = (predtype *) cnt_malloc(sizeof(predtype))) == NULL) {
    printf("\nERROR (connopt.c - ren_get_red_pred() ):");
    printf(" Memory can not be allocated.\n");
    exit(1);
  }

  erg->symb  = pr->symb;
  erg->sign  = pr->sign;
  erg->arity = pr->arity;
  erg->litnr = nr;
  erg->claus = clause;
  erg->wu    = NULL;
  erg->next  = NULL;
  
  term = copy_terms(pr->t_list, ALL_TERM);
  rename_variables(term, ren_nr);

  
  if (constr) {
    /* linke und rechte Seite der Substitution (sigma) */     
    left = constr->t_list->t_list;
    right = constr->t_list->next->t_list;
    
    /* einsetzen */      
    while ((left) && (right)) {
      term = replace_in_term2(term, left, right);	  
      left  = left->next;
      right = right->next;
    }
  }
  erg->t_list = term;
  return erg;
}


/*****************************************************************************/
/* get_red_pred2()                                                           */
/* Description:                                                              */
/*****************************************************************************/
predtype *get_red_pred2(pr,constr,clause)
predtype *pr;
predtype *constr;
claustype *clause;
{
  predtype *erg;
  termtype *term,*left,*right;

  if ((erg = (predtype *) cnt_malloc(sizeof(predtype))) == NULL) {
    printf("\nERROR (connopt.c - ren_get_red_pred2() ):");
    printf(" Memory can not be allocated.\n");
    exit(1);
  }

  erg->symb  = pr->symb;
  erg->sign  = pr->sign;
  erg->arity = pr->arity;
  erg->wu    = pr->wu;
  erg->litnr = pr->litnr;
  erg->claus = pr->claus;
  erg->next  = NULL;
  erg->kp    = clause;
  erg->missing_link = pr->missing_link;
  
  term = copy_terms(pr->t_list,ALL_TERM);
  if (constr) {
    left = constr->t_list->t_list;
    right = constr->t_list->next->t_list;
    while (left && right) {
      term = replace_in_term2(term,left,right);

      left  = left->next;
      right = right->next;
    }
  }
  
  erg->t_list = term;
  
  return(erg);
}


/*****************************************************************************/
/* replace_in_term2()                                                         */
/* Description: Ersetzt in term jedes Vorkommen von left(Variable) durch     */
/*              eine Kopie von right.                                        */
/*****************************************************************************/
termtype *replace_in_term2(term, left, right)
termtype *term,*left,*right;
{
  termtype *ht,*hn;
  
  if (!term) 
    return NULL;
  
  if ((term->type == VARIABLE) || (term->type == SVARIABLE)) {
      hn = term->next;
      if ((term->symb == left->symb) && (term->value == left->value)) {
	ht = copy_terms(right, ONE_TERM);
	
	term = ht;
      }
      term->next = replace_in_term2(hn, left, right);
  }
  else {
    term->next = replace_in_term2(term->next, left, right);
    term->t_list = replace_in_term2(term->t_list, left, right);
  }
  
  return term;
}


/*****************************************************************************/
/* pos_lits()                                                                */
/* Description: number of positive literals in clause                        */
/*****************************************************************************/
int pos_lits(clause)
claustype *clause;
{
  predtype *pp;
  int returnValue = 0;
  
  for (pp = clause->p_list; pp; pp = pp->next) {
    if (pp->sign == '+')
      returnValue++; 
  }
  return returnValue;
}


/*****************************************************************************/
/* rename_terms()                                                            */
/* Description: rename claus_number in term if                               */
/*              term->clause_number = checkvalue                             */
/*****************************************************************************/
void rename_terms(termlist, resolvente_nr) 
     termtype *termlist;
     int resolvente_nr;
{
  if (!termlist)
    return;
  if (termlist->next)
    rename_terms(termlist->next, resolvente_nr);
  if (termlist->t_list)
    rename_terms(termlist->t_list, resolvente_nr);
  termlist->clause_number = resolvente_nr;
}

/*****************************************************************************/
/* rename_variables()                                                        */
/* Description: rename variable if termlist->clause_number = checkvalue      */
/*****************************************************************************/
void rename_variables(termlist, checkvalue) 
     termtype *termlist;
     int checkvalue;
{
  char name[80];

  if (!termlist)
    return;
  if (termlist->next)
    rename_variables(termlist->next, checkvalue);
  if (termlist->t_list)
    rename_variables(termlist->t_list, checkvalue);
  if (termlist->clause_number == checkvalue) {
    /* `New_`-Prefix */
    if (termlist->type == VARIABLE || termlist->type == SVARIABLE) {
      sprintf(name,"New_%s",symbtab[termlist->symb].name);
      termlist->symb = enter_name(enter_token(name), symbtab[termlist->symb].type,0,0);
    } 
  }
}

/*****************************************************************************/
/* copy_clause()                                                             */
/* Description: copys a clause and returns the copy of the clause            */
/*              fanning == TRUE - fanned clauses are copied recursively      */
/*              recursive == TRUE - clause-list ist copied recursivly        */
/*              Be aware : Constraints and Modul are not copied, just        */
/*              referenced                                                   */
/*****************************************************************************/
claustype *copy_clause(org_clause, fanning, recursive)
     claustype *org_clause;
     int fanning;
     int recursive;
{
  claustype *copied_clause;

  /* Allocate memory for clause */ 
  if ((copied_clause = (claustype *) cnt_malloc(sizeof(claustype))) == NULL) {
    printf("\nERROR (connopt.c - copy_clause() ):");
    printf(" Memory can not be allocated.\n");
    exit(1);
  }
  
  /* copy attributes */
  copied_clause->clnr = org_clause->clnr;
  copied_clause->rescl1 = org_clause->rescl1;
  copied_clause->rescl2 = org_clause->rescl2;
  copied_clause->nr_lits = org_clause->nr_lits;
  copied_clause->clr_nr_lits = org_clause->clr_nr_lits;
  copied_clause->nr_built = org_clause->nr_built;
  copied_clause->poslits = org_clause->poslits;
  copied_clause->lineno = org_clause->lineno;
  copied_clause->delpos = org_clause->delpos;
  copied_clause->nrvars = org_clause->nr_vars;
  copied_clause->nrstrvars = org_clause->nrstrvars;
  copied_clause->order = org_clause->order;
  copied_clause->ddepth = org_clause->ddepth;
  copied_clause->dinf = org_clause->dinf;
  copied_clause->del = org_clause->del;
  copied_clause->connections = org_clause->connections;
  copied_clause->max_termdepth = org_clause->max_termdepth;
  copied_clause->max_termsize = org_clause->max_termsize;
  copied_clause->flag_nalloc = org_clause->flag_nalloc;
  copied_clause->flag_constraints = org_clause->flag_constraints;
  copied_clause->complexity = org_clause->complexity;
  copied_clause->kriterium = org_clause->kriterium;
  copied_clause->nr_vars = org_clause->nr_vars;
  /* just reference */
  copied_clause->constr = org_clause->constr;
  copied_clause->p_list = copy_predicate(org_clause->p_list, copied_clause);
  if (fanning == TRUE) {
    if (org_clause->fann != NULL) {
      copied_clause->fann = copy_clause(org_clause->fann, fanning, recursive);
    } else {
      copied_clause->fann = NULL;
    }
  } else {
    /* just reference */
    copied_clause->fann = org_clause->fann;
  }
  /* just refernce */
  copied_clause->modul = org_clause->modul;
  
  if (recursive == TRUE) {
    if (org_clause->next != NULL) {
      copied_clause->next = copy_clause(org_clause->next, fanning, recursive);
    } else {
      copied_clause->next = NULL;
    }
  } else {
    /* just reference */
    copied_clause->next = org_clause->next;
  }
  
  return(copied_clause);
}

/*****************************************************************************/
/* copy_predicate()                                                          */
/* Description: copys a predicate_list recursively                           */
/*              Be aware of just referenced fields                           */
/*****************************************************************************/
predtype *copy_predicate(org_pred, copied_clause) 
predtype *org_pred;
claustype *copied_clause;
{
  predtype *copied_pred;

  /* Allocate memory for predicate */ 
  if ((copied_pred = (predtype *) cnt_malloc(sizeof(predtype))) == NULL) {
    printf("\nERROR (connopt.c - copy_predicate() ):");
    printf(" Memory can not be allocated.\n");
    exit(1);
  }
  
  /* copy attributes */
  copied_pred->symb = org_pred->symb;
  copied_pred->flag = org_pred->flag;
  copied_pred->sign = org_pred->sign;
  copied_pred->arity = org_pred->arity;
  copied_pred->litnr = org_pred->litnr;
  copied_pred->nr_connections = org_pred->nr_connections;
  copied_pred->var_dependency = org_pred->var_dependency;
  copied_pred->missing_link = org_pred->missing_link;
  copied_pred->complexity = org_pred->complexity;
  copied_pred->literal_flag = org_pred->literal_flag;
  copied_pred->subgoal_flag = org_pred->subgoal_flag;
  copied_pred->claus = copied_clause;

  /* just reference - start */
  copied_pred->kp = org_pred->kp;
  copied_pred->s_list = org_pred->s_list;
  copied_pred->wu = org_pred->wu;
  copied_pred->slistnext = org_pred->slistnext;
  /* just reference - end */

  if (org_pred->next != NULL) {
    copied_pred->next = copy_predicate(org_pred->next);
  } else {
    copied_pred->next = NULL;
  }
  if (org_pred->t_list != NULL) {
    copied_pred->t_list = copy_termlist(org_pred->t_list);
  } else {
    copied_pred->t_list = NULL;
  }
 
  return (copied_pred);
}

/*****************************************************************************/
/* copy_termlist()                                                           */
/* Description: copys a term_list recursively                                */
/*              Be aware of just referenced fields                           */
/*****************************************************************************/
termtype *copy_termlist(org_term, copied_pred) 
termtype *org_term;
predtype *copied_pred;
{
  termtype *copied_term;

  /* Allocate memory for term */ 
  if ((copied_term = (termtype *)cnt_malloc(sizeof(termtype))) == NULL) {
    printf("\nERROR (connopt.c - copy_termlist() ):");
    printf(" Memory can not be allocated.\n");
    exit(1);
  }
  
  /* copy attributes */
  copied_term->symb = org_term->symb;
  copied_term->type = org_term->type;
  copied_term->variable_number = org_term->variable_number;
  copied_term->clause_number = org_term->clause_number;
  copied_term->value = org_term->value;
  copied_term->grflag = org_term->grflag;
  copied_term->literal = copied_pred;
  if (org_term->t_list != NULL) {
    copied_term->t_list = copy_termlist(org_term->t_list, copied_pred);
  } else {
    copied_term->t_list = NULL;
  }
  if (org_term->next != NULL) {
    copied_term->next = copy_termlist(org_term->next, copied_pred);
  } else {
    copied_term->next = NULL;
  }

  return(copied_term);
}


/*****************************************************************************/
/* is_subsumable()                                                           */
/* Description: pr"uft ob clause von einer Klausel der Liste cll subsumiert  */
/*              wird. Return-Wert:  JA  : TRUE / NEIN : FALSE                */
/*****************************************************************************/
int is_subsumable(cll,clause)
claustype *cll, *clause;
{
  while (cll) {

    if ((cll->clnr) && 
	(cll->parsed_rule_type != PRULE) && 
	(cll->parsed_rule_type != PQUERY) && 
	(clause->parsed_rule_type != PRULE) && 
	(clause->parsed_rule_type != PQUERY)) {
      if (clause_s_constr(cll, clause, CONNOPTC) == 1) {
    
	/* 
	   don't return TRUE if the subsuming resolvent and
	   the subsumed resolvent have the same parent clauses and 
	   if the resolvent is not a self resolvent 
	*/
	
	if (!((cll->clnr      >= nr_clauses)     && /* cll is resolvent */
	      (clause->clnr   >= nr_clauses)     && /* clause is resolvent */ 
	      (clause->rescl1 != clause->rescl2) && /* different parents */
	      (cll->rescl1    == clause->rescl1) && 
	      (cll->rescl2    == clause->rescl2))) {
	  return TRUE;
	}	      
      }
    }   
    cll = cll->next;
  }
  return FALSE; 
}


/*****************************************************************************/
/* get_other_conn_lit()                                                      */
/* Description: Liefert den einzigen Konnektionspartner von lit in clause.   */
/*              Falls lit mehrere Konnektionspartner hat dann NULL.          */
/*****************************************************************************/
predtype *get_other_conn_lit(lit,clause)
predtype *lit;
claustype *clause;
{
  predlist *wul = lit->wu;
  int nr = 0;
  predtype *erg = NULL;
  
  while (wul) {
    if (wul->pr->claus == clause) {
      nr++;
      erg = wul->pr;
    }       
    wul = wul->next;
  }
  
  /* nur 1 Konnektionspartner ? */ 
  if (nr == 1) {
    return(erg);
  } else {
    return(NULL);
  }

}
