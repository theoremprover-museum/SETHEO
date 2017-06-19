/******************************************************
 *    S E T H E O                                     *
 *                                                    *
 * FILE: store_subg.c
 * VERSION:
 * DATE:                6.5.93
 * AUTHOR: Dagmar Holzinger
 * NAME OF FILE:
 * DESCR:
 *      store the subgoals (not yet touched) of a searchfront
 * MOD:
 ******************************************************/



#ifdef STATPART


#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "defaults.h"
#include "codedef.h"
#include "symbtab.h"
#include "disp.h"
#include "sps.h"

extern int t_depth;

/* IAbegin */
print_subgoal(filep,lcb,bp,negated)
FILE                    *filep;
literal_ctrl_block      *lcb;        
environ                 *bp;
int                     negated;

{
  int sign;
  int arity;
  int i;

  /* calculate the correct sign   */
  sign = GETVAL(lcb->ps_symb);
  if (negated)
  {
    CHANGE_THE_SIGN(sign);
  }

  if (IS_NEG(sign))
  {
    /* print a ~ */
    fprintf(filep,"~");
  }

  /* print the predicate symbol   */
  fprintf(filep,"%s",symbtab[GET_PRED(GETVAL(lcb->ps_symb))].name);

  /* are there terms ? */

  arity = symbtab[GET_PRED(GETVAL(lcb->ps_symb))].arity;

  if (arity > 0)
  {
    fprintf(filep,"(");

    for (i = 0; i < arity; i++)
    {
      /* display the current term */
      disp_(filep, code + lcb->argv + i, bp);
      /* separate the items with a ',' */
      if (i < arity - 1)
      {
        fprintf(filep,",");
      }
    }
      fprintf(filep,")");
  }
}


print_open_subg(fp, bp)
FILE    *fp;
environ *bp;
{
  WORD  *sg_i;
  int   nr_sgls;
  int   i;

  sg_i =  (WORD *)bp + SIZE_IN_WORDS(environ)
        + GETNUMBER(bp->lcbptr->nr_vars);

  nr_sgls = GETNUMBER(bp->lcbptr->nr_sgls);

  /* offene Subgoals des aktuellen Environments ausgeben */
  for( i = 0; i < nr_sgls; i++ ){
       if( sg_i > bp->tp ){ /* subgoals not yet been touched */
           num_of_subgs++;
	   if( i > 0 )
	       fprintf(fp,", ");
           print_subgoal(fp, (literal_ctrl_block *) GETPTR(*sg_i), bp, 0);
           }
       sg_i++;
       }
}


print_or_node(fp, bp)
FILE    *fp;
environ *bp;
{
  if( ornode_counter == 1 ){
      fprintf(fp, "\n#Search front for ");
      if( maxinf != DEF_INFCOUNT )
          fprintf(fp, "-i %d ", maxinf);
      if( depth == 0 )
          fprintf(fp, "-d %d ", t_depth);
      fprintf(fp, "\n");
      }
  while (bp >= (environ *) stack)
  {
    print_open_subg(fp, bp); /* offene Subg. eines Environments ausgeben */
    bp = bp->dyn_link;
  }
  fprintf(fp, ".\n"); /* Oder-Knoten mit CR abschliessen */
}
/* IAend */

#else
/* ANSI C doesn't like empty files ... */
int dummy_dummy_store_subg () { return 42; }
#endif /* STATPART */
