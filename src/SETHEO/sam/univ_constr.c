/*#########################################################################
# File:			univ_constr.c
# SCCS-Info:		8/7/92, @(#)univ_constr.c	1.2
# Author:		M.Bschorer 
# Date:			15.01.92
# Context:		setheo (SETHEO)
#
# Contents:		functions for all constraint-types               
#                       each of them can use it !!
#
# Modifications:
#	(when)		(who)		(what)
#	15.01.92	M.Bschorer	new function disp_constr();
#	15.01.92	M.Bschorer	new function disp_con();   
#	16.01.92	M.Bschorer	new function c_optimize_gen();
#	22.01.92	M.Bschorer	new function c_create();
#	13.01.92	M.Bschorer	new function c_genzaehler(); 
#	14.01.92	M.Bschorer	new function c_trail();
#	27.01.92	M.Bschorer	new function c_stack_control();
#	12.02.92	M.Bschorer	new function constr_statistik();
#	13.02.92	M.Bschorer	new function c_gen_strvar();
#	14.04.92	M.Bschorer	new function c_anl_strvar();
#	24.11.92	C.Mueller	c_create and c_check: subconstraint pointers
#							are written to T_FVAR directly (instead of 0)
#							choicepoint optimization is removed
#	30.12.92    C.Mueller   c_genzaehler becomes c_gencounter
#	11.3.93	Mueller,Goller neue Constraints, Statistics
#
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
#########################################################################*/

#include <stdio.h> 
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "disp.h"
#include "codedef.h"
#include "proto_unify.h"
#include "symbtab.h"
/**/
#include "univ_constr.h"
#include "antilemma.h"
#include "deref.h"
#include "errors.h"
#include "unifred.h"


#ifdef CONSTRAINTS

/**************************************************************************
 * declarations
 **************************************************************************/

static void           c_stack_control();
#ifdef STATPART
/*extern int     cputimelimit; */ /* CS 16.9.84 */
extern float   evaltime;  /* time for all it-deep. cycles so far */
int check_cnt = 0;
#endif

extern unsigned long debug_level;

/**************************************************************************
 * code
 **************************************************************************/


/*#######################################################################*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ disp_con
+ - displays right subconstraint side
+
+ special remarks:
+ - called for debugging purposes only
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static void  disp_con (obj, locbp)

WORD           *obj;
environ        *locbp;

{ /*=====================================================================*/
  
  WORD           *op;
  WORD           *ga;
  short           sh;
  int             is_list;
  char           *s1;
  char            c;
  literal_ctrl_block  *lp;

  /*-----------------------------------------------------------------------*/

    lp = bp->lcbptr;
    ga = deref (obj, locbp);
    is_list = 0;

    switch (GETTAG (*ga)) {
	case T_CONST:
	    if (symbflg) {
		if (CONSTCELLISSYMBOL(*ga)) {
		    /* is a constant */
		    /* prepare special characters */
		    s1 = GETPRINTNAME(*ga);
		    while (*s1) {
		        if (*s1 == '\\') {
			    switch (*++s1) {
			        case 't':  c = '\t';
			       	           break;
			        case 'b':  c = '\b';
			       	           break;
			        case 'n':  c = '\n';
			       	           break;
			        case '0':
			        case '1':
			        case '2':
			        case '3':  c = *s1++ - '0';
			       	           c = 8 * c + *s1++ - '0';
			       	           c = 8 * c + *s1 - '0';
			       	           break;
			        default:   c = *s1;
			    }
			}
		        else {     /*  (*s1 != '\\')  */
		            c = *s1;
		        }
		        s1++;
		        printf("%c",c); 
		    }   /* while */
		}
		else {
		    /* is a number */
		    sh = (short) GETSYMBOL (*ga);
		    printf ("%d", sh);
		}
	    }
	    else {
		printf ("c"GETVALFORMAT, GETVAL (*ga));
	    }
	    break;

	case T_STRVAR:
	    printf("X<%d>", (int)ga);
	    break;

	case T_FVAR:
	    if ( ga > heap && ga < hp )
		printf ("HX%d", (int) (ga - heap));
	    if ( ga > stack && ga < sp )
		printf ("SX%d", (int) (ga - stack));
	    break;

	case T_GLOBL:
	    printf ("$X%d", (int) (ga - heap));
	    break;

	case T_CRTERM:
	case T_GTERM:
	case T_NGTERM:
	    /* display functor */
#ifdef BUILT_INS
	    /* do we have a set */
	    if (GETSYMBOL (*ga) == SETFUNCT) {
		/*disp_set (fout, ga);*/
		break;
	    }
#endif

	    if (symbflg) {
		/* test if it is a list */
		/* if (*(symbtab[GETSYMBOL(*ga)].name) == '[') */
	        if (GETSYMBOL (*ga) == LISTFUNCT) {
		    is_list = 1;
		    printf ("[");
		}
		else {
		    printf ("%s(", symbtab[GETSYMBOL (*ga)].name);
		}
	    }
	    else {
		printf ("c"GETVALFORMAT"(", GETSYMBOL (*ga));
	    }
	    op = ++ga;
	    while (GETTAG (*op) != T_EOSTR) {
		/* display terms */
		disp_con (op++, locbp);
		if (GETTAG (*op) != T_EOSTR) {
		    printf ((is_list) ? "|" : ",");
		}
	    }
	    if (is_list) {
		printf ("]");
	    }
	    else {
		printf (")");
	    }
	   break;

	case T_CREF:
	    /* deref it & display */
	    disp_con (GETVAL (*ga) + code, locbp);
	    break;

	default:
	    printf("Tag "GETTAGFORMAT"\n", GETTAG(*ga));
	    sam_error ("illegal tag in disp_con", ga, 1);
    }
} /*=====================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ c_stack_control
+ - checks c-stack for overflow
+
+ special remarks:
+ - called, whenever increasing c_sp
+ - exit setheo in case of overflow
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static void  c_stack_control()            

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

  if ( (c_sp + SIZE_IN_WORDS(constr_right)) >= c_stack_end )
       sam_error("Constraint-Stack-Overflow", NULL, 2);

} /*=====================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ pu_untrail
+ - untrails the protocol-unification-trail
+
+ special remarks:
+ - set the pu_tr down to the base of the pu_trail
+ - <side effects, return values>
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void  pu_untrail()              


{ /*=====================================================================*/
	
  s_trail    *pu_trpt;

  /*-----------------------------------------------------------------------*/

    for ( pu_trpt = pu_tr - 1 ; pu_trpt >= pu_trail ; pu_trpt-=1 ){
        /* memcpy(pu_trpt->ptr,(void*)&pu_trpt->oval,sizeof(WORD)); */
	/* replaced by: */
#ifdef WORD_64
	pu_trpt->ptr->l1 = pu_trpt->oval.l1;
	pu_trpt->ptr->l2 = pu_trpt->oval.l2;
#else
 	*(pu_trpt->ptr) = (WORD) pu_trpt->oval;
#endif      

    }
    pu_tr = pu_trail;

} /*=====================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ c_gencounter
+ - generates a counter for a constraint on the c_stack 
+
+ special remarks:
+ - called from c_gen_optimize() 
+ - returns a pointer to the new counter
+ - increments the c-stackpointer after checking for c_stack overflow
+ - initialize new counter to value 0
+ - exit setheo if counter cannot be generated in case of c_stack overflow
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

c_counter *c_gencounter(tag,ch,infc) 
int         tag;      /* kind of constraint for which counter is generated */
choice_pt  *ch;       /* choicepoint from which constraint is generated */
int         infc;     /* only important for antilemmata: 
                         infcount of corresponding antilemma */
{ /*=====================================================================*/
	
  c_counter *c_zaehler;

/*-----------------------------------------------------------------------*/

    if (c_sp + SIZE_IN_WORDS(c_counter) >= c_stack_end)
	sam_error("Constraint-Stack-Overflow", NULL, 2);

    c_zaehler = (c_counter *)c_sp;
    c_sp += SIZE_IN_WORDS(c_counter);

    /************************************************************************/
    /* no subconstraint at creation of counter => length of constraint is 0 */
    /************************************************************************/
    c_zaehler->length = 0;

#ifdef ANTILEMMATA
    if (anl_constr) {
        c_zaehler->chp = ch;
        c_zaehler->infcount = infc;
    }
#endif
    GENOBJ(c_zaehler->next, (unsigned int)c_counter_sp, tag);
    c_counter_sp = c_zaehler;

    return c_zaehler;
  
} /*=====================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ c_trail
+ - trails object on the protocol-unification-trail
+
+ special remarks:
+ - checks for trail-overflow and produces an error when trail overflows
+ - returns 1 if the object was successfully trailed
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int c_trail(obj)                 
WORD *obj;

{ /*=====================================================================*/

    /***************************/
    /* pu_trail-overflow-check */	
    /***************************/
    if ( (s_trail *)pu_tr >= (s_trail *)pu_tr_last - 1 )
        sam_error(" protocol-unification-trail overflow ",NULL, 2);

    /*********************************/
    /* trail adress & original value */
    /*********************************/
    pu_tr->ptr = (TRAIL_STORAGE_TYPE*)obj;
    /* memcpy((void*)&pu_tr->oval,obj,sizeof(WORD)); */
    /* replaced by: */
#ifdef WORD_64
    pu_tr->oval.l1 = obj->tag;
    pu_tr->oval.l2 = obj->value;
#else
    pu_tr->oval = (WORD) *obj;
#endif    
    pu_tr++;
        
    return 1;
} /*=====================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ un_protoco
+ - protocol - unification    
+
+ special remarks:
+ - very similiar to un_red(...) from the modul unifred.c
+ - returns 1 if unification possible, 0 if not.
+ - side effects: possible unifications produce trailings, and pro
+   unification, trail the environment and the argument-pointer of the
+   trailed T_FVAR.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int un_protoco (gp1, lbp1, gp2, lbp2, nargs)
    environ        *lbp1;
    environ        *lbp2;
    WORD           *gp1;
    WORD           *gp2;
    int             nargs;

{ /*=====================================================================*/
	
  WORD           *lga,
                 *lha;
  int             i;
/*-----------------------------------------------------------------------*/

#define FAIL_U2(x) {return 0;}

    for (i = 0; i < nargs; gp1++, gp2++, i++) {

	/**********************/
	/* unify one argument */
	/**********************/

	lha = deref (gp1, lbp1);
	lga = deref (gp2, lbp2);

	switch (GETTAG(*lha)) {
	    case T_STRVAR:   PU_STRVAR(lha, lbp1, lga, lbp2)
	    case T_CONST:    PU_CONST(lha, lbp1, lga, lbp2)
	    case T_FVAR:     PU_FVAR(lha, lbp1, lga, lbp2)
	    case T_CRTERM:   PU_CRTERM(lha, lbp1, lga, lbp2)
	    case T_GTERM:    PU_GTERM(lha, lbp1, lga, lbp2)
	    case T_NGTERM:   PU_NGTERM(lha, lbp1, lga, lbp2)
	    default:         FAIL_U2("bla")
			     break;
	}
    }

    /*******************************************************/
    /* return 1 means: protocol-unification was successful */
    /*******************************************************/

    return 1;

#undef FAIL_U
} /*=====================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ c_gen_optimize      
+ - the optimized constraint is placed on the pu_trail between pu_tr and
+   the pu_trail.  -  take it
+
+ special remarks:
+ - pu_trail must be less than pu_tr                     
+ - returns 1, if subconstraints are generated 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int c_gen_optimize(zaehler)          

c_counter *zaehler;                      /* Pointer to the zaehler */
                                         /* of a constraint.       */
{ /*=====================================================================*/
	
  s_trail  *tpt;
  WORD     *right_gp;
  WORD     *left_gp;

/*-----------------------------------------------------------------------*/

    /********************************************************/
    /* look in the trail, which variables were bound by the */
    /* protocol-unification                                 */
    /********************************************************/
    for (tpt = pu_tr-1; tpt >= pu_trail; tpt--) {

	/************************************************************/
	/* left_gp points to the T_FVAR that was bound by protocol  */
        /* unification. Look for what it's been bound to and create */
        /* a constraint                                             */
	/************************************************************/
	left_gp = (WORD *)tpt->ptr;
	right_gp = deref(left_gp, NULL);

        /* memcpy(tpt->ptr,(void*)&tpt->oval,sizeof(WORD)); */
	/* replaced by: */
#ifdef WORD_64
	tpt->ptr->l1 = tpt->oval.l1;
	tpt->ptr->l2 = tpt->oval.l2;
#else
	*(tpt->ptr) = (WORD) tpt->oval;      /* untrail the bound var */
#endif                
	c_create(left_gp, right_gp, zaehler);

    }
       
    pu_tr = pu_trail;                        /* reset pu_trail */
         
    return 1;

} /*=====================================================================*/





/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ c_create
+ - creates a subconstraint on the c_stack
+
+ special remarks:
+ - only called from c_gen_optimize (above) and anl_constr.c
+ - increaeses the c_sp with sizeof(constr_right)
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int  c_create(left_gp, right_gp, zaehler)
WORD       *left_gp;          /* Goal-pointer of left side of sub_constr. */
WORD       *right_gp;         /* Goal-pointer of right side of sub_constr */
c_counter  *zaehler;          /* Pointer to counter of constraint         */

{ /*=====================================================================*/

    trail_var(left_gp);       /* trail variable cause it contains  */
	                      /* the pointer to the subconstr.list */
	                      /* which will be changed             */
    c_stack_control();

    ((constr_right *)c_sp)->sub_gp  = (WORD *)right_gp;
// printf("%s:%d: sub_gp: %lx\n",__FILE__,__LINE__,right_gp);
// printf("%s:%d: sub_gp: %lx\n",__FILE__,__LINE__,(WORD *)right_gp);
    ((constr_right *)c_sp)->zaehler = zaehler;
    ((constr_right *)c_sp)->next    = (constr_right *) GETPTR(*left_gp);
// printf("%s:%d: left_gp: %lx\n",__FILE__,__LINE__,left_gp->value);
// printf("%s:%d: left_gp: %lx\n",__FILE__,__LINE__,(constr_right *) GETPTR(*left_gp));

    /********************************************************/
    /* c_sp points to the newly generated constraint        */
    /* the old subconstraint pointer is saved in next       */
    /* to get a linked list of subconstraints per variable  */
    /********************************************************/

//JSC-092016    GENOBJ(*left_gp, (unsigned int)c_sp, T_FVAR);
    GENOBJ(*left_gp, (unsigned long)c_sp, T_FVAR);

    c_sp += SIZE_IN_WORDS(constr_right);
    zaehler->length += 1;

    return 1;

} /*=====================================================================*/



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ c_check 
+ - Called whenever unification with a T_FVAR is planned. Only T_FVARs do
+   have constraints - the pointers to the subconstraint lists are
+   written directly into the T_FVAR. NULL-pointer means: no constraints
+   do exist for the said variable. c_check gets the term to which the
+   variable is to be bound (the binding has not been completed yet).
+   (term = right_gp)
+   c_check will compare the term to all terms occurring in the 
+   list where sub_constr points to. This comparision is handled by
+   un_protoco (protocol unification). If the term doesn't match any of the
+   subconstraints then it returns to the caller with a permission to
+   bind term to the T_FVAR. Otherwise the unification will fail and
+   no binding is produced.
+
+ special remarks:
+ - called by all macros in unification.h, by files usimple.ins and
+   unifterm.ins
+ - returns 1, if unification is allowed, and 0, if a fail must follow.
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int c_check(var, right_gp)
WORD         *var;                  /* pointer to T_FVAR which will be    */
				    /* bound if no constraint forbids     */
				    /* binding to right_gp                */
WORD         *right_gp;             /* pointer to object which is planned */
				    /* to be bound to var by unification  */

{ /*=====================================================================*/
	
  constr_right   *sub_constr;         /* pointer to subconstr. list        */
  
  WORD           *lhp;

/*-----------------------------------------------------------------------*/

// printf("%s:%d: var: %lx\n",__FILE__,__LINE__,var);
    lhp = hp;
    pu_tr = pu_trail;
    sub_constr = (constr_right *) GETPTR(*var);
// printf("%s:%d: var: %lx\n",__FILE__,__LINE__,sub_constr);
// printf("%s:%d: var: %lx\n",__FILE__,__LINE__,var->value);
    if (!sub_constr)                    /* var doesn't have subconstraints   */
	return 1;                       /* return - there's nothing to check */

#ifdef STATISTICS2
    c_check_count++;                    /* increment the counter */
#endif                                  /* for c_checks          */


    while (sub_constr) {
                                                 
//	printf("%s:%d: sub_constr: %lx\n",__FILE__,__LINE__,sub_constr);
//	printf("%s:%d: sub_constr->zaehler: %lx\n",__FILE__,__LINE__,sub_constr->zaehler);
//	printf("%s:%d: sub_constr->zaehler->length: %d\n",__FILE__,__LINE__,sub_constr->zaehler->length);
	if (sub_constr->zaehler->length == -1) {
	    /****************************************************/
	    /* constraint to which sub_constr belongs cannot be */
	    /* violated any more - look at next subconstraint   */
	    /****************************************************/
	    sub_constr = (constr_right *)sub_constr->next;
	    continue;
	}

	if (un_protoco(right_gp, NULL, sub_constr->sub_gp, NULL, 1)) {
	    /*******************************/
	    /* subconstr. may be violated. */
	    /*******************************/

	    if (pu_tr == pu_trail) {
	      	/*****************************/
	      	/* subconstraint is violated */
	       	/*****************************/
	       	trail_int(&sub_constr->zaehler->length);/* trail old zaehler */
	       	sub_constr->zaehler->length -= 1;

	       	if(!sub_constr->zaehler->length) {
		    /********************************************/
	            /* the whole constraint is violated -> fail */
		    /********************************************/

#ifdef STATISTICS1
		    switch(GETTAG(sub_constr->zaehler->next)) {

			case REGULARITY:  c_reg_fails++;
					  break;
		        case TAUTOLOGIE:
		        case SUBSUMPTION: c_ts_fails++;
					  break;
#ifdef ANTILEMMATA
			case ANTILEMMA:
                            if (inf_bound  &&  sub_constr->zaehler->infcount > infcount) {
                                delay_anl_constr(sub_constr->zaehler,INF_BOUND);
                                sub_constr = sub_constr->next;
			        continue;
			    }
                            c_anl_fails++;
			    break;
#endif
			default:          printf("\nConstraint Flag "GETTAGFORMAT" not recognized in c_check.\n",
						 GETTAG(sub_constr->zaehler->next) );
					  break;
		    }
#endif
/* CS 16.9.94 */
/*
#ifdef HP 
		      if( cputimelimit && check_cnt++ == 10 ){          
		        check_cnt = 0;
		        check_timelimit();
		        }
#endif
*/
		    return 0; 
	        }
	    }   /* if (pu_tr == pu_trail) */

	    else {   /* if not (pu_tr == pu_trail) */
		/******************************************************/
		/* new subconstraints have to be generated from the   */
	    	/* variable bindings made during protocol unification */
		/******************************************************/

		trail_int(&sub_constr->zaehler->length);
	    	sub_constr->zaehler->length -= 1;

	       	c_gen_optimize(sub_constr->zaehler);

	    }   /* if not (pu_tr == pu_trail) */

	}   /* if (un_protoco(...)) */

	else {   /* if not (un_protoco(...)) */
	    /*******************************************************/
	    /* the checked subconstraint cannot be violated any    */
	    /* more so the whole constraint cannot be violated and */
	    /* should not be checked in future c_check calls       */
	    /*******************************************************/

	    hp = lhp;            /* garbage collection on the heap */
	    pu_untrail();        /* reset pu-trail     */

	    trail_int(&sub_constr->zaehler->length);
	    sub_constr->zaehler->length = -1;

	}   /* if not (un_protoco(...)) */

	sub_constr = sub_constr->next;
				  
    }   /* while */

    return 1;

} /*=====================================================================*/


#endif

/*#######################################################################*/

/* CS 16.9.94 */
/*
check_timelimit(){   
struct tms buffer;

times( &buffer );
PRINT_LOG2("Checking timelimit (%d secs): %4.1f\n",cputimelimit,((float)buffer.tms_utime / (float)CLK_TCK) - evaltime);
if( ((float)buffer.tms_utime / (float)CLK_TCK) - evaltime > cputimelimit ){
     spsighand(0);  
     }
}
*/
