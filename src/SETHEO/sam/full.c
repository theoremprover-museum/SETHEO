/*#########################################################################
  # File:			full.c     
  # SCCS-Info:		8/31/92, @(#)full.c	1.2
  # Author:		J. Schumann
  # Date:			23.12.87
  # Context:		setheo (SETHEO)
  #
  # Contents:		functions for full-unification
  #
  # Modifications:
  #	(when)		(who)		(what)
  #	23.04.92	Max Moser	`restyled'
  #			C.Mueller	mode COPY or not COPY resp. removed
  #					first switch haa then
  #					for each case switch gaa
  #	18.2.93		jsc		break added after MACROS
  #
  #	11.3.93	Mueller,Goller neue Constraints, Statistics
  
  # Bugs:
  #
  # --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
  #########################################################################*/

/**************************************************************************
 * declarations
 **************************************************************************/


#define COPY 1

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"

extern unsigned long debug_level;

#define FAIL_U {return failure;}
#define FAIL_U2(x) {return failure;}
#define UNIF_FAIL(x) FAIL_U

#include "unification.h"

#ifdef CONSTRAINTS
#include "proto_unify.h"
#endif

/**/
#include "full.h"
#include "debug.h"
#include "deref.h"
#include "disp.h"


/**************************************************************************
 * code
 **************************************************************************/
/***************************************************************/
/* full_unify is always called with 2 complex terms     Goller */
/***************************************************************/

int             full_unify (ga, ha, bpgoal, bphead)
WORD           *ga; 
WORD           *ha;
environ        *bpgoal;
environ        *bphead;
{
  WORD           *gaa, *haa;
  
#ifdef DEBUG
  printf ("Full_unify %lx %lx\n", (long) ga, (long) ha);
  printf ("Tags: %x %x\n", GETTAG (*ga), GETTAG (*ha));
  disp_ (stdout, ga, bpgoal); printf(" mit ha ");
  disp_ (stdout, ha, bphead); printf("\n");
  printf ("\n");
#endif

  if (!EQUAL_WORD(*ga,*ha)) {
    return failure;
  }
  ga++; ha++;
  
  while (GETTAG (*ga) != T_EOSTR) {
    
    gaa = deref (ga, bpgoal);
    haa = deref (ha, bphead);
    
    
    switch(GETTAG(*haa)) {
    case T_CONST:
      UN_CONST(haa, bphead, gaa, bpgoal)
      break;
    case T_FVAR:
      UN_FVAR(haa, bphead, gaa, bpgoal)
      break;
    case T_CRTERM:
      UN_CRTERM(haa, bphead, gaa, bpgoal)
      break;
    case T_GTERM:
      UN_GTERM(haa, bphead, gaa, bpgoal)
      break;
    case T_NGTERM:
      UN_NGTERM(haa, bphead, gaa, bpgoal)
      break;
    case T_GLOBL:
      UN_GLOBL(haa, bphead, gaa, bpgoal)
      break;
    default:
      UNIF_FAIL("def")
      break;
    }
    
    ga++;
    ha++;
  }
  
  return success;
} /*=====================================================================*/


#ifdef CONSTRAINTS
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  + full_proto_unify
  + - <informelle beschreibung>
  +
  + special remarks:
  + - <e.g. called from>
  + - <side effects, return values>
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int             full_proto_unify (ga, ha, bpgoal, bphead)
WORD           *ga;
WORD           *ha;
environ        *bpgoal;
environ        *bphead;

{ /*=====================================================================*/
  
  WORD           *gaa, *haa;
  
  /*-----------------------------------------------------------------------*/
  
#ifdef DEBUG
  printf ("Full_proto_unify %lx %lx\n", (long) ga, (long) ha);
  printf ("Tags: %x %x\n", GETTAG (*ga), GETTAG (*ha));
  disp_ (stdout, ga, bpgoal);
  disp_ (stdout, ha, bphead);
  printf ("\n");
#endif
  
  if (!EQUAL_WORD(*ga,*ha)) {
    return failure;
  }
  ga++; ha++;
  
  while (GETTAG (*ga) != T_EOSTR) {
    
    gaa = deref (ga, bpgoal);
    haa = deref (ha, bphead);
    
    /***************************************************************************
     * T_GLOBL couldn't occur in complex terms in Johann's SETHEO
     * I have added the additional tag-combinations ( T_GLOBL | ~ ) Goller
     * thus erasing bugs glob2 ... glob7
     ****************************************************************************/
    
    
    switch(GETTAG(*haa)) {
    case T_STRVAR:
      PU_STRVAR(haa, bphead, gaa, bpgoal)
	case T_CONST:
	  PU_CONST(haa, bphead, gaa, bpgoal)
	case T_FVAR:
	  PU_FVAR(haa, bphead, gaa, bpgoal)
	case T_CRTERM:
	  PU_CRTERM(haa, bphead, gaa, bpgoal)
	case T_GTERM:
	  PU_GTERM(haa, bphead, gaa, bpgoal)
	case T_NGTERM:
	  PU_NGTERM(haa, bphead, gaa, bpgoal)
	default:
	  FAIL_U
	break;
    }
    
    ga++;
    ha++;
  }
  
  return success;
  
} /*=====================================================================*/

#endif

#undef FAIL_U 
#undef UNIF_FAIL 

/* occ_ck can't treat T_GLOBL */

int             occ_ck (vp, st, base)
WORD           *vp;
WORD           *st;
environ        *base;
{
  WORD           *stt;

#ifdef DEBUG
  int             i;
  printf ("Occur check: %x * %x base: %x\n", vp, *vp, base);
  disp_(stdout,st,base); 
  for (i = 0; i < 20; i++) {
    printf ("==> %x %x, %x\n", 
	    (st + i), *(st + i), deref ((st + i), base));
  }
#endif
  
  
  /********************************************************************/
  /* test for NULL added by Goller - if pointer var is NULL           */
  /* then no occurs check will be done - applies for global variables */
  /********************************************************************/
  if (GETTAG (*st) == T_GTERM || vp == NULL)
    return 0;
  
  st++;
  while (GETTAG (*st) != T_EOSTR) {
    stt = deref (st, base);
    
#ifdef DEBUG
    printf ("Occur check parameter: %x * %x\n", stt, *stt);
    disp_ (stdout, stt, base);
#endif
    
    switch (GETTAG (*stt)) {
    case T_CONST:
    case T_GTERM:
      break;
#ifdef CONSTRAINTS
    case T_STRVAR:
#endif
    case T_FVAR:
      if (stt == vp) {
	return 1;
      }
      else
	break;
    case T_CRTERM:
      if (occ_ck (vp, stt, base))
	return 1;
      break;
      /* T_NGTERM is impossible here Goller */
    default:
      printf("******===========>Illegal tag in occur-check "); 
      disp_word(stt); printf("\n");
      break;
    }
    
    st++;
  }
  
  return 0;
}
/*#######################################################################*/


