/*######################################################################
# File:			built_ins.c
# Author:		J.Breiteneicher
# Date:			15.5.91
# Context:		inwasm (SETHEO)
# SCCS:			@(#)built_ins.c	18.5  06/22/98
# 
# Contents:		enter built-ins in symboltable
#
# Modifications:	
#	(when)		(who)		(what)
#	25.3.93		jsc		do not generate built,No
#					for eq,neq,is*,
#	4.6.93		jsc	new structure for built-ins
#				libraries are not supported
#				any more
#	2.7.93		jsc	extern.h
#	21.7.93		jsc	debugging
#	23.7.93		jsc	built-ins from goller
#	17.9.93		jsc	genlemma,dumplemma,tell,told
#	24.3.94		jsc	precut,cut
#	07.6.94		jsc	*lemma
#	22.1.96		jsc	hex-out
#
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
########################################################################*/

#include "types.h"
#include "extern.h"
#include "opcodes.h"
#include "uopcodes.h"


/*************************************************************************/
/* built-in function definitions                                         */
/*   declared in codegen/cgbuilt.c                                       */
/*************************************************************************/
extern int b_generic();
extern int b_assign();
extern int b_greater(),b_less();
extern int b_greateq(),b_lesseq();
extern int b_globeval(), b_eval();
extern int b_eqpred();
extern int b_error();
extern int b_gen_lcb_t();

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ enter_built_ins()
+ - enter predefined symbols into symbol table
+
+ special remarks:
+ - called from : init_symb
+ - calls: enter_built(token,type,arity,built-in-function)
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void enter_built_ins()
{
	/* QUERY should be the first one   */

  /***************************************************************************/
  /* The following call-table has the parameters:                            */
  /*  Printname, Type, Type of built_in, arity, code-func, name of SAM instr.*/
  /***************************************************************************/
  /*****************************************************************/
  /* artificial query                                              */
  /*****************************************************************/
enter_built(QUERY_PRTNA,	PREDSYM,
		NONE,	0,	b_generic,	"stop");
/*****************************************************************/
/* constants and operators !! do not change order!               */
/*****************************************************************/
enter_built(LIST_HEAD,		CONSTANT,
		NONE,	2,	b_error,"",0);   	  /* HEAD_SYMB 1 */
enter_built(NIL,		CONSTANT,
		NONE,	0,	b_error,"",0);   	  /* NIL_SYMB  2 */

enter_built(PLUS,		CONSTANT,
		NONE,	2,	b_error,"",0);   	  /* PLUS_SYMB  3 */
enter_built(MINUS,		CONSTANT,
		NONE,	2,	b_error,"",0);   	  /* MINUS_SYMB 4 */
enter_built(MULT,		CONSTANT,
		NONE,	2,	b_error,"",0);	  /* MULT_SYMB  5 */
enter_built(DIV,		CONSTANT,
		NONE,	2,	b_error,"",0);   	  /* DIV_SYMB   6 */
enter_built(UMINUS,		CONSTANT,
		NONE,	1,	b_error,"",0);   	  /* UMI_SYMB   7 */

enter_built("$Copies",		GVARIABLE,
		NONE,	0,	b_error,"",0); 	  /* COPI_SYMB  8 */
enter_built("$Depth",		GVARIABLE,
		NONE,	0,	b_error,"",0);  	  /* DEPT_SYMB  9 */
 enter_built("$Inf",		GVARIABLE,
 		NONE,	0,	b_error,"",0);  	  /* INF_SYMB  10 */

enter_built(ASSIGN_PRTNA,	BUILTIN,
		INTERNAL,2,	b_assign,"",0);	   /* ASSI_SYMB 11 */
enter_built(EVASSIGN_PRTNA,	BUILTIN,
		INTERNAL,2,	b_globeval,"",0);  	/* EVAS_SYMB 12 */
enter_built(IS_PRTNA,		BUILTIN,
		INTERNAL,2,	b_eval,"",0);    	   /* IS_SYMB   13 */
enter_built(UNIF_PRTNA,		BUILTIN,
		INTERNAL,2,	b_generic,"unify_built",
					OPCODE_unify_built); /* UNIF_SYMB 14 */
enter_built("$mkglob",		BUILTIN,
		INTERNAL,2	,b_error,"",0);  	   /* MKGLOB_SYMB15*/

enter_built(GREATER_PRTNA,	BUILTIN,
		INTERNAL,2,	b_greater,"",0);   	/* GREA_SYMB 16 */
enter_built(LESS_PRTNA,		BUILTIN,
		INTERNAL,2,	b_less,"",0);   	   /* LESS_SYMB 17 */
enter_built(LESSEQU_PRTNA,	BUILTIN,
		INTERNAL,2,	b_lesseq,"",0);   /* LEQU_SYMB 18 */
enter_built(GREAEQU_PRTNA,	BUILTIN,
		INTERNAL,2,	b_greateq,"",0);   /* GRQU_SYMB 19 */
enter_built(EQAL_PRTNA,		BUILTIN,
		INTERNAL,2,	b_generic,"eq_built",
				OPCODE_eq_built);	   /* EQAL_SYMB 20 */
enter_built(NQAL_PRTNA,		BUILTIN,
		INTERNAL,2,	b_generic,"neq_built",
				OPCODE_neq_built);	   /* NQAL_SYMB 21 */

enter_built(NOTEQU_CONSTR,	BUILTIN,
		INTERNAL,2,	b_error,"",0);	

/* Begin: SEMANTIC */
/*****************************************************************
 * semantic constraints !! do not change order!
 *****************************************************************/
enter_built(SEM_AND,		CONSTANT,
		NONE,	2,	b_error,"",0);   	   /* AND_SYMB 23 */
enter_built(SEM_OR,		CONSTANT,
		NONE,	2,	b_error,"",0);   	   /* OR_SYMB 24 */
enter_built("$equconstr",       BUILTIN,
        DETERMINISTIC,  2,      b_generic,      "equconstr",
                                OPCODE_equconstr);         /* EQUCONSTR 25 */
enter_built("$disconstr",       BUILTIN,
        DETERMINISTIC,  2,      b_generic,      "disconstr",
                                OPCODE_disconstr);         /* DISCONSTR 26 */
enter_built("$conjconstr",      BUILTIN,
        DETERMINISTIC,  2,      b_generic,      "conjconstr",
                                OPCODE_conjconstr);        /* CONJCONSTR 27 */
/* End: SEMANTIC */    

/*****************************************************************/
/* ``low-levels''                                                */
/*****************************************************************/
enter_built("$monitor",		BUILTIN,
	DETERMINISTIC,	0,	b_generic,	"break",
				OPCODE_break);   	
enter_built("$eqpred",		BUILTIN,
	DETERMINISTIC,	1,	b_eqpred,	"",
				OPCODE_eqpred);   	
enter_built("$write",		BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"out",
				OPCODE_out);   	
enter_built("$fail",		BUILTIN,
	DETERMINISTIC,	0,	b_generic,	"fail",
				OPCODE_fail);   	
enter_built("$stop",		BUILTIN,
	DETERMINISTIC,	0,	b_generic,	"stop",
				OPCODE_stop);   	

enter_built("$printtree",	BUILTIN,
	DETERMINISTIC,	0,	b_generic,	"printtree",
				OPCODE_printtree);   	

/*****************************************************************/
/* = (syntactic) and =/=                                         */
/*****************************************************************/
enter_built("$eq",		BUILTIN,
	DETERMINISTIC,	2,	b_generic,	"eq_built",
				OPCODE_eq_built);   	
enter_built("$neq",		BUILTIN,
	DETERMINISTIC,	2,	b_generic,	"neq_built",
				OPCODE_neq_built);   	

/*****************************************************************/
/* checks for data types                                         */
/*****************************************************************/
enter_built("$isvar",		BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"is_var",
				OPCODE_is_var);   	
enter_built("$isnonvar",		BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"isnon_var",
				OPCODE_isnon_var);   	
enter_built("$isconst",		BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"is_const",
				OPCODE_is_const);   	
enter_built("$isnumber",		BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"is_number",
				OPCODE_is_number);   	
enter_built("$iscompl",		BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"is_compl",
				OPCODE_is_compl);   	

/*****************************************************************/
/* unification and check for unifiability                        */
/*****************************************************************/
enter_built("$unify",		BUILTIN,
	DETERMINISTIC,	2,	b_generic,	"equ_unif",
				OPCODE_equ_unif);   	
enter_built("$unifiable",	BUILTIN,
	DETERMINISTIC,	2,	b_generic,	"is_unifiable",
				OPCODE_is_unifiable);   	
enter_built("$notunifiable",	BUILTIN,
	DETERMINISTIC,	2,	b_generic,	"is_notunifiable",
				OPCODE_is_notunifiable);   	

/*****************************************************************/
/* sizes                                                         */
/*****************************************************************/
enter_built("$size",	BUILTIN,
	DETERMINISTIC,	2,	b_generic,	"size",
				OPCODE_size);   	
enter_built("$tdepth",	BUILTIN,
	DETERMINISTIC,	2,	b_generic,	"tdepth",
				OPCODE_tdepth);   	

enter_built("$tdepth3",	BUILTIN,
	DETERMINISTIC,	3,	b_generic,	"tdepth3",
				OPCODE_tdepth3);   	

/*****************************************************************/
/* counters                                                      */
/*****************************************************************/  
enter_built("$initcounters",  BUILTIN,
        DETERMINISTIC,  1,      b_generic,      "init_counters",
				OPCODE_init_counters);
enter_built("$getcounter",    BUILTIN,
	DETERMINISTIC,	2,	b_generic,	"getcounter",
				OPCODE_getcounter);   	
enter_built("$setcounter",    BUILTIN,
	DETERMINISTIC,	2,	b_generic,	"set_counter",
				OPCODE_set_counter); 	

/*****************************************************************/
/* set and get values                                            */
/*****************************************************************/
enter_built("$setrand",	BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"setrand",
				OPCODE_setrand);   	
enter_built("$getdepth",	BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"getdepth",
				OPCODE_getdepth);   	
enter_built("$setdepth",	BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"set_depth",
				OPCODE_set_depth);   	
enter_built("$getinf",	BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"getinf",
				OPCODE_getinf);   	
enter_built("$setinf",	BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"set_inf",
				OPCODE_set_inf);   	
enter_built("$getmaxinf",BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"getmaxinf",
				OPCODE_getmaxinf);   	
enter_built("$setmaxinf",BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"set_maxinf",
				OPCODE_set_maxinf);   	
enter_built("$setlocinf",BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"set_locinf",
				OPCODE_set_locinf);   	
enter_built("$getlocinf",BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"getlocinf",
				OPCODE_getlocinf);   	
enter_built("$functor",		BUILTIN,
	DETERMINISTIC,	3,	b_generic,	"functor",
				OPCODE_functor);   	
enter_built("$arg",		BUILTIN,
	DETERMINISTIC,	3,	b_generic,	"arg",
				OPCODE_arg);   	
enter_built("$negate",		BUILTIN,
	DETERMINISTIC,	2,	b_generic,	"negate",
				OPCODE_negate);
enter_built("$getmaxfvars",   BUILTIN,
        DETERMINISTIC,  1,      b_generic,      "get_maxfvars",
				OPCODE_get_maxfvars);
enter_built("$setmaxfvars",   BUILTIN,
        DETERMINISTIC,  1,      b_generic,      "set_maxfvars",
				OPCODE_set_maxfvars);
enter_built("$getmaxtc",      BUILTIN,
        DETERMINISTIC,  1,      b_generic,      "get_maxtc",
				OPCODE_get_maxtc);
enter_built("$setmaxtc",      BUILTIN,
        DETERMINISTIC,  1,      b_generic,      "set_maxtc",
				OPCODE_set_maxtc);
enter_built("$getmaxsgs",     BUILTIN,
        DETERMINISTIC,  1,      b_generic,      "get_maxsgs",
				OPCODE_get_maxsgs);
enter_built("$setmaxsgs",     BUILTIN,
        DETERMINISTIC,  1,      b_generic,      "set_maxsgs",
				OPCODE_set_maxsgs);
  	 
 	
/*****************************************************************/
/* unit-lemma handling                                           */
/*****************************************************************/
enter_built("$dumplemma",	BUILTIN,
	DETERMINISTIC,	0,	b_generic,	"dumplemma",
				OPCODE_dumplemma);   	
enter_built("$checkindex",	BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"checkindex",
				OPCODE_checkindex);   	
enter_built("$addtoindex",	BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"addtoindex",
				OPCODE_addtoindex);   	
enter_built("$genlemma",		BUILTIN,
	DETERMINISTIC,	2,	b_generic,	"genlemma",
				OPCODE_genlemma);   	
enter_built("$genulemma",	BUILTIN,
	DETERMINISTIC,	3,	b_gen_lcb_t,	"genulemma",
				OPCODE_genulemma);   	
enter_built("$checklemma",	BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"checklemma",
				OPCODE_checklemma);   	
enter_built("$checkulemma",	BUILTIN,
	DETERMINISTIC,	2,	b_gen_lcb_t,	"checkulemma",
				OPCODE_checkulemma);   	
enter_built("$addlemma",		BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"addlemma",
				OPCODE_addlemma);   	
enter_built("$addulemma",	BUILTIN,
	DETERMINISTIC,	2,	b_gen_lcb_t,	"addulemma",
				OPCODE_addulemma);   	
enter_built("$uselemma",		BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"uselemma",
				OPCODE_uselemma);   	
enter_built("$getnrlemmata",	BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"getnrlemmata",
				OPCODE_getnrlemmata);   	
enter_built("$foldupparam",	BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"foldup_par",
				OPCODE_foldup_par);   	
enter_built("$assumehead",	BUILTIN,
	DETERMINISTIC,	0,	b_generic,	"assume_head",
				OPCODE_assume_head);   	
enter_built("$delrange",	BUILTIN,
	DETERMINISTIC,	2,	b_generic,	"delrange",
				OPCODE_delrange);   	  	
enter_built("$path2list",	BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"path2list",
				OPCODE_path2list);
enter_built("$assume",	        BUILTIN,
	DETERMINISTIC,	0,	b_generic,	"assume",
				OPCODE_assume);   	

/*****************************************************************/
/* cuts                                                          */
/*****************************************************************/
enter_built("$precut",           BUILTIN,
        DETERMINISTIC,  0,      b_generic,      "pre_cut",
				OPCODE_pre_cut);
enter_built("$cut",              BUILTIN,
        DETERMINISTIC,  0,      b_generic,      "cut",
				OPCODE_cut);

/*****************************************************************/
/* orderings                                                     */
/*****************************************************************/   	
enter_built("$lpo",	        BUILTIN,
	DETERMINISTIC,	3,	b_generic,	"lpo",
				OPCODE_lpo);     	  	  	
enter_built("@$lpo",	        BUILTIN,
	DETERMINISTIC,	3,	b_generic,	"lpo",
				OPCODE_lpo);     	  	
enter_built("$lpoeq",	        BUILTIN,
	DETERMINISTIC,	3,	b_generic,	"lpoeq",
				OPCODE_lpoeq);
enter_built("@$lpoeq",	        BUILTIN,
	DETERMINISTIC,	3,	b_generic,	"lpoeq",
				OPCODE_lpoeq);
enter_built("$kbo",	        BUILTIN,
	DETERMINISTIC,	3,	b_generic,	"kbo",
				OPCODE_kbo);  
enter_built("@$kbo",	        BUILTIN,
	DETERMINISTIC,	3,	b_generic,	"kbo",
				OPCODE_kbo);  

/*****************************************************************/
/* signature                                                     */
/*****************************************************************/
enter_built("$sig",	        BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"sig",
				OPCODE_sig);  

/*****************************************************************/
/* input-output                                                  */
/*****************************************************************/
enter_built("$tell",		BUILTIN,
	DETERMINISTIC,	1,	b_generic,	"tell",
				OPCODE_tell);   	
/*---------------------------------------------------------------*/
/*           -------------------------------                     */
/*	NOTE: This built-in should be the                        */
/*		LAST                                             */
/*	      in this list, since several                        */
/*	      scripts depend on it!                              */
/*           -------------------------------                     */
/*---------------------------------------------------------------*/
enter_built("$told",		BUILTIN,
	DETERMINISTIC,	0,	b_generic,	"told",
				OPCODE_told);   	
}

/*######################################################################*/
