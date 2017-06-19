/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: disp.c
 * VERSION:
 * DATE: 23.12.87
 * AUTHOR: J. Schumann
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *	13.4.88 new data structure
 *	29.4.88 voids & tvars
 *	5.10.88 variables
 *	14.2.89 numbers
 *	27.2.89 sets and lists
 *	28.2. file output
 *	25.4.   list bug
 *	1.6.
 *	5.6. partheo-messages
 *	15.6. LISTFUNCTOR for list-display
 *
 *	9.8.93	jsc		disp_ for terms in code as well
 *					Note: then, disp_ must be called with locbp=NULL
 *					symbflg entfernt
 *	16.9.93	jsc		exchanged for old version
 *	01.6.94	jsc		with new macros and different display of stack-rel
 *					and heap-related variables
 *
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "codedef.h"
#include "disp.h"
#include "symbtab.h"
/**/
#include "disp.h"
#include "deref.h"
#include "errors.h"

/******************************************************
 * disp_ displays an object on fout
 ******************************************************/
void  disp_ (fout, obj, locbp)
FILE           *fout;
WORD           *obj;
environ        *locbp;
{
  WORD           *op;
  WORD           *ga;
  short           sh;
  int             is_list;
  char           *s1;
  char            c;
  
  
  /* are we printing unprocessed code-terms? */
  if (!locbp &&  ISVAR(*obj)){
    /* we do not dereference */
    ga = obj;
  }
  else {
    ga = deref (obj, locbp);
  }
  is_list = 0;
  
  switch (GETTAG (*ga)) {
  case T_CONST:
    if (CONSTCELLISSYMBOL(*ga)) {
      /* is a constant */
      
      /* check, if it a ``negated'' constant */
      if (ISNEGATED(*ga)){
	/* yes, print a '~' */
	fputc ('~', fout);
      }
      /* prepare special characters */
      s1 = GETPRINTNAME(*ga);
      while (*s1) {
	if (*s1 == '\\') {
	  switch (*++s1) {
	  case 't':
	    c = '\t';
	    break;
	  case 'b':
	    c = '\b';
	    break;
	  case 'n':
	    c = '\n';
	    break;
	  case '0':
	  case '1':
	  case '2':
	  case '3':
	    c = *s1++ - '0';
	    c = 8 * c + *s1++ - '0';
	    c = 8 * c + *s1 - '0';
	    break;
	  default:
	    c = *s1;
	  }
	}
	else {
	  /* is NOT \\ */
	  c = *s1;
	}
	s1++;
	
	fputc (c, fout);
      }
    }
    else {
      /* is a number */
      sh = GETNUMBER(*ga);
      fprintf (fout, "%d", sh);
    }
    break;
  case T_FVAR:
    if (ga >= stack){
      fprintf (fout, "X%d", (int) (ga - stack));
    }
    else {
      fprintf (fout, "X_%d", (int) (ga - heap));
    }
    break;
  case T_VAR:
    fprintf (fout, "X"GETVALFORMAT, GETVAL(*ga));
    break;
  case T_GLOBL:
    fprintf (fout, "$X%d", (int) (ga - heap));
    break;
  case T_CRTERM:
  case T_GTERM:
  case T_NGTERM:
    /* display functor */
    
#ifdef BUILT_INS
    /* do we have a set */
    if (GETSYMBOL (*ga) == SETFUNCT) {
      disp_set (fout, ga);
      break;
    }
#endif
    
    /* test if it is a list */
    if (GETSYMBOL(*ga) == LISTFUNCT) {
      is_list = 1;
      fprintf (fout, "[");
    }
    else {
      /* is the functor a negated (predicate) symbol ? */
      if (ISNEGATED(*ga)){
	fputc('~',fout);
      }
      fprintf (fout, "%s(", GETPRINTNAME(*ga));
    }
    op = ++ga;
    while (GETTAG (*op) != T_EOSTR) {
      /* display terms */
      disp_ (fout, op++, locbp);
      if (GETTAG (*op) != T_EOSTR) {
	fprintf (fout, (is_list) ? "|" : ",");
      }
    }
    if (is_list) {
      fprintf (fout, "]");
    }
    else {
      fprintf (fout, ")");
    }
    break;
  case T_CREF:
    /* deref it & display */
    disp_ (fout, GETVAL (*ga) + code, locbp);
    break;
  default:
    sam_error ("illegal tag in disp_", ga, 1);
  }
}
