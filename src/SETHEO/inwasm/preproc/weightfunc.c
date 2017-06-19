/*******************************************
;;;*****************************************
;;;MODULE NAME:   w_func1.c
;;;
;;;PARENT     :   generator.c
;;;
;;;PROJECT    :   setheo
;;;
;;;AUTHOR     :   J. Schumann
;;;
;;;DATE       :   23.9.87
;;;
;;;DESCRIPTION:   xxxxxxxxxxxx
;;;               xxxxxxxxxxxxxx
;;;MODIF      :   xxxxx (when, what, who)
;;;
;;;REMARKS    :   xxxxxxx
;;;#****************************************
*******************************************/
#include "w_types.h"

#define WCONST 1
#define WVAR   1

#ifdef DEBUG
extern FILE *fout;
#endif

extern int maxlit;
extern int maxunif;		/* max length of w_unifications */

static int max;

/****************************************************************************/
/* functions (preproc/weigthfunc.c)                                         */
/****************************************************************************/
weighttype w_e_1();
static void sum_up();
weighttype w_e_2();
weighttype w_e_3();
weighttype w_e_4();
weighttype w_d_1();
static void move_thru();
static int var_isin();

/****************************************************************************/
/* w_e_1                                                                    */
/****************************************************************************/
/* w_e_1 counts the number of constants                                     */
/* in one predicate                                                         */
/**/
weighttype w_e_1(pp)
predtype *pp;
{
int sumc,sumv;

sumc = sumv = 0;

#ifdef DEBUG
	printf("w_e_1 of ");
	print_clause(pp,fout);
	fflush(stdout);
#endif
sum_up(pp->t_list,&sumc,&sumv);
#ifdef DEBUG
	printf("sumc %d sumvars %d\n",sumc,sumv);
	fflush(stdout);
#endif
return (weighttype)(WCONST*sumc);
}

/*********************************************************/
/* sum_up()                                              */
/*********************************************************/
static void sum_up(tp,sc,sv)
termtype *tp;
int *sc,*sv;
{
int iv, ic;
iv = ic =0;
while (tp) {
	if (tp->type == VARIABLE )
		iv++;
	else {
		ic++;
		if (tp->t_list) {
			sum_up(tp->t_list,&ic,&iv);
			}
		}
	tp=tp->next;
	}
*sv = *sv + iv;
*sc = *sc + ic;
}


/****************************************************************************/
/* w_e_2                                                                    */
/****************************************************************************/
/* count number of variables                                                */
/**/
weighttype w_e_2(pp)
predtype *pp;
{
int sumc,sumv;

sumc = sumv = 0;

#ifdef DEBUG
	printf("w_e_2 of ");
	print_clause(pp,stdout);
	fflush(stdout);
#endif
sum_up(pp->t_list,&sumc,&sumv);
#ifdef DEBUG
	printf("sumc %d sumvars %d\n",sumc,sumv);
	fflush(stdout);
#endif
return (weighttype)(WVAR * sumv);
}


/****************************************************************************/
/* w_e_3                                                                    */
/****************************************************************************/
/* count # of connections                                                   */
/**/
weighttype w_e_3(pp)
predtype *pp;
{
int cnt;
predlist * w;

#ifdef DEBUG
	printf("w_e_3 of ");
	print_clause(pp,fout);
	fflush(stdout);
#endif
for( cnt=0, w=pp->wu; w; cnt++, w=w->next);

if (cnt)
	return (maxunif * maxlit/(weighttype)cnt);
else
	return 9999;
}

/****************************************************************************/
/* w_e_4                                                                    */
/****************************************************************************/
/* count # of literals in connection-clauses                                */
/**/

weighttype w_e_4(pp)
predtype *pp;
{
int sum;
predlist * w;

#ifdef DEBUG
	printf("w_e_4 of ");
	print_clause(pp,fout);
	fflush(stdout);
#endif
for( sum=0, w=pp->wu; w;  w=w->next){
	sum += w->pr->claus->nr_lits;
	}


#ifdef DEBUG
	printf("sum = %d\n",sum);
#endif


if (sum)
	return (maxlit *maxlit * maxunif/(weighttype)sum);
else
	return 9999;
}


/****************************************************************************/
/* w_d_1                                                                    */
/****************************************************************************/
/* move thru the head literal and looks for variable binding in the rest    */
/* in one predicate                                                         */
/**/
weighttype w_d_1(pp,head,wt)
predtype *pp, *head;
weighttype *wt;
{

max=0;
move_thru(head->t_list,wt,pp);

#ifdef DEBUG
int i;
	printf("moved thru \n");
	printterm(head->t_list,stdout);
	printf("\n");
	for(i=0; i<head->clnr->nr_lits;i++)
		printf(" %f",wt[i]);
	printf("\nMax: %d\n",max);
#endif
return (weighttype)(max ? max : 1);
}

/*********************************************************/
/* move_thru()                                           */
/*********************************************************/
static void move_thru(tl, wt, pp)
termtype *tl;
weighttype *wt;
predtype *pp;
{
  int var, i;
  predtype *lit;

  while (tl) {
    if (tl->type == VARIABLE) {
      var = tl->symb;
      /* debug: printf("head variable: %d\n",var); */
      /* look for that variable in the body */
      for (i=0, lit=pp; lit; i++, lit=lit->next) {
	/* debug: printf("literal # : %d\n",i); fflush(stdout); */
	if (tl == lit->t_list) continue;
	if (var_isin(var, lit->t_list)) {
	  /* debug: printf("found variable: %d\n",var); fflush(stdout); */
	  if (max < ++wt[i]) max = wt[i];
	}
      }
    }
    if (tl->t_list) move_thru(tl->t_list,wt,pp);
    tl=tl->next;
  }
}


/*********************************************************/
/* var_isin()                                            */
/*********************************************************/
static int var_isin(var,term)
termtype *term;
int var;
{
/* returns 1 if found, 0 otherwise */
while (term) {
	if (term->type == VARIABLE
	    && term->symb == var) return 1;
	if (term->t_list)
		if (var_isin(var,term->t_list))
		    return 1;
	term=term->next;
	}
#ifdef DEBUG
	printf("varisin :%d not there\n",var);
#endif
return 0;
}

