/******************************************************************************
;;; MODULE NAME : interface.c
;;;
;;; PARENT      : pars.y
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)interface.c	15.1 01 Apr 1996 
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 15.5.91
;;;
;;; DESCRIPTION : link ex-/imported predicates
;;;
;;; REMARKS     :
;;;		2.7.93	jsc	extern.h
;;;
******************************************************************************/

#include "symb.h"
#include "extern.h"

/*****************************************************************************/
/* functions (inwasm/interface.c)                                            */
/*****************************************************************************/
void enter_interface();
void join_loclist();
int get_symbol();


/*****************************************************************************/
/* enter_interface()                                                         */
/* Description: make links between modules                                   */
/*****************************************************************************/
void enter_interface(module)
modtype *module;
{
  char text[60];
  int surmodnr,lsymb,lmodu;
  symbollist *slp;
  importlist *ilp;
  modtype    *mdl;
  
  mdl	 = module->m_list;
  surmodnr = module->modunr;
  
  while (mdl) {
    /*************************************************************/
    /* import-list                                               */
    /*************************************************************/
    if (ilp = mdl->interface.import) {
      for ( ; ilp; ilp=ilp->next) {

#ifdef DEBUG
	fprintf(stderr,"search module: %s\n",NAM(ilp->modusymb));
#endif
	lmodu = ilp->modusymb;
	while (symbtab[lmodu].type != MODUSYM && symbtab[lmodu].next) 
	  lmodu = symbtab[lmodu].next->symbnr;
	if (symbtab[lmodu].type != MODUSYM) {
	  warning(NAM(lmodu),"is not a module");
	  /* next import entry */
	  continue;
	}
	else
	  /* there may be trouble if many modules have the same name */
	  lmodu = symbtab[lmodu].modunr;
	   
#ifdef DEBUG
	fprintf(stderr,"found module: %s  with number: %d\n",
		NAM(ilp->modusymb),lmodu);
#endif

	for (slp=ilp->impos ; slp; slp=slp->next) {	  
	  /* get token */
	  lsymb = get_symbol(NAM(slp->symbol),lmodu);
	  
#ifdef DEBUG
	  fprintf(stderr,"link: %s   %d to %d\n",NAM(lsymb),mdl->modunr,lmodu);
#endif

	  /* arity check */
	  if (symbtab[lsymb].arity == -1) 
	    symbtab[lsymb].arity = symbtab[slp->symbol].arity;
	  else if (symbtab[lsymb].arity != symbtab[slp->symbol].arity) {
	    sprintf(text,"imported predicate with different arity %d/%d",
		    symbtab[lsymb].arity,symbtab[slp->symbol].arity);
	    warning(NAM(lsymb),text);
	    /* next modul import entry */
	    continue;
	  }
	  
	  if (symbtab[slp->symbol].loclist) {
	    /* there are occurencies */
	    join_loclist(symbtab[slp->symbol].loclist, lsymb);
	  }
	  else {
	    /* no occurencies may be already ex/imported */
	    sprintf(text,"imported predicate in module %s never defined",
		    NAM(mdl->symb));
	    warning(NAM(slp->symbol),text);
	    continue;
	  }
	  
	  /* delete occurencies of exported predicate */
	  symbtab[slp->symbol].loclist = NULL;
	  symbtab[slp->symbol].index = lsymb;
	   } /*endfor slp */
      } /*endfor ilp */
    }
    
    /*************************************************************/
    /* export-list                                               */
    /*************************************************************/
    if (slp = mdl->interface.export) 
      for (; slp; slp=slp->next) {
	if (!symbtab[slp->symbol].loclist) {
	  sprintf(text,"\"%s\" in module %s never defined",
		  NAM(slp->symbol),NAM(mdl->symb));
	  warning("exported predicate ",text);
	  continue;
	}
	/* get symbnr of predicate which slp is exported to */
	lsymb = get_symbol(NAM(slp->symbol),surmodnr);
	
#ifdef DEBUG
	fprintf(stderr,"link: %s  %d to %d\n",NAM(lsymb),mdl->modunr,surmodnr);
	fprintf(stderr,"          %d ,  %d\n",slp->symbol,lsymb);
#endif

	/* arity check */
	if (symbtab[lsymb].arity == -1) 
	  symbtab[lsymb].arity = symbtab[slp->symbol].arity;
	else if (symbtab[lsymb].arity != symbtab[slp->symbol].arity) {
	  sprintf(text,"exported predicate with different arity %d/%d",
		  symbtab[lsymb].arity,symbtab[slp->symbol].arity);
	  warning(NAM(lsymb),text);
	  continue;
	}
	
	/* append loclist to lsymb loclist */
	join_loclist(symbtab[slp->symbol].loclist, lsymb);
	
	/* delete occurencies of exported predicate */
	symbtab[slp->symbol].loclist = NULL;
	symbtab[slp->symbol].index = lsymb;
      } /* next export entry */
       
    mdl = mdl->next;
  }
}


/*****************************************************************************/
/* join_loclist()                                                            */
/* Description: append loc1 at beginning of loclist(lsymb)                   */
/*****************************************************************************/
void join_loclist(loc1,lsymb)
predlist *loc1;
int lsymb;
{
  predlist *loc12 = loc1;
  
#ifdef DEBUG
  fprintf(stderr,"changing %d to %d\n",loc1->pr->symb,lsymb);
#endif

  loc12->pr->symb = lsymb;

  for (; loc1->next; loc1 = loc1->next) {
    loc1->pr->symb = lsymb;
  }

  loc1->pr->symb = lsymb;
  loc1->next = symbtab[lsymb].loclist;
  symbtab[lsymb].loclist = loc12;
}


/*****************************************************************************/
/* get_symbol()                                                              */
/* Description: search in list for defined predicate in special modul        */
/*****************************************************************************/
int get_symbol(token,modunr)
char *token;
int modunr;
{
  int  head;
  syel *sp;
  
  head = get_symbnr(token);
  sp = symbtab+head;
  while (sp) {
    if (sp->modunr == modunr && sp->type == PREDSYM)
      return sp->symbnr;
    else
      sp = sp->next;
  }
  warning(token," not found in other module !");
  return enter_name(head,PREDSYM,-1,modunr);
}
