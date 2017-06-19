/**************************************************************************
;;; MODULE NAME : oconstr.c
;;;
;;; PARENT      : main.c 
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)oconstr.c	15.1 01 Apr 1996
;;;
;;; AUTHOR      : Mohamed Salah, Hamdi 
;;;
;;; DATE        : 15.11.92 
;;;
;;; DESCRIPTION : generate partial overlap constraints 
;;;
;;; REMARKS     :
;;;		30.12.93	Hamdi	extern.h,get_all_var(),
;;;					append_with_comp(),	
;;;					concat_with_comp()
;;;		19.01.94	hamdi	conn_with_other() modified
;;;		03.03.94	hamdi	best search
;;;		16.03.94	Hamdi	ordering of new clauses
;;;		30.03.94	Hamdi	taut constr for first
;;;					clause of overlap
;;;             10.07.96        RZ      removed the old renaming
;;;
***************************************************************************/

/*********************** INCLUDE ******************************************/

#include "extern.h"            /* symb.h: types.h, defines.h, symbtab ... */ 
#include "otypes.h"
#include "defines.h" 


/*********************** GLOBALE DATEN ************************************/

deltype *dellist = NULL;      
static int newlitcnt = 1;      
static int stop_overlap = 0; 	
static int generated_contrapositives = 0;
static int deleted_contrapositives = 0;

/*********************** PROTOTYPEN ***************************************/

claustype *gen_overl_constr();
claustype *overlap_constr();
claustype *concat_two_cl();
claustype *contrap_overlap();
void get_first_contrap();
claustype *get_second_contrap();
claustype *get_third_contrap();
termtype *replace_in_term();
predtype *get_red_pred();
predtype *get_new_pred();
predtype *get_red_tail();
predtype *get_red_clist();
predtype *get_red_constr();
symbollist *get_all_var();
symbollist *append_with_comp();
symbollist *concat_with_comp();
predtype *getsigma();
predtype *add_to_list();     
void ren_constr();  
void conn_with_other(); 
void print_overl_del();     
int constr_length();
void save_clnr();



/*********************** FUNKTIONEN ***************************************/
/*---------------------- gen_overl_constr --------------------------------*/

/* Hauptprogramm fuer overlap constraints.
   mode: gibt die Maximal-Laenge der zu generierenden Constraints an
   */
/*---------------------- gen_overl_constr --------------------------------*/

claustype *gen_overl_constr(Gesamtstruktur,mode,lopout)
claustype *Gesamtstruktur;
int mode;
int lopout; 
{
  claustype *newcl;

  int i = 1;
  dellist = NULL; 
 
  for (i = 0; i <= mode; i++)
    {
      /*********************************/
      /* Solange es neue Kontrap. gibt */
      /*********************************/
      newcl = overlap_constr(Gesamtstruktur,i);
      Gesamtstruktur = concat_two_cl(Gesamtstruktur,newcl);

      while (newcl)  
	{
	  /*******************************************/
	  /* vergleiche bisherige Klauseln paarweise */
	  /*******************************************/
	  newcl = overlap_constr(newcl,i);
	  Gesamtstruktur = concat_two_cl(Gesamtstruktur,newcl);
	}
    }
  print_overl_del(); 
  
  return( Gesamtstruktur );
}


/*---------------------- overlap_constr ----------------------------------*/

   /* Erzeugt overlap constraints fuer die eingegebene Liste von Klauseln.
      Die Kps werden nach folgendem Schema paarweise ueberlappt:

	p(s1) :- q(t1), rest1.
	p(s2) :- q(t2), rest2.
	----------------------
	p(s1) :- q(t1),h(..).
	h(..) :- rest1.

	p(s2) :- q(t2), rest2: [s2,t2] =/= [s,t].
	h(..) :- rest2: [s2,t2] == [s,t].	  
   */
/*---------------------- overlap_constr ----------------------------------*/

claustype *overlap_constr(cllist,mode)
claustype *cllist;
int mode;
{
  claustype *erg = NULL;                     /* Ergebnis */
  claustype *newcl = NULL;                   /* Zeiger auf neue Kp */
  claustype *firstcl,*othercl;               /* Hilfspointer */
  claustype *fann1,*fann1other,*fann2;       /* Hilfspointer */
  
  /************************/
  /* Klauseln vergleichen */
  /************************/
  
  firstcl = cllist;
  while (firstcl)
    {      
      if (firstcl->nr_lits > 2)
	{
          /***************************************/
	  /* Klausel mit eignen fann-komponenten */
          /***************************************/
	  fann1 = firstcl->fann;
	  while (fann1)
	    {
	      if (fann1->nr_lits > 2)
		{
		  erg = contrap_overlap(firstcl,fann1,mode);
		  if (erg)
		    {
		      newcl = concat_two_cl(newcl,erg);
		    }
		}
 	      fann1 = fann1->next;
	    }
	  
          /******************************/
	  /* Klausel mit allen hinteren */
          /******************************/
	  othercl = firstcl->next;
	  while (othercl)
	    {
	      /* Klausel mit Nachfolger */
	      if (othercl->nr_lits > 2)
		{
		  erg = contrap_overlap(firstcl,othercl,mode);
		  if (erg)
		    {
		      newcl = concat_two_cl(newcl,erg);
		    } 		  
		}
	      
	      /*****************************************/
	      /* Klausel mit fann-Komp. von Nachfolger */
	      /*****************************************/
	      fann2 = othercl->fann;
	      while (fann2)
		{
		  if (fann2->nr_lits > 2)
		    {
		      erg = contrap_overlap(firstcl,fann2,mode);
		      if (erg)
			{
			  newcl = concat_two_cl(newcl,erg);
			}
		    }
		  fann2 = fann2->next;
		}
	      othercl = othercl->next;
	    } 	 
	}
      
      /********************************/
      /* Fann-Komponenten vergleichen */
      /********************************/
      fann1 = firstcl->fann;
      while (fann1)
	{
	  if (fann1->nr_lits > 2)
	    {
	      /************************************************************/
	      /* fann1 mit allen hinteren fann-Kompn. der eigenen Klausel */
	      /************************************************************/
	      fann1other = fann1->next;
	      while (fann1other)
		{
		  if (fann1other->nr_lits > 2)
		    {
		      erg = contrap_overlap(fann1,fann1other,mode);
		      if (erg)
			{
			  newcl = concat_two_cl(newcl,erg);
			}
		    }
		  fann1other = fann1other->next;
		}
	      
	      /************************************************************/
	      /* fann1 mit allen hinteren ausserhalb der eigenen Klausel */
	      /************************************************************/
	      othercl = firstcl->next;
	      while (othercl)
		{
		  /* fann1 mit Nachfolger der eigenen Klausel */
		  if (othercl->nr_lits > 2)
		    {
		      erg = contrap_overlap(fann1,othercl,mode);
		      if (erg)
			{
			  newcl = concat_two_cl(newcl,erg);
			}
		    }
		  
		  /*******************************************************/
		  /* fann1 mit fann-Komp. von Nachfolger der eigenen Kl. */
		  /*******************************************************/
		  fann2 = othercl->fann;
		  while (fann2)
		    {
		      if (fann2->nr_lits > 2)
			{
			  erg = contrap_overlap(fann1,fann2,mode);
			  if (erg)
			    {
			      newcl = concat_two_cl(newcl,erg);
			    }
			}
		      fann2 = fann2->next;
		    }
		  othercl = othercl->next;
		} 	      
	    } 	  
	  fann1 = fann1->next;
	}
      firstcl = firstcl->next;
    }
  
  return(newcl);
}


/*---------------------- concat_two_cl -----------------------------------*/

/* Haengt cl2 an cl1 hinten an */

/*---------------------- concat_two_cl -----------------------------------*/

claustype *concat_two_cl(cl1,cl2)
claustype *cl1,*cl2;
{
  claustype *hcl;

  if (!cl1) return(cl2);
  if (!cl2) return(cl1);

  hcl = cl1;
  while (hcl->next)
    hcl = hcl->next;
  hcl->next = cl2;
  
  return(cl1);
}


/*---------------------- contrap_overlap----------------------------------*/

/* Erzeugt overlap constraints fuer k1 und k2.
   Return-Wert: die drei neuen Kontrapositive oder NULL.
   */

/*---------------------- contrap_overlap----------------------------------*/

claustype *contrap_overlap(k1,k2,mode)
claustype *k1,*k2;
int mode;
{
  claustype *erg = NULL;
  claustype *cl_mem;
  predtype *head1,*head2;
  predtype *tail1,*tail2;
  predtype *best1,*best2;
  predtype *sigma = NULL;
  predtype *oconstr = NULL;
  predtype *tconstr;
  predtype *pr_mem;
  symbollist *vlist;
  int difference;
  int reverse = FALSE;
   
  if (k1->del == DELETED || k2->del == DELETED)
    {
      return(NULL);
    }

  if (inwasm_flags.overlap == 1) 
    {
      if (k1->parsed_rule_type == PRULE || k2->parsed_rule_type == PRULE ||
	  k1->parsed_rule_type == PQUERY || k2->parsed_rule_type == PQUERY) 
	{
	  return(NULL);
	}
    }
  
  if (inwasm_flags.overlap == 2) 
    {
      if (k1->nr_built > 0 || k2->nr_built > 0)
	{
	  return(NULL);
	}
    }
  
  head1 = k1->p_list;
  head2 = k2->p_list;
  
  if (!head1 || !head2) 
    {
      return(NULL);
    }
  
  if (head1->symb != head2->symb || head1->sign != head2->sign)
    /**************************************/
    /* head1 und head2 sind keine Partner */
    /**************************************/
    {
      return(NULL);
    }
  
  difference = k1->dinf - k2->dinf;
  if (difference < 0)
    difference = - difference;
  
  if (difference > mode)
    {
      return(NULL);    	
    }
  
  newrearrange();
  high_rename(head1->t_list,ALL_TO_1);
  if ( newunify(head1->t_list,head2->t_list) != SUCCESS )
    /**************************************/
    /* head1 und head2 nicht unifizierbar */
    /**************************************/
    {
      high_rename(head1->t_list,ALL_TO_0);
      return(NULL);
    }

  /*******************************************************************/
  /* Bestimme das beste Paar von Tailliteralen fuer die Ueberlappung */
  /*******************************************************************/
  best1 = NULL;
  best2 = NULL;
  
  /****************************/
  /* Zuerst Var. von k1 zu SV */
  /****************************/
  tail1 = head1->next;
  while (tail1)
    {
      tail2 = head2->next;
      while (tail2)
	{
	  if (tail1->symb == tail2->symb && tail1->sign == tail2->sign &&
	      !tail1->missing_link && !tail2->missing_link)
	    {
	      newrearrange();
	      high_rename(tail1->t_list,ALL_TO_1);
	      if (newunify(head1->t_list,head2->t_list) == SUCCESS &&
		  newunify(tail1->t_list,tail2->t_list) == SUCCESS )
		{
		  vlist = get_all_var(tail1->t_list);
		  if (vlist && nr_var_in_lit(head1->t_list,vlist, 2) == 0)
		    {
		      high_rename(tail1->t_list,ALL_TO_0);
		      high_rename(head1->t_list,ALL_TO_0);
		      return(NULL);
		    } 
		  vlist = get_all_var(tail2->t_list);
		  if (vlist && nr_var_in_lit(head2->t_list,vlist, 2) == 0) 
		    {
		      high_rename(tail1->t_list,ALL_TO_0);
		      high_rename(head1->t_list,ALL_TO_0);
		      return(NULL);
		    } 
		  if (tail1->complexity * (float) (k1->nr_lits) < k1->complexity)
		    difference += 1;
		  if (tail2->complexity * (float) (k2->nr_lits) < k2->complexity)
		    difference += 1;
 		  
		  if (constr_length() + difference <= mode)
		    {
		      oconstr = ErzeugeConstr(k2->p_list->claus, GENOVERLAPCONS);
		      sigma = getsigma(k2->p_list->claus);
		      if (oconstr) 
			{
			  oconstr->sign = OVER_CONSTR;
			  if (!testsubs(oconstr,k2->constr))
			    {
			      best1 = tail1;
			      best2 = tail2;
			      reverse = FALSE; /* k1 und K2 nicht vertauschen */
			      /*print_parents (k1,k2,sigma,oconstr); */
			      high_rename(tail1->t_list,ALL_TO_0);
			      high_rename(head1->t_list,ALL_TO_0);
			      goto weiter;
			    }
			}
		      else 
			{
			  best1 = tail1;
			  best2 = tail2;
			  reverse = FALSE; /* k1 und K2 nicht vertauschen */
			  /*print_parents (k1,k2,sigma,oconstr);*/
			  high_rename(tail1->t_list,ALL_TO_0);
			  high_rename(head1->t_list,ALL_TO_0);
			  goto weiter;
			}
		    }
		}
	      high_rename(tail1->t_list,ALL_TO_0);
	    }
	  tail2 = tail2->next;
	}
      tail1 = tail1->next;
    }
  high_rename(head1->t_list,ALL_TO_0);

  /***************************/
  /* Jetzt Var. von k2 zu SV */
  /***************************/

  high_rename(head2->t_list,ALL_TO_1);
  tail2 = head2->next;
  while (tail2)
    {
      tail1 = head1->next;
      while (tail1)
	{
	  if (tail2->symb == tail1->symb && tail2->sign == tail1->sign &&
	      !tail1->missing_link && !tail2->missing_link)
	    {
	      newrearrange();
	      high_rename(tail2->t_list,ALL_TO_1);
	      if (newunify(head2->t_list,head1->t_list) == SUCCESS &&
		  newunify(tail2->t_list,tail1->t_list) == SUCCESS )
		{
		  vlist = get_all_var(tail1->t_list);
		  if (vlist && nr_var_in_lit(head1->t_list,vlist, 2) == 0) 
		    {
		      high_rename(tail2->t_list,ALL_TO_0);
		      high_rename(head2->t_list,ALL_TO_0);
		      return(NULL);
		    } 
		  vlist = get_all_var(tail2->t_list);
		  if (vlist && nr_var_in_lit(head2->t_list,vlist, 2) == 0) 
		    {
		      high_rename(tail2->t_list,ALL_TO_0);
		      high_rename(head2->t_list,ALL_TO_0);
		      return(NULL);
		    } 
		  
		  if (tail1->complexity * (float) (k1->nr_lits) < k1->complexity) 
		    difference += 1;
		  if (tail2->complexity * (float) (k2->nr_lits) < k2->complexity)
		    difference += 1;

		  if (constr_length() + difference <= mode)
		    {
		      oconstr = ErzeugeConstr(k1->p_list->claus, GENOVERLAPCONS);
		      sigma = getsigma(k1->p_list->claus);
		      if (oconstr) 
			{
			  oconstr->sign = OVER_CONSTR;
			  if (!testsubs(oconstr,k1->constr))
			    {
			      best1 = tail1;
			      best2 = tail2;
			      reverse = TRUE; /* k1 und K2 vertauschen */
			      /*print_parents (k1,k2,sigma,oconstr); */
			      high_rename(tail2->t_list,ALL_TO_0);
			      high_rename(head2->t_list,ALL_TO_0);
			      goto weiter;
			    }
			}
		      else
			{
			  best1 = tail1;
			  best2 = tail2;
			  reverse = TRUE; /* k1 und K2 vertauschen */
			  /*print_parents (k1,k2,sigma,oconstr); */
			  high_rename(tail2->t_list,ALL_TO_0);
			  high_rename(head2->t_list,ALL_TO_0);
			  goto weiter;
			}
		    }
		}
	      high_rename(tail2->t_list,ALL_TO_0);
	    }
	  tail1 = tail1->next;
	}
      tail2 = tail2->next;
    }
  high_rename(head2->t_list,ALL_TO_0);

weiter:

  if (!best1 || !best2)
    return(NULL);
  
  if (reverse == TRUE) 
    {
      /* k1 und k2 vertauschen */
      cl_mem = k1;
      k1     = k2;
      k2     = cl_mem;
      
      /* head1 und head2 aktualisieren */
      head1 = k1->p_list;
      head2 = k2->p_list;
      
      /* best1 und best2 vertauschen */
      pr_mem = best1;
      best1  = best2;
      best2  = pr_mem;
    }

  /*******************************************/
  /* Ergebnis der Ueberlappung sind drei Kps */
  /*******************************************/
  erg = get_second_contrap(k1,best1);
  get_first_contrap(erg,k1,best1);
  erg->next = get_third_contrap(k2,best2,sigma,oconstr,erg->p_list,k1->clnr);

  generated_contrapositives += 2;
  
  /*************************************************/
  /* Erzeuge Tautologie-Constraints fuer erstes Kp */
  /*************************************************/

  newrearrange();
  if (head1->symb == best1->symb && 
      head1->sign != best1->sign && 
      newunify(head1->t_list,best1->t_list) == SUCCESS) 
    {
      tconstr = ErzeugeConstr(k1, GENOVERLAPCONS);
      tconstr->sign = OVER_CONSTR;
      tconstr->next = k1->constr;
      k1->constr = tconstr;
    }

  /********************************************/
  /* Erhoehe Zaehler fuer neue Praedikatnamen */
  /********************************************/
  newlitcnt++;

  /*******************************************/
  /* Beim vollen overlap das 2. Kp loeschen  */ 
  /*******************************************/
  if (!oconstr)
    {
      save_clnr(k2);
      deleted_contrapositives += 1;
      k2->del = DELETED;
    }
  else 
    /*****************************/
    /* fuege oconstr zu k2 hinzu */
    /*****************************/
  {
      oconstr->sign = OVER_CONSTR;
      ren_constr(oconstr);
      oconstr->next = k2->constr;
      k2->constr = oconstr;
    }

  /*print_result(k1,k2,erg); */
    
  conn_with_other(k1,erg);

  /****************************/
  /* berechne ddepth und dinf */
  /****************************/
  k1->dinf = (k1->dinf <= k2->dinf)? k1->dinf : k2->dinf;
  
  erg->ddepth = 0;
  erg->dinf = (k1->dinf <= k2->dinf)? 0: (k2->dinf - k1->dinf);
  
  erg->next->ddepth = 0;
  erg->next->dinf = (k1->dinf <= k2->dinf)? (k2->dinf - k1->dinf):0;
  
  return(erg);
}


/*---------------------- conn_with_other ---------------------------------*/

   /* 
     Verbindet das neue subgoal newp.. mit den neuen heads newp .. 
   */
/*---------------------- conn_with_other ---------------------------------*/

void conn_with_other(k1,erg)
claustype *k1;
claustype *erg;
{
  predlist *winf1,*winf2;

  /**********************************/
  /* new wulists for new predicates */
  /**********************************/
  winf1 = (predlist *) cnt_malloc(sizeof(predlist));
  winf2 = (predlist *) cnt_malloc(sizeof(predlist));

  if ((winf1 == NULL) || (winf2 == NULL))
    {
      fprintf(stderr, "  Error (oconstr.c - conn_with_other()): Memory allocation error.\n");
      exit(50);
    }

  /****************************************/
  /* wu-list reordering and concatenation */
  /***************************************/
  if (erg->p_list->claus->order < erg->next->p_list->claus->order) 
    {
      winf1->pr = erg->p_list;
      winf1->next = winf2;
      winf2->pr = erg->next->p_list;
      winf2->next = NULL;
    }
  else
    {
      winf1->pr = erg->next->p_list;
      winf1->next = winf2;
      winf2->pr = erg->p_list;
      winf2->next = NULL;
    }
  k1->p_list->next->next->wu = winf1;
  k1->p_list->next->next->nr_connections = 2;
  k1->p_list->next->next->kp = erg;
}


/*---------------------- save_clnr ---------------------------------------*/

   /* Haengt einen Eintrag mit clause und clause->clnr zu dellist
      hinten dran.
   */
/*---------------------- save_clnr ---------------------------------------*/

void save_clnr(clause)
claustype *clause;
{
  deltype *hp,*newdel;
  
  newdel = (deltype *) cnt_malloc(sizeof(deltype));

  if (newdel == NULL)
    {
      fprintf(stderr, "  Error (oconstr.c - save_clnr()): Memory allocation error.\n");
      exit(50);
    }

  newdel->litnr = clause->p_list->litnr;
  newdel->clnr = clause->p_list->claus->clnr;
  newdel->next = NULL;

  if (!dellist)
    dellist = newdel;
  else
    {
      hp = dellist;
      while (hp->next)
	hp = hp->next;
      hp->next = newdel;
    }
}


/*---------------------- get_first_contrap -------------------------------*/

void get_first_contrap(k1,k,best)
claustype *k1, *k;
predtype *best;
{

  int new_nr;
  predtype *head,*h;
  
  k->constr   = NULL;
  k->nr_lits  = 3;
  k->nr_built = (best->symb < nrbuilt)? 1 : 0; 
  
  head = k->p_list;
  head->next = best;
  
  new_nr = 100 + newlitcnt;
  
  k->p_list->next->next = get_new_pred(new_nr,'-',k,head,best);
  k->p_list->next->next->kp = k1; 
  k->p_list->next->next->claus = best->claus; 
  k->parsed_rule_type = PRULE;
  
  k->complexity = 0.0;
  
  for (h = k->p_list; h; h = h->next)
    k->complexity += h->complexity;
}


/*---------------------- replace_in_term ---------------------------------*/

   /* Ersetzt in term jedes Vorkommen von left(Variable) durch
      eine Kopie von right.
      Strukturvariablen in right werden in der Kopie wieder
      zu normalen Variablen. (nur '#' wird von der SV entfernt)
   */

/*---------------------- replace_in_term ---------------------------------*/

termtype *replace_in_term(term,left,right)
termtype *term,*left,*right;
{
  termtype *ht,*hn;
  
  if (!term) 
    return(NULL);
  
  if (term->type == VARIABLE || term->type == SVARIABLE)
    {
      hn = term->next;
      if ((term->symb == left->symb) && (term->value == left->value))
	{
	  ht = copy_terms(right,ONE_TERM);

	  /*  #V<..> --> V<..> */
	  /*my_rename(ht,0,SV_TO_V);*/

	  term = ht;
	}
      term->next = replace_in_term(hn,left,right);
    }
  else
    {
      term->next = replace_in_term(term->next,left,right);
      term->t_list = replace_in_term(term->t_list,left,right);
    }

  return(term);
}


/*---------------------- get_new_pred ------------------------------------*/

predtype *get_new_pred(nr,sign,clause,pr,pr2)
int nr;
char sign;
claustype *clause;
predtype *pr,*pr2;
{
  predtype *erg;
  symbollist  *varlist; /* Variablen in pr und pr2 */
  termtype *args = NULL;
  termtype *newarg;
  int len = 0;
  char name[NAMELENGTH];
  
  varlist = concat_with_comp(get_all_var(pr->t_list),get_all_var(pr2->t_list));
  
  /**************************************************************/
  /* bilde Argumentliste und berechne Aritaet = Laenge(varlist) */
  /**************************************************************/
  while (varlist)
    {
      newarg = init_term(NULL,varlist->symbol,VARIABLE,0,NGROUND);
      newarg->next = args;
      args = newarg;
      
      varlist = varlist->next;
      len++;
    }
  
  sprintf(name,"new_p%d",newlitcnt);
  
  erg = (predtype *) cnt_malloc(sizeof(predtype));

  if (erg == NULL)
    {
      fprintf(stderr, "  Error (oconstr.c - get_new_pred()): Memory allocation error.\n");
      exit(50);
    }
  
  erg->symb  = enter_name(enter_token(name),PREDSYM,len,clause->modul->modunr);
  erg->sign  = sign;
  erg->arity = len;
  erg->litnr = nr;
  erg->wu    = NULL;
  erg->next  = NULL;
  erg->t_list = args;
  erg->complexity = 1.0;
  erg->missing_link = 0;
  
  return(erg);
}


/*---------------------- get_second_contrap ------------------------------*/

claustype *get_second_contrap(k,best,constr)
claustype *k;
predtype *best;
predtype *constr;
{
  claustype *erg;
  predtype *head,*h;
  
  for (h = k->p_list; h->next; h = h->next)
    {
      if (h->next == best) 
	{
	  h->next = h->next->next;
	  break;
	}
    }

  nr_clauses += 1;
  erg = (claustype *) cnt_malloc(sizeof(claustype));

  if (erg == NULL)
    {
      fprintf(stderr, "  Error (oconstr.c - get_second_contrap()): Memory allocation error.\n");
      exit(50);
    }

  erg->clnr     = nr_clauses;
  erg->order    = k->order;
  erg->modul    = k->modul;
  erg->lineno   = k->lineno; 
  erg->nr_lits  = k->nr_lits - 1;
  erg->nr_built = (best->symb < nrbuilt)? 
    (k->nr_built - 1) : k->nr_built; 
  erg->constr   = k->constr;
  erg->fann     = NULL;
  erg->next     = NULL;
  erg->delpos   = PRULE;
  erg->parsed_rule_type = k->parsed_rule_type;
  
  head = k->p_list;

  erg->p_list = get_new_pred(0,'+',erg,head,best);
  erg->p_list->kp = erg;
  erg->p_list->claus = erg;

  erg->p_list->next = head->next;

  erg->complexity = 0.0;

  for (h = erg->p_list; h; h = h->next)
    erg->complexity += h->complexity;
  
  return(erg);
}


/*---------------------- get_third_contrap ------------------------------*/

claustype *get_third_contrap(k,best,sigma,constr,head,number)
claustype *k;
predtype *best;
predtype *sigma,*constr;
predtype *head;
int number;
{
  claustype *erg;
  termtype *term,*left,*right;
  predtype *h;
  
  nr_clauses += 1;
  erg = (claustype *) cnt_malloc(sizeof(claustype));

  if (erg == NULL)
    {
      fprintf(stderr, "  Error (oconstr.c - get_third_contrap()): Memory allocation error.\n");
      exit(50);
    }

  erg->clnr     = nr_clauses;
  erg->order    = k->order;
  erg->modul    = k->modul;
  erg->lineno   = k->lineno; 
  erg->nr_lits  = k->nr_lits - 1;
  erg->nr_built = (best->symb < nrbuilt)? 
    (k->nr_built - 1) : k->nr_built; 
  erg->fann     = NULL;
  erg->next     = NULL;
  erg->delpos   = PRULE;
  erg->parsed_rule_type = k->parsed_rule_type;
  erg->p_list = get_red_pred(head,NULL,erg); 
  erg->p_list->claus = erg;
  
  term = erg->p_list->t_list;

  high_rename(term,ALL_TO_1);
  
  if (sigma)
    {
      left = sigma->t_list->t_list;
      right = sigma->t_list->next->t_list;
      while (left && right)
	{
	  term = replace_in_term(term,left,right);
	  
	  left  = left->next;
	  right = right->next;
	}
    }

  if (constr)
    {
      left = constr->t_list->t_list;
      right = constr->t_list->next->t_list;
      while (left && right)
	{
	  term = replace_in_term(term,left,right);
	  
	  left  = left->next;
	  right = right->next;
	}
    }

  

  /****************************************************/
  /* im Head: alle SV wieder zu normalen Var. #V -> V */
  /****************************************************/
  high_rename(term,ALL_TO_0);

  erg->p_list->t_list = term;
  
  erg->p_list->next = get_red_tail(k,best,constr,erg);
  erg->constr  = get_red_clist(k->constr,erg,constr);
  
  erg->p_list->complexity = KriteriumEinesPraedikates(erg->p_list);
  erg->complexity = 0.0;
  
  for (h = erg->p_list; h; h = h->next)
    erg->complexity += KriteriumEinesPraedikates(h); 
  
  return(erg);
}


/*---------------------- get_red_pred ------------------------------------*/

predtype *get_red_pred(pr,constr,clause)
predtype *pr;
predtype *constr;
claustype *clause;
{
  predtype *erg;
  termtype *term,*left,*right;
  
  erg = (predtype *) cnt_malloc(sizeof(predtype));

 if (erg == NULL)
    {
      fprintf(stderr, "  Error (oconstr.c - get_red_pred()): Memory allocation error.\n");
      exit(50);
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
  if (constr)
    {
      left = constr->t_list->t_list;
      right = constr->t_list->next->t_list;
      while (left && right)
	{
	  term = replace_in_term(term,left,right);
	  
	  left  = left->next;
	  right = right->next;
	}
    }
  
  erg->t_list = term;
  
  return(erg);
}


/*---------------------- get_red_tail ------------------------------------*/

predtype *get_red_tail(k,best,constr,clause)
claustype *k;
predtype *best;
predtype *constr;
claustype *clause;
{
  predtype *tail,*newpred;
  predtype *first = NULL;
  predtype *last  = NULL;
  
  tail = k->p_list->next;
  while (tail)
    {
      if (tail != best)
        {
	  newpred = get_red_pred(tail,constr,clause);
	  newpred->kp = constr ? NULL: tail->kp;
	  
	  if (!first) 
	    first = newpred;
	  else 
	    last->next = newpred;
	  last = newpred;
	}
      
      tail = tail->next;
    }
  
  return(first);
}


/*---------------------- print_overl_del ---------------------------------*/

   /* print deleted contrap.
      Format: (clnr,contrapnr)
   */

/*---------------------- print_overl_del ---------------------------------*/

void print_overl_del()
{
  deltype *hp = dellist;
  int i = 1;
  
  if (hp)
    {
      printf("      Message: Deleted contrapositives : \n               ");
      while (hp)
	{
	  printf("%d.%d ", hp->clnr, hp->litnr); 	  
	  if (i%10 == 0) 
	    {
	      printf("\n               ");
	      i = 1;
	    }
	  else i++;
	  hp = hp->next;
	}       
      printf("\n");
    }   
  printf("      Message: %d contrapositives removed", deleted_contrapositives);
  printf(" and %d added\n",generated_contrapositives);
}


/*---------------------- get_red_clist -----------------------------------*/

   /* clause vererbt clist. Dabei wird zuerst die Substitution sigma
      auf clist angewendet.

      Falls ein vererbtes Constr. leer wird, dann Abbruch: kein 
      echter Overlap.

      Return-Wert: die neue gebildete Constr.-Liste.
   */

/*---------------------- get_red_clist -----------------------------------*/
 
predtype *get_red_clist(clist,clause,sigma)
predtype *clist;
claustype *clause;
predtype *sigma;  /* in Form eines Constraints */
{
  predtype *erg = NULL;  /* Ergebnis */
  predtype *hpconstr;    /* Hilfsvar. */
  
  /* Solange clist nicht leer und Overlap moeglich */
  if (sigma) 
    {
      stop_overlap = 0;
      
      while (clist && !stop_overlap)
	{
	  /****************************/ 
	  /* sigma auf clist anwenden */
	  /****************************/ 
	  hpconstr = get_red_constr(clist,clause,sigma);
	  if (hpconstr) 
	    erg = add_to_list(hpconstr,erg);
	  
	  clist = clist->next;
	}

      return(erg);
    }
  else

    return(clist);
}


/*---------------------- get_red_constr ----------------------------------*/

   /* Wendet sigma auf constr an und liefert das entsprechende
      Ergebnis zurueck.
   */
/*---------------------- get_red_constr ----------------------------------*/

predtype *get_red_constr(constr,clause,sigma)
predtype *constr;
claustype *clause;
predtype *sigma;
{
  predtype *erg = NULL;
  termtype *left,*right,*sigleft,*sigright;
  
  if (!constr) 
    return(NULL);
  
  if (!sigma) /* Constr. Kopieren */ 
    {
      return(constr);
    } 
  
  left  = copy_terms(constr->t_list->t_list,ALL_TERM);
  right = copy_terms(constr->t_list->next->t_list,ALL_TERM);
  
  sigleft  = sigma->t_list->t_list;
  sigright = sigma->t_list->next->t_list; 
  
  /*****************************/
  /* Substitution durchfuehren */
  /*****************************/
  while (sigleft && sigright)
    {
      left  = replace_in_term(left,sigleft,sigright);
      right = replace_in_term(right,sigleft,sigright);
      
      sigleft  = sigleft->next;
      sigright = sigright->next;
    }

  /********************************/
  /* left =/= right normalisieren */
  /********************************/
  newrearrange();
  if (newunify(left,right) == SUCCESS)
    {
      erg = ErzeugeConstr(clause, GENOVERLAPCONS);
      if (erg)
	{
	  erg->sign = constr->sign;
	  erg->litnr = constr->litnr;
	}
      else 
	{
	  /******************************************/
	  /* constr wurde durch sigma leer: []=/=[] */
	  /******************************************/
	  stop_overlap = 1;  
	  clause->del = DELETED;
	}
      
      return(erg);
    }

  else
    return(NULL);
}


/*---------------------- get_all_var -------------------------------------*/

   /* Liefert die liste aller Var. und SVar. in tlist */

/*---------------------- get_all_var -------------------------------------*/

symbollist *get_all_var(tlist)
termtype *tlist;
{
  
  if (! tlist) return(NULL);
  else if (tlist->type == VARIABLE || tlist->type == SVARIABLE)
    return(append_with_comp(get_all_var(tlist->next),tlist->symb,tlist->value));
  else return( concat_with_comp( get_all_var(tlist->next),
				 get_all_var(tlist->t_list) ) );
}


/*---------------------- getsigma ----------------------------------------*/

predtype *getsigma(claus)
claustype *claus;
{
  trailelt * trp;
  termtype * right;
  termtype * htp;
  predtype * constr = NULL;
  termtype * newleft = NULL;
  termtype * newright = NULL;
  char functsymb[NAMELENGTH]; /* "const"arity_  */
  int newarity = 0;
  int newsymb;
  int varsymb;
  int i;
  
  /*****************************/
  /* no bindings no constraint */
  /*****************************/
  if (!trailcnt) 
    return (NULL);
  
  for (i=trailcnt-1; i >=0; i--) 
    {
      /**************************************************/
      /* take binding from trail make subconstraintlist */
      /**************************************************/
      /*trp = (trailelt*)symbtab[trail[i].symb].loclist;*/
       trp = NULL;
       trp = (trailelt*) *(trail[i].loc);
    
       varsymb = trp->symb;
       right = trp->term;
      
       if ((int *)trail[i].loc > (int *)&(symbtab[varsymb]))
	 /* falls groeser -> highloclist-Eintrag fuer varsymb */
	 /*if (symbtab[varsymb].name[0] == '#')*/
	 /* there is a bound stucture variable, subconstraint */
	{
	  newarity++;
	  /******************************************************/
	  /* bound vars become left side vars of the constraint */
	  /******************************************************/
	  if (!newleft)
	    {
	      newleft  = init_term(NULL,varsymb,VARIABLE,0,NGROUND);
	      newleft->value = 1;
	    }
	  else 
	    {
	      htp = init_term(NULL,varsymb,VARIABLE,0,NGROUND);
	      htp->next = newleft;
	      newleft = htp;
	      newleft->value = 1;
	    }
	
  
	  /*******************************************************/
	  /* the terms become right side terms of the constraint */
	  /*******************************************************/
	  if (!newright)
	    newright = copy_terms(right,ONE_TERM);
	  else 
	    {
	      htp = copy_terms(right,ONE_TERM);
              htp->next = newright;
              newright = htp;
	    }
	} 
    }
  
  if (!newarity)
    /****************************************/
    /* there are no bindings, no constraint */
    /****************************************/
    return (NULL);

  /***********************************************/
  /* create a function symbol for the constraint */
  /* of the following form: "const^newarity_"    */
  /***********************************************/
  sprintf(functsymb,"const%d_",newarity);
  newsymb = enter_token(functsymb);

  /********************************************/
  /* generate constraint newleft =/= newright */
  /********************************************/
  constr = (predtype *)cnt_malloc(sizeof(predtype));

  if (constr == NULL)
    {
      fprintf(stderr, "  Error (oconstr.c - getsigma()): Memory allocation error.\n");
      exit(50);
    }
  
  constr->arity   = 2;
  constr->symb    = NQAL_SYMB;
  constr->claus   = claus;
  constr->litnr   = 0;
  constr->wu      = NULL;
  constr->next    = NULL;
  constr->t_list  = init_term(newleft, newsymb, CONSTSYM,
			      0, GROUND);
  constr->t_list->next = init_term(newright, newsymb, CONSTSYM,
				   0, GROUND);
  return (constr);
}


/*---------------------- constr_length -----------------------------------*/

int constr_length()
{
  trailelt * trp;
  termtype * right;
  int varsymb;
  int newarity = 0;
  int i;
  
  /* no bindings no constraint */
  if (!trailcnt) return (0);
  
  for (i=trailcnt-1; i >=0; i--) 
    {
      /* take binding from trail make subconstraintlist */
      /*trp = (trailelt*)symbtab[trail[i].symb].loclist;*/
      trp = NULL;
      trp = (trailelt*) *(trail[i].loc);
      
      varsymb = trp->symb;
      right = trp->term;

      if ((int *)trail[i].loc > (int *)&(symbtab[varsymb]))
	/* falls groeser -> highloclist-Eintrag fuer varsymb */
	/*if (symbtab[varsymb].name[0] == '#') */

	/* there is a bound stucture variable, no subconstraint */
	continue;
      else newarity++;
    }

  return (newarity);
}


/*---------------------- print_parents -------------------------------------*/

void print_parents (k1,k2,sigma,oconstr)
claustype *k1,*k2;
predtype *sigma, *oconstr;
{
  printf("\n\n------------------------------ sigma -------------------------\n");
  print_lop_constr(sigma);
  printf("\n\n------------------------------ constr ------------------------\n");
  print_lop_constr(oconstr);
  
  printf("\n\n------------------------------ K1 ----------------------------\n");
  print_lop_clause(k1,k1->p_list,k1->delpos,0,1);
  print_lop_constr(k1->constr);
  printf("\n\n------------------------------ K2 ----------------------------\n");
  print_lop_clause(k2,k2->p_list,k2->delpos,0,1);
  print_lop_constr(k2->constr);
} 


/*---------------------- print_result -------------------------------------*/

void print_result(k1,k2,erg)
claustype *k1,*k2,*erg;
{
  printf("\n\n------------------------------ K1+K2 -------------------------\n");
  print_lop_clause(k1,k1->p_list,k1->delpos,0,1);
  printf("\n");
  print_lop_module(erg,0,0);
  if ( k2->del != DELETED ) {
    print_lop_clause(k2,k2->p_list,k2->delpos,0,1);
    print_lop_constr(k2->constr);
  }
  
  printf("\n\n==============================================================\n");
}


/*---------------------- add_to_list -------------------------------------*/

/* Fuegt ein Constraint zu einer Constraintliste hinten ein.
   Return-Wert: die neue Constraintliste.
   */
/*---------------------- add_to_list -------------------------------------*/

predtype *add_to_list(constr,list)
predtype *constr,*list;
{
  predtype *hp;
  ren_constr(constr);
  
  if (!list)
    list = constr;
  else {
    hp = list;
    while (hp->next)
      hp = hp->next;
    hp->next = constr;
  }
  return(list);
}


/*---------------------- ren_constr --------------------------------------*/

   /* Nennt Strukturvariablen auf der rechten Seite des Constraints um.
      #V  ---->  #V_counter
   */
/*---------------------- ren_constr --------------------------------------*/

void ren_constr(constr)
predtype *constr;
{
  if (constr->t_list)
    {
      high_rename(constr->t_list->next,REN_CONSTR);
      var_counter++;
    }
}


/*---------------------- append_with_comp --------------------------------*/

   /* haengt symbol hinten an list an falls symbol nicht bereits in list
      enthalten ist.
      Liefert die gebildete Liste zurueck.
   */
/*---------------------- append_with_comp --------------------------------*/

symbollist *append_with_comp(list,symbol,value)
symbollist *list;
int symbol;
int value;
{
  symbollist *newsymb,*hp;
  int found = FALSE;

  newsymb = (symbollist *) cnt_malloc(sizeof(symbollist));

  if (newsymb == NULL)
    {
      fprintf(stderr, "  Error (oconstr.c - append_with_comp()): Memory allocation error.\n");
      exit(50);
    }

  newsymb->symbol = symbol;
  newsymb->value = value;
  newsymb->next = NULL;
  
  if (! list)
    list = newsymb;
  else
    {
      hp = list;
      while (hp->next && !found)
	{
          if (hp->symbol == symbol && hp->value == value)
	    found = TRUE;
          hp = hp->next;
	}
      if ((! found) && (hp->symbol != symbol || (hp->symbol == symbol && hp->value != value)))
	hp->next = newsymb;
    }

  return(list);
}

/*---------------------- concat_with_comp --------------------------------*/

   /* Haengt alle Symbole aus list2, die in list1 nicht vorkommen, hinten
      an list1 an.
      Ergebnis: die gebildete Liste.
   */
/*---------------------- concat_with_comp --------------------------------*/

symbollist *concat_with_comp(list1,list2)
symbollist *list1,*list2;
{
  symbollist *erg;
  
  erg = list1;
  
  while (list2)
    {
      erg = append_with_comp(erg,list2->symbol,list2->value);
      list2 = list2->next;
    }

  return(erg);
}


/*########################################################################*/


