/*******************************************************************************
 *    S E T H E O                                                              
 *                                                                             
 * FILE: deref.c                                                               
 * VERSION:                                                                    
 * DATE: 23.12.87                                                              
 *	  5.2.88                                                            
 *		atari version and review
 * 		8.3.88 terms
 *		14.3.88
 *		13.4.88 CREF
 *		17.6.88 Temp-vars
 *		7.11.88	global variables
 *		31.1.89 (char) in switch
 *		10.05.89 error
 *   SUN ONLY ************************
 *		23.7.89
 *		22.10.89 >= error in trail
 * AUTHOR: J. Schumann
 * NAME OF FILE:
 * DESCR:
 * MOD: 
 *      23.01.92 CONSTRAINTS added by bschorer
 *  21.12.92 deref measurements DEREF_STATISTICS
 *      count number of deref calls
 *      and the length of the deref-chains
 *      up to a length of DEREF_MAX
 *	11.3.93	Mueller,Goller neue Constraints, Statistics
 *	17.9.93		jsc		remove of old lemma stuff, tell,told
 *					copy-binding
 * BUGS:
 *******************************************************************************/


#include <stdio.h>
#include <string.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
/**/
#include "deref.h"
#include "errors.h"


/******************************************************
 * statics + data for DEREF_STATISTICS
 ******************************************************/
#ifdef DEREF_STATISTICS

/* max length of deref-chain to be recorded */
#define DEREF_MAX 20

static long deref_cnt = 0;
static long deref_var = 0;
static long deref_bvar = 0;
static long deref_cref = 0;
static long deref_href = 0;
static long deref_others = 0;

static  long    deref_length[DEREF_MAX];

#endif


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  + deref
  + - dereferences bound terms, gives stack address for variable in
  +   code area, etc.
  +
  + special remarks:
  + - called from almost everywhere (setheo is mostly occupied with 
  +   dereferencing objects - tests by J.Schumann)
  + - obj points to goal-arg-area
  + - variables are addressed rel to lbp
  + - terms are handled as follows:
  +   o GTERM,NGTERM: cell contains offset/code
  +   o CRTERM: cell contains offset/heap
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

WORD *deref (obj, lbp)
register WORD  *obj;
environ        *lbp;

{ /*=====================================================================*/
  
#ifdef DEREF_STATISTICS
  register int    i;
  
  deref_cnt++;
#endif
  
  switch ((char) GETTAG (*obj)) {
    
  case T_CSTRVAR:
  case T_VAR:
    /* look object up in lbp area */
    obj = (WORD *) lbp +
      SIZE_IN_WORDS (environ) + GETVAL (*obj);
#ifdef DEREF_STATISTICS
    deref_var++;
#endif
    break;
    
  case T_BVAR:
#ifdef DEREF_STATISTICS
    deref_bvar++;
#endif
    break;
    
  case T_CREF:
#ifdef DEREF_STATISTICS
    deref_length[0]++;
    deref_cref++;
#endif
    return code + GETVAL (*obj);
    
  case T_HREF:
    obj = heap + GETVAL (*obj);
#ifdef DEREF_STATISTICS
    deref_href++;
#endif
    break;
    
  default:
#ifdef DEREF_STATISTICS
    deref_length[0]++;
    deref_others++;
#endif
    return obj;
  }
  
#ifdef DEREF_STATISTICS
  i=0;
#endif
  while (GETTAG(*obj) == T_BVAR) {
    obj = (WORD *) GETPTR (*obj);
#ifdef DEREF_STATISTICS
    i++;
#endif
  }
  
#ifdef DEREF_STATISTICS
  if (i >= DEREF_MAX){
    deref_length[DEREF_MAX]++;
  }
  else {
    deref_length[i]++;
  }
#endif
  
  return obj;
} /*=====================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  + ref_head
  + - <informelle beschreibung>
  +
  + special remarks:
  + - <e.g. called from>
  + - obj is a skelton
  + - NOTE: is NOT a constant
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

WORD *ref_head (obj)
WORD           *obj;

{ /*=====================================================================*/
  
  while (GETTAG(*obj) == T_BVAR) {
    obj = (WORD *) GETPTR (*obj);
  }
  
  /* is a non-pointer type */
  return obj;
  
} /*=====================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  + trail_var
  + - <informelle beschreibung>
  +
  + special remarks:
  + - <e.g. called from>
  + - return 1 if trailed object
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int trail_var (obj)
WORD   *obj;

{ /*=====================================================================*/
  
  if( (s_trail *)tr >= (s_trail *)trlast )
    sam_error("Trail overflow", NULL, 2);

  if (sizeof(WORD)>sizeof(TRAIL_STORAGE_TYPE))
    /* help !!! we can't store word's on the trail ... *//* somewhat suboptimal */
    sam_error("Trail storage type too small for WORD", NULL, 2);
    
  
  tr->ptr = (TRAIL_STORAGE_TYPE*)obj;
  tr->type = TRAIL_TYPE_WORD;

  /* memcpy(&tr->oval,obj,sizeof(WORD)); */
  /* replaced by: */
#ifdef WORD_64
  tr->oval.l1 = obj->tag;
  tr->oval.l2 = obj->value;
#else
  tr->oval = (unsigned long) *obj;
#endif

  tr++;
  
  return 1;
  
} /*=====================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  + trail_lint
  + - <informelle beschreibung>
  +
  + special remarks:
  + - <e.g. called from>
  + - return 1 if trailed object
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int trail_lint (obj)
long int *obj;

{ /*=====================================================================*/
  
  if( (s_trail *)tr >= (s_trail *)trlast )
    sam_error("Trail overflow", NULL, 2);

  if (sizeof(long int)>sizeof(TRAIL_STORAGE_TYPE))
    /* help !!! we can't store word's on the trail ... *//* somewhat suboptimal */
    sam_error("Trail storage type too small for long int", NULL, 2);

  tr->ptr = (TRAIL_STORAGE_TYPE*)obj;
  tr->type = TRAIL_TYPE_LINT;

  /* memcpy(&tr->oval,obj,sizeof(long int)); */
  /* replaced by: */
#ifdef WORD_64
  tr->oval.l1 = (unsigned long) *obj;
#else
  tr->oval    = (unsigned long) *obj;
#endif

  tr++;
  
  return 1;
  
} /*=====================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  + trail_int
  + - <informelle beschreibung>
  +
  + special remarks:
  + - <e.g. called from>
  + - return 1 if trailed object
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int trail_int (obj)
int *obj;

{ /*=====================================================================*/
  
  if( (s_trail *)tr >= (s_trail *)trlast )
    sam_error("Trail overflow", NULL, 2);

  if (sizeof(int)>sizeof(TRAIL_STORAGE_TYPE))
    /* help !!! we can't store word's on the trail ... *//* somewhat suboptimal */
    sam_error("Trail storage type too small for int", NULL, 2);

  tr->ptr = (TRAIL_STORAGE_TYPE*)obj;
  tr->type = TRAIL_TYPE_INT;

  /* memcpy(&tr->oval,obj,sizeof(int)); */
  /* replaced by: */
#ifdef WORD_64
  tr->oval.l1 = (unsigned long) *obj;
#else
  tr->oval     = (unsigned long) *obj;
#endif

  tr++;
  
  return 1;
  
} /*=====================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  + trail_ptr
  + - <informelle beschreibung>
  +
  + special remarks:
  + - <e.g. called from>
  + - return 1 if trailed object
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int trail_ptr (obj)
void **obj;

{ /*=====================================================================*/
  
  if( (s_trail *)tr >= (s_trail *)trlast )
    sam_error("Trail overflow", NULL, 2);

  if (sizeof(void*)>sizeof(TRAIL_STORAGE_TYPE))
    /* help !!! we can't store word's on the trail ... *//* somewhat suboptimal */
    sam_error("Trail storage type too small for ptr", NULL, 2);

  tr->ptr = (TRAIL_STORAGE_TYPE*)obj;
  tr->type = TRAIL_TYPE_PTR;

  /* memcpy(&tr->oval,obj,sizeof(void *)); */
  /* replaced by: */
#ifdef WORD_64
  tr->oval.l1 = (unsigned long) *obj;
#else
  tr->oval     = (unsigned long) *obj;
#endif

  tr++;
  
  return 1;
  
} /*=====================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  + untrail
  + - <informelle beschreibung>
  +
  + special remarks:
  + - <e.g. called from>
  + - untrail from tr downto tpt
  +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void untrail (tpt)
s_trail        *tpt;
{ /*=====================================================================*/
  
  s_trail        *lpt;
  
  /*-----------------------------------------------------------------------*/
  
  for (lpt = tr - 1; lpt >= tpt; lpt--) {
    switch (lpt->type) {

      case TRAIL_TYPE_WORD:
        /* memcpy(lpt->ptr,&lpt->oval,sizeof(WORD)); */
	/* replaced by: */
#ifdef WORD_64
	lpt->ptr->l1 = lpt->oval.l1;
	lpt->ptr->l2 = lpt->oval.l2;
#else
	*(lpt->ptr) = (WORD) lpt->oval;
#endif
        break;

      case TRAIL_TYPE_LINT:
        /* memcpy(lpt->ptr,&lpt->oval,sizeof(long int)); */
	/* replaced by: */
#ifdef WORD_64
	*((long int*) lpt->ptr) = (long int) lpt->oval.l1;
#else
	*((long int*) lpt->ptr) = (long int) lpt->oval;
#endif
        break;

      case TRAIL_TYPE_INT:
        /* memcpy(lpt->ptr,&lpt->oval,sizeof(int)); */
	/* replaced by: */
#ifdef WORD_64
	*((int*) lpt->ptr) = (int) lpt->oval.l1;
#else
	*((int*) lpt->ptr) = (int) lpt->oval;
#endif
        break;

      case TRAIL_TYPE_PTR:
        /* memcpy(lpt->ptr,&lpt->oval,sizeof(void *)); */
	/* replaced by: */
#ifdef WORD_64
	*((void**) lpt->ptr) = (void *) lpt->oval.l1;
#else
	*((void**) lpt->ptr) = (void *) lpt->oval;
#endif
        break;

      default:
        sam_error("unknown trail data type",NULL,2);
    }
  }
  
} /*=====================================================================*/


#ifdef DEREF_STATISTICS
/******************************************************
 * print_deref_statistics
 ******************************************************/
void print_deref_statistics()
{
  int i;
  printf("Number of calls to deref():   %ld\n",deref_cnt);
  printf("Number of calls w/datatype\n");
  printf("T_VAR : %ld\n",deref_var);
  printf("T_BVAR: %ld\n",deref_bvar);
  printf("T_CREF: %ld\n",deref_cref);
  printf("T_HREF: %ld\n",deref_href);
  printf("T_*   : %ld\n\n",deref_others);
  
  printf("length of deref chains: [0..%d]\n",DEREF_MAX);
  printf("(last entry: %d or larger)\n",DEREF_MAX);
  for(i=0;i<DEREF_MAX;i++){
    printf("%3.3d    %ld\n",i,deref_length[i]);
  }
}

#endif

/*#######################################################################*/


