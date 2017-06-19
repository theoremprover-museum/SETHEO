/************************************************************************/
/*                                                                      */
/*	    P A R T H E O                                               */
/* FILE:    machine.h                                                   */
/* VERSION: @(#)machine.h	8.2 27 Feb 1996                         */
/* PROJECT:                                                             */
/*                                                                      */
/* AUTHOR: J. Schumann                                                  */
/* DATE:   8.88                                                         */
/* DESCR:                                                               */
/*          same as setheo/machine.h                                    */
/* MOD:                                                                 */
/* BUGS:                                                                */
/*	11.1.88	proof-tree                                              */
/*	6.6. reproof option & display                                   */
/*	14.6. reproof changed (chp->red_lvl)                            */
/*	27.7. reproof check                                             */
/*	29.7. debugging                                                 */
/*      10.10.                                                          */
/*	7.11.   global variables                                        */
/*	11.11.	temp area                                               */
/*	20.2.89 built-ins                                               */
/*	28.2.89 built ins & new output                                  */
/*	25.4.89 statistics                                              */
/*	12.6.89      "                                                  */
/*	24.7.89 DEF'd deref                                             */
/*	3.5.90  logo                                                    */
/*	29.6.90 nr_built-ins                                            */
/*      23.7.91 code_last, pt_area_last added by Goller                 */
/*      08.01.92  REG_CONSTR: structure c_right                         */
/*                added by bschorer                                     */
/*      09.01.92  REG_CONSTR: environment-structure modified            */
/*                added by bschorer                                     */
/*      07.04.92  ANL_CONSTR: added by bschorer                         */
/*	02.09.92  instr-functions                                       */
/*	11.3.93	Mueller,Goller neue Constraints, Statistics             */
/*	29.6.93		jsc    include-defines                          */
/*	17.9.93		jsc    remove of old lemma stuff, tell,told     */
/*			       copy-binding                             */
/*	22.3.94		jsc    for sunOS5.X                             */
/*	27.9.94		jsc    cpu-time limit                           */
/*	13.1.95		jsc    new entries in lcb-block                 */
/*	9.3.95		jsc    changed entries in lcb-block             */
/*			       batchmode                                */
/*	1.5.97		jsc	hide-predicates                         */
/************************************************************************/
/************************************************************************/
#ifndef MACHINE_H
#define MACHINE_H


/*************************************************************/
/* Logo-Definitions                                          */
/*************************************************************/
extern char *version;		/* logo for setheo main.c    */
extern char *ident;		/* logo for setheo main.c    */

/*************************************************************/
/* File-Definitions                                          */
/*************************************************************/
extern FILE           *logfile;
extern FILE           *ftree;
extern FILE	      *outputfile;

#ifdef STATPART
extern FILE	      *taskfile;
extern FILE           *resfile;
extern FILE           *modfile;
extern FILE           *subgsfile; /* IAnd */
#endif

/******************************************************************/
/*** Definition of a printf-Makro for using the setheo-log-file ***/
/******************************************************************/
#define PRINT_LOG4(A,B,C,D,E) {fprintf(stderr,A,B,C,D,E);\
	                       fprintf(logfile,A,B,C,D,E);\
                               fflush(stderr);\
                               fflush(logfile);}
#define PRINT_LOG3(A,B,C,D)   {fprintf(stderr,A,B,C,D);\
                               fprintf(logfile,A,B,C,D);\
                               fflush(stderr);\
                               fflush(logfile);}
#define PRINT_LOG2(A,B,C)     {fprintf(stderr,A,B,C);\
                               fprintf(logfile,A,B,C);\
                               fflush(stderr);\
                               fflush(logfile);}
#define PRINT_LOG1(A,B)       {fprintf(stderr,A,B);\
                               fprintf(logfile,A,B);\
                               fflush(stderr);\
                               fflush(logfile);}
#define PRINT_LOG0(A)         {fprintf(stderr,A);\
                               fprintf(logfile,A);\
                               fflush(stderr);\
	                       fflush(logfile);}
/******************************************************************/

/******************************************************************/
/*** MACROS for memory management                                 */
/******************************************************************/

#ifndef linux
#define sam_memory_init(X) 
#define sam_malloc(X) (malloc(X))
#define sam_calloc(X,Y) (calloc(X,Y))
#endif

/******************************************************************/
/*** MACROS for argument access                                   */
/******************************************************************/
#define ARG(I)     (*(pc + I))
#define ARGPTR(I)  (code + GETVAL(*(pc +I)))
#define ARGV(I)	   (ARGPTR(1) +I)

/******************************************************************/
/*** MACROS for weighted depth bound                              */
/******************************************************************/
#define MAXIMUM(A,B)   ((A >= B) ? A : B)
#define MINIMUM(A,B)   ((A <= B) ? A : B)

/******************************************************************/
/*** MACROS for subgoal access                                    */
/******************************************************************/
/* Offset for next clause of current subgoal: */
#define NEXT_CL_OFFSET(env) \
        GETNUMBER(env->lcbptr->nr_sgls)

/* Offset for reduction pointer of current subgoal: */
#define REDPTR_OFFSET(env) \
        (2 * GETNUMBER(env->lcbptr->nr_sgls))

/* Offset for delay level of current subgoal: */
#define DL_OFFSET(env) \
        (3 * GETNUMBER(env->lcbptr->nr_sgls))

/* Get next clause of current subgoal: */
#define GET_NEXT_CL_OF_SG(env) \
        GETPTR(*(env->tp + NEXT_CL_OFFSET(env)))

/*
        (WORD*) GETVAL(*(env->tp + NEXT_CL_OFFSET(env)))
*/

/* Get reduction pointer of current subgoal: */
#define GET_REDPTR_OF_SG(env) \
        GETPTR(*(env->tp + REDPTR_OFFSET(env)))

/* Get delay level of current subgoal: */
#define GET_DL_OF_SG(env) \
        GETNUMBER(*(env->tp + DL_OFFSET(env)))

/* Generate next clause of current subgoal: */
#define GEN_NEXT_CL_OF_SG(next_cl,env) \
        GENPTR(*(env->tp + NEXT_CL_OFFSET(env)), next_cl)

/* Generate reduction pointer of current subgoal: */
#define GEN_REDPTR_OF_SG(redptr,env) \
        GENPTR(*(env->tp + REDPTR_OFFSET(env)), redptr)

/* Generate delay level of current subgoal: */
#define GEN_DL_OF_SG(dl,env) \
        GENNUMBER(*(env->tp + DL_OFFSET(env)), dl)



/**************************************************************
         Definition of choice-points and environments
                *** preproof reordering only ***
***************************************************************/


/***************************************************************/
/* choice-point                                                */
/***************************************************************/
typedef struct chp {
    struct chp *ch_link;    /* choice-point chain   */
    struct tr_s *top_trail; /* top of trail         */
    WORD *top_heap;         /* top of heap          */
    struct env *goal_bp;    /* current env of goal  */
    struct env *red_ptr;    /* reduction pointer    */
#ifdef FOLD_UP
    WORD *red_tp;           /* pointer into tp-list of the environment
			       red_ptr */
    int open_sgls;          /* # subgoals in red_ptr, which have not yet
			       been used in optimistic factorization for
			       the current chp */	
#endif
    struct copytr *cpy_save;/* copy-trail save      */
    WORD *ret_addr;         /* current return addr  */
    WORD *this_clause;      /* pointer to currently examined cl. */
    WORD *next_clause;      /* pointer to next cl. to examine    */
    WORD *gp_save;          /* saved pointer to ga  */
    int  depth;             /* saved depth          */
    int  infcount;          /* inference count      */
    int  sgscount;          /* open subgoals count  */
    int  sigcount;          /* value of signature   */
    int  inf_lookahead;     /* inference lookahead  */
    int  local_inf_bound;   /* save current         */
                            /* local_inf_bound      */
                            /* added by bschorer    */

#ifdef REPROOF_MODE
    struct p_t *ptree_save; /* saved proof-treeptr  */
    int  red_lvl;           /* reduct. level used   */
                            /* for reproof only     */
#endif

#ifdef CONSTRAINTS
#ifdef ANTILEMMATA
    struct env  *head_bp;   /* pointer to head of next environment */
    int          nr_fvars;  /* number of variables, which are 
                               free before inferences are applied to 
                               this node */
    WORD       *block_fvars; /* pointer to block of variables, which are 
                               free before inferences are applied to 
                               this node */
    struct tr_s *end_trail; /* end of trail (if not NULL chp is solved) */
    struct antilemma_list  *anl_list;  /* pointer to list of antilemmata */
    int  end_infcount;          /* nr of infs at end of subtree           */
    int  end_total_inf;         /* total nr of infs at end of subtree     */
#endif
    struct constraint_counter *deact_c_counter_sp;
    WORD *c_sp;             /* pointer to top of constraint stack */
    struct constraint_counter *c_counter_sp;
#endif

    WORD * sp;		    /* save sp in choice pt */

    } choice_pt;
/***************************************************************/



/***************************************************************/
/* environment                                                 */
/***************************************************************/
typedef struct env {
    struct env *dyn_link;   /* dynamic link                           */
    WORD *ret_addr;         /* return address                         */
    struct lctrbl *lcbptr;  /* literal_ctrl_block pointer             */
    WORD *tp;               /* ptr to list of proof-tree	      */
    WORD flags;             /* bit 28 indicates, that the environment
			       is currently folded_up; the bits 0-23
			       hold the inference_number which is
			       determined not until ptree             */
    int   depth;            /* saved depth (for weighted depth)       */
    int   infcount;         /* saved infcount (for weighted depth)    */
    int   depth_free;       /* saved free depth (for weighted depth)  */
#ifdef FOLD_UP
    struct env *path_link;      /* new predecessor */
    struct env *red_marking;    /* pointer to the environment of the 
				   clause, from where the latest usage 
				   by a reduct comes */
#endif
#ifdef CONSTRAINTS
    struct chp  *chp;       /* pointer to actual choice-point         */
    struct constraint_counter *c_counter_sp;
                            /* points to last gen. constraint counter */
#endif
#ifdef STATPART
    WORD *next_adr;         /* CS 4.9.94 fuer iAND-Behandlung         */
#endif
} environ;
/***************************************************************/


/***************************************************************/
/* literal control block                                       */
/***************************************************************/
typedef struct lctrbl {
    WORD clauselbl;         /* code-offset for contrapositive of this    */
                            /* literal                                   */
    WORD ps_symb;
    WORD type;              /* type of lcb (declarative, proc_fact,      */
                            /* proc_query, proc_rule, reduct)            */
    WORD clause_nr;
    WORD lit_nr;
    WORD nr_vars;	    /* number of all distinct variables	         */
    WORD nr_strvars;        /* number of distinct structure variables    */
    WORD nr_sgls;           /* number of subgoals                        */
    WORD ddepth;            /* value by which depth is decremented       */
    WORD dinf;              /* value by which infcount is incremented    */
    WORD argv;              /* code-offset for arguments of this literal */
    WORD subgoal_list;      /* code-offset for list of subgoals          */
    WORD orlabel;           /* code-offset for orlabel                   */
} literal_ctrl_block;

#ifdef STATPART
/********************************************************************/
/* union of one cell in the list of subgoals (pointed to by bp->tp) */
/********************************************************************/
typedef union sg_info {
	environ 	*env;	/* points to a subgoal in work or solved */
	literal_ctrl_block *lcb;
				/* points to a subgoal not yet touched	*/
	} subgoal_info;
#endif

/***************************************************************/
/* structure of trail (usable for global variables)            */
/***************************************************************/

/* types of trails  */
#define TRAIL_TYPE_WORD 1
#define TRAIL_TYPE_LINT 2
#define TRAIL_TYPE_INT 3
#define TRAIL_TYPE_PTR 4

#ifdef WORD_64
typedef struct { unsigned long l1,l2;} TRAIL_STORAGE_TYPE;
#else
typedef unsigned long TRAIL_STORAGE_TYPE;
#endif

typedef struct tr_s {
    int  type;           /* type of variable trailed (int, WORD, etc) */
    TRAIL_STORAGE_TYPE *ptr;		/* ptr to bound object	*/
    TRAIL_STORAGE_TYPE oval;		/* orig. value		*/
} s_trail;

#ifdef REPROOF_MODE
/***************************************************************/
/***************************************************************/
/* proof-tree structure */
typedef struct p_t {
	WORD codeptr;		/* ptr (coderel) to clau*/
	int red_lvl;		/* level of reduction */
	} p_tree;
/***************************************************************/
#endif

#ifdef CONSTRAINTS

/***************************************************************/
typedef struct constraint_counter {
    int  length;                  /* counter of constraint                */
#ifdef ANTILEMMATA
    struct chp  *chp;             /* pointer to actual choice-point       */
    int         infcount;        /* only important for antilemmata: 
                                     infcount of corresponding antilemma  */
#endif
    WORD next;                    /* next constraint counter              */
} c_counter;
/***************************************************************/

/***************************************************************/
typedef struct c_right {
        WORD             *sub_gp;     /* pointer to the argument of the   */
                                      /* the subconstraint-right-side     */
        struct constraint_counter *zaehler;
                                      /* pointer to the counter of this   */
                                      /* subconstraint                    */
        struct c_right   *next;       /* pointer to the next subconstraint*/
                                      /* refering to the variable         */
} constr_right;
/***************************************************************/

#ifdef ANTILEMMATA
/***************************************************************/
typedef struct constraint_list {
        struct constraint_counter *zaehler; /* pointer to constraint counter */
        struct constraint_list    *next;    /* pointer to next constraint    */
                                            /* within this constraint list   */
} c_list;
/***************************************************************/


/***************************************************************/
typedef struct antilemma_list {
    int                    active;     /* =1 <=> anl is active         */
    int                    arity ;     /* nr of subantilemmata         */
    WORD                  *left;       /* pointer to left side of anl  */
    WORD                  *right;      /* pointer to right side of anl */
    int                    infcount;   /* infcount of environment      */
                                       /* at time of anl generation    */
    struct antilemma_list *next;       /* pointer to next antilemma    */
} a_list;
/***************************************************************/
#endif


/***************************************************************/
typedef struct subgoal_result {
    int  tc;                           /* termcomplexity of subgoal       */
    int  fvars;                        /* nr of free variables in subgoal */
    int  ground;                       /* ground == 1  <==>  sg is ground */
    int  ps;                           /* index of pred symb of subgoal   */
    int  dl;                           /* delay level of subgoal          */
                                       /* (== nr_sgls of next alternative)*/
    int  var_common;                   /* var_common == 1  <==>  tag      */
                                       /* T_MARKED_FVAR found             */
} sg_result;
/***************************************************************/

extern char    anl_constr;       /* Flag for using Antilemma-Constraints  */
extern char    reg_constr;       /* Flag for using Regularity-Constraints */
extern char    ts_constr;        /* Flag for using Tautologie-Constraints */
                                 /* AND Subsumption-Constraints           */
extern char    constraints;      /* Flag for using all constraints        */

extern WORD *c_stack;        /* pointer to beginning of c-stack            */
extern WORD *c_stack_end;    /* end of c-stack                             */
extern WORD *c_sp;           /* pointer to TOP-C-STACK                     */
extern c_counter *c_counter_sp; /* pointer to last generated */
                                /* constraint counter        */

extern s_trail *pu_trail;     /* pointer to beginning of pu_trail           */
extern s_trail *pu_tr;        /* pointer to current top-of-pu-trail         */
extern s_trail *pu_tr_last;   /* pointer to end of pu_trail                 */



/* STATISTICS1 */
extern int  c_reg_fails;     /* # of regularity-constraint-fails           */
extern int  c_ts_fails;      /* # of taut-subs-constraint-fails            */
extern int  c_anl_fails;     /* # of antilemma-constraint-fails            */
extern int  gen_reg_constrs; /* # of generated regularity-constraints      */
extern int  gen_ts_constrs;  /* # of generated taut/subs -constraints      */
extern int  gen_anl_constrs; /* # of generated antilemma -constraints      */

#ifdef STATISTICS2
extern int  c_check_count;     /* # of constraint-checks                     */
extern int  c_immed_reg_fails; /* # of fails prod. by empty reg-constraints  */
extern int  c_immed_ts_fails;  /* # of fails prod. by empty ts -constraints  */
extern int  c_immed_anl_fails; /* # of fails prod. by empty anl-constraints  */
#endif

#endif



/**************************************************************/
/*        Definition of the machine registers                 */
/**************************************************************/
/* global variables:                                          */
/**************************************************************/

extern WORD *pcstart;   /* start address */ 
extern WORD *pc;        /* Program counter -> code area         */
extern WORD *sp;        /* Stack pointer                        */
extern struct env *bp;      /* Base Pointer                     */
extern struct env *goal_bp; /* Base Pointer                     */
extern WORD *hp;        /* HEAP Pointer                         */
extern s_trail *tr;     /* Trail pointer                        */
extern WORD *ra;        /* Return address                       */
extern choice_pt *chp;  /* choice-point-pointer                 */
extern WORD *gp;        /* points to goal structure             */
extern literal_ctrl_block	*lcbptr;

extern char zflag;	/* <>0 if zero      			*/
extern char cflag;	/* <>0 if <= 0         			*/
extern char boundflag;	/* <>0 if bound fail			*/

extern int verbose;	/* verbose mode				*/
extern int batchmode;	/* if <>0: ^C aborts			*/
extern int print_all_trees; /* if <>0: all computed proof trees 
			               are written into the .tree 
				       file
			       else:   only the first tree is
			               written into the .tree 
				       file                     */
extern int print_lemmata; /* if <>0: the generated lemmata are 
			             written into outputfile    */

#ifdef REPROOF_MODE
extern p_tree *pt_ptr;	  /* proof-tree pointer			*/
extern p_tree *pt_max;	  /* proof-tree end pointer		*/
extern int pmode;	/* ==0 for proof, <>0 for reproof	*/
#endif

/**************************************************************/
/*** machine registers for subgoal reordering               ***/ 
/**************************************************************/
extern int  dynsgreord;     /* level of dynamic subgoal reordering  */

/**************************************************************/
/*** machine registers for subgoal alternation              ***/ 
/**************************************************************/
extern int  delaycount;     /* current nr of delays                 */
extern int  single_delay;   /* flag for single delay of subgoals    */
extern int  multi_delay;    /* flag for multi-delay of subgoals     */
extern int  delay_resource; /* minimum resource that must be        */
                            /* available to allow delay             */
extern int  force_ground;   /* force_ground == 1 <==> do not delay, */
                            /* if actual subgoal is ground          */
extern int  force_ps;       /* force_ps == 1 <==> change to a       */
                            /* subgoal with the same predicate      */
                            /* symbol                               */
extern int  force_fvar;     /* force_fvar == 1 <==> change to a     */
                            /* subgoal with at least one commom     */
                            /* free variable                        */
extern int  force_delay;    /* force_delay == 1 <==> recognize      */
                            /* delay-criterions in call-instruction */
extern int  spread_reducts; /* spread_reducts == 1 <==>             */
			    /* If first alternative at a subgoal is */
                            /* a reduction step:                    */
                            /* - If there is a second alternative   */
                            /*   delay level of the subgoal is      */
                            /*   initialized with number of subgoals*/
                            /*   in second alternative.             */
                            /* - If there is no second alternative  */
                            /*   delay level of the subgoal is      */
                            /*   initialized with 0.                */
extern int  delay_switch;   /* flag indicates switching between 
                               delay options:
                               clauses up to length <switch_length>
                               are handeled with <option_for_shortcl>
                               the other clauses are handeled with 
                               <option_for_longcl> */
extern int  switch_length;         
extern int  option_for_shortcl;
extern int  option_for_longcl;

/*********************************************************************/
/*** machine registers for equality handling                       ***/ 
/*********************************************************************/
extern int  eq_by_ste_mod;  /* if on, lookahead information is used  */
                            /* in wd-bound in a way suitable for     */
                            /* equality handling by STE-modification */

/***************************************************************/
/*** machine registers for look ahead                        ***/ 
/***************************************************************/
extern int  lookahead_flag; /* if on, a look ahead for bound 
			       fail is performed               */
extern int  inf_lookahead;  /* min nr of additional inferences */

/**************************************************************/
/*** machine registers for depth-bound search               ***/ 
/**************************************************************/
extern int  depth;          /* current depth                  */
extern int  depth_bound;    /* depth-bound flag               */

/**************************************************************/
/*** machine registers for weighted depth-bound search      ***/ 
/**************************************************************/
extern int  wd_bound;     /* weighted-depth-bound flag        */
extern int  wd1;          /* initialisation constant          */
extern int  wd2;          /* initialisation factor            */
extern int  wd3;          /* dynamic factor                   */
extern int  wd4;          /* dynamic factor for integrating  
			     local-inference-bound strategy   */
extern int  wd4_flag;     /* flag: if on, wd-bound search can 
                             simulate local inf bound search  */

/**************************************************************/
/*** machine registers for inference-bound search           ***/ 
/**************************************************************/
extern int  intmd_inf;     /* intermediate nr of inferences      */
extern int  infcount;      /* current nr of inference		*/
extern int  maxinf;	  /* maximum nr of inferences		*/
extern int  inf_bound;     /* inference-bound flag               */
#ifdef ANTILEMMATA
extern c_list  **delayed_by_inf_bound; /* pointer to array of maxinf */
                                       /* pointers for anl/infbound  */
                                       /* handling                   */
#define  INF_BOUND  0     /* parameter for procedure delay_anl_constr, */
                          /* ensures easy extendability if delay       */
                          /* mechanism is applied to more bounds       */
#endif

/**************************************************************/
/*** machine registers for local_inference-bound search     ***/ 
/**************************************************************/
extern int local_inf_bound;   /* current local_inf bound      */

/**************************************************************/
/*** machine registers for free variables-bound search      ***/ 
/**************************************************************/
extern int intmd_fvars;    /* intermediate nr of free variables	*/
extern int maxfvars;	   /* max allowed nr of free variables  */
extern int fvars_fails;    /* # of fvars-fails                  */
extern int fvars_bound;    /* fvars bound flag                  */

/**************************************************************/
/*** machine registers for free variables-bound search      ***/
/*** (dependent on depth)                                   ***/
/**************************************************************/
extern int fvarsd_flag;   /* if on, fvars bound search is dependent on depth */
extern int fvarsd1;       /* fvarsd constant */
extern int fvarsd2;       /* fvarsd factor   */


/**************************************************************/
/*** machine registers for term-complexity-bound search     ***/ 
/**************************************************************/
extern int intmd_tc;          /* intermediate termcomplexity    */
extern int maxtc;	      /* max allowed termcomplexity     */
extern int tc_fails;          /* # of termcomplexity-fails      */
extern int tc_bound;          /* tc bound flag                  */
#define  COMPL_FSYMB  1       /* complexity of function symbols */
#define  COMPL_CONST  1       /* complexity of constants        */
#define  COMPL_FVAR   0       /* complexity of free variables   */
#define  COMPL_DAG    1       /* complexity of seen terms/variables */

/**************************************************************/
/*** machine registers for term-complexity-bound search     ***/ 
/*** (dependent on depth)                                   ***/
/**************************************************************/
extern int tcd_flag;    /* if on, tc bound search is dependent on depth */
extern int tcd1;        /* tcd constant */
extern int tcd2;        /* tcd factor   */

/**************************************************************/
/*** machine registers for open-subgoal-bound search        ***/ 
/**************************************************************/
extern int sgscount;           /* current nr of open subgoals	*/
extern int intmd_sgs;          /* intermediate nr of open sgs   */
extern int maxsgs;	       /* max allowed nr of open sgs    */
extern int sgs_fails;          /* # of open-subgoals-fails      */
extern int sgs_bound;          /* open sgs bound flag           */

/**************************************************************/
/*** machine registers for open-subgoal-bound search        ***/ 
/*** (dependent on depth)                                   ***/
/**************************************************************/
extern int sgsd_flag;    /* if on, sgs bound search is dependent on depth */
extern int sgsd1;        /* sgsd constant */
extern int sgsd2;        /* sgsd factor   */

/**************************************************************/
/*** machine registers for signature-bound search           ***/ 
/**************************************************************/
extern int sigcount;     /* current value of used signature      */
extern int intmd_sig;    /* intermediate value of used signature */
extern int maxsig;	 /* max allowed value of used signature  */
extern int sig_fails;    /* # of signature-bound-fails           */
extern int sig_bound;    /* signature bound flag                 */
extern int *sig_occurrence;  /* array for indicating, if a symbol*/
                             /* has already accurred in proof    */

/**************************************************************/
/*** machine registers for signature-bound search           ***/ 
/*** (dependent on depth)                                   ***/
/**************************************************************/
extern int sigd_flag;    /* if on, sig bound search is dependent on depth */
extern int sigd1;        /* sigd constant */
extern int sigd2;        /* sigd factor   */

#ifdef ANTILEMMATA
/**************************************************************/
/*** machine registers for antilemma handling               ***/ 
/**************************************************************/
extern int      anlcount;      /* current nr of antilemmata                  */
extern int      mincost;       /* characterizes the min cost of search       */
                               /* necessary to make an antilemma interesting */
extern int      lpanl;         /* flag: if on, anl generation only for facts */
                               /* and for reduction steps                    */
extern int      stronganl;     /* flag: if on, generation of stronger anl    */
extern int      horn;          /* flag: if on, input files are treated as    */
                               /*       horn formulae during generation of   */
                               /*       strong anl                           */
#define DELAYED_BY_INF_BOUND(i)    delayed_by_inf_bound[i-1]
                               /* necessary for anl/infbound handling        */
#endif

/**************************************************************/
/*** machine registers for counter handling                 ***/ 
/**************************************************************/
extern WORD    *counter_start;      /* heap address of 1st counter  */
extern int      num_of_counters;    /* total number of counters     */


/**************************************************************/
/*** some statistic counters (STATISTICS1)                  ***/ 
/**************************************************************/
	
extern float overall_time;
extern float evaltime;
extern int 	cputime_limit;
extern int 	realtime_limit;
extern struct tms buffer;
extern int total_inf;       /* # of tried inferences        */
extern int global_tot_inf;  /* # of tried inferences (total)*/
extern int total_reduct;    /* # of tried reduction-steps   */
extern int reductions_in_proof; /* is calculated in ptree */

typedef struct fold_stat {
int total_factorizations; 
int factorizations_in_proof; /* is calculated in ptree */
int foldings;
int possible_lemmas;
int one_level_foldings;
} folding_stat;

extern folding_stat folding_statistics;

extern int fails;           /* # of fails                   */
extern int unif_fails;      /* # of unification fails       */
extern int depth_fails;     /* # of depth-bound-fails       */
extern int inf_fails;       /* # of inf-fails               */
extern int local_inf_fails; /* # of local_inf - bound-fails */
extern int cnt;             /* count # of instructions      */

/**************************************************************
        Definition of Data areas
                STACK
                TRAIL
                CODE
                HEAP
        with dynamic allocation

**************************************************************/

extern WORD *stack;
extern WORD *up_stack;

extern s_trail *trail;
extern s_trail *trlast; /* pointer to end of Trail              */

extern WORD *code;
extern WORD *code_last;

extern WORD *heap;
extern WORD *hplast;    /* Pointer to end of HEAP               */

/*************************************************************/
/* structure for hiding predicates in the tree-output        */
/*************************************************************/
typedef struct st_hidden_preds {
	char *name;
	struct st_hidden_preds	*next;
	} t_hidden_preds;

extern t_hidden_preds	*hidden_preds;

/*************************************************************/


/*************************************************************/
/* instruction function table                                */
/*************************************************************/
typedef struct _i_tab {
	instr_result	(*instruction)();/* function for exec.	     */
				/* the instruction	     */
	char	*name;		/* name of instruction	     */
	int	length;		/* size of instr. in WORDS   */
} instr_tab;


/******************************************************************/
/* the instr_table must be extern, since it is defined in conf.c  */
/******************************************************************/

#ifdef extern
#undef extern
extern instr_tab    instr_table[];
#define extern
#else
extern instr_tab    instr_table[];
#endif

extern int   nr_instr;

#include "sam_prototypes.h"

#endif


