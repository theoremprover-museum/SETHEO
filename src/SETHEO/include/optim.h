/*########################################################################
# Include File:		optim.h
# SCCS-Info:		10/30/91, @(#)optim.h	1.1
# Author:		J. Schumann
# Date:			11.08.88
# Context:		wasm (SETHEO)
# 
# Contents:		???
#
# Modifications:	
#	(when)		(who)		(what)
#	23.10.91	Max Moser	`restyled'
#	29.6.93		jsc		include-defines
#
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
########################################################################*/
#ifndef OPTIM_H
#define OPTIM_H

/*------------------------------------------------------------------------
- Definitions for sizes in the optimizer
------------------------------------------------------------------------*/

#define MAXWORDS	2000		/* # data in one readlist-part 	*/
#define MAXNAMES	200		/* # names in one co_na_li part */
#define MAXDTREE	10000		/* # od d_tree's allocd at once */


/*------------------------------------------------------------------------
- data structure to read in the list
------------------------------------------------------------------------*/

typedef struct li_in {
	int val[MAXWORDS];		/* store read in list 		*/
	int tag[MAXWORDS];		/* store read in list 		*/
	struct li_in *next,		/* pointer to next if not suff	*/
		* prev;			/* ptr to prev.			*/
	} ;


/*------------------------------------------------------------------------
- data structure to store sets of names
------------------------------------------------------------------------*/

typedef struct co_na_li {
	int valid;			/* if <>0: was already printed  */
	int namval[MAXNAMES];		/* store names 			*/
	int namtag[MAXNAMES];		/* store names 			*/
	int index;			/* index into it		*/
	struct co_na_li *next, *same;	/* ptr to next set / to next	*/
					/* part of same set		*/
	struct disc_tree *dtree;	/* ptr to beginning of data	*/
	};


/*------------------------------------------------------------------------
- data structure for discrimination tree
------------------------------------------------------------------------*/

typedef struct disc_tree {
	int val;			/* data				*/
	int tag;			/* data				*/
	struct disc_tree *other;	/* to other nodes on same lvl   */
	struct disc_tree *next;		/* ptr to next level		*/
	struct disc_tree *prev;		/* ptr to prev level		*/
	struct co_na_li *co_na;		/* ptr to name set 	        */
	} d_tree;


#endif
/*######################################################################*/
