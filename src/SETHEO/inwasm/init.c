/*****************************************************************************
;;; MODULE NAME : init.c
;;;
;;; PARENT      : pars.y
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)init.c	15.1  01 Apr 1996
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 15.5.91
;;;
;;; DESCRIPTION : initialize dynamic structures 
;;;
;;; REMARKS     : included as C code in pars.y
;;;
;;;	3.3.93	jsc	init_query: parsed_rule_type
;;;	2.7.93	jsc	extern.h
;;;	30.3.94 hamdi	static functions decl.
;;;
******************************************************************************/

#include "extern.h"

/*****************************************************************************/
/* static functions   				                             */
/*****************************************************************************/
static int check_subterm();

/*****************************************************************************/
/* functions (inwasm/init.c)                                                 */
/*****************************************************************************/
#ifdef __LINUX__OLD_JSC
termtype *init_term();
symbollist *new_symboll();
importlist *init_import();
static predtype *init_pred();
int count_arity();
claustype * newclause();
modtype *init_mod();
static void prefix_query();
#endif


/*****************************************************************************/
/* init_term()                                                               */
/* Description: initialize a term                                            */
/*****************************************************************************/
termtype *init_term(tl,symb,type,value,is_ground)
int symb;
int type;
int value;
int is_ground;
termtype *tl;
{
  int arity = 0;
  char functsymb[NAMELENGTH];
  register termtype *tp;

  if ((tp = (termtype *)cnt_malloc(sizeof(termtype))) == NULL) {
    fprintf(stderr, "  Error (init.c - init_term()): Memory allocation error.\n");
    exit(50);
  }

  arity	= count_arity(tl);

  if (symb == -1) {
    /* only for constraints */
    sprintf(functsymb,"const%d_",arity);
    symb = enter_token(functsymb);
  }

  /* entered in symbtab with modunr 0 */
  tp->symb 	= enter_name(symb,type,arity,0);
  tp->type 	= type;
  tp->value 	= value;
  tp->t_list	= tl;
  tp->next 	= NULL;

  if (!tl)
    tp->grflag = is_ground;
  else if ((type == CONSTANT) || (type == CONSTSYM))
    /* there is a function */
    tp->grflag = check_subterm(tl);
  else
    /* global array !? */
    tp->grflag = is_ground;

  return tp;
}


/*****************************************************************************/
/* int check_subterm()                                                       */
/* Description:                                                              */
/*****************************************************************************/
static int check_subterm(tl)
termtype *tl;
{
  while (tl) {
    if (tl->grflag == NGROUND)
      return NGROUND;
    tl = tl->next;
  }
return GROUND;
}


/*****************************************************************************/
/* new_symboll()                                                             */
/* Description: append a new listel at list with entry symb                  */
/*****************************************************************************/
symbollist *new_symboll(symb,list)
int symb;
symbollist *list;
{
  register symbollist *sl ;

  if ((sl = (symbollist *)cnt_malloc(sizeof(symbollist))) == NULL) {
    fprintf(stderr, "  Error (init.c - new_symboll()): Memory allocation error.\n");
    exit(50);
  }

  sl->symbol = symb;
  sl->next   = list;
  return sl;
}


/*****************************************************************************/
/* init_import()                                                             */
/* Description:                                                              */
/*****************************************************************************/
importlist *init_import(slp,modsymb)
symbollist *slp;
int modsymb;
{
  register importlist *il;

  if ((il  = (importlist *)cnt_malloc(sizeof(importlist))) == NULL) {
    fprintf(stderr, "  Error (init.c - init_import()): Memory allocation error.\n");
    exit(50);
  }

  il->modusymb	= modsymb;
  il->impos	= slp;
  return il;
}


/*****************************************************************************/
/* init_pred()                                                               */
/* Description: initialize predicate                                         */
/*****************************************************************************/
static predtype *init_pred(tp,symb)
register termtype *tp;
int symb;
{
  register predtype *pp;

  if ((pp = (predtype *)cnt_malloc(sizeof(predtype))) == NULL) {
    fprintf(stderr, "  Error (init.c - init_pred()): Memory allocation error.\n");
    exit(50);
  }

  pp->arity	= count_arity(tp);
  pp->symb	= enter_name(symb,PREDSYM,pp->arity,modunr);
  pp->claus	= currclause;
  pp->sign 	= aktsign;
  pp->litnr 	= litnr++;
  pp->kp	= NULL;
  pp->wu 	= NULL;
  pp->next 	= NULL;
  pp->t_list 	= tp;
  pp->nr_connections = 0;
  pp->missing_link = 0;

  /* enter occurrency of predicate in loclist of symbtab */
  enter_occ(pp);

  /* count built-ins query_ is NOT a built-in */
  if (pp->symb && pp->symb < nrbuilt) 
    builtnr++;

  return pp;
}


/*****************************************************************************/
/* count_arity()                                                             */
/* Description: count arity of a term list                                   */
/*****************************************************************************/
int count_arity(tp)
termtype *tp;
{
  int i = 0;
  for ( ; tp ; tp = tp->next) 
    i++;
  return i;
}


/*****************************************************************************/
/* newclause()                                                               */
/* Description: allocate space for a new clause                              */
/*****************************************************************************/
claustype * newclause()
{
  claustype *newclaus;
 
  if ((newclaus = (claustype *)cnt_malloc(sizeof(claustype))) == NULL) {
    fprintf(stderr, "  Error (init.c - newclause()): Memory allocation error.\n");
    exit(50);
  }
  
  return newclaus; 
}


/*****************************************************************************/
/* init_mod()                                                                */
/* Description: initialize module                                            */
/*****************************************************************************/
modtype *init_mod(symb)
{
  modtype *module;

  if ((module = (modtype *)cnt_malloc(sizeof(modtype))) == NULL) {
    fprintf(stderr, "  Error (init.c - init_mod()): Memory allocation error.\n");
    exit(50);
  }

  module->symb      = enter_name(symb,MODUSYM,0,modunr);
  module->modunr    = modunr;
  module->strategy  = NULL;
  module->interface.export = NULL;
  module->interface.import = NULL;
  module->interface.global = NULL;
  module->next	  = NULL;
  module->prev	  = currmodule;
  
  currmodule = module;

  return module;
}


/*****************************************************************************/
/* prefix_query()                                                            */
/* Description: the default query is the first clause (of first module)      */
/*****************************************************************************/
static void prefix_query(module)
modtype *module;
{
  claustype *query;
  register predtype *pp;

  if ((pp = (predtype *)cnt_malloc(sizeof(predtype))) == NULL) {
    fprintf(stderr, "  Error (init.c - prefix_query()): Memory allocation error.\n");
    exit(50);
  }
  
  query = startclause = newclause();
  query->clnr	   = 0;
  query->lineno   = 0;
  query->delpos   = PQUERY;
  query->parsed_rule_type   = PQUERY;
  query->modul	   = currmodule;
  query->nr_lits  = 1;
  query->nr_built = 0;
  query->fann	   = NULL;
  query->constr   = NULL;
  query->p_list   = pp;
  query->nrvars   = 0;
  query->nrstrvars = 0;
  query->order    = 0;
  query->ddepth   = 1;
  query->dinf     = 1;
  query->del      = 0;
  query->max_termdepth = 0;
  query->max_termsize = 0;
  pp->arity = 0;
  pp->symb  = 0;
  pp->sign  = '-';
  pp->claus = query;
  pp->kp = query;
  pp->litnr = 1;
  pp->t_list= NULL;
  pp->wu    = NULL;
  pp->next  = NULL;
  enter_occ(pp);
  query->next = module->c_list;
  module->c_list = query;
}




