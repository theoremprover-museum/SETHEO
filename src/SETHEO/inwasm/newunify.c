/****************************************************
;;; MODULE NAME : newunify.c
;;;
;;; PARENT      : wunif.c
;;;
;;; PROJECT     : MPLOP
;;;
;;; AUTHOR      : Rudolf Zeilhofer
;;;
;;; DATE        : 18.04.96
;;;
;;; DESCRIPTION : unify recursively 2 termlists
;;;
;;; REMARKS     : 
;;;
****************************************************/

#include "symb.h"
#include "extern.h"

/*****************************************************************************/
/* prototypes                                                                */
/*****************************************************************************/
void newrearrange();
termtype *newderef();
int newunify();
int newmakeref();
int newocc_check();
void high_rename();


/*****************************************************************************/
/* newrearrange                                                              */
/*****************************************************************************/
/* unwind the trail, i.e. reset the pointers to the trail in the             */
/* symbol table and make the trail empty (trailcnt=0)                        */
/**/
void newrearrange()
    {
    int i;

    for (i=0; i<trailcnt; i++)
      {
	symbtab[trail[i].symb].loclist = NULL;
	symbtab[trail[i].symb].highloclist = NULL;
      };
    trailcnt = 0;
    }


/*****************************************************************************/
/* newderef                                                                  */
/*****************************************************************************/
/* dereference a variable cell                                               */
/* if it unbound (i.e. there is no trail-entry in the symbol table,          */
/* return the orig. term                                                     */
/* otherwise look at the term, the trail points to and follow up             */
/**/
termtype *newderef(term)
termtype *term;
    { 
    trailelt *tp;

    /* dereference variables only */
    while (term->type == VARIABLE || term->type == SVARIABLE) 
        {
	  switch (term->value)
	    {
	    case 0:
	      if (symbtab[term->symb].loclist)
		{
		  tp = (trailelt *)symbtab[term->symb].loclist;
		  /*printf("{Y0}");*/
		  term = tp->term;
		}
	      else
		{
		  /*printf("{N0}");*/
		  return term;
		}
	      break;
	    case 1:
	      if (symbtab[term->symb].highloclist)
		{
		  tp = (trailelt *)symbtab[term->symb].highloclist;
		  /*printf("{Y1}");*/
		  term = tp->term;
		}
	      else
		{
		  /*printf("{N1}");*/
		  return term;
		}
	      break;
	    default:
	      if (symbtab[term->symb].loclist)
		{
		  tp = (trailelt *)symbtab[term->symb].loclist;
		  /*printf("{Y0}");*/
		  term = tp->term;
		}
	      else
		{
		  /*printf("{N0}");*/
		  return term;
		}
	      break;

	      printf("\n-- Term : ");
	      print_lop_term(term);
	      printf (" -- %d (Value) --\n",term->value);
	      printf("ERROR (NEWDEREF - newunify.c) : Wrong value.");
	      exit(200);
	    }
	}
   
    return term;
    }

/*****************************************************************************/
/* unify                                                                     */
/*****************************************************************************/
/* unify 2 terms.                                                            */
/* returns: SUCESS or FAIL                                                   */
/* needs an empty trail, does no rearrange                                   */
/**/
int newunify(t1,t2)
termtype *t1, *t2;
    {
    termtype *lt1, *lt2;
    int rv;

   

    /* move thru term list */
    while (t1) 
        {
	if (!t2) /* may not happen !! */
	    return FAIL;
	/*	printf("\n(d1:) ");*/
	/* dereference the two terms */
	lt1 = newderef(t1);

	/*printf(" -- (d2:) ");*/
	lt2 = newderef(t2);

	/*printf("\n(t1:) ");print_lop_term(t1);
printf(" (%d) -- (lt1:) ",t1->value);print_lop_term(lt1);
printf(" (%d) \n(t2:) ",lt1->value);print_lop_term(t2);
printf(" (%d) -- (lt2:)",t2->value);print_lop_term(lt2);
printf(" (%d) ==> ",lt2->value);*/

	switch (lt1->type) 
            {
	    case STRINGCONST:
	    case NUMCONST:
	    case CONSTANT:
		 switch (lt2->type) 
                     {
		     case STRINGCONST:
		     case NUMCONST:
		     case CONSTANT:
			  if (lt1->symb != lt2->symb)
			     return FAIL; 

		   	  /* there are function symbols */
		   	  if (lt1->t_list) 
                              {
			      rv = newunify(lt1->t_list,lt2->t_list);
			      if (rv == FAIL)
			          return FAIL;
			      }
			  break;
		     case GVARIABLE:
			  /* there is a globvar array */
			  if (lt2->t_list)
			      break;
		     case SVARIABLE:
		     case VARIABLE :
			  /* lt2 is a (unbound) variable */
			  /* bind to lt2 variable        */
			  if (newmakeref(lt1,lt2))
			      return FAIL;
			  break;
		     default :
			  warning(NAM(lt1->symb)," new weak-unif error ");
			  break;
		     }
	         break;
            case GVARIABLE:
		 /* there is a globvar array */
		 if (lt1->t_list)
		     break;
            case SVARIABLE:
	    case VARIABLE :
		 /* lt1 is a (unbound) variable       */
		 /* bind lt2 to lt1, whatever lt2 is  */
	     	 if (newmakeref(lt2,lt1))
	      	     return FAIL;
		 break;
	    default :
		 warning(NAM(lt1->symb)," new weak-unif error ");
		 break;
	    }
	t1 = t1->next;
	t2 = t2->next;
	}
    return SUCCESS;
    }
		

/*****************************************************************************/
/* newmakeref                                                                */
/*****************************************************************************/
/* generate a new entry on the trail and make an assignment                  */
/* - pointer in the symbol-table for symb                                    */
/* - new trail entry with l/r-assignment                                     */
/* - there is at least one variable                                          */
/* - if necessary do the occur check                                         */
/* - returns: 0 if ok, -1 if occur check                                     */
/**/
int newmakeref(term1,term2)
termtype *term1,*term2;
    {
    termtype *term;
    termtype *symb;

    if (term1->type==CONSTANT || 
        term1->type==NUMCONST || 
        term1->type==STRINGCONST) 
        {
	/* bind term1 on symbol(term2) */

	term = term1;
	symb = term2;

	if (term1->t_list && newocc_check(term2,term1->t_list))
	    return (-1);
	}

    else 
        {
	/* there are two variables      */
	/* bind lower to higher         */
	/* no occur check needed        */
	/* dont bind variable to itself */

	if ((term1->symb == term2->symb) && (term1->value == term2->value))
	    /* no self reference !!  */
	    return (0);

	/* variable symb  is bound to variable term	  */
	/* use fixed order to avoid cycles ???     	  */
	/* structural variables ALWAYS bound to variables */

	if (term1->value < term2->value) 
	  {
 	    term = term1;
	    symb = term2;
	      }
	else if (term1->value == term2->value && term1 < term2) 
	  {	    
	    term = term1;
	    symb = term2;
	  }
	else 
	  {
	    term = term2;
	    symb = term1;
	  }
	}
    
    /* unbound variable make new trail cell */
    if (trailcnt == MAXTRAIL-1)
        {
	nferror("trail too small",0);
	return (0);
	}

    if (symb->value != 1) 					      
     globalcnt++;

    trail[trailcnt].symb = symb->symb;
    trail[trailcnt].term = term;
   
    switch (symb->value)
      {
      case 1:
	trail[trailcnt].loc  = &symbtab[symb->symb].highloclist;
	symbtab[symb->symb].highloclist = (predlist *)(trail + trailcnt);
	break;
      case 0:
	trail[trailcnt].loc  = &symbtab[symb->symb].loclist;
	symbtab[symb->symb].loclist = (predlist *)(trail + trailcnt);
	break;
      default:
	trail[trailcnt].loc  = &symbtab[symb->symb].loclist;
	symbtab[symb->symb].loclist = (predlist *)(trail + trailcnt);

	break;
	printf("ERROR (NEWMAKEREF - newunify.c) : Wrong value.");
	exit(200);
      }
    
    trailcnt++;
 
    return (0);
    }

/*****************************************************************************/
/* occ_check                                                                 */
/*****************************************************************************/
/* do the occur check :                                                      */
/*    a functor is bound to a variable                                       */
/*    test if the variable occurs in functor                                 */
/**/
int newocc_check(varterm,functerm)
termtype *varterm, *functerm;
{
termtype *term2;

while (functerm) {
      
	term2 = newderef(functerm);
	if ((term2->type == VARIABLE || term2->type == SVARIABLE) 
	     && term2->symb==varterm->symb && term2->value == varterm->value) {
	      return (1);
	   }
	if (term2->t_list)
	   if (newocc_check(varterm,term2->t_list))
	      return 1;
	functerm = functerm->next;
	}
return 0;
}


/*****************************************************************************/
/* high_rename                                                               */
/*****************************************************************************/

void high_rename(tlist,mode)
termtype *tlist;
int mode;
{
  
char name[80];

  if (! tlist)
    return;
  if (tlist->next)
    high_rename(tlist->next,mode);
  if (tlist->t_list)
    high_rename(tlist->t_list,mode);
  if (tlist->type == VARIABLE || tlist->type == SVARIABLE)
    {
      switch (mode)
	{
	case ALL_TO_1:
	  tlist->value = 1;/* flag set with newvalue */
	    break;

	case ALL_TO_0:
	  tlist->value = 0;/* flag set with newvalue */
	    break;

	case ALL_TO_NEW:
	  if (tlist->value == 1)
	    {
	      sprintf(name,"New_%s",symbtab[tlist->symb].name);
	      
	      tlist->symb = enter_name(enter_token(name),
				       symbtab[tlist->symb].type,0,0);
	      tlist->value = 0;
	    }
	  break;

	case REN_CONSTR:
	  if (tlist->value == 1) 
	    {
	      sprintf(name,"%s_%d",symbtab[tlist->symb].name,var_counter);
	      tlist->symb = enter_name(enter_token(name),SVARIABLE,0,0);
	    }
	  break;

	default:
	  printf("ERROR (INSERTVARCNT - newunify.c) : Wrong mode.");
	  exit(200);
	}
      
    }
     
}

/*######################################################################*/
