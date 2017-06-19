/*#########################################################################
# File:			symbtab.c
# SCCS-Info:		@(#)symbtab.c	7.2 01/16/97
# Author:		J. Schumann
# Date:			23.12.87
# Context:		wasm (SETHEO)
#
# Contents:		functions for symbol table
#
# Modifications:
#	(when)		(who)		(what)
#	23.10.91	Max Moser	`restyled'
#	26.8.92		jsc		comments + error handling
#	26.2.93		jsc 		change in retrive_undef
#					(output of label name)
#
#	12.1.95		jsc		check for errors
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
#########################################################################*/



#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include "wasm.h"

/**************************************************************************
* external function
**************************************************************************/
extern ENTRY *hsearch();

/**************************************************************************
* debugging macro
**************************************************************************/
#define DEBUG(X)

/* #define DEBUG(X)	X; */

/**************************************************************************
* static declarations and definitions
**************************************************************************/
	/* index of current free place for equates			*/
static int      eqptr = 0;
	/* index of current free place for label definition 		*/
	/* NOTE: the first LABSTART-1 elements in the symbol table      */
	/* 	 are used for ``equates''				*/
static int      labptr = LABSTART;

	/* linked list of undefined labels				*/
static uelt    *unlist = NULL;	



/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ init_symbtab
+ - initialise the symbol table and allocate space
+
+ special remarks:
+ -
+ -
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void  init_symbtab ()
{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

    symbtab = (symbentry *) malloc (MAXID * sizeof (symbentry));
	
	if (!symbtab){
		exit(fprintf(stderr,"wasm: not enough memory\n"));
		}

	/* create the hash-table 	*/
    hcreate (MAXID);

} /*=====================================================================*/






/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ putdef
+ - put definition of label into symbol table
+	given: * name, *value, mode (IEQU or NULL)
+   NOTE: the value must be given already
+
+ special remarks:
+	returns 0 if not already defined, 1 + warning otherwise
+ -
+ -
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int             putdef (s, mode, value)
    char           *s;		/* name of the label			*/
    int            mode;	/* label (NULL) or an ISEQU ?		*/
    int             value;	/* value 				*/

{ /*=====================================================================*/

    int             defval;	/* dummy				*/

/*-----------------------------------------------------------------------*/

    DEBUG(printf("Enter %s with value %d into symbol table (tag=%d)\n", s, value,proc));

	/* is that label already defined?    */
    if (getdef (s, mode, &defval) == VALUE) {
		/* yes */
	warning ("Label defined twice");
	return 1;
    }
    /* enter value of label */
    symbtab[defval].idval = value;
    symbtab[defval].idtag = mode;
    return 0;	/* was not there */

} /*=====================================================================*/






/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ getdef
+ - get definition of label and return value and type (if already in the
+	symbol table
+	otherwise enter the label
+ - given its name
+   name: input name
+   mode: ISEQU or label (NULL)
+   defval: output of defined value
+	
+	returns:
+		UNDEF + index: if label is undefined or new undefined
+		VALUE + value (idval):  if found and defined
+
+	
+ special remarks:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int             getdef (s, mode, defval)
    char           *s;		/* name					*/
    int            mode;	/* mode					*/
    int            *defval;	/* value (if defined)			*/

{ /*=====================================================================*/

    int             i;	
    ENTRY           e;	
    ENTRY          *ep;	

/*-----------------------------------------------------------------------*/

    DEBUG(printf("Get %s from symbol table\n", s));

	/* serach for the element in the hash-table			*/
    e.key = s;
    e.data = NULL;
    ep = hsearch (e, FIND);

    if (ep) {
	/* found !! 							*/
	i = (int) ep->data;
	if (symbtab[i].idtag == ISUNDEF) {
		/* is still undefined					*/
		/* return its index					*/
	    *defval = i;
	    return UNDEF;
	}
	*defval = symbtab[i].idval;
	return VALUE;
    }
    else {	/* enter new label */

	DEBUG(printf ("have to insert ne label %s\n", s));

		/* enter equ label */
	if (mode == ISEQU) {
	    strcpy (symbtab[eqptr].idname, s);
	    symbtab[eqptr].idtag = ISUNDEF;
	    *defval = eqptr++;
	   if(eqptr >= LABSTART){
		fatal("too many '.equ' labels",1);
		return UNDEF;
		}
	}
		/* enter label */
	else {
	    if(labptr == MAXID){
		fatal("symbol table overflow",1);
		return UNDEF;
		}
	    strcpy (symbtab[labptr].idname, s);
	    symbtab[labptr].idtag = ISUNDEF;
	    *defval = labptr++;
	}

		/* enter the stuff into the hash-table 		*/
	e.key = symbtab[*defval].idname;
	e.data = (char *) *defval;
	hsearch (e, ENTER);
	return UNDEF;
    }

} /*=====================================================================*/





/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ start_proc
+ - init. symbol table and remove old labels
+
+ special remarks:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void  start_proc ()

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

    start_code ();
    /* initialise the  symbol table */
    labptr = LABSTART;
    unlist = NULL;	/* remove all undefined values; */

} /*=====================================================================*/





/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ enter_nodedef
+ - enter an occurrence of a still undefined label
+   to the list of undefined occurrences (unlist)
+   given:
+	* val:     index of name in symbol table
+	* pc:      currwent program counter
+	* hosttag: tag, if the occurrence is tagged
+	* lpos:    position (offset in output file)
+
+ special remarks:
+ -
+ -
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void  enter_nodef (val, pc, lpos, hosttag)
    int             val,	
                    pc,		
                    hosttag;	
    long            lpos;	

{ /*=====================================================================*/

    uelt           *ptr;

/*-----------------------------------------------------------------------*/

	/* generate a new list-element 					*/
    ptr = (uelt *) (malloc (sizeof (uelt)));
	/* fill it up with info						*/
    ptr->lpos = lpos;
    ptr->val = val;
    ptr->pc = pc;
    ptr->hosttag = hosttag;
	/* and append it to the list					*/
    ptr->next = unlist;
    unlist = ptr;

} /*=====================================================================*/





/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ retrieve_undef
+ - get and remove the first element of undefined label occurrences
+	returns: NULL if list is empty
+		 UNDEF: if label for this occurrence is still undefined
+		 VALUE: otherwise + set info
+
+
+ special remarks:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int  retrieve_undef (valp, pcp, lposp, hosttagp)
    int            *valp,	
                   *pcp,	
                   *hosttagp;	
    long           *lposp;	

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

	/* list is empty: we are done					*/
    if (unlist == NULL)
	return 0;

    if (symbtab[unlist->val].idtag == ISUNDEF) {
	/* label is still undefined					*/
	/* produce an error message					*/
	pass2error("Reference undefined for label", 
		symbtab[unlist->val].idname);

	unlist = unlist->next;
	return UNDEF;
    }
	/* extract info and return					*/
    *valp = symbtab[unlist->val].idval;
    *pcp = unlist->pc;
    *lposp = unlist->lpos;
    *hosttagp = unlist->hosttag;
	/* remove that element						*/
    unlist = unlist->next;
    return VALUE;

} /*=====================================================================*/





#ifdef SYPRINT
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ syprint
+ - ???
+
+ special remarks:
+ - <e.g. called from>
+ - <side effects, return values>
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void  syprint (val)
    int             val;	/* ???					 */

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

    fprintf (stderr, "Label: %s\n", symbtab[val].idname);

} /*=====================================================================*/
#endif





#ifdef PRT_REF
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ prt_ref
+ - ???
+
+ special remarks:
+ - <e.g. called from>
+ - <side effects, return values>
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void  prt_ref (fout, type)
    FILE           *fout;	/* ???					 */
    int             type;	/* -1 if hex 1 otherwise 		 */

{ /*=====================================================================*/

    int             i;		/* ???					 */

/*-----------------------------------------------------------------------*/

    fprintf (fout, "**** EQUATES ****\n");
    for (i = 0; i < eqptr; i++) {
	if (type > 0)
	    fprintf (fout, "%s : %d\n", symbtab[i].idname, symbtab[i].idval);
	else
	    fprintf (fout, "%s : %xh\n", symbtab[i].idname, symbtab[i].idval);
    }
    fprintf (fout, "**** LABELS ****\n");
    for (i = LABSTART; i < labptr; i++) {
	if (type > 0)
	    fprintf (fout, "%s : %d\n", symbtab[i].idname, symbtab[i].idval);
	else
	    fprintf (fout, "%s : %xh\n", symbtab[i].idname, symbtab[i].idval);
    }

} /*=====================================================================*/
#endif





/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ pdef
+ - put definition (of a label) into table
+   given the index and the value
+
+ special remarks:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
int  pdef (index, value)
    int             index,
                    value;

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

    symbtab[index].idval = value;
    symbtab[index].idtag = 0;
    return 0;

} /*=====================================================================*/




/*#######################################################################*/
