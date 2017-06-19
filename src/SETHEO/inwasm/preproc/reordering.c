/****************************************************
;;; MODULE NAME : reordering.c
;;;
;;; PARENT      : main.c
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)reordering.c	18.1 10/22/97
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 26.6.91
;;;
;;; DESCRIPTION : reorder (fanned) clauses
;;;
;;; REMARKS     :
;;;
****************************************************/

#include "w_types.h"

#define DYNREORDER 
#define NRTRAFOS 	4
#define NRDYNTRAFOS 	1

extern weighttype w_e_1(), w_e_2(), w_e_3(), w_e_4();	/* stat weight func */
extern weighttype w_d_1();				/*  dyn weight func */

/* table of static transformations */
w_elt wstat_func[NRTRAFOS] = {
	{ w_e_1 , 1 },
	{ w_e_2 , 1 },
	{ w_e_3 , 1 },
	{ w_e_4 , 1 }
	};

/* table of dynamic transformations */
w_elt wdyn_func[NRDYNTRAFOS] = {
	{ w_d_1 , 1 }
	};

extern int maxlit;

static int nrtrafos	= NRTRAFOS;
static int nrdyntrafos	= NRDYNTRAFOS;
static weighttype *wstat = NULL;
static weighttype *wdyn, *wtmp;
static predtype **wind;			/* index array */


/****************************************************************************/
/* genreorder
/****************************************************************************/
void genreorder(module)
modtype *module;
{
claustype *cp;
claustype *fcp;
predtype  *pp;
int headnu;

while (module) {
	if (module->m_list) genreorder(module->m_list);

	if (do_it(module,"noreorder",COMPILE)) {
	   module = module->next;
	   continue;
	   }

	for (cp=module->c_list; cp; cp=cp->next) {

	    if (cp->nr_lits < 3) {
	       /* chain rules, facts and 1-,2-literal queries */
	       /*  dont be changed			      */
	       continue;
	       }
	    
	    /* print clause number on stdout */
	    printf("%d ",cp->clnr); 

	    /* compute static weights */
	    genweight(cp);

	    reorder(cp);


	    for (fcp=cp->fann,headnu=2; headnu<=cp->nr_lits; headnu++) {
		if (fcp && headnu==fcp->p_list->litnr) {
	           if (fcp->nr_lits < 3) {
		      /* there is fanned contrapositv without defaultquery */
	    	      continue;
		      }

	           /* reorder fanned clause */
	           reorder(fcp);

		   fcp = fcp->next;
	           }
	        else {
		   }
	        } /* next fanned clause */
	    } /* next original clause */
	module = module->next;
	}
}


/****************************************************************************/
/* reorder()
/****************************************************************************/
reorder(cp)
claustype *cp;
{
predtype *head = cp->p_list;
predtype *pp;
int i;

#ifdef DEBUG
fprintf(stderr,"reordering clause %d,%d\n",cp->clnr,cp->p_list->litnr);
#endif

#ifdef DYNREORDER
	gendynweight(cp,head); /**/
	sortweight(cp->nr_lits,wdyn);
#else
	sortweight(cp->nr_lits,wstat);
#endif

head->next = pp = (wind[0]==head)? wind[1] : wind[0];
for (i=0; i<cp->nr_lits; i++) {
	if (wind[i] != head) {
	   pp->next = wind[i];
	   pp = wind[i];
	   }
	}
pp->next = NULL;
}


/****************************************************************************
/* genweight
/****************************************************************************
/* generate static weights
/* of one clause currcl
/* sets wstat[],wind[]
/**/
genweight(currcl)
claustype *currcl;
{
predtype *pp;
weighttype (*func)();
weighttype max, maxold, wei,trwei;
int trafo, i;

if (!wstat) {
	wstat = (weighttype *)malloc((unsigned)maxlit*sizeof(weighttype));
	wdyn  = (weighttype *)malloc((unsigned)maxlit*sizeof(weighttype));
	wtmp = (weighttype *)malloc((unsigned)maxlit*sizeof(weighttype));
	wind  = (predtype **)malloc((unsigned)maxlit*sizeof(predtype *));
	}

/* set weights to 0 */
for (i=0; i< maxlit; i++){ 
	*(wstat + i) = 0;
	*(wtmp + i)  = 0;
	*(wind + i)  = NULL;
	}

max = maxold = 1;
/* move thru transformation list */
for (trafo = 0; trafo < nrtrafos; trafo++){
	/* apply function to all predicates */
	func = wstat_func[trafo].we;
	trwei = wstat_func[trafo].weight;
	for (i=0,pp=currcl->p_list; pp; i++,pp=pp->next) {
		/* apply it to pp */
		wei = (*func)(pp) ;
		if (wei > max) max = wei;
		wstat[i] += wtmp[i] / maxold ;
		wtmp[i] = wei * trwei;
		}
	maxold = max;
	/* to next trafo */
	}
/* norm last transformation */
/* and set wind[]           */
for (i=0,pp=currcl->p_list;pp;i++,pp=pp->next) {
	wstat[i] += wtmp[i] / maxold;
	wind[i] = pp;
	}

#ifdef DEBUG
	fprintf(stderr,"Clnr: %d Headnr: %d \n",
		currcl->clnr,currcl->p_list->litnr);
	for (i=0,pp=currcl->p_list; pp; i++,pp=pp->next) {
	   fprintf(stderr,"i: %d wtmp: %8.6f wstat: %8.6f wdyn: %8.6f ",i,wtmp[i],wstat[i],wdyn[i]);
	   fprintf(stderr,"\twind: %d/%d\n",wind[i]->claus->clnr, wind[i]->litnr);
	   }
#endif
}
 
/****************************************************************************
/* gendynweight
/****************************************************************************
/* generate dynamic weights
/* of one clause currcl where the current head has # i
/* leave the weight in wdyn
/* set wind[]
/**/
gendynweight(currcl,head)
claustype *currcl;
predtype *head;
{
weighttype max, trwei;
predtype *pp;
int trafo, i;


/* set weights to 0 */
/* set index for sorting */
for (i=0,pp = currcl->p_list; pp; pp=pp->next,i++){ 
	*(wdyn + i)  = wstat[pp->litnr-1];
	*(wtmp + i) = 0;
	*(wind + i)  = pp;
	}

max = 1;
pp = currcl->p_list;
/* move thru transformation list */
for (trafo = 0; trafo < nrdyntrafos;trafo++){
	/* apply function to all predicates */
	trwei = wdyn_func[trafo].weight;
	max = (*wdyn_func[trafo].we)(pp,head,wtmp);

	/* multiply it with trwei & add it to wdyn */
	for (i=0; i<currcl->nr_lits; i++)
		wdyn[i] += wtmp[i]*trwei/max;
	}

#ifdef DEBUG
fprintf(stderr,"\tClnr: %d Headnr: %d \n",
	currcl->clnr,currcl->p_list->litnr);
for (i=0,pp=currcl->p_list; pp; i++,pp=pp->next) {
   fprintf(stderr,"\ti: %d wtmp: %8.6f wstat: %8.6f wdyn: %8.6f ",i,wtmp[i],wstat[i],wdyn[i]);
   fprintf(stderr,"\twind: %d/%d\n",wind[i]->claus->clnr, wind[i]->litnr);
   }
#endif
}
 

/****************************************************************************
/* static sortweight
/****************************************************************************
/* sort weights in decreasing order */
/* bubble sort
/**/
static sortweight(litnu,wv)
int litnu;
weighttype *wv;
{
int end,newend,j;
predtype *twind;
weighttype tmp;

/* simplest bubble sort */
end = litnu;
while (end != 0) {
	newend=0;
	for (j=0; j<end-1; j++){
		if (wv[j] < wv[j+1]) {
			tmp = wv[j];
			wv[j] = wv[j+1];
			wv[j+1] = tmp;
			twind = wind[j];
			wind[j] = wind[j+1];
			wind[j+1] = twind;
			newend = j+1;
			}
		}
	end = newend;
	}
#ifdef DEBUG
	printf("/* Order: ");
	for (j=0; wind[j]; j++)
	    printf("%d  ",wind[j]->litnr);
	printf("*/\n");
printf("wdyn[i]\t wvect[i]\t wind[i]->litnr\n");

for (j=0; wind[j]; j++)
    printf("%f  %f  \t%d\n",wdyn[j],wstat[j], wind[j]->litnr);
#endif
}
