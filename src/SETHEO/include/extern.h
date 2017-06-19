/*########################################################################
# File:			extern.h
# Author:		J. Schumann
# Date:			2.7.93
# Context:		inwasm (SETHEO)
# INWASMSCCS:		%W  07 Apr 1995
# SCCS        : @(#)extern.h	10.1 02 Apr 1996
# 
# Contents:		external definitions of global variables
#
# Modifications:	
#	(when)		(who)		(what)
#	12.7.93		jsc		grouping of global variables
#	14.12.93	jsc		purity, fold
#			hamdi		include ficonst.h
#	11.9.95		jsc		predicate_nr
#	3/12/96		jsc	moved to ~/include
#	3/12/96		jsc		wasm-less inwasm
#	3/26/96		jsc		does not include ficonst.h
#
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
########################################################################*/

#ifndef EXTERN_H
#define EXTERN_H

#include <stdio.h>
#include "symb.h"

/***************************************************************************/
/* local macros                                                            */
/***************************************************************************/
	/* handle the initialization */
#ifndef extern
#	define INIT(X)
#else
#	define INIT(X) = X
#endif


/***************************************************************************/
/* matrix: intern representation of formula                                */
/***************************************************************************/
extern modtype *matrix;		             /* global, internal Structure */


/***************************************************************************/
/* globals for code generation                                             */
/***************************************************************************/
extern claustype *startclause INIT(NULL);
extern symbollist *redlist INIT(NULL);	       /* list for reduction facts */
extern symbollist *cpylist INIT(NULL);             /* list for copy bounds */
extern int nrvars;		       /* number of vars+strvars in clause */ 
extern int nrstrvars;			    /* number of strvars in clause */ 
extern int failure_clause_needed INIT(0);


/***************************************************************************/
/* Symbol table                                                            */
/***************************************************************************/
extern int predicate_nr INIT(1);   /* number of predicates in symbol table */
extern int max_arity_fsymb INIT(0);      /* max. arity of function symbols */

/***************************************************************************/
/* Counters                                                                */
/***************************************************************************/
extern int maxlit INIT(0);	                         /* longest clause */
extern int orbcal INIT(0);	                 /* counter for call or... */
extern int detcal INIT(0);	              /* counter for call l[np]... */
extern int nr_globals INIT(0);		     /* number of global variables */
extern int nr_constants INIT(0);	 /* number of constants in formula */
extern int nr_functions INIT(0);	 /* number of functions in formula */
extern int nr_numbers INIT(0);		   /* number of numbers in formula */
extern int nr_predicates INIT(0);	/* number of real predicate symb's */
extern int nr_constraints INIT(0);     /* number of constraints in formula */
extern int nr_negated_clauses INIT(0);	     /* number of negative clauses */
extern int nr_definite_clauses INIT(0);	     /* number of definite clauses */
extern int nr_indefinite_clauses INIT(0);  /* number of indefinite clauses */
extern int nr_facts INIT(0);		                /* number of facts */
extern int nr_literals INIT(0);	                     /* number of literals */
extern int nr_connections INIT(0);	          /* number of connections */
extern int maxlits INIT(0);		          /* maximal clause length */
extern int minlits INIT(0);		          /* minimal clause length */
extern float complexity INIT(0); 	             /* literal complexity */
extern int subs_unif_counter INIT(0); 
extern int subs_max_counter INIT(0);      /* in order to break subsumption */

/***************************************************************************/
/* etc                                                                     */
/***************************************************************************/
extern int opt_incount;		                  /* for hex_out-optimizer */
extern int opt_outcount;            		  /* for hex_out-optimizer */

/***************************************************************************/
/* scanner and parser                                                      */
/***************************************************************************/
extern int yylineno;
extern int nofanning INIT(0);	 /* there is at least one nofanning clause */
extern int loc_cpy INIT(0);	             /* there is a local copybound */
extern int ignorelits_flag;
extern int subgoal_flag;

/***************************************************************************/
/* subsumption and symmetry constraints                                    */
/***************************************************************************/
extern int nr_gen_constr INIT(0) ;

extern int no_subsumption;
extern int subsumption;
extern int is_subsumption;

extern int abbruch;
extern int var_counter INIT(0);        /* Zaehler zur Umbenennung von Var. */
extern int globalcnt INIT(0);      /* Global counter for various functions */
extern sortarray *ssort;		                          

/***************************************************************************/
/* constraint subsumption                                                  */
/***************************************************************************/

extern int Deleted; 
extern int Olddeleted; 
extern int Generated;

/***************************************************************************/
/* cpu-time, real-time and memory limitation                               */
/***************************************************************************/
extern double memorylimit INIT(0);
extern double memoryused INIT(0);
extern double *cnt_malloc();
extern void  free_termtype();
extern void free();

extern int cputimelimit INIT(0);
extern int realtimelimit INIT(0);
extern float overall_time;
extern int start_time;                  /* variable which stores the beginning time */
extern void sig_alrm();

/* JSC 2002
extern int alarm();
*/
extern long start_time_in_sec;


/***************************************************************************/
/* extern variables                                                        */
/***************************************************************************/
/* extern variables (inwasm/wunif.c)                                       */
extern int maxunif INIT(0);	                    /* maximum choicepoint */
extern int nrbuilt INIT(0);         /* number of built-ins in symbol table */
extern syel *symbtab;		                /* pointer to symbol table */
/* extern variables (inwasm/filehandler.c)                                 */
extern FILE *del_file;		              /* deleted connection file w */
extern FILE *code_file;		                     /* code output file w */
extern FILE *lopout_file;	                      /* lop output file w */
extern FILE *yyin;		                       /* lop input file r */
/* extern variables (inwasm/generator.c)                                   */
extern int nr_clauses INIT(0);	          /* number of clauses in matrix   */
extern int parsed_nr_clauses;             /* number of clauses in formula  */
extern int ishorn INIT(1);	                      /* is a horn formula */
extern int nump	INIT(0);	      /* number of symbols in symbol table */
extern int VariableEnum;                    /* counter for enumeration of all 
  					          variables in all clauses */
extern t_fcb fcb;                     /* internal representation of matrix */
/* extern variables (inwasm/main.c)                                        */
extern char filename_base[100];                       /* filename to parse */
extern inwasm_cmdline_flags inwasm_flags;                  /* inwasm flags */
extern float sgreord_at;                  /* PJ opt para for sg_reordering */ 
extern float sgreord_av;                                 /* weight formula */
extern float sgreord_ac;
extern int overlap_mode INIT(2);             /* maximal overlap-complexity */
extern int deletion_mode INIT(6);         /* maximal link-deletion-complexity
					       test up to 5000 connections */
extern int errors INIT(0);                 /* errors during codegeneration */
/* extern variables (inwasm/unify.c)                                       */
extern int UnificationMode INIT(0);       /* stores unif-mode for makeref  */
extern trailelt * trail INIT(NULL);              /* weak unification trail */
extern int trailcnt;                          /* unification trail counter */
extern int LowClauseNr;                       /* stores clnr of clause 1/2 */
extern int HighClauseNr;                                       /* to unify */
extern int BoundVariables;                     /* counter for bound variables
					        svariables are not counted */
/* extern variables (inwasm/preproc/connopt.c)                             */
extern int nr_del_conn;
extern int nr_links_tested;
/* extern variables (inwasm/preproc/sconstr.c)                             */
extern predtype *ConstraintList INIT(NULL);  
/* extern variables (inwasm/codegen/cgen.c)                                */
extern int constrnum;
/* extern variables (inwasm/preproc/cl_reordering.c)                       */
extern int cl_reord_start INIT(1);
extern int cl_reord_start_commline INIT(1);
extern int curr_clausenr INIT(0);


/*** EXTERN FUNCTIONS *** EXTERN FUNCTIONS *** EXTERN FUNCTIONS *** EXTERN */


/***************************************************************************/
/***INWASM *** INWASM *** INWASM *** INWASM *** INWASM *** INWAMS *** IN ***/
/***************************************************************************/

/***************************************************************************/
/* extern functions (inwasm/built_ins.c)                                   */
/***************************************************************************/
extern void enter_built_ins();

/***************************************************************************/
/* extern functions (inwasm/fanning.c)                                     */
/***************************************************************************/
extern int negative_goals();
extern void genfanning();

/***************************************************************************/
/* extern functions (inwasm/filehandler.c)                                 */
/***************************************************************************/
extern void open_io_files();

/***************************************************************************/
/* extern functions (inwasm/generator.c)                                   */
/***************************************************************************/
extern void parse_input_file();
extern void generate();
extern void generate_code();
extern int generate_var_enum_predsym();

/***************************************************************************/
/* extern functions (inwasm/init.c)                                        */
/***************************************************************************/
extern symbollist *new_symboll();
extern termtype *init_term();

/***************************************************************************/
/* extern functions (inwasm/interface.c)                                   */
/***************************************************************************/
extern void enter_interface();

/***************************************************************************/
/* extern functions (inwasm/lopout.c)                                      */
/***************************************************************************/
extern void lop_output();
extern void print_lop_formel();
extern void print_lop_term();
extern void print_lop_constr();
extern void print_lop_clause();
extern void print_lop_module();
extern void print_lop_pred();

/***************************************************************************/
/* extern functions (inwasm/newunify.c)                                    */
/***************************************************************************/
extern void newrearrange();
extern termtype *newderef();
extern int newunify();
extern int newmakeref();
extern void high_rename(); 

/***************************************************************************/
/* extern functions (inwasm/output.c)                                      */
/***************************************************************************/
extern void nferror();
extern void warning();
extern void fatal();

/***************************************************************************/
/* extern functions (inwasm/symb.c)                                        */
/***************************************************************************/
extern void count_codenr();
extern int enter_built();
extern char *get_name();
extern void enter_occ();
extern int enter_name();               
extern int enter_token();              
extern int  get_codenr();

/***************************************************************************/
/* extern functions (inwasm/unify.c)                                       */
/***************************************************************************/
extern void init_trail();	  	          
extern int unify();
extern void rearrange();
extern termtype *deref();

/***************************************************************************/
/* extern functions (inwasm/wunif.c)                                       */
/***************************************************************************/
extern void genwunif();
extern void print_wunif();
extern void map_clause_number();

/***************************************************************************/
/* extern functions (inwasm/yy.tab.c)                                      */
/***************************************************************************/
extern int yyparse();

/***************************************************************************/
/*** PREPROC *** PREPROC *** PREPROC *** PREPROC *** PREPROC *** PREPROC ***/
/***************************************************************************/

/***************************************************************************/
/* extern functions (inwasm/preproc/cl_reordering.c)                       */
/***************************************************************************/
extern int make_sortarray();
extern void orbranch_reorder();
extern void gen_subgoal_weights();
extern int nr_var_in_lit();
extern float KriteriumEinesPraedikates();

/***************************************************************************/
/* extern functions (inwasm/preproc/connopt.c)                             */
/***************************************************************************/
extern void opt_connections();
extern termtype *replace_in_term2();
extern int pos_lits();
extern claustype *do_resolution();

/***************************************************************************/
/* extern functions (inwasm/preproc/constrsu.c)                            */
/***************************************************************************/
extern void gen_constr_subs();
extern int nr_bindings();
extern int testsubs();

/***************************************************************************/
/* extern functions (inwasm/preproc/hlpfkt.c)                              */
/***************************************************************************/
extern termtype *copy_terms();         
extern predtype *ErzeugeConstr();   
extern int FuegeConstraintHinzu();   
extern predtype *GenerateConstraint();	 

/***************************************************************************/
/* extern functions (inwasm/preproc/oconstr.c)                             */
/***************************************************************************/
extern claustype *gen_overl_constr();
extern predtype *get_red_pred();  
extern termtype *replace_in_term();
extern symbollist *get_all_var();

/***************************************************************************/
/* extern functions (inwasm/preproc/purity.c)                              */
/***************************************************************************/
extern int  genpurity(); 
extern void delete_p();                  
extern void del_occlist();             	    

/***************************************************************************/
/* extern functions (inwasm/preproc/sconstr.c)                             */
/***************************************************************************/
extern claustype *gen_s_constr();
extern int clause_s_constr(); 
extern symbollist *app_sym_to_list();
extern void make_s_list(); 
extern int contains_constraint_builtin();

/***************************************************************************/
/* extern functions (inwasm/preproc/sg_reodering.c)                        */
/***************************************************************************/
extern void sg_reordering();

/***************************************************************************/
/* extern functions (inwasm/preproc/symconstr.c)                           */
/***************************************************************************/
extern void gen_sym_constr();
extern void make_sym_s_list(); 

/***************************************************************************/
/* extern functions (inwasm/preproc/tconstr.c)                             */
/***************************************************************************/
extern int FindeTautologie();         
extern claustype *gen_taut_constr();
            		
/***************************************************************************/
/*** CODEGEN *** CODEGEN *** CODEGEN *** CODEGEN *** CODEGEN *** CODEGEN ***/
/***************************************************************************/

/***************************************************************************/
/* extern functions (inwasm/codegen/cgarith.c)                             */
/***************************************************************************/
extern int cga_ev();

/***************************************************************************/
/* extern functions (inwasm/codegen/cgen.c)                                */
/***************************************************************************/
extern void gencode();		     

/***************************************************************************/
/* extern functions (inwasm/codegen/cgconstr.c)                            */
/***************************************************************************/
extern void cg_constrainterms();
extern void cg_constraints();
extern void cg_genregs();

/***************************************************************************/
/* extern functions (inwasm/codegen/cutil.c)                               */
/***************************************************************************/
extern int do_it();
extern void cg_graph();
extern int  get_copy_bound();
extern int  adr_var();
extern void cg_argvectors();
extern void cg_struct();
extern void tl_gen_av();
extern void cg_cpybd();
extern void cg_unifs();
extern void cg_goals();
extern void cg_queryvar();
extern void cg_queryvarlab();
extern void cg_copy();
extern void cg_gen_subgoal_list();
extern void cg_red();
extern void cg_failure_fact();
extern void cg_randomfact();

/***************************************************************************/
/* extern functions (inwasm/codegen/dumpst.c)                              */
/***************************************************************************/
extern void cg_symbtab();

/***************************************************************************/
/* extern functions (inwasm/codegen/instrs.c)                              */
/***************************************************************************/
extern void cgin_finish();
extern void cgin_nextcoment();
extern void cgin_clause();
extern void new_cllab();
extern void cgin_constr();
extern void cgin_cvlab();
extern void cgin_genregx();
extern void cgin_genregu();
extern void cgin_genreg();
extern void cgin_glod();
extern void cgin_lconst();
extern void cgin_lod();
extern void cgin_sub();
extern void cgin_add();
extern void cgin_mul();
extern void cgin_div();
extern void cgin_lconst();
extern void cgin_assign();
extern void cgin_jmpg_blab();
extern void cgin_blab();
extern void cgin_jmpz_blab();
extern void cgin_setdepth();
extern void cgin_gsto();
extern void cgin_sto();
extern void cgin_eqpred();
extern void cgin_prepare_ndbuilt();
extern void cgin_gen_built_in_noargs();
extern void cgin_gen_built_in_args();
extern void cgin_b_gen_lcb_t();
extern void cgin_alloc();
extern void cgin_trymode_fact();
extern void cgin_exec_ps_fact();
extern void cgin_dwfcb();
extern void cgin_save_clause();
extern void cgin_save_fact();
extern void cgin_dealloc();
extern void cgin_init();
extern void cgin_foldup();
extern void cgin_foldupx();
extern void cgin_subgoal_list_label();
extern void cgin_cpset();
extern void cgin_galloc();
extern void cgin_loadfcb();
extern void cgin_disptree();
extern void cgin_label_ortree();
extern void cgin_fail();
extern void cgin_stop();
extern void cgin_coment();
extern void cgin_orlab();
extern void cgin_orbr();
extern void cgin_por();
extern void cgin_opt();
extern void cgin_lab();
extern void cgin_rand_reord_label();
extern void cgin_trymode_label();
extern void cgin_rdw();
extern void cgin_lcbdw();
extern void cgin_exec_ps_label();
extern void cgin_noopt();
extern void cgin_unvoid();
extern void cgin_unvar();
extern void cgin_ungterm();
extern void cgin_unngterm();
extern void cgin_unconst();
extern void cgin_unglobl();
extern void cgin_lcblab();
extern void cgin_avlab();
extern void cgin_typed();
extern void cgin_dwterm();
extern void cgin_ldw();
extern void cgin_fail_label();
extern void cgin_avdw();
extern void cgin_subgoal_list_dw();
extern void cgin_dw();
extern void cgin_ordw();
extern void cgin_termlab();
extern void cgin_call();
extern void cgin_dred();
extern void cgin_rlab2();
extern void cgin_ureduct();
extern void cgin_reduct();
extern void cgin_save_reduct();
extern void cgin_proceed();
extern void cgin_rlab();
extern void cgin_rdw2();
extern void cgin_fail_clause();
extern void cgin_rand_reord_clause();
extern void cgin_cpybd();
extern void cgin_label();
extern void cgin_qout();
extern void cgin_qvlab();
extern void cgin_sgl_lcbdw();

/***************************************************************************/
/* extern functions (inwasm/codegen/hexout.c)                              */
/***************************************************************************/
extern void hex_out_init();
extern void hex_out_finish();
extern void hex_out_enter_label();
extern void hex_out_put_word_tagged();
extern void hex_out_use_label_tagged();
extern void hex_out_put_word();
extern void hex_out_optimize();
extern void hex_out_start_put_symbol();
extern void hex_out_put_symbol();
extern void hex_out_finish_put_symbol();

/***************************************************************************/
/*** WASM *** WASM *** WASM *** WASM *** WASM *** WASM *** WASM *** WASM ***/
/***************************************************************************/

/***************************************************************************/
/* extern functions (wasm/optim.c)                                         */
/***************************************************************************/
extern void list2tree();
extern int init_optim();
extern void o_name();
extern void enter_o_list();
extern void print_dtree(); 

#endif
