/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: i_lpo_mod.c                                  */
/* VERSION:                                           */
/* DATE:   11.04.1996                                 */
/* AUTHOR: Elvira & Joachim Steinbach                 */
/* NAME OF FILE:                                      */
/* DESCR:  Comparison of two terms wrt. the           */
/*         lexicographic path ordering                */
/* MOD:                                               */
/* BUGS:                                              */
/******************************************************/

#include <stdio.h>
#include "tags.h"
#include "machine.h"
#include "symbtab.h"
#include "unifeq.h"
#include "unification.h"

#ifdef STATISTICS1
#define UNIF_FAIL(x) {unif_fails++;FAIL_U}
#else
#define UNIF_FAIL(x) FAIL_U
#endif

#define COPY_CONST
#define FAIL_U {return failure;}

#include <sys/types.h>
#include <sys/times.h>

#include <time.h>
#ifndef CLK_TCK
#define CLK_TCK 60
#endif

#define INSTR_LENGTH		2

float  lpo_time;
int    ti;


static int is_prec_gr(symb1,symb2)
unsigned long  symb1;
unsigned long  symb2;
{
    return ((int) symb1 < (int) symb2);
}


static int is_subterm(v,t)
WORD  *v,*t;
{
    WORD  *var,*term;
    int   arity_term,i;
    
    var = deref(v,bp); term = deref(t,bp);
    
    if (chk_arg(var,bp,term,bp)) 
       return 1;

    if (GETTAG(*term) == T_FVAR) return 0;

    arity_term = GETARITY(*term);
    for ( i=1; i <= arity_term; i++ )
        if (is_subterm(var,term+i)) return 1;
    
    return 0;
}


static int lpo(s_in,t_in)
WORD *s_in,*t_in;
{
   WORD *s,*t,*args_s,*args_t;
   int  arity_s,arity_t,i,j,out;

   s = deref(s_in,bp); t = deref(t_in,bp);

   if (GETTAG(*s) == T_FVAR)
     { if (s == t) return 2;
       return 0; }

   if (GETTAG(*t) == T_FVAR)
      return (is_subterm(t_in,s_in));

   arity_s = GETARITY(*s); arity_t = GETARITY(*t);

   if (is_prec_gr(GETSYMBOL(*s),GETSYMBOL(*t)))
     { if (arity_t == 0) return 1;
       args_t = t+1;
       for (i=0; i < arity_t; i++)
         { out = lpo(s,args_t+i);
           if ((out == 2) || (out == 0)) return 0; }
       return 1; }

   if (GETSYMBOL(*s) == GETSYMBOL(*t))
     { if (arity_s == 0) return 2;
       i = 0;
       out = 2;
       args_s = s+1; args_t = t+1;
       while ((i < arity_s) && (out == 2))
         { out = lpo(args_s+i,args_t+i);
           if (out == 1)
	     { j = i + 1;
               while ((j < arity_t) && (out == 1))
	         { if (lpo(s,args_t+j) != 1) out = 0;
                   j = j + 1; }
               if (out == 1) return 1; }
           i = i + 1; }
       if (out == 2) return 2;
       if (arity_s == 1) return 0; }
   
   if (arity_s != 0)
     { args_s = s+1;
       for (i=0; i < arity_s; i++)
           if (lpo(args_s+i,t) >= 1) return 1; }

   return 0;
 }


instr_result i_lpoeq()
{
    WORD  *arg1,*arg2,*arg3;       /* arguments of built-in */
    WORD   ret_val;                /* return value */
    
    arg1 = deref(ARGV(0),bp);
    arg2 = deref(ARGV(1),bp);
    arg3 = deref(ARGV(2),bp);
    
    pc += INSTR_LENGTH;
    
    if (lpo(arg2,arg1) == 1) {
        GENNUMBER(ret_val,1);
    }
    else {
	GENNUMBER(ret_val,0);
    }

    /* unify the 3rd argument with ret_val */
    trail_var(arg3);
    UN_CONST((&ret_val), bp, arg3, bp);
    return success;
  }

/******************************************************************/
/* returns 1, if the corresponding constraint cannot be satisfied */
/* otherwise, 0 is returned                                       */
/******************************************************************/
