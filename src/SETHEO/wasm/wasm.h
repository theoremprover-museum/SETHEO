/*########################################################################
# Include File:		wasm.h
# SCCS-Info:		10/30/91, @(#)codedef.h	1.1
# Author:		J. Schumann
# Date:			23.12.87
# Context:		wasm (SETHEO)
#
# Contents:		all definitions and configuration of wasm
#
# Modifications:
#	(when)		(who)		(what)
#     	31.10.88 			common object file
#	16.11.89 			great length of symbol
#       28.09.91 	Goller		HEUMOD adapted from gh
#	23.10.91	Max Moser	`restyled'
#	30.80.93	jsc		increased number of ID's
#	21.3.94		jsc		typedefs cleaned up
#	12.1.95		jsc		length of code,...
#
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
########################################################################*/

#ifndef WASM_H
#define WASM_H

#include <stdio.h>
#include <search.h>
#include "codedef.h"

/*
 * include prototype definitions
 */
#include "wasm-prototypes.h"

/**************************************************************************
* Definitions of sizes and numbers
**************************************************************************/
#define MAXID 	 150000		/* max. nr of ID's 			 */
#define LABSTART 200		/* here start the labels 		 */
				/* 0.. LABSTART-1: equ labels 		 */

#define TOKENLENGTH	255	/* length of token == YYLMAX		*/
#define NAMELENGTH 30		/* length of a label/equ name		*/

#define PATHLENGTH	100	/* max. length of file+path name	*/
#define MAX_INCL	8	/* max. level of includes is 8 */

	/* defines for the optimiser					*/
#define MAXWORDS	2000		/* # data in one readlist-part 	*/
#define MAXNAMES	200		/* # names in one co_na_li part */
#define MAXDTREE	10000		/* # od d_tree's allocd at once */

/**************************************************************************
* Definitions (not to be changed)
**************************************************************************/
#define UNDEF           4
#define VALUE           8
#define ISEQU           (-1)
#define ISUNDEF         (-2)
#define ISLABEL		(-4)

/*************************************************************************
* Definitions of data structures
*************************************************************************/

	/****************************************************************/
	/* symbol table element						*/
	/****************************************************************/
typedef struct {
    char            idname[NAMELENGTH];		/* printname		*/
    int             idval;			/* value		*/
    short           idtag;			/* tag and mode		*/
    int		    lineno;			/* first defined in line*/
} symbentry;


	/****************************************************************/
	/* element of list of undefined labels				*/
	/****************************************************************/
typedef struct ulist {		/* list for undefined labels 		 */
    long            lpos;	/* file offset 				 */
    int             pc,		/* ???					 */
                    val,	/* ???					 */
                    hosttag;	/* ???					 */
    struct ulist   *next;	/* ???					 */
} uelt;

	/****************************************************************/
	/* data structure to read in the list (optimiser)               */
	/****************************************************************/
typedef struct li_in {
	int val[MAXWORDS];		/* store read in list 		*/
	int tag[MAXWORDS];		/* store read in list 		*/
	struct li_in *next,		/* pointer to next if not suff	*/
		* prev;			/* ptr to prev.			*/
	} list_in;


	/****************************************************************/
	/* data structure to store sets of names (optimiser)            */
	/****************************************************************/
typedef struct co_na_li {
	int valid;			/* if <>0: was already printed  */
	int namval[MAXNAMES];		/* store names 			*/
	int namtag[MAXNAMES];		/* store names 			*/
	int index;			/* index into it		*/
	struct co_na_li *next, *same;	/* ptr to next set / to next	*/
					/* part of same set		*/
	struct disc_tree *dtree;	/* ptr to beginning of data	*/
	} co_na_list;

	/****************************************************************/
	/* data structure for discrimination tree (optimiser)           */
	/****************************************************************/
typedef struct disc_tree {
	int val;			/* data				*/
	int tag;			/* data				*/
	struct disc_tree *other;	/* to other nodes on same lvl   */
	struct disc_tree *next;		/* ptr to next level		*/
	struct disc_tree *prev;		/* ptr to prev level		*/
	struct co_na_li *co_na;		/* ptr to name set 	        */
	} d_tree;


/*************************************************************************
* Definitions of exteral data structures
*************************************************************************/
extern symbentry      *symbtab;	/* symbol table				*/
extern int      curr_pc;	/* current program counter		*/
extern FILE    *fout;		/* code output file			*/
extern long     currpos;	/* current file position		*/

extern int      optflg;		/* 1 iff -O option set			*/
extern int      opt_incount;	/* # of values read			 */
extern int      opt_outcount;	/* # of values output after opt 	 */

extern char     token[TOKENLENGTH];/* token between scanner & parser	*/
extern int      numval;		/* numeric value ""  "          "    	*/
extern FILE    *yyin;
extern int	yylineno;	/* line number of input file		*/
extern FILE    *infiles[MAX_INCL];/* files for include mechanism	*/
extern  int     incl_ptr;	/* index for include mechanism	 	*/

extern int      errors;		/* number of errors			*/

extern int	maxpc;		/* max. memory address			*/
extern int	nrsymbols;	/* number of symbols            	*/

#ifdef XXXX

static int      eqptr = 0;	/* ???					 */
static int      labptr = LABSTART;	/* ???				 */
static uelt    *unlist = NULL;	/* ???					 */


/**************************************************************************
* declarations and definitions
**************************************************************************/

static int      init_req = 0;	/* initialize processors 		 */
static int      init_proc;	/* ???					 */

--------------------


#define IMMEDIATE	8
#define ABSOLUTE	4
#define REGISTER	2



/*------------------------------------------------------------------------
- definitons for debug-file
------------------------------------------------------------------------*/

#define DEBUG_FORM ":%c:%d:%d:%x:\n"

------------------------------------------------------------------------*/



/*######################################################################*/
------------------------------------------------------------------------*/

#endif


#endif
