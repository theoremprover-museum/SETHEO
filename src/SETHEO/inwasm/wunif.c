/*******************************************************************************
;;; MODULE NAME : wunif.c
;;;
;;; PARENT      : main.c
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)wunif.c	15.1  01 Apr 1996
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 15.5.91
;;;
;;; DESCRIPTION : make the weak unification ( graph )
;;;
;;; REMARKS     : 1.3.93	jsc	connections are always in both 
                                        directions
;;;		  3.3.93        jsc	print_wunif adapted to new format
;;;		  4.3.93	jsc	start wunif-list with a # on each line
;;;		  2.7.93	jsc	extern.h
;;;		 21.7.93	jsc	debugging
;;;
*******************************************************************************/

#include "symb.h"
#include "extern.h"
#include "defines.h"


/******************************************************************************/
/* functions (inwasm/wunif.c)                                                 */
/******************************************************************************/
void genwunif();
void weakunif();
void print_wunif();
void rename_clause_number();
void map_clause_number();

/******************************************************************************/
/* genwunif()                                                                 */
/* Description: make the weak unification & create the data structure.        */
/*              for each predicate (symmetric) the unification is only        */
/*              called from '-' to '+' connections                            */            
/******************************************************************************/
void genwunif()
{
  int counter;
  
  init_trail();
  maxunif = 0;
  
  /* default query */
  weakunif(0);
  
  /* make for every user defined predicate the connection list */
  for (counter = nrbuilt; counter < nump; counter++) 
    if (symbtab[counter].type == PREDSYM) 
      weakunif(counter);
}

/*****************************************************************************/
/* weakunif()                                                                */
/* Description: enter connections of predicate with number symb              */
/*****************************************************************************/
void weakunif(symbol_nr)
int symbol_nr;
{
  int nrunif;
  predlist *predicate1, *predicate2;
  termtype *term1, *term2;
  predlist *winf;
  int flag_rename = FALSE;

  /* move thru negative loclist and unify with positive list */
  predicate1 = symbtab[symbol_nr].loclist;
  
  while (predicate1) {
    nrunif = 0;
    /* get one predicate & its termlist term1 */
    if (predicate1->pr->sign == '+') {
      predicate1 = predicate1->next;
      continue;
    }
    term1 = predicate1->pr->t_list;
    
    /* inner while loop */
    predicate2 = symbtab[symbol_nr].loclist;
    while (predicate2) {
      if (predicate2->pr->sign == '-') {
	predicate2 = predicate2->next;
	continue;
      }
      term2 = predicate2->pr->t_list;
     
      /* falls zwei Literale einer Klausel unifiziert werden sollen
	 so muessen zuerst die clause_number(s) der termtype(s) der 
	 2.Klausel umbenannt werden. Nur noetig falls zwei Literale
	 einer Klausel, da gleiche Variablen unterschieden werden
	 muessen. clause_numbers werden umbennant -> 
	 Klauselnummer + 5000 */

      if ((term1 && term2) && 
	  (term1->clause_number == term2->clause_number)) {
	rename_clause_number(term2, (term2->clause_number + 5000));
	flag_rename = TRUE;
      }
	    
      switch (unify(term1, term2, UNIFY_WEAK)) {
      case SUCCESS:
	nrunif++;
	
	/* have to store this information in both directions (2*predl.) */
	if ((winf = (predlist *)cnt_malloc( 2 * sizeof(predlist))) == NULL) {
	  fprintf(stderr, "  Error (wunif.c - weakunif()): Memory allocation error.\n");
	  exit(50);
	}	
	/* we always store the connection in both  */
	/* directions,  */

	/* predicate1 -> predicate2 */
	winf->next = predicate1->pr->wu;
	winf->pr = predicate2->pr;
	predicate1->pr->wu = winf++;
	predicate1->pr->nr_connections += 1;

	/* predicate 2 -> predicate1 */
	winf->next = predicate2->pr->wu;
	winf->pr = predicate1->pr;
	predicate2->pr->wu = winf;
	predicate2->pr->nr_connections += 1;

	break;
      case FAIL:
	/* store nothing */
	break;
      default:
	warning(symbtab[symbol_nr].name," weak unify: illegal switch");
      }

      /* re-renaming nach unifiaktion */
      if (flag_rename == TRUE) {
	rename_clause_number(term2, term1->clause_number);
	flag_rename = FALSE;
      }
      
      rearrange();
      
      predicate2 = predicate2->next;
    } /* end of inner loop */
    
    if (nrunif > maxunif) maxunif = nrunif;
    predicate1 = predicate1->next;
  }
  rearrange();
}


/*****************************************************************************/
/* print_wunif()                                                             */
/* Description: print weak unification                                       */
/*****************************************************************************/
void print_wunif(modulist,fout,skip)
modtype *modulist;
FILE *fout;
int skip;
{
  claustype	*clauses;
  predlist	*w;
  predtype	*p;
  int		cnt;
  
  skip = 0;
  
  while (modulist) {
    if (modulist->m_list) 
      print_wunif(modulist->m_list,fout,0);
    for (clauses=modulist->c_list; clauses; clauses=clauses->next) {
      /* move thru the literals of one clause */
      for (p = clauses->p_list;p;p=p->next) {
	/* skip default query */
	if (!p->symb)
	  continue;
	/* skip head of prules and pfacts */
	if (!clauses->fann && clauses->p_list == p)
	  continue;
	fprintf(fout,"# %d.%d ", p->claus->clnr,p->litnr);
	/* skip built-ins */
	if (symbtab[p->symb].type == BUILTIN ) {
	  fprintf(fout," [built-in]\n");
	  continue;
	}
	fprintf(fout,"to ( ");
	/* move thru wulist & print predics */
	for (w = p->wu,cnt = 0; w; w=w->next,cnt++) 
	  if (w->pr->kp && w->pr->kp->del != DELETED)
	    fprintf(fout,"%d.%d ",
		    w->pr->claus->clnr,w->pr->litnr);
	fprintf(fout,")\n");
      }
    }
    modulist = modulist->next;
  }
}


/*****************************************************************************/
/* rename_clause_number()                                                    */
/* Description: clause_number of termtype is going to be renamed with        */
/*              parameter_value                                              */
/*****************************************************************************/
void rename_clause_number(tlist, value) 
termtype *tlist;
int value;
{
  if (!tlist) {
  } else {
    if (tlist->next != NULL) {
      rename_clause_number(tlist->next, value);
    }
    if (tlist->t_list != NULL) {
      rename_clause_number(tlist->t_list, value);
    }
    tlist->clause_number = value;
  }
  return;
}

/*****************************************************************************/
/* rename_clause_numbers_in_clause()                                         */
/* Description: clause_number of termtype is going to be renamed with        */
/*              parameter_value                                              */
/*****************************************************************************/
void map_clause_number(clause, value) 
claustype *clause;
int value;
{
  predtype *pred_list = clause->p_list;
  termtype *term_list = NULL;
  clause->clnr = (short)value;
  
  while(pred_list != NULL) {
    term_list = pred_list->t_list;
    rename_clause_number(term_list, value);
    pred_list = pred_list->next;
  }
}


