/******************************************************************************
;;; MODULE NAME : preproc/hlpfkt.c
;;;
;;; PARENT      : 
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : 
;;;
;;; AUTHOR      : Martin Ziegenaus 
;;;
;;; DATE        : 13.09.91
;;;
;;; DESCRIPTION : general help functions
;;;
;;; REMARKS     : 12.11.92     hlpfkt.c       hamdi                        
;;;               12.01.94     extern.h       hamdi 
******************************************************************************/

#include "extern.h"

/*****************************************************************************/
/* functions (preproc/hlpfkt.c)					             */
/*****************************************************************************/
int FuegeConstraintHinzu();
int KopiereTerm();
termtype *copy_terms();
termtype *deref_copy_terms();
predtype *ErzeugeConstr();
predtype *GenerateConstraint();
termtype *copy_dereferenced_terms();


/*****************************************************************************/
/* FuegeConstraintHinzu()              				             */
/* Description: Der uebergebene Constraint wird hinten an die Constraints-   */
/*              Liste der uebergebenen Klausel angehaengt.                   */
/*****************************************************************************/
int FuegeConstraintHinzu( Constraint, Clausel)
predtype *Constraint;
claustype *Clausel;
{
  predtype *hpc; 	/* zum Durchlaufen */
  termtype *tList;

  /* set variable_number and clause_number values for the constraint */
  /* left side of constraint */
  tList = Constraint->t_list;
  tList->variable_number = VariableEnum;
  tList->clause_number   = 1000;
  VariableEnum++;
  if (tList->t_list)
    VariableEnum = 
      generate_var_enum_predsym(1000, VariableEnum, tList->t_list);
  /* right side of constraint */
  tList = Constraint->t_list->next;
  tList->variable_number = VariableEnum;
  tList->clause_number   = 1000;
  VariableEnum++;
  if (tList->t_list)
    VariableEnum = 
      generate_var_enum_predsym(1000, VariableEnum, tList->t_list);
  if (!Clausel->constr) {
    Clausel->constr = Constraint;
    return(0);
  }

  /* Constraint wird hinten angehaengt */
  hpc = Clausel->constr;    
  while (hpc->next)
    hpc = hpc->next;
  hpc->next = Constraint;
  
  
  /* Constraint wird vorne eingefuegt 
  hpc = Clausel->constr;  
  Clausel->constr = Constraint;
  Constraint->next = hpc;
  */  

  return(0);
}


/*****************************************************************************/
/* KopiereTerm()              				                     */
/* Description: Kopiert einen Term. Nur der Term selber, keine t_list- und   */
/*              next-Verweise, wird kopiert. Also nur bei Variablen          */
/*              verwenden !                                                  */
/*              Eingabe: zwei Pointer auf Ziel und Quelle.                   */
/*              Achtung: Ziel muss schon allociert sein.                     */
/*              Ausgabe : 0 alles ok                                         */
/*****************************************************************************/
int KopiereTerm( ziel, quelle )
termtype *ziel;
termtype *quelle;
{
  ziel->symb = quelle->symb;
  ziel->type = quelle->type;
  ziel->variable_number = quelle->variable_number;      
  ziel->clause_number = quelle->clause_number;         
  ziel->value = quelle->value;
  ziel->grflag = quelle->grflag;
  ziel->t_list = NULL;
  ziel->next = NULL;

  return 0;
}


/*****************************************************************************/
/* copy_terms()              				                     */
/* Description:                                                              */
/*****************************************************************************/
termtype *copy_terms(quelle, mode)
termtype * quelle;
int mode;
{
  register termtype *tp;
  termtype *htp;

  if (!quelle) 
    return (NULL);

  htp = quelle;

  /* I dont know what this means
  if (quelle->value == 1)
    htp = newderef(quelle);
    */

  if ((tp = (termtype *)cnt_malloc(sizeof(termtype))) == NULL) {
    fprintf(stderr, "  Error (hlpfkt.c - copy_terms()): Memory allocation error.\n");
    exit(50);
  }

  KopiereTerm(tp, htp);

  switch (mode) {
  case ALL_TERM:
    /* copy all recursivly */
    tp->next = (quelle->next)?
      copy_terms(quelle->next,mode) : NULL;
    tp->t_list = (htp->t_list)?
      copy_terms(htp->t_list,ALL_TERM) : NULL;
    break;
  case ONE_TERM:
    tp->next = NULL;
    tp->t_list = (htp->t_list)?
      copy_terms(htp->t_list,ALL_ARGS) : NULL;
    break;
  case ALL_ARGS:
    tp->next = (quelle->next)?
      copy_terms(quelle->next,mode) : NULL;
    tp->t_list = (htp->t_list)?
      copy_terms(htp->t_list,ALL_ARGS) : NULL;
    break;
  default:
    nferror("wrong mode in copy_terms",0);
  }

  if (symbtab[tp->symb].name[0] == '#') {
    symbtab[tp->symb].type = SVARIABLE;
    tp->type = SVARIABLE;
  }

  return (tp);
}


/*****************************************************************************/
/* deref_copy_terms()                 			                     */
/* Description:                                                              */
/*****************************************************************************/
termtype *deref_copy_terms(quelle,mode,mode2)
termtype * quelle;
int mode;
int mode2;
{
  register termtype *tp;
  termtype *htp;
  char name[80];

  if (!quelle) 
    return (NULL);

  htp = newderef(quelle);

  if ((tp = (termtype *)cnt_malloc(sizeof(termtype))) == NULL) {
    fprintf(stderr, "  Error (hlpfkt.c - deref_copy_terms()): Memory allocation error.\n");
    exit(50);
  }

  KopiereTerm(tp, htp);

  switch (mode) {
  case ALL_TERM:
    /* copy all recursivly */
    tp->next = (quelle->next)?
      deref_copy_terms(quelle->next,mode,mode2) : NULL;
    tp->t_list = (htp->t_list)?
      deref_copy_terms(htp->t_list,ALL_TERM,mode2) : NULL;
    break;
  case ONE_TERM:
    tp->next = NULL;
    tp->t_list = (htp->t_list)?
      deref_copy_terms(htp->t_list,ALL_ARGS,mode2) : NULL;
    break;
  case ALL_ARGS:
    tp->next = (quelle->next)?
      deref_copy_terms(quelle->next,mode,mode2) : NULL;
    tp->t_list = (htp->t_list)?
      deref_copy_terms(htp->t_list,ALL_ARGS,mode2) : NULL;
    break;
  default:
    nferror("wrong mode in deref_copy_terms",0);
  }

  if  (tp->value == 1 && mode2 == GENCONS) {
    tp->type = SVARIABLE;
    sprintf(name,"#%s_%d",symbtab[tp->symb].name,var_counter);
    tp->symb = enter_name(enter_token(name),SVARIABLE,0,0);
    
  }
  
  return (tp);
}


/*****************************************************************************/
/* ErzeugeConstr()                  			                     */
/* Description: mode : - CONNOPT wird von connopt.c zur sigma-Generierung    */
/*              aufgerufen.                                                  */
/*              - GENCONS wird bei CONSTRAINT-Generierung                    */
/*               verwendet.                                                  */
/*****************************************************************************/
predtype *ErzeugeConstr(claus,mode)
claustype *claus;
int mode;
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

  /* no bindings, no constraint */
  if (!trailcnt) {
    return (NULL);
  }

  for (i=trailcnt-1; i >=0; i--) {
    /* take binding from trail make subconstraintlist */
    trp = NULL;
    trp = (trailelt*) *(trail[i].loc);
    varsymb = trp->symb;
    right = trp->term;
    
    if ((mode == GENCONS || mode == GENOVERLAPCONS) && 
	(right->value == 1) && 
	(right->type == VARIABLE))        
      /* falls right den value 1 besitzt */
      continue;
    else if ((mode == GENCONS || mode == GENOVERLAPCONS) && 
	     ((int *)trail[i].loc > (int *)&(symbtab[varsymb])))
      /* falls groeser -> highloclist-Eintrag fuer varsymb */        
	continue;
    else {
      newarity++;
      
      /* enter variable in left side of the constraint */
      /* only a variable can be bound to a term	    */
      if (!newleft)
	newleft  = init_term(NULL,varsymb,VARIABLE,0,NGROUND);
      else {
	htp = init_term(NULL,varsymb,VARIABLE,0,NGROUND);
	htp->next = newleft;
	newleft = htp;
      }
      
      if ((mode == CONNOPT) &&
	  ((int *)trail[i].loc > (int *)&(symbtab[varsymb]))) {
	newleft->value = 1; 
      }
  
      /* enter term in right side of the constraint */
      if (!newright)
	newright = deref_copy_terms(right,ONE_TERM,mode);
      else {
	htp = deref_copy_terms(right,ONE_TERM,mode);
	htp->next = newright;
	newright = htp;
      }
    }
  }
  if (!newarity) 
    /* no bindings of non-struct-vars, no constraint */
    return (NULL);

  /* create a constraintsymbol  const newarity _    */
  sprintf(functsymb,"const%d_",newarity);
  newsymb = enter_token(functsymb);

  /* generate constraint newleft =/= newright */
  constr = (predtype *)cnt_malloc(sizeof(predtype));

  if (constr == NULL) {
    fprintf(stderr, "  Error (hlpfkt.c - ErzeugeConstr()): Memory allocation error.\n");
    exit(50);
  }

  constr->arity	= 2;
  constr->symb	= NQAL_SYMB;
  constr->claus	= claus;
  constr->litnr	= ++nr_constraints;
  constr->wu	= NULL;
  constr->next 	= NULL;
  constr->t_list	= init_term(newleft, newsymb, CONSTSYM,
				    0, GROUND);
  constr->t_list->next = init_term(newright, newsymb, CONSTSYM,
				   0, GROUND);
  nr_gen_constr++;
  var_counter++;
  return (constr);
}


/*****************************************************************************/
/* GenerateConstraint()          			                     */
/* Description: mode : - CONNOPT wird von connopt.c zur sigma-Generierung    */
/*              aufgerufen.                                                  */
/*              - GENCONS wird bei CONSTRAINT-Generierung                    */
/*               verwendet.                                                  */
/*****************************************************************************/
predtype *GenerateConstraint(claus, mode, twoClauses)
claustype *claus;
int mode;
int twoClauses;
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
  int trailNumber;

  /* no bindings, no constraint */
  if (!trailcnt) {
    return (NULL);
  }

  for (trailNumber = trailcnt-1; trailNumber >= 0; trailNumber--) {

    /* take binding from trail make subconstraintlist */
    trp     = NULL;
    trp     = (trailelt*) *(trail[trailNumber].loc);
    varsymb = trp->symb;
    right   = trp->term;        

    if ((mode == GENSUBCONS || mode == GENTAUTOCONS) && 
	((right->type == VARIABLE) && 
	 (twoClauses == TRUE) &&
	 (right->clause_number == trail[trailNumber].highClauseNr))) {       
      /* falls right aus dem 'umbenannten' (dem 2.) Term stammt */       
      continue;
    }
    else if ((mode == GENSUBCONS || mode == GENTAUTOCONS) && 
	     (trail[trailNumber].reference == HIGH_REFERENCE)) {
      /* highloclist-Eintrag fuer varsymb */      
      continue;
    }
    else {
      newarity++;      
      /* constraint will be generated */       
      
      /* enter variable in left side of the constraint */
      /* only a variable can be bound to a term	    */       
      if (!newleft) {
	newleft  = init_term(NULL,varsymb,VARIABLE,0,NGROUND);
      }
      else {
	htp = init_term(NULL,varsymb,VARIABLE,0,NGROUND);
	htp->next = newleft;
	newleft = htp;
      }
  
      /* enter term in right side of the constraint */      
      if (!newright) {
	newright = copy_dereferenced_terms(right, ONE_TERM, mode, trail[trailNumber].highClauseNr); 	
      }
      else {
	htp = copy_dereferenced_terms(right, ONE_TERM, mode, trail[trailNumber].highClauseNr);
	htp->next = newright;
	newright = htp;
      }

      /* special mode for connopt.c */
      if  (mode == CONNOPT) {
	if (trail[trailNumber].reference == HIGH_REFERENCE) {
	  newleft->clause_number = trail[trailNumber].highClauseNr;
	}
      }
      
    }
  }
  
  if (!newarity) {   
    /* no bindings of non-struct-vars, no constraint */
    return (NULL);
  }

  /* create a constraintsymbol  const newarity _    */
  sprintf(functsymb,"const%d_",newarity);
  newsymb = enter_token(functsymb);

  /* generate constraint newleft =/= newright */
  constr = (predtype *)cnt_malloc(sizeof(predtype));

  if (constr == NULL) {
    fprintf(stderr, "  Error (hlpfkt.c - GenerateConstraint()): Memory allocation error.\n");
    exit(50);
  }

  constr->arity	= 2;
  constr->symb	= NQAL_SYMB;
  constr->claus	= claus;
  constr->litnr	= ++nr_constraints;
  constr->wu	= NULL;
  constr->next 	= NULL;
  constr->t_list	= init_term(newleft, newsymb, CONSTSYM,
				    0, GROUND);
  constr->t_list->next  = init_term(newright, newsymb, CONSTSYM,
				    0, GROUND);
  nr_gen_constr++;
  return (constr);
}


/*****************************************************************************/
/* copy_dereferenced_terms()                 			                     */
/* Description:                                                              */
/*****************************************************************************/
termtype *copy_dereferenced_terms(quelle, mode, constraintMode, highClauseNr)
termtype * quelle;
int mode;
int constraintMode;
int highClauseNr;
{
  register termtype *tp;
  termtype *htp;
  char name[80]; 

  if (!quelle) 
    return (NULL);

  htp = deref(quelle);

  tp = (termtype *)cnt_malloc(sizeof(termtype));

  if (tp == NULL) {
    fprintf(stderr, "  Error (hlpfkt.c - copy_dereferenced_terms()): Memory allocation error.\n");
    exit(50);
  }

  KopiereTerm(tp, htp);

  switch (mode) {
  case ALL_TERM:
    /* copy all recursivly */
    tp->next = (quelle->next)?
      copy_dereferenced_terms(quelle->next, mode, constraintMode, highClauseNr) : NULL;
    tp->t_list = (htp->t_list)?
      copy_dereferenced_terms(htp->t_list, ALL_TERM, constraintMode, highClauseNr) : NULL;
    break;
  case ONE_TERM:
    tp->next = NULL;
    tp->t_list = (htp->t_list)?
      copy_dereferenced_terms(htp->t_list, ALL_ARGS, constraintMode, highClauseNr) : NULL;
    break;
  case ALL_ARGS:
    tp->next = (quelle->next)?
      copy_dereferenced_terms(quelle->next, mode, constraintMode, highClauseNr) : NULL;
    tp->t_list = (htp->t_list)?
      copy_dereferenced_terms(htp->t_list, ALL_ARGS, constraintMode, highClauseNr) : NULL;
    break;
  default:
    nferror("wrong mode in deref_copy_terms",0);
  }

  if  ((constraintMode == GENSUBCONS) &&
       (tp->clause_number == highClauseNr) && 
       (tp->type == VARIABLE)) {
    tp->type = SVARIABLE;
    sprintf(name,"#%s_%d",symbtab[tp->symb].name,var_counter);
    tp->symb = enter_name(enter_token(name),SVARIABLE,0,0);
  }  

  return (tp);
}
