/*****************************************************************************
;;; MODULE NAME : symconstr.c   
;;;  
;;; PARENT      : main.c   
;;;  
;;; PROJECT     : MPLOP  
;;;  
;;; SCCS        : @(#)symconstr.c	13.2 07 Apr 1995  
;;;  
;;; AUTHOR      : Mohamed Salah, Hamdi   
;;;  
;;; DATE        : 15.11.92   
;;;  
;;; DESCRIPTION : generate symmetry constraints   
;;;  
;;; REMARKS     : 30.12.1993	Hamdi	extern.h  
;;;		  25.02.94	Hamdi	SUBS,RSUBS ..  
;;;               16.04.96      RZ      removed extern and defines  
;;;  
******************************************************************************/  
  
#include "extern.h"    
#include "defines.h"   
  
/*****************************************************************************/  
/* functions (preproc/ symstr.c)                                             */  
/*****************************************************************************/  
void gen_sym_constr();  
int sym_constr();  
void print_del_contra();  
  
  
/*****************************************************************************/  
/* gen_sym_constr()                                                          */  
/* Description: Generiert Symmetrie Constraints                              */  
/*****************************************************************************/  
void gen_sym_constr(Gesamtstruktur)  
claustype *Gesamtstruktur;  
{  
  claustype *hc;   
  nr_gen_constr = 0;  
  hc = Gesamtstruktur;  
     
  /* Default query "uberspringen */   
  if (!hc->p_list->symb)   
    hc = hc->next;  
    
  while (hc) {  
    if (hc->fann) {  
      if (inwasm_flags.linksubs) { 	  
	if (pos_lits(hc) <= 1) {   
	  /* Fann-Komponenten vergleichen */
	  /* Begin: SEMANTIC */
 	  if (!contains_constraint_builtin(hc)) {
	    sym_constr(hc);   
	  }
	  /* End: SEMANTIC */
	}
      }  
      else {  
	/* Fann-Komponenten vergleichen */  
	/* Begin: SEMANTIC */
	if (!contains_constraint_builtin(hc)) {
	  sym_constr(hc);   
	}
	/* End: SEMANTIC */
      }  
    }  
    hc = hc->next;  
  }  
  if (nr_gen_constr)  
    printf("        Message: %d symmetry-constraints generated.\n",nr_gen_constr);  
}  
  
  
/*****************************************************************************/  
/* sym_constr()                                                              */  
/* Description: Bearbeitet die Kontrapositiven von clause und generiert      */  
/*              gegebenfalls Constraints.                                    */  
/*****************************************************************************/  
int sym_constr(clause)  
claustype *clause;  
{  
  claustype *hp,*hp2;  
  symbollist *dellist = NULL;   
  
  if (!clause)  
    return 0;  
    
  if (clause->delpos == PRULE) {            /* Kontrapositiv ist keine Query */         
    /* Erstes Kontrapositiv mit allen hinteren */  
      
    hp = clause->fann;  
    while (hp) {  
      if (clause_s_constr(clause, hp, CONTRAC) == TRUE) {  
        /* Subsumption zwischen Kontrapositiven */	     
        /* Aktualisiere dellist */	     
        dellist = app_sym_to_list(dellist,(- hp->clnr));  
        hp->del = DELETED;	  
      }  
      hp = hp->next;  
    }    
  }  
    
  /* Restliche Kontrapositive mit allen hinteren */   
  hp = clause->fann;  
  while (hp) {  
    if (hp->del != DELETED) {  
      hp2 = hp->next;  
      while (hp2) {  
	if (hp2->del != DELETED) {  
	  if (clause_s_constr(hp, hp2, CONTRAC) == TRUE) {  
	    /* Subsumption zwischen Kontrapositiven */		     
	    /* Aktualisiere dellist */		       
	    dellist = app_sym_to_list(dellist, (- hp2->clnr));  
	    hp2->del = DELETED;	  
	  }    
	}  
	hp2 = hp2->next;  
      }  
    }  
    hp = hp->next;  
  }  
    
  /* Ausgabe der DELETED-Kontrapositive */  
  if (dellist)  
    print_del_contra(clause->clnr, dellist);  
    
  return 0;  
}  
  
  
/*****************************************************************************/  
/* make_sym_s_list()                                                         */  
/* Description:                                                              */  
/*****************************************************************************/  
void make_sym_s_list(kpp,lpp)  
predtype *kpp;  
predtype *lpp;  
{  
  plittype *newslist;  
    
  if (kpp && lpp && (kpp->symb >= nrbuilt)) {  
   
    rearrange();  
        
    if ((kpp->symb == lpp->symb) &&   
	(kpp->sign == lpp->sign) &&        /* Kopfliterale unifizierbar */  
	(unify(lpp->t_list, kpp->t_list, UNIFY_SLIST2) == SUCCESS)) {   
	    
      /* Initialisierungen */      	    
      if ((newslist = (plittype *) cnt_malloc(sizeof(plittype))) == NULL) {  
	fprintf(stderr, "  Error (symconstr.c - make_sym_s_list()): Memory allocation error.\n");
	exit(50);
      }  
        
      newslist->pr = lpp;  
      newslist->notunif = FALSE;  
      newslist->next = NULL;  
      kpp->s_list = newslist;  
	  	  
      /* Berechnung von s_list fuer die Tails beider Klauseln */
      make_s_list(kpp->next, lpp->next, kpp);
    }  
    else {    
      /* Kopfliterale nicht unifizierbar */   
      no_subsumption = 1;  
    }  
  }  
}  
  
  
/*****************************************************************************/  
/* print_del_contra()                                                        */  
/* Description:                                                              */  
/*****************************************************************************/  
void print_del_contra(clnr,list)  
int clnr;   
symbollist  *list;  
{  
  printf("        Message: Deleted contrapositives are : ");  
  while  (list) {  
    printf("%d.%d ",clnr,list->symbol);  
    list = list->next;  
  }  
  printf("\n");  
}  
