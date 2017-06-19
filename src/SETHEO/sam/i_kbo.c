/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: i_kbo.c                                      */
/* VERSION:  1                                        */
/* DATE:     03.07.96                                 */
/* AUTHOR:   Elvira Steinbach                         */
/* NAME OF FILE:                                      */
/* DESCR:                                             */
/* MOD:                                               */
/* BUGS:                                              */
/******************************************************/

#include <stdio.h>
#include "stdlibwrap.h"
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

#define INSTR_LENGTH		2

/*
 * static void print_varlist(v_list)
 * varlist *v_list;
 * { varlist *ptr;
 *
 * ptr = v_list;
 * while ( ptr != NULL )
 *   { disp_(stderr,ptr->var,bp);
 *     fprintf(stderr,": %d\n\n",ptr->no_occ);
 *     ptr = ptr->next; }
 * }
 */

struct var_list {
  WORD             *var;
  int              no_occ;
  struct var_list  *next;
};

typedef struct var_list  varlist;

static int op_weight(symb)
WORD  *symb;
{ return 1;
}

static int weight_multiplier()
{ return 1;
}

static int term_weight(term)
WORD  *term;
{ WORD  *t, *args_t;
  int   i, arity_head, weight;

  t = deref(term,bp);
//JSC-092016  weight = op_weight(*t);
  weight = op_weight(t);

  if ( GETTAG(*t) != T_FVAR )
    { arity_head = GETARITY(*t);
      if ( arity_head != 0 )
        { args_t = t+1;
          for ( i=0; i < arity_head; i++ )
              weight += weight_multiplier() * term_weight(args_t+i); } }
  return weight;
}

static int is_prec_gr(symb1,symb2)
unsigned long symb1, symb2;
{ return ( (int) symb1 < (int) symb2 );
}

static varlist *get_varlist(term,v_list,occ_var)
WORD      *term;
varlist   *v_list;
int        occ_var;
{ WORD     *t, *args_t;
  int      i, found, arity_t;
  varlist  *ptr, *new_elem;

  t = deref(term,bp);

  if ( GETTAG(*t) == T_FVAR )
    { found = 0;
      ptr = v_list;

      while ( ptr != NULL )
	   { if ( ptr->var == t )
               { ptr->no_occ++;
                 ptr = NULL;
                 found = 1; }
             else ptr = ptr->next; }
      if ( found == 0 )
        { new_elem = sam_malloc(sizeof(varlist));
          new_elem->var = t;
          new_elem->no_occ = occ_var;
          new_elem->next = v_list;
          v_list = new_elem; } }
  else
    { arity_t = GETARITY(*t);
      if ( arity_t != 0 )
        { args_t = t+1;
          for ( i=0; i < arity_t; i++ )
	     { ptr = get_varlist(args_t+i,v_list,occ_var * weight_multiplier());
               v_list = ptr; } } }
  return v_list;
}

static int var_supseteq(term1,term2)
WORD  *term1, *term2;
{ WORD     *t1, *t2;
  varlist  *v_list1, *ptr1, *v_list2, *ptr2;
  int      found;

  t1 = deref(term1,bp);
  t2 = deref(term2,bp);
  v_list1 = get_varlist(t1,NULL,1);
  v_list2 = get_varlist(t2,NULL,1);

  ptr2 = v_list2;

  while ( ptr2 != NULL )
       { ptr1 = v_list1;
         found = 0;
         while ( ptr1 != NULL )
	      { if ( ptr1->var == ptr2->var )
                  { if ( ptr2->no_occ > ptr1->no_occ )
		       return 0;
                    else
                      { ptr1 = NULL;
                        found = 1; } }
                else ptr1 = ptr1->next; }
         if ( found == 0 )
            return 0;
         ptr2 = ptr2->next; }
  return 1;
}

static int kbo(term1,term2)
WORD  *term1, *term2;
{ WORD  *t1, *t2, *args_t1, *args_t2;
  int   i, w1, w2, arity_head;

  t1 = deref(term1,bp);
  t2 = deref(term2,bp);

  if ( !( var_supseteq(t1,t2) ) )
     return 0;

  w1 = term_weight(t1);
  w2 = term_weight(t2);

  if ( w1 > w2 )
     return 1;
  if ( w1 == w2 )
    { if ( GETTAG(*t1) == T_FVAR )
         return 0;
      if ( GETTAG(*t2) == T_FVAR )
         return 1;
      if ( is_prec_gr(GETSYMBOL(*t1),GETSYMBOL(*t2)) )
         return 1;
      if ( GETSYMBOL(*t1) == GETSYMBOL(*t2) )
        { arity_head = GETARITY(*t1);
          if ( arity_head != 0 )
            { args_t1 = t1+1;
              args_t2 = t2+1;
              for ( i=0; i < arity_head; i++ )
		 { if ( kbo(args_t1+i,args_t2+i) )
                      return 1;
                   else
                     { if ( !( chk_arg(args_t1+i,bp,args_t2+i,bp) ) )
                          return 0; } } } } }
  return 0;
}
       
instr_result i_kbo()
{
    WORD  *arg1,*arg2,*arg3;       /* arguments of built-in */
    WORD   ret_val;                /* return value */
    
    arg1 = deref(ARGV(0),bp);
    arg2 = deref(ARGV(1),bp);
    arg3 = deref(ARGV(2),bp);
    
    pc += INSTR_LENGTH;
    
    if ( kbo(arg2,arg1) ) { 
      GENNUMBER(ret_val,1); 
    }
    else
      { if ( chk_arg(arg1,bp,arg2,bp) )
         { GENNUMBER(ret_val,1);}
      else { GENNUMBER(ret_val,0); } }

    /* unify the 3rd argument with ret_val */
    trail_var(arg3);
    UN_CONST((&ret_val), bp, arg3, bp);
    return success;
  }
