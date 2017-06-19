/******************************************************************************
;;;MODULE NAME:   purity.c
;;;
;;;PARENT     :   generator.c
;;;
;;;VERSION    :	  @(#)purity.c	13.2  07 Apr 1995
;;;
;;;PROJECT    :   setheo
;;;
;;;AUTHOR     :   J. Schumann, TUM
;;;
;;;DATE       :   ddddddd
;;;
;;;DESCRIPTION:   purity preprocessing step
;;;
;;;MODIF      :   29.2.88
;;;		  15.4.88                    with built in pred's
;;;		  15.6.88                    new rule
;;;               26.6.88                    change deleted also in loclist
;;;		  19.10.88                   built ins
;;;		  24.7.91                    adapted for MPLOP? J.Breiteneicher
;;;		  2.7.93	jsc	     extern.h
;;;		  14.12.93	jsc	     purity, fold
;;;		  12.04.94	hamdi	     include "symb.h"! 
;;;               19.02.97      RNZ          removed obsolete code, structuring
;;;	
******************************************************************************/

#include "extern.h"

/*****************************************************************************/
/* statics                                                                   */
/*****************************************************************************/
static int nrdeleted = 0;

/*****************************************************************************/
/* functions (preproc/purity.c)                                              */
/*****************************************************************************/
int genpurity();
int pure_mod();
void del_clauses();
int conns();
void delete_p();
void del_occlist();


/*****************************************************************************/
/* genpurity()                                                               */
/* Description: look for predicates with no connection out. Delete them.     */
/*****************************************************************************/ 
int genpurity(module)
modtype *module;
{
  int start = (module == matrix);

  printf("\n        Message: Deleted clauses : "); 
  while (module) {
    if (do_it(module, "nopure", COMPILE)) {
      module = module->next;
      continue;
    }
    if (module->m_list)
      genpurity(module->m_list);
    
    if (module->c_list->p_list->wu == NULL) {
      /* if there's no connection to the goal: we abort */
      printf("0");
      return 1;
    }
    if (pure_mod(module->c_list)) {
      /* if purity removed all clauses, we abort */
      printf("0");
      return 1;
    }
    module = module->next;
  }
  if (start) {    
    del_clauses(matrix);    
    if (!nrdeleted) {
     printf("None.");
    }
  }
  return 0;
}

/*****************************************************************************/
/* pure_mod()                                                                */
/* Description: search in clausellist for pur predicates                     */
/*              delete 1) predicate                                          */
/*                     2) occurence in loclist                               */
/*	               3) all connections into (new clause=>pure?)           */
/*****************************************************************************/
int pure_mod(clausellist)
claustype *clausellist;
{
  int change = 1;
  claustype *cll = clausellist;
  predtype *pr;
  predlist *wli, *wl2;

  while (change) {
    cll = clausellist;
    change = 0;
    while (cll) {
    is_deleted:
      pr = cll->p_list;
      if (cll->clnr == DELETED) {
	while (pr) {
	  /* move thru all lits remove occs and conns */
	  del_occlist(pr);
	  wli = pr->wu;
	  while (wli) {
	    /* look at the other predicate */
	    wl2 = wli->pr->wu;
	    if ((!conns(wli->pr)) || 
		(!wl2->next)) {
	      if (wli->pr->claus->clnr != DELETED) {
		change = 1;
		printf("%d ", wli->pr->claus->clnr);
		if (!wli->pr->claus->clnr) {
		  /* artificial query is pure */
		  return 1;
		}
		wli->pr->claus->clnr = DELETED;
	      }
	      wli->pr->wu = NULL; /* is pure */
	    }
	    else {
	      delete_p(pr, wli->pr);
	    }
	    delete_p(wli->pr, pr); 
	    wli = wli->next;
	  }
	  pr = pr->next;
	}
      }
      else {
	/* move thru literals and grep pure ones */
	while (pr) {
	  if (!conns(pr)) {
	    change = 1;
	    printf("%d ", pr->claus->clnr);
	    if (!pr->claus->clnr){
	      /* artificial query is pure */
	      return 1;
	    }
	    pr->claus->clnr = DELETED;
	    goto is_deleted;
	  }
	  pr=pr->next;
	}
      }
      cll=cll->next;
    } /* while(cll) */
  } /* while(change) */
  return 0;
}


/*****************************************************************************/
/* del_clauses()                                                             */
/* Description: delete clauses in matrix                                     */
/*****************************************************************************/
void del_clauses(module)
modtype *module;
{
  claustype *cll, *cllv;

  while (module) {
    if (module->m_list) {
      del_clauses(module->m_list);
    }
    /* eliminate deleted clauses */
    cll = module->c_list;
    cllv = NULL;
    while (cll){
      if (cll->clnr == DELETED) {
	/* is deleted */
	nrdeleted++;
	if (cll == module->c_list) 
	  cll = module->c_list = cll->next;
	else {
	  cllv->next = cll->next;
	  cll = cll->next;
	}
      }
      else {
	cllv = cll;
	cll = cll->next;
      }
    }
    module = module->next;
  }
  nr_clauses -= nrdeleted;
}


/*****************************************************************************/
/* conns()                                                                   */
/* Description: test for connections                                         */
/*              == p->wu                                                     */
/*              + check for built-ins (which are never pure                  */
/*              + only one connection inside ONE clause                      */
/*              return 0 if pure                                             */
/*****************************************************************************/ 
int conns(p)
predtype *p;
{
  if (p->symb < nrbuilt) return 1;
  if (!(p->wu)) return 0;
  if (!(p->wu->next) && 
      (p->claus == p->wu->pr->claus)) {
    return 0;
  }
  return 1;
}


/*****************************************************************************/
/* delete_p()                                                                */
/* Description: remove pr entry in wulist of prw                             */
/*****************************************************************************/
void delete_p(pr,prw)
predtype *pr, *prw;
{
  predlist *wll,*wulist;

  wulist = prw->wu;
  wll = NULL;
  while (wulist){
    if (wulist->pr == pr) {
      /* delete that entry */
      if (!wll)
	prw->wu = wulist->next;
      else
	wll->next = wulist->next;
      /* update counter */
      prw->nr_connections -= 1;
      return;
    }
    wll = wulist;
    wulist = wulist->next;
  }
}


/*****************************************************************************/
/* del_occlist()                                                             */
/* Description: remove pr-entry from predicate occurrence list li            */
/*****************************************************************************/
void del_occlist(pr)
predtype *pr;
{
  predlist *h1 = symbtab[pr->symb].loclist;
  predlist *h2 = NULL;

  while (h1) {
    if (h1->pr == pr) {
      /* we found that entry */
      if (h1 == symbtab[pr->symb].loclist) {
	symbtab[pr->symb].loclist = h1->next;
      }
      else {
	h2->next = h1->next;
      }
      return;
    }
    else {
      h2 = h1;
      h1 = h1->next;
    }
  }
}
