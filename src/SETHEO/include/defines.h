/****************************************************
;;; MODULE NAME : defines.h
;;;
;;; PARENT      :
;;;
;;; PROJECT     : MPLOP
;;;
;;; INWASMSCCS  : @(#)defines.h	14.1  12 Mar 1996
;;; SCCS        : @(#)defines.h	10.1 02 Apr 1996
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 15.5.91
;;;
;;; DESCRIPTION : header file with all defines
;;;
;;; REMARKS     :
;;;		26.3.93	jsc	increased number of NRLAB
;;;		4.6.93	jsc 	new structure for built-ins
;;;		2.7.93	jsc	includes,extern.h
;;;		3/12/96		jsc	moved to ~/include
;;;		3/12/96		jsc		wasm-less inwasm
;;;
****************************************************/

#ifndef DEFINES_H
#define DEFINES_H

#include <stdio.h>

#ifdef OLD
char *malloc();
char *strcpy();
char *strncpy();
#endif


/*****************************************************/
/* elementary definitions*/
/*****************************************************/

#define TRUE 1
#define FALSE 0

/*****************************************************/
/* definitions*/
/*****************************************************/

#define NAMELENGTH 40           /* max. length of a symbol */
#define MAXSYMB    (unsigned )50000  /* length of symbol table */
#define MAXPREDSYMB	10000	/* max. number of predicate symbols */

#define MAXTRAIL   100		/* length of trail for weak unif */
#define NRLAB	   10000	/* max. nr of labels in a term   */

#define MODUSYM 'm'		/* type of symbol is module */
#define PREDSYM 'p'		/* " is a predicate */
#define CONSTANT 'c'		/* " is a constant */
#define NUMCONST 'n'		/* " is a number */
#define STRINGCONST 'z'		/* " is a string */
#define VARIABLE 'v'		/* " is a variable */
#define GVARIABLE 'g'		/* " is a global variable */
#define SVARIABLE 's'		/* " is a structural variable */
#define BUILTIN 'b'		/* " is a built_in   */
#define CONSTSYM 'a'		/* " is a constraint symbol */

#define GROUND	'g'		/* term is ground */
#define NGROUND 'n'		/* comp. term is non-ground */

/*****************************************************/
/* definitions for output (names of predicates & matrix)*/
/*****************************************************/
#define QUERY_PRTNA 		"query__"
#define LIST_HEAD 		"["
#define ASSIGN_PRTNA		":="
#define EVASSIGN_PRTNA		":is"
#define IS_PRTNA		"is"
#define UNIF_PRTNA		"$unif"
#define EQAL_PRTNA		"$equal"
#define NQAL_PRTNA		"$nequal"
#define GREATER_PRTNA		"$greater"
#define LESS_PRTNA		"$less"
#define GREAEQU_PRTNA		"$geq"
#define LESSEQU_PRTNA		"$leq"

#define NOTEQU_CONSTR		"=/="

#define NIL 			"[]"	/*"nil_"	*/
#define PLUS			"+"
#define MINUS			"-"
#define MULT			"*"
#define DIV			"/"
#define UMINUS			"-_"

#define SEM_AND                 "sem_and"
#define SEM_OR                  "sem_or"

/*****************************************************/
/* definitions for fixed inidices in symboltable */
/*****************************************************/
#define HEAD_SYMB		1
#define NIL_SYMB		2
#define PLUS_SYMB		3
#define MINUS_SYMB		4
#define MULT_SYMB		5
#define DIV_SYMB		6
#define UMI_SYMB		7
#define COPI_SYMB		8
#define DEPT_SYMB		9
#define INF_SYMB		10
#define ASSI_SYMB		11
#define EVAS_SYMB		12
#define IS_SYMB			13
#define UNIF_SYMB		14
#define MKGLOB_SYMB		15
#define GREA_SYMB		16
#define LESS_SYMB		17
#define LEQU_SYMB		18
#define GRQU_SYMB		19
#define EQAL_SYMB		20
#define NQAL_SYMB		21
/* Begin: SEMANTIC */
#define NOTEQU_SYMB		22
#define AND_SYMB		23
#define OR_SYMB 		24
#define EQUCONSTR 		25
#define DISCONSTR 		26
#define CONJCONSTR 		27
/* End: SEMANTIC */

#define VERB(x)		((inwasm_flags.verbose^inwasm_flags.verbose>>x)&1)
#define DEB(x)		((debug^debug>>x)&1)

#define NAM(x)		(symbtab[x].name)

#define INVERSE(x)	(MAXPREDSYMB+x) /* pos symbolindex +MAXPREDSYMB */
#define INV		4	/* log10 MAXPREDSYMB */

/*****************************************************/
/* def's for unify*/
/*****************************************************/
#define FAIL (-1)
#define SUCCESS 0

/* Copy_terms */
#define ONE_TERM 0
#define ALL_TERM 1
#define ALL_ARGS 2

/*****************************************************/
/* definitions for interface*/
/*****************************************************/
#define GLOBAL 1

/*******************************************************/
/* definitions for subsumption and symmetry constraints*/
/*******************************************************/

#define NORMALC 0   	/* Subsumption zwischen normalen Klauseln */
#define CONTRAC 1   	/* Subsumption zwischen Kontrapositiven einer
                           Klausel. */
#define SUBSTEST 2      /* Subsumption zwischen normalen Klauseln aber
                           ohne Constraint-Generierung */
#define CONNOPTC 3
#define DELETED (-9999)
#define SUBS 1   	/* modes fuer den Test auf Subsumption */
#define RSUBS 2  	/* SUBS: alle Klauseln vergleichen     */
		 	/* RSUBS: nur Regeln vergleichen       */

/*******************************************************/
/* definitions for connection optimizing (connopt.c)   */
/*******************************************************/

#define NOOPT (-100)     /* zur Markierung eines Literals */ 
#define DELCONN 1   /* modes fuer Konnektionsoptimierung      */
#define RDELCONN 2  /* DELCONN: alle Klauseln vergleichen     */
		    /* RDELCONN: nur Regeln vergleichen       */

/*****************************************************/
/* definitions for fanning clausel in prologstyle*/
/*****************************************************/
#define PFACT  (-1)
#define PQUERY (-2)
#define PRULE  (-3)
#define AXIOM  (0)
#define REDUCTION_TYPE  (4)

#define COMPILE (-5)
#define CONTROL (-6)

/*****************************************************/
/* misc def's for code gen*/
/*****************************************************/
#define IDTAG   0x00100000        /* tag for non-num. constants */

#define CONST	0
#define VAR_	1
#define GTERM	2
#define NGTERM	3
#define CRTERM	4
#define EOSTR	5
#define VOID	6
#define CREF	7
#define GLOBO	8
#define NCONST	9
#define SVAR	10

/*****************************************************/
/* constraint types*/
/*****************************************************/
#define SEM_CONSTR  0
#define TAUT_CONSTR 1
#define SUB_CONSTR  2
#define SYMM_CONSTR 3
#define PUR_CONSTR  4
#define OVER_CONSTR 5

/*****************************************************/
/* unification modes                                 */
/*****************************************************/
#define UNIFY_WEAK     0   /* Weak unification       */
#define UNIFY_TCONSTR  1   /* Tautology Constraints  */
#define UNIFY_SLIST    2   /* Subsumption (S-list)   */
#define UNIFY_SCONSTR  3   /* Subsumption Constraints*/
#define UNIFY_CONSTRSU 4   /* Constraint Subsumption */
#define UNIFY_CONNOPT  5   /* Optimize Connections   */
#define UNIFY_SLIST2   6   /* Symmetry (S-List)      */

/*****************************************************/
/* makeref modes                                     */
/*****************************************************/
#define NO_REFERENCE 0
#define LOW_REFERENCE 1
#define HIGH_REFERENCE 2

/*****************************************************/
/* generator.c (set_fcb)                             */
/*****************************************************/
#define SET_MAX(W,V) if ((V)>(W)) W=(V)

/*****************************************************/
/* definitions (enum type) for built-in types*/
/*****************************************************/
typedef enum {
	NONE,
	DETERMINISTIC,		/* deterministic built-in	*/
	NONDETERMINISTIC,	/* non- -"-			*/
	INTERNAL,		/* built-in w/internal name	*/
	INFRULE_A,		/* inference rule before or-branch */
	INFRULE_Z 		/* inference rule after or-branch */
	} built_in_type;

/***************************************************************************/
/* new definitions for testing & newfunctions*/
/***************************************************************************/

#define ALL_TO_1 1              /* for new_rename - high_rename */
#define ALL_TO_0 2
#define ALL_TO_NEW 3
#define REN_CONSTR 4

#define CONNOPT 1               /* modes for ErzeugeConstraint */
#define GENCONS 2               /* gilt fuer Alle */
#define GENOVERLAPCONS 3
#define GENSUBCONS 4
#define GENTAUTOCONS 5
#define GENSYMMCONS 6

#define CL_REORDER 1;       /* modes for nr_var_in_lit - (cl_reordering.c) */
#define GEN_OCONSTR 2;

#define ALL 1         /* modes fuer my_rename */
#define ALL_TO_SV 2   /* siehe my_rename(), ~~ NEW_NAME */
#define ONLY_SV 3
#define TO_OLD 4      /*                    ~~ OLD_NAME */
#define SV_TO_V 5
#define SV_TO_XV 6

/***************************************************************************/
/* definitions for (inwasm/main.c)                                         */
/***************************************************************************/
#ifndef CLK_TCK
#define CLK_TCK               60
#endif
/* definitions for verbose mode                                            */
#define PF_NO		      0  
#define PF_PARS		      1 
#define PF_TCONSTR 	      2 
#define PF_SCONSTR            3 
#define PF_FIRSTDEL 	      4 
#define PF_PUR 		      5 
#define PF_FAN 		      6 
#define PF_SYMCONSTR 	      7 
#define PF_SECONDDEL 	      8 
#define PF_OCONSTR 	      9 
#define PF_THIRDDEL 	      10 
#define PF_SGREORD 	      11 
#define PF_CLREORD 	      12 
#define PF_ALWAYS 	      99 


/***************************************************************************/
/* definitions for (preproc/tconstr.c)                                     */
/***************************************************************************/
#define NO_CONSTR 0
#define TAUTOLOGY 1
#define TAUTOLOGY_CONSTR 2

/***************************************************************************/
/* definitions for (preproc/hlpfkt.c)                                      */
/***************************************************************************/
#define ONE_TERM 0
#define ALL_TERM 1
#define ALL_ARGS 2

#endif
/*######################################################################*/
