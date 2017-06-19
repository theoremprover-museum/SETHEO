/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_rand_reorder.c
 * VERSION:
 * DATE: 25.5.93
 * AUTHOR: C. Goller
 * NAME OF FILE:
 * DESCR: rand_reorder is called as one alternative of a choice-point,
 *		  dereferenced by an artificial literal_ctrl_block; it copies a 
 *        randomly selected permutation of all subsequent alternatives 
 *        (lcbpointers) onto the stack and redirects chp->next_clause;
 *		  to prevent damage (reducts placed at the end of a chp !!!)
 *		  the last alternative has to be lcbfailure, so that the chp is
 *		  not discarded too early.
 * MOD:
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
/**/
#include "errors.h"

#define DEBUG(X)

#define INSTR_LENGTH		1

#define MAXORBRANCHFACT 200     /* should be set globally ??? (W.E.) */

void            quicksort ();
static int      rlist[MAXORBRANCHFACT];     /* Vector of random numbers */
static int      nlist[MAXORBRANCHFACT];     /* Vector with ordering */

instr_result i_rand_reorder()
{
    WORD           *p1,
                   *p2;
    int             i,                                      
                    length;


    p1 = chp->next_clause;
    p2 = sp;
    length = 0;

    DEBUG(printf("reorder:\n");)
    DEBUG(printf("*(p1-1)%x\n", *(p1-1));)
    DEBUG(while (*p1) )
   		DEBUG(printf("   %x\n", *p1++);)
    DEBUG(printf("\n");)
    DEBUG(p1 = chp->next_clause;)

    while (GETPTR(*p1)) {
        p1++;
        length++;
        GENPTR(*sp,(void*)(-1)); sp++;   /* not yet occupied */
    }

    /* last element has to be lcbfailure (for chp-optimization) */
    length--;
    *(sp - 1) = *(p1 - 2);
    GENPTR(*sp,NULL);                    /* end of list */
    sp++;

    if (length > MAXORBRANCHFACT)
	sam_error("ERROR: randreord: OR-branching factor too big", NULL, 4);

    p1 = chp->next_clause;

    for (i = 0; i < length; i++) {
        rlist[i] = lrand48 ();  /* Permutation index of choice i */
        nlist[i] = i;   /* Original index of choice i */
    }
    /******* sorting of permutation indices, i.e. random permutation of ******/
    /******* original indices                                              ******/
    quicksort (0, length - 1, rlist, nlist);

    /******* reordering of SETHEO choice point **********/
    /******* addresses of alternatives (OR-branches) are copied from code area  */
    /******* to stack. */
    for (i = 0; i < length; i++) {
        *(p2 + i) = *(p1 + nlist[i]);
    }

    /****** actual pointer to next alternative (OR-branch) is set to p2 */
    chp->next_clause = p2;
    chp->sp = sp;

    DEBUG(p1 = chp->next_clause;)
    DEBUG(while (*p1) )
    	DEBUG(printf("   %x\n", *p1++);)
    DEBUG(printf("*(sp-1)%x\n", *(sp-1));)
    DEBUG(printf("*(sp-2)%x\n", *(sp-2));)
    DEBUG(printf("\n");)

    pc +=INSTR_LENGTH;
    return failure;
}


/*******************************************************
 * quicksort
 * by W. Ertel, 22.11.91
 *******************************************************/
void            quicksort (left, right, list0, list1)
    int             left;
    int             right;
    int            *list0;
    int            *list1;
{
    int             l,
                    r,
                    save0,
                    save1;

    l = left;
    r = right;

    save0 = list0[l];
    save1 = list1[l];

    while (l != r) {
        while ((list0[r] >= save0) && (l < r)) {
            --r;
        }
        list0[l] = list0[r];
        list1[l] = list1[r];

        while ((list0[l] <= save0) && (l < r)) {
            ++l;
        }
        list0[r] = list0[l];
        list1[r] = list1[l];

        list0[l] = save0;
        list1[l] = save1;
    }

    if (left < (l - 1))
        quicksort (left, l - 1, list0, list1);
    if ((l + 1) < right)
        quicksort (l + 1, right, list0, list1);
}
