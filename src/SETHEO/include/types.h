/****************************************************
;;; MODULE NAME : types.h
;;;
;;; PARENT      :
;;;
;;; PROJECT     : MPLOP
;;;
;;; INWASMSCCS  : @(#)types.h	14.1  12 Mar 1996
;;; SCCS        : @(#)types.h	10.1 02 Apr 1996
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 15.5.91
;;;
;;; DESCRIPTION : type definitions
;;;
;;; REMARKS     :
;;;		MOD jsc	4.11.92
;;;		MOD jsc	22.2.93   number of strvars in clause type
;;;		MOD jsc 3.3.93	  add'tl item in clause: parsed_rule_type
;;;		2.7.93	jsc	includes,extern.h
;;;		12.7.93	jsc	inwasm_flags structure
;;;		14.12.93	jsc		purity, fold
;;;		28.7.94		jsc	interactive, searchtree
;;;		21.6.95		fcb
;;;		3/12/96		jsc	moved to ~/include
;;;		3/12/96		jsc		wasm-less inwasm
;;;             16.04.96        RZ      included stypes.h
;;;
****************************************************/

#ifndef TYPES_H
#define TYPES_H

#include "defines.h"

/*****************************************************/
/* definition of the interface*/
/*****************************************************/
typedef struct symbl {
	int symbol;		/* index in symboltable */
        int value;
	struct symbl * next;
	} symbollist;

typedef struct impo {
	int modusymb;
	symbollist *impos;
	struct impo *next;
	} importlist;

typedef struct inter {
	symbollist *export;
	importlist *import;
	symbollist *global;
	} intertype;

/*****************************************************/
/* type definitions*/
/*****************************************************/
/* concerning the dynamic data structures            */
/*****************************************************/

/*****************************************************/
/* definition of a term*/
/*****************************************************/
typedef struct term 
{
  int symb,type;		/*symbol and its type 		*/
  int variable_number;          /*number of variable            */
  int clause_number;          /*number of clause, when type = variable */
  int value;		        /* value if it is a number   	*/
  int grflag;		/* ground-flag for compounds 	*/
  /* or first-nth-usage of variable */
  struct pred * literal;   /* pointer to the literal
			   in which the term occurs */
  struct term * t_list;	/* pointer to subterms 		*/
  struct term * next;	/* to next term on same level 	*/
} termtype;

/*****************************************************/
/* definition of a predicate*/
/*****************************************************/
typedef struct pred {
	short symb;		/* predicate symbol 	   		    */
	short flag;             /* misc. flag                               */
	short sign,arity,litnr;	
	short nr_connections;	/* number of connections		    */
	short var_dependency;   /* number of variables PJ                   */
	short missing_link;	/* deleted connections for this pred        */ 
	float complexity;	/* termcomplexity 			    */
        short literal_flag;  /* if set, the literal is not counted */
        short subgoal_flag;  /* if set, the literal is moved to the end of
				the clause */
	struct clau *claus;	/* points to the original clause 	    */
	struct clau *kp;	/* points to the corresponding kp 	    */
	struct term *t_list;	/* points to terms of predicate	   	    */
        struct plit *s_list;    /* points to list of partner-literals       */
	struct preli *wu;	/* list of weak unifyable lits  	    */
	struct pred *next;	/* to next predicate 			    */
  struct pred *slistnext;  /* pointer to next slist element */
	} predtype;


/*****************************************************/
/* definition of a clause*/
/*****************************************************/
typedef struct clau {
	short	clnr;		/* number of clause 		*/
        short     rescl1;       /* if clnr<0 or >nr_clauses: clause1 of resolvent */
        short     rescl2;       /* if clnr<0 or >nr_clauses: clause1 of resolvent */
  short	nr_lits;	/* number of literals in clause */
  short clr_nr_lits;  /* for clause-reordering relevant literals */
	short	nr_built;	/* number of built-ins     	*/
	short	poslits;	/* number of pos. literals 	*/
	short	lineno;		/* source line number 		*/
	short	delpos;		/* pos of <- or QUERY/FACT/RULE	*/
	short	parsed_rule_type; /* PQUERY/PFACT/PRULE/AXIOM	*/
	short	nrvars;		/* number of vars+strvars       */
	short   nrstrvars;      /* number of strvar             */
	short	order;		/* sort kriterium		*/
	short   ddepth;         /* decrement of $Depth          */
        short   dinf;           /* decrement of $Inf            */
	short	del;		/* for overlap constraints	*/
	short	connections; 	/* number of literalconnections */ 
	short	max_termdepth;	/* max. term-depth of clause 	*/
	short	max_termsize;	/* max. term-size of clause 	*/
        short   flag_nalloc;    /* set if head-literal is 
				   ^<name>                      */
        short   flag_constraints; /* set if parsed clause has 
				   constraints (in lop-File)    */
	float    complexity;	/* clause complexity 		*/
        float   kriterium;
        short   nr_vars;
	predtype    * constr;	/* pt to list of constraints 	*/
	predtype    * p_list;	/* pt to list of predicates 	*/
	struct modu * modul;	/* pt to module 		*/
	struct clau * fann;	/* to fanned clauses 		*/
	struct clau * next;	/* to next clause 		*/
	} claustype;

/*****************************************************/
/* definition of a module*/
/*****************************************************/
typedef struct modu {
	int	modunr;		/* number of the module 	*/
	int	symb;		/* modul symbol			*/
	intertype   interface;	/* 3 symbollists 		*/
	claustype   * strategy;	/* list of literals=strategy	*/
	claustype   * c_list;	/* clausellist  		*/
	struct modu * prev;	/* to stat before module 	*/
	struct modu * m_list;	/* to submodules 		*/
	struct modu * next;	/* to next module 		*/
	} modtype;

/*****************************************************/
/* structure for formula control block */
/*****************************************************/
typedef struct struct_fcb {
	short	nr_clauses;	/* overall nr. of clauses	*/
	short	nr_literals;	/* overall nr. of literals	*/
	int 	nr_connections;	/* overall nr of connections	*/

	short	nr_const;	/* number of constants		*/
	short	nr_fsymb;	/* number of function symbols	*/
	short	nr_predsymb;	/* number of predicate symbols	*/
	short	max_arity_fsymb;/* max. arity of f-symbols	*/

	short	nr_queries;	/* # of <-, ?-			*/
	short	max_lit_queries;	/* max #		*/
	short	sum_lit_queries;	/*      		*/
	short	max_vars_queries;	/*      		*/
	short	max_termdepth_queries;	/*      		*/
	short	max_termsize_queries;	/*      		*/

	short	nr_punits;	/* # pos. units			*/
	short	max_vars_punits;	/*      		*/
	short	max_termdepth_punits;	/*      		*/
	short	max_termsize_punits;	/*      		*/

	short	nr_nunits;	/* # neg. units (not queries)	*/
	short	max_vars_nunits;	/*      		*/
	short	max_termdepth_nunits;	/*      		*/
	short	max_termsize_nunits;	/*      		*/

	short	nr_defclauses;	/* # def. clauses		*/
				/* L <- T1,..,Tn, N>=1 		*/
	short	max_lit_defclauses;	/* max #		*/
	short	sum_lit_defclauses;	/*      		*/
	short	max_vars_defclauses;	/*      		*/
	short	max_termdepth_defclauses;/*      		*/
	short	max_termsize_defclauses;/*      		*/

	short	nr_indefclauses;/* # indef. clauses		*/
				/* H1,...,Hm <- T1,..,Tn, M>1	*/
	short	max_lit_indefclauses;	/* max #		*/
	short	sum_lit_indefclauses;	/*      		*/
	short	max_vars_indefclauses;	/*      		*/
	short	max_termdepth_indefclauses;/*      		*/
	short	max_termsize_indefclauses;/*      		*/
	short	max_plit_indefclauses;	/* max #		*/
	short	sum_plit_indefclauses;	/*      		*/
	} t_fcb;

/*****************************************************/
/* structure predlist : wu_list loclist*/
/*****************************************************/
typedef struct preli {
	struct pred * pr;	/* occurrence of pred. pt to it */
	struct preli * next;      /* to next occurrence 	*/
	} predlist;

/*****************************************************/
/* structure for libraries */
/*****************************************************/
typedef struct {
	char libname[NAMELENGTH]; /* name of library       */
	int used;
	} library;

/*****************************************************/
/* structure trail including dynamic renaming */
/*****************************************************/
typedef struct trelt {
	int symb;		/* symbol-nu of variable 	*/
	termtype *term;	 	/* is bound to term 		*/
        int reference;          /* LOW_REFERENCE or HIGH_REFERENCE */
        int highClauseNr;       /* Clause_nr of the renamed clause */
        struct preli **loc;     /* pointer to symbol's loclist  */ 
	} trailelt;


/*****************************************************/
/* inwasm command-line flags*/
/*****************************************************/
typedef struct struct_inwasm_cmdline_flags {
 int lop        ;               /* generate lop-output file */
 int all	;		/* allsolution flag */
 int verbose	;
 int reduct	;		/* reductionsteps	 	*/
 int fan        ;              	/* fanning 			*/
 int predfail	;		/* equal predecessor flag 	*/
 int ptree	;		/* generate proof tree flag	*/
 int randreord	;		/* random reordering flag	*/
 int clausreord	;		/* heuristic clause reordering 	*/
 int sgreord	;		/* heuristic sg reordering 	*/ 
 int genreg	;		/* regularity			*/
 int purity	;		/* purity removal		*/
 int qvariable	;		/* query variable flag    	*/
 int set_subgoal_list	;	/* partialtree			*/
 int interactive;		/* interactive			*/
 int searchtree ;		/* searchtree			*/
 int nocode	;		/* no code generation		*/
 int cpybd	;		/* there is a global copybound  */
 int cpylimit	;		/* limit of global copybound    */
 int subs	;		/* subsumtion and symmetry	*/ 
 int tauto	;		/* tautology constraints	*/ 
 int overlap	;		/* overlap constraints		*/ 
 int linksubs	;		/* deleting connections		*/
 int foldup	;		/* fold-up 			*/
 int foldupx	;		/* fold-up  w/ ext. reg.        */
 int folddown	;		/* fold-down 			*/
 int folddownx	;		/* fold-down  w/ ext. reg.      */
 int hex_out	;		/* generate .hex directly       */
 int hex_out_opt;		/* optimize .hex-file		*/
 int oldclausreord;             /* uses the old clause reordering */
 int ignorelits;                /* activates '^'-Option for literals */
                                /* if '^' is a prefix of a literal then
				   the literal is not counted    */
 int cputimelimit;              /* cpu-time limitation to x sec. */
 int realtimelimit;             /* realtime limitation to x sec. */
 int memorylimit;               /* memorylimit in byte */
  int freememory;                /* activates deallocation of memory */
  int logprogsubs;     
  int ignoreclstart;
  int clstart;
}
inwasm_cmdline_flags;

/****************************************************************/
/* Zum Sortieren der Klauseln nach Laenge, termkomplexitaet und */
/* Variablenanzahl                                              */
/****************************************************************/
typedef struct say {
        double kriterium;
        int varanz;
        claustype *Clause;
} sortarray;

/******************************************************************/
/* Struktur fuer Partner-Literale eines Literals aus kurzeClausel */
/******************************************************************/
typedef struct plit  {
  predtype *pr;        /*  Partner-Literal */
  int notunif;         /* = 1 falls Partner-Literal
			  mit zugh. Literal nicht
			  unifizierbar. sonst 0 */
  struct plit *next;   /* next Partner-Literal */
} plittype;


/*******************************************************************/
/* Struktur fuer aus kurzeClausel kopierte und umbenannte Literale */
/*******************************************************************/
typedef struct lit  {
  int symb;            /* Praedikatsymbol */
  int sign;
  struct term *t_list;
  plittype *s_list;    /* Partner-Literale */
  struct lit *next;    /* naechstes Literal */
} littype;

#endif
/*######################################################################*/
