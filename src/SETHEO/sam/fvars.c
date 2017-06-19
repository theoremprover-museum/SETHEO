/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: fvars.c                                      */
/* VERSION:                                           */
/* DATE:                                              */
/* AUTHOR: Ortrun Ibens                               */
/* NAME OF FILE:                                      */
/* DESCR: routines for free variables handling        */
/* MOD:                                               */
/* MOD:                                               */
/* BUGS:                                              */
/******************************************************/


#include <stdio.h>
#include "tags.h"
#include "machine.h"
#include "symbtab.h"
/**/
#include "fvars.h"
#include "deref.h"
#include "errors.h"



static int   process_args();



WORD  *get_free_variables(lcp)
register choice_pt 	*lcp;
{   
  register int  i,nr_new_free_vars;
  int           nr_new_vars,nr_old_free_vars;
  WORD         *newblock,*oldblock,*new_vars,*ptr;
  environ      *h_bp;
  
  if (lcp->ch_link < (choice_pt*)stack) {
    lcp->nr_fvars = 0;
    return(NULL);
  }
  
  newblock = sp;
  nr_new_free_vars = 0;
  nr_old_free_vars = lcp->ch_link->nr_fvars;
  oldblock = lcp->ch_link->block_fvars;
  if (oldblock != NULL) {
    /* get free variables from oldblock: */
    for ( i=0 ; i<nr_old_free_vars ; i++ ) {
      ptr = (WORD*) GETPTR(*(oldblock+i));
      if (GETTAG(*ptr) == T_FVAR) {
	GENOBJ(*(newblock+nr_new_free_vars), (WORD_cast)ptr,T_BVAR);
	nr_new_free_vars++;
      }
    }
  }
  
  h_bp = lcp->ch_link->head_bp;
  if (h_bp != NULL) {
    new_vars = (WORD*)h_bp + SIZE_IN_WORDS(environ);
    nr_new_vars =   GETNUMBER(h_bp->lcbptr->nr_vars)
      - GETNUMBER(h_bp->lcbptr->nr_strvars);
    /* get new free variables: */
    for ( i=0 ; i<nr_new_vars ; i++ ) {
      if ( GETTAG(*(new_vars+i)) == T_FVAR ) {
	GENOBJ(*(newblock+nr_new_free_vars),(WORD_cast)(new_vars+i),T_BVAR);
	nr_new_free_vars++;
      }
    }
  }
  
  sp += nr_new_free_vars;
  lcp->nr_fvars = nr_new_free_vars;
  return(newblock);
}




void  mark_irrelevant_fvars(lcp)
register choice_pt    *lcp;
{   
  s_trail                      *save_tr;
  register environ             *env;
  register literal_ctrl_block  *sgi;
  WORD                         *vars,*args,*sgls,*gai,*ptr;
  register int                  nr_vars,nr_args,nr_sgls,max_fvars,i,k;
  
  /* Save trail pointer: */
  save_tr = tr;
  
  /* Get maximal number of markable free variables: */
  max_fvars = lcp->nr_fvars;
  
  /* Trail and mark relevant variables in open brother subgoals: */
  nr_sgls = GETNUMBER(goal_bp->lcbptr->nr_sgls);
  sgls =   (WORD*) goal_bp + SIZE_IN_WORDS(environ)
    + GETNUMBER(goal_bp->lcbptr->nr_vars);
  
  for ( k = 0  ; k < nr_sgls ; k++ ) {
    if (GETTAG(*(sgls+k)) == NOT_SOLVED) 
      break;
  }
  /* Instead of for (k=0 ; ...) 
   *
   * k = goal_bp->current_sg + 1;   oder mit tp
   */
  
  for ( i = k+1 ; i < nr_sgls ; i++ ) {
    sgi = (literal_ctrl_block*) GETPTR(*(sgls+i));
    args = code + GETVAL(sgi->argv);
    nr_args = GETARITY(sgi->ps_symb);
    max_fvars = process_args(args,nr_args,goal_bp,max_fvars);
    if (!max_fvars)
      break;
  }
  
  if (horn) {
    /* Trail and mark relevant variables in open subgoals: */
    for ( env = goal_bp->dyn_link ; env ; env = env->dyn_link ) {
      nr_sgls = GETNUMBER(env->lcbptr->nr_sgls);
      sgls =   (WORD*)env + SIZE_IN_WORDS(environ)
	+ GETNUMBER(env->lcbptr->nr_vars);
      
      for ( k = 0  ; k < nr_sgls ; k++ ) {
	if (GETTAG(*(sgls+k)) == NOT_SOLVED) 
	  break;
      }
      /* Instead of for (k=0 ; ...) 
       *
       * k = env->current_sg + 1;   oder mit tp
       */
      
      for ( i = k ; i < nr_sgls ; i++ ) {
	sgi = (literal_ctrl_block*) GETPTR(*(sgls+i));
	args = code + GETVAL(sgi->argv);
	nr_args = GETARITY(sgi->ps_symb);
	max_fvars = process_args(args,nr_args,env,max_fvars);
	if (!max_fvars)
	  break;
      }
    }
  }
  else {
    /* Trail and mark relevant variables in path: */
    for ( env = goal_bp ; env ; env = env->dyn_link ) {
      args = code + GETVAL(env->lcbptr->argv);
      nr_args = GETARITY(env->lcbptr->ps_symb);
      max_fvars = process_args(args,nr_args,env,max_fvars);
      if (!max_fvars)
	break;
    }
  }
  
  /* Mark pointers to irrelevant free variables: */
  vars = lcp->block_fvars;
  nr_vars = lcp->nr_fvars;
  for ( i=0 ; i<nr_vars ; i++ ) {
    gai = (WORD*) GETPTR(*(vars+i));
    if ( GETTAG(*gai) != T_MARK ) {
      ptr = (WORD*) GETPTR(*(vars+i));
      GENOBJ(*(vars+i), (WORD_cast)ptr,T_MARK);
    }
  }
  
  /* Untrail: */
  untrail(save_tr);
  tr = save_tr;
}



static int  process_args(args,nr_args,env,max_fvars) 
WORD     *args;
int       nr_args;
environ  *env;
int       max_fvars;      /* max number of markable free variables */
{
  register int   i;
  WORD          *gai;
  int            sub_arity;            /* arity of subterm */
  
  for ( i=0 ; i<nr_args ; i++ ) {
    gai = deref(args+i,env);
    switch (GETTAG(*gai)) {
      
    case T_MARK:  
    case T_CONST:
    case T_GTERM:  break;
      
    case T_FVAR:   trail_var(gai);
      GENOBJ(*gai,0,T_MARK);
      max_fvars--;
      if (!max_fvars)
	return(0);
      break;
      
    case T_NGTERM:
    case T_CRTERM: sub_arity = GETARITY(*gai);
      max_fvars = process_args(gai+1,sub_arity,env,max_fvars);
      if (!max_fvars)
	return(0);
      /* Mark term to avoid multiple processing: */
      trail_var(gai);
      GENOBJ(*gai,0,T_MARK);
      break;
      
    default:       printf("GETTAG(*gai) = "GETTAGFORMAT"\n",GETTAG(*gai));
      sam_error("process_args: illegal tag",gai,3);
    }
  }
  
  return(max_fvars);
}
