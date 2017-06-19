/*#########################################################################
# File:			optim.c
# SCCS-Info:		10/30/91, @(#)optim.c	1.2
# Author:		J. Schumann
# Date:			11.08.88
# Context:		wasm (SETHEO)
#
# Contents:		???
#
# Modifications:
#	(when)		(who)		(what)
#	23.10.91	Max Moser	`restyled'
#	12.1.95		jsc		error handling
#
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
#########################################################################*/


#include <stdio.h>
#include <stdlib.h>
#include "wasm.h"

/**************************************************************************
* declarations and definitions
**************************************************************************/

static
d_tree         *dtree,		/* ptr to discr. tree 			 */
               *dtp,		/* current ptr to freespace		 */
               *dtend,		/* ptr to end of free-space     	 */
               *currdt;		/* current node of dtree        	 */

static struct li_in *inlist;	/* list to read in data			 */
static struct li_in *currin;	/* current ptr to part			 */
static int      in_index;	/* index into inlist			 */

static int      in_name;	/* name of list to be read in   	 */
static int      in_tag;		/* name of list to be read in   	 */

static
struct co_na_li *name_list,	/* list of set of names			 */
               *curr_name_list;	/* current ptr to name list		 */

static int      atbegin;	/* =1 if at beginning of tree   	 */


static d_tree  * dtalloc ();
static void      prt_dt  _ANSI_ARGS_((d_tree *dtp,void (*pna) (),void (*pnlab) ()));
static int       e_tree  _ANSI_ARGS_((int el,int tag));
static void      prt_na  _ANSI_ARGS_((struct co_na_li *cna,void (*pna) ()));


/**************************************************************************
* code
**************************************************************************/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ init_optim
+ - ???
+
+ special remarks:
+ - <e.g. called from>
+ - <side effects, return values>
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void  init_optim ()
{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

    if (!(dtree = (d_tree *) malloc (MAXDTREE * sizeof(d_tree)))){
    	fatal("could not allocate enough memory, try '-noopt''\n",1);
	return;
	}
    		

    currdt = NULL;
    atbegin = 1;

    opt_incount = 0;
    opt_outcount = 0;

    dtp = dtree;
    dtend = dtree + MAXDTREE;

    if(!(inlist = (struct li_in *) malloc (sizeof (struct li_in)))){
    	fatal("could not allocate enough memory, try '-noopt''\n",1);
	return;
	}
    currin = inlist;
    currin->next=NULL;
    currin->prev=NULL;
    in_index = 0;

} /*=====================================================================*/





/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ dtalloc
+ - ???
+
+ special remarks:
+ - <e.g. called from>
+ - <side effects, return values>
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static d_tree  *dtalloc ()
{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

    /* alloc space for ONE dtree */
    if (dtp == dtend) {
	/* have to alloc new space */
	if(!(dtp = (d_tree *) malloc (MAXDTREE * sizeof (d_tree)))){
    	   fatal("could not allocate enough memory, try '-noopt''\n",1);
	   return NULL;
	   }

	dtend = dtp + MAXDTREE;
    }

    return dtp++;

} /*=====================================================================*/





/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ endter_o_list
+ -  enter a name (or numeric value) and its tag into the input
+	list
+	context:
+	.dw	xxx
+
+ special remarks:
+ - <e.g. called from>
+ - <side effects, return values>
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void  enter_o_list (el, tag)
    int             el;		/* ???					 */
    int             tag;	/* ???					 */

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

    if (in_index == MAXWORDS) {
	if (!currin->next) {
	    if(!(currin->next = 
		(struct li_in *) malloc (sizeof (struct li_in)))){
    		fatal("could not allocate enough memory, try '-noopt''\n",1);
		return;
		}
	    currin->next->prev = currin;
	    currin->next->next=NULL;
	}
	currin = currin->next;
	in_index = 0;
    }
    currin->val[in_index] = el;
    currin->tag[in_index++] = tag;

} /*=====================================================================*/






/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ o_name
+ - enter the current name and tag of the label
+	in context: lab:
+
+
+ special remarks:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void  o_name (name, tag)
    int             name;	/* ???					 */
    int             tag;	/* ???					 */

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

    in_name = name;
    in_tag = tag;

} /*=====================================================================*/





/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ list2tree
+ - ???
+
+ special remarks:
+ - read inlist in reverse order
+ - <side effects, return values>
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void  list2tree ()
{ /*=====================================================================*/

    struct co_na_li *cna;	/* ???					 */

/*-----------------------------------------------------------------------*/

#ifdef DEBUG
    printf ("list2tree index: %d\n", in_index);
    fflush (stdout);
#endif

    atbegin = 1;

    for (;;) {
	while (in_index--) {
	    e_tree (currin->val[in_index], currin->tag[in_index]);
	}
	if (currin == inlist)
	    break;
	currin = currin->prev;
	in_index = MAXWORDS;
    }
    in_index = 0;	/* set inlist new */

#ifdef DEBUG
    printf ("list2tree entering name1\n");
    fflush (stdout);
#endif

    if (!currdt->co_na) {
	/* used for the first time */
	/* is up to now the longest one */

#ifdef DEBUG
	printf ("list2tree entering name1-alloc\n");
	fflush (stdout);
#endif

	if(!(cna = (struct co_na_li *) malloc (sizeof (struct co_na_li)))){
    		fatal("could not allocate enough memory, try '-noopt''\n",1);
		return;
		}
	if (!name_list) {
	    /* is the very first time */
	    curr_name_list = name_list = cna;
	}
	else {
	    curr_name_list->next = cna;
	    curr_name_list = cna;
	}
	currdt->co_na = cna;
	cna->valid = 0;
	cna->next = NULL;
	cna->same = NULL;
	cna->index = 0;
	cna->dtree = currdt;
    }
    else {

#ifdef DEBUG
	printf ("list2tree entering name already there\n");
	fflush (stdout);
#endif

	cna = currdt->co_na;
    }


    if (!currdt->next)
	cna->valid = 0;	/* is longest now */

    while (cna->same)
	cna = cna->same;

#ifdef DEBUG
    printf ("list2tree entering name2\n");
    fflush (stdout);
#endif

    /* enter in_name into cna  */
    if (cna->index == MAXNAMES) {
	/* no space, have to allocate next */
	if(!(cna->same = 
	    (struct co_na_li *) malloc (sizeof (struct co_na_li)))){
    	fatal("could not allocate enough memory, try '-noopt''\n",1);
	return;
	}
	cna = cna->same;
	cna->next = NULL;
	cna->same = NULL;
	cna->index = 0;
    }
    cna->namval[cna->index] = in_name;
    cna->namtag[cna->index++] = in_tag;


    currdt = dtree;

#ifdef DEBUG
    printf ("list2tree entering name3\n");
    fflush (stdout);
#endif

} /*=====================================================================*/







/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ print_dtree
+ - ???
+
+ special remarks:
+ - <e.g. called from>
+ - <side effects, return values>
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void  print_dtree (pna, plab)
    void            (*pna) (),	/* ???					 */
                    (*plab) ();	/* ???					 */

{ /*=====================================================================*/

    struct co_na_li *cna;	/* ???					 */

/*-----------------------------------------------------------------------*/

    for (cna = name_list; cna; cna = cna->next) {
	if (cna->valid)
	    continue;
	/* do print names of longest branches only */
	/* print data */

#ifdef DEBUG
	printf ("print_dtree dtree = %xhex\n", cna->dtree);
	fflush (stdout);
#endif

	prt_dt (cna->dtree, pna, plab);
    }

} /*=====================================================================*/






/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ prt_na
+ - ???
+
+ special remarks:
+ - <e.g. called from>
+ - <side effects, return values>
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static void  prt_na (cna, pna)
    struct co_na_li *cna;	/* ???					 */
    void            (*pna) ();	/* ???					 */

{ /*=====================================================================*/

    struct co_na_li *cna2;	/* ???					 */
    int             i;		/* ???					 */

/*-----------------------------------------------------------------------*/

#ifdef DEBUG
    printf ("prt_na: start \n");
    fflush (stdout);
#endif

    /* print names */
    for (cna2 = cna; cna2; cna2 = cna2->same) {
	for (i = 0; i < cna2->index; i++) {
	    (*pna) (cna2->namval[i], cna2->namtag[i]);
	}
    }

#ifdef DEBUG
    printf ("prt_na: end \n");
    fflush (stdout);
#endif

} /*=====================================================================*/






/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ prt_dt
+ - ???
+
+ special remarks:
+ - <e.g. called from>
+ - <side effects, return values>
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static void  prt_dt (dtp, pna, pnlab)
    d_tree         *dtp;	/* ???					 */
    void            (*pna) (),	/* ??? 					 */
                (*pnlab) ();	/* ???					 */

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

    while (dtp) {
	if (dtp->co_na) {	/* print those name lists */

#ifdef DEBUG
	    printf ("prt_dt: print namelist \n");
	    fflush (stdout);
#endif

	    prt_na (dtp->co_na, pna);
	}

#ifdef DEBUG
	printf ("prt_dt: print dates %d  \n", dtp->val);
	fflush (stdout);
#endif

	(*pnlab) (dtp->val, dtp->tag);
	opt_outcount++;
	dtp = dtp->prev;
    }

} /*=====================================================================*/






/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ e_tree
+ - ???
+
+ special remarks:
+ - <e.g. called from>
+ - <side effects, return values>
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

static int  e_tree (el, tag)
    int             el;		/* ???					 */
    int             tag;	/* ???					 */

{ /*=====================================================================*/

    d_tree         *cdt;	/* ???					 */

/*-----------------------------------------------------------------------*/

#ifdef DEBUG
    printf ("e_tree (val,tag) = %d %d\n", el, tag);
    fflush (stdout);
#endif

    opt_incount++;

    /* check if we are at the beginning */
    if (currdt == NULL) {	/* just enter it */
	currdt = dtalloc ();
	currdt->val = el;
	currdt->tag = tag;
	currdt->next = currdt->prev = currdt->other = NULL;
	atbegin = 0;

#ifdef DEBUG
	printf ("e_tree at the beginning(val,tag) = %d %d\n", el, tag);
	fflush (stdout);
#endif

	return 1;
    }

    /* go down one level */
    cdt = (atbegin) ? currdt : currdt->next;
    atbegin = 0;

    while (cdt) {

#ifdef DEBUG
	printf ("e_tree: cdt-llop (val,tag) %d %d\n",
		cdt->val, cdt->tag);
	fflush (stdout);
#endif

	if ((cdt->val == el) && (cdt->tag == tag)) {
	    /* if it was an old leaf, set namlist to valid */
	    if (cdt->co_na)
		cdt->co_na->valid = 1;
	    currdt = cdt;

#ifdef DEBUG
	    printf ("e_tree: found the same\n");
	    fflush (stdout);
#endif

	    return 0;
	}
	cdt = cdt->other;
    }

#ifdef DEBUG
    printf ("e_tree: have to enter new one\n");
    fflush (stdout);
#endif

    /* have to enter a new one */
    cdt = dtalloc ();
    cdt->other = currdt->next;
    currdt->next = cdt;
    cdt->next = NULL;
    cdt->prev = currdt;
    cdt->tag = tag;
    cdt->val = el;
    cdt->co_na = NULL;
    currdt = cdt;

#ifdef DEBUG
    printf ("e_tree end\n");
    fflush (stdout);
#endif

    return 1;

} /*=====================================================================*/





/*#######################################################################*/
