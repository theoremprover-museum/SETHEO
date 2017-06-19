/*########################################################################
  # Include File:		match.c
  # SCCS-Info:		@(#)match.c	8.1
  # Author:		J. Schumann
  # Date:			16.5.94
  # Context:		sam
  # 
  # Contents:		matching
  #
  # Modifications:	
  #	(when)		(who)		(what)
  #	6.6.94		jsc			new macros
  #
  # Bugs:
  #
  # --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
  ########################################################################*/


#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "symbtab.h"
#include "disp.h"
#include "unitlemma.h"
#include "opcodes.h"
#include "defaults.h"
/**/
#include "match.h"
#include "deref.h"
#include "disp.h"
#include "unifeq.h"
#include "errors.h"

/***********************************************************************
 * local MACROS
 ***********************************************************************/
#define DEBUG(X)	
#define DEBUG2(X)	

/*------------------------------------------------------------------------
  README:
  ------------------------------------------------------------------------*/


/***********************************************************************
 * statics
 ***********************************************************************/
/* static counter for
 * newly generated constants
 * their symbol-value must be greater than 2*MAXPREDSYMB+1;
 */
static	int var_to_const_cntr =2*MAXPREDSYMB+1;

static int do_the_match();

/***********************************************************************
 * match
 ***********************************************************************/
/*	match two terms
 *	returns:
 *	1 if LHS is an instance of RHS     pXX < PXY, but PXY <| PYY
 *	0 otherwise
 *	
 *  Note: this outer function just initializes the counter
 *			for the new ``constants'' and calls the static
 *			function ``do_the_match''.
 *
 *
 */
int match(ga,bp_ga,ha,bp_ha)
WORD *ga, *ha;
environ *bp_ga, *bp_ha;
{
  var_to_const_cntr =2*MAXPREDSYMB+1;

  DEBUG2(printf("match("));
  DEBUG2(disp_(stdout,ga,bp_ga));
  DEBUG2(printf(" , "));
  DEBUG2(disp_(stdout,ha,bp_ha));
  DEBUG2(printf(")\n"));
  return do_the_match(ga,bp_ga,ha,bp_ha);
}


/***********************************************************************
 * match
 ***********************************************************************/
/*	match two terms
 *	returns:
 *	1 if LHS is an instance of RHS     pXX < PXY, but PXY <| PYY
 *	0 otherwise
 *	
 ***********************************************************************/
static int do_the_match(ga,bp_ga,ha,bp_ha)
WORD *ga, *ha;
environ *bp_ga, *bp_ha;
{
  WORD *gaa;
  WORD *haa;
  int i;
  
  
  gaa = deref(ga,bp_ga);
  haa = deref(ha,bp_ha);
  
  switch(GETTAG(*gaa)){
    
    /*
     * LHS is a constant
     */
  case T_CONST:
    /*
     * RHS is a constant.
     *  we succeed, if both constants are equal
     */
    if (GETTAG(*haa) == T_CONST){
      if (!EQUAL_WORD2(*gaa,*haa)) {
	/* clash failure */
	return 0;
      }
      else {
	return 1;
      }
    }
    /*
     * RHS is a variable
     *  we bind the variable to the constant
     *  and succeed
     */
    if (GETTAG(*haa) == T_FVAR) {
      trail_var(haa);
      GENCONST(*haa,*gaa);
      return 1;
    }
    
    /*
     * RHS is a compound term: fail
     */
    return 0;
    
    
    /*
     * LHS is a free variable
     *	which occurs for the FIRST time
     */
  case T_FVAR:
    /* 
     * on the RHS, there must be an unbound variable
     */
    if (GETTAG(*haa) != T_FVAR){
      return 0;
    }
    /*
     * convert it into a ``special constant''
     * and bind the RHS variable to it
     */
    trail_var(gaa);
    GENSYMBOL(*gaa,var_to_const_cntr,T_CONST);
    trail_var(haa);
    GENSYMBOL(*haa,var_to_const_cntr,T_CONST);
    var_to_const_cntr++;
    return 1;
    
    /*
     * LHS is a compound term
     */
  case T_GTERM:
  case T_NGTERM:
  case T_CRTERM:
    
    switch(GETTAG(*haa)){
      
    case T_FVAR:
      /*
       * if RHS is an unbound variable,
       * bind it, to the term, but remember this fact
       * this is done using the tag T_EOSTR
       */
      trail_var(haa);
      GENOBJ(*haa,(WORD_cast)gaa,T_EOSTR);
      return 1;
      
    case T_GTERM:
    case T_CRTERM:
    case T_NGTERM:
      /*
       * the RHS is a compound term as well
       *   we have to check for the equality of
       *   the functors and then we have 
       *   to match the subterms
       */
      if (GETSYMBOL(*gaa) != GETSYMBOL(*haa)){
	/*
	 * functors are not equal
	 * Note: they may have different
	 * Tags
	 */
	return 0;
      }
      i=GETARITY(*gaa);
      DEBUG(printf("arity = %d\n>>>>\n",i);)
	while (i--){
	  /*
	   * match all subterms
	   */
	  gaa++;
	  haa++;
	  DEBUG(disp_(stdout,gaa,bp_ga));
	  DEBUG(printf("\n"));
	  DEBUG(disp_(stdout,haa,bp_ha));
	  DEBUG(printf("\n"));
	  if (! do_the_match(gaa,bp_ga,haa,bp_ha)){
	    /* 
	     * there is a problem with the
	     * subterms
	     */
	    return 0;
	  }
	}
	/*
	 * we are done
	 */
	return 1;
	
    case T_EOSTR:
      /*
       * we have to match two terms, the one of them
       * (on the RHS) is a remembered variable substsitution
       *
       * in this case, we only compare for
       * syntactic equality
       *
       * 1. go to the RHS term
       */
      haa = (WORD *)GETPTR(*haa);
			/* 2nd argument is from X[left-side]\t */
      return chk_arg(gaa,bp_ga,haa,bp_ga);
      
    default:
      /*
       * in all other cases:
       *  fail
       */
      return 0;
    }
    
  default:
    /*
     * this is an erroneous case
     */
    sam_error("illegal tag in ``match''", gaa, 1);
    return 0;
  }
  
}













