/***************************************************************/
/*    S E T H E O                                              */
/*                                                             */
/* FILE: conf.c                                                */
/* VERSION:                                                    */
/* DATE: 3.5.90                                                */
/* AUTHOR: J. Schumann                                         */
/* NAME OF FILE:                                               */
/* DESCR:                                                      */
/* MOD:  built 27,28,29,30 added for Breiteneicher      Goller */
/*       built 11 and 26 are dummies for later use             */
/*	02.09.92 restructuring                                 */
/*	16.9.93:	genlemma, uselemma, dumplemma          */
/*  17.9.93:	tell, told                                     */
/*	24.3.94		genreg* arguments                      */
/*	9.3.95		no setsubg                             */
/*                                                             */
/* BUGS:                                                       */
/***************************************************************/
#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "conf.h"

/***************************************************************/
/* identification name of that SETHEO version                  */
/***************************************************************/
char           *ident = "4.0[LINUX,PIL]";
char           *version = "June 1999";
int		magic = 123;

/***************************************************************/
/* global list of all SETHEO instructions                      */
/***************************************************************/
instr_tab	instr_table[] =    {

/*------------------------------------------------------------*/
/* function		name		length                */
/*------------------------------------------------------------*/
{  i_alloc		,"alloc"	,2		},
{  i_nalloc		,"nalloc"	,2		},
{  i_dealloc		,"dealloc"	,1		},
{  i_ndealloc		,"ndealloc"	,1		},
{  i_fold_up		,"fold_up"	,1		},
{  i_reg_fold_up	,"reg_fold_up"	,1		},
/*------------------------------------------------------------*/
{  i_call		,"call"		,1		},
{  i_stop		,"stop"		,0		},
{  i_break		,"break"	,1		},
{  i_jmp		,"jmp"		,2		},
{  i_jmpz		,"jmpz"		,2		},
{  i_jmpg		,"jmpg"		,2		},
/*------------------------------------------------------------*/
{  i_unvoid		,"unvoid"	,2		},
{  i_unconst		,"unconst"	,2		},
{  i_unvar		,"unvar"	,2		},
{  i_ungterm		,"ungterm"	,2		},
{  i_unngterm		,"unngterm"	,2		},
{  i_unglobl		,"unglobl"	,2		},
/*------------------------------------------------------------*/
{  i_orbranch		,"orbranch"	,2		},
{  i_porbranch		,"porbranch"	,4		},
{  i_fail		,"fail"		,1		},
/*------------------------------------------------------------*/
{  i_reduct		,"reduct"	,3		},
{  i_ureduct		,"ureduct"	,3		},
{  i_proceed		,"proceed"	,1		},
/*------------------------------------------------------------*/
{  i_add		,"add"		,1		},
{  i_mul		,"mul"		,1		},
{  i_sub		,"sub"		,1		},
{  i_div		,"div"		,1		},
{  i_lod		,"lod"		,2		},
{  i_sto		,"sto"		,2		},
{  i_glod		,"glod"		,2		},
{  i_gsto		,"gsto"		,2		},
{  i_lconst		,"lconst"	,2		},
{  i_assign		,"assign"	,3		},
/*------------------------------------------------------------*/
{  i_genreg		,"genreg"	,2		},
{  i_genxreg		,"genxreg"	,2		},
{  i_genureg		,"genureg"	,2		},
{  i_cgen		,"cgen"		,2		},
/*------------------------------------------------------------*/
{  i_ptree		,"printtree"	,1		},
/*------------------------------------------------------------*/
{  i_out		,"out"		,2		},
{  i_tell		,"tell"		,2		},
{  i_told		,"told"		,1		},
/*------------------------------------------------------------*/
{  i_eqpred		,"eqpred"	,4		},
/*------------------------------------------------------------*/
{  i_eq_built		,"eq_built"	,2		},
{  i_neq_built		,"neq_built"	,2		},
{  i_equ_unif		,"equ_unif"	,2		},
{  i_isunifiable    	,"is_unifiable" ,2              },
{  i_isnotunifiable 	,"is_notunifiable" ,2           },
/*------------------------------------------------------------*/
{  i_pre_cut		,"pre_cut"	,1		},
{  i_cut		,"cut"		,1		},
/*------------------------------------------------------------*/
{  i_is_var		,"is_var"	,2		},
{  i_is_nonvar		,"isnon_var"	,2		},
{  i_is_number		,"is_number"	,2		},
{  i_is_const		,"is_const"	,2		},
{  i_is_compl       	,"is_compl"     ,2              },
/*------------------------------------------------------------*/
{  i_rand_reorder	,"rand_reorder"	,1		},
{  i_setrand		,"setrand"	,2		},
/*------------------------------------------------------------*/
{  i_galloc		,"galloc"	,2		},
/*------------------------------------------------------------*/
{  i_size		,"size"		,2		},
{  i_tdepth		,"tdepth"	,2		},
{  i_tdepth3		,"tdepth3"	,2		},
/*------------------------------------------------------------*/
{  i_getdepth		,"getdepth"	,2		},
{  i_getinf		,"getinf"	,2		},
{  i_getmaxinf		,"getmaxinf"	,2		},
{  i_getlocinf		,"getlocinf"	,2		},
{  i_set_depth		,"set_depth"	,2		},
{  i_set_inf		,"set_inf"	,2		},
{  i_set_maxinf		,"set_maxinf"	,2		},
{  i_set_locinf		,"set_locinf"	,2		},
/*------------------------------------------------------------*/
{  i_functor		,"functor"	,2		},
{  i_arg		,"arg"		,2		},
/*------------------------------------------------------------*/
{  i_genlemma		,"genlemma"	,2		},
{  i_genulemma		,"genulemma"	,3		},
{  i_checklemma		,"checklemma"	,2		},
{  i_checkulemma	,"checkulemma"	,3		},
{  i_addtoindex		,"addtoindex"	,2		},
{  i_checkindex		,"checkindex"	,2		},
{  i_uselemma		,"uselemma"	,2		},
{  i_dumplemma		,"dumplemma"	,1		},
{  i_getnrlemmata	,"getnrlemmata"	,2		},
{  i_delrange		,"delrange"	,2		},
{  i_path2list		,"path2list"	,2		},
{  i_assume		,"assume"	,1		},
/*------------------------------------------------------------*/
{  i_init_counters      ,"init_counters" ,2             },
{  i_getcounter         ,"getcounter"   ,2              },
{  i_set_counter        ,"set_counter"  ,2              },
{  i_get_maxfvars       ,"get_maxfvars" ,2              },
{  i_set_maxfvars       ,"set_maxfvars" ,2              },
{  i_get_maxtc          ,"get_maxtc"    ,2              },
{  i_set_maxtc          ,"set_maxtc"    ,2              },
{  i_get_maxsgs         ,"get_maxsgs"   ,2              },
{  i_set_maxsgs         ,"set_maxsgs"   ,2              },
/*------------------------------------------------------------*/
{  i_lpo                ,"lpo"          ,2              },
{  i_lpoeq              ,"lpoeq"        ,2              },
{  i_kbo                ,"kbo"          ,2              },
{  i_sig                ,"sig"          ,2              },
/*------------------------------------------------------------*/
/* Instruction files used by Sptheo - BEGIN */
/*------------------------------------------------------------*/
{  i_precall		,"precall"	,1		},
{  i_isempty		,"isempty"	,2		},
{  i_issubset		,"issubset"	,2		},
{  i_ismember		,"ismember"	,2		},
{  i_card		,"card"		,2		},
{  i_mkempty		,"mkempty"	,2		},
{  i_addtoset		,"addtoset"	,2		},
{  i_remfromset		,"remfromset"	,2		},
{  i_selnth		,"selnth"	,2		},
{  i_selnext		,"selnext"	,2		},
{  i_postunify		,"postunify"	,2		},
{  i_argsize		,"argsize"	,3		},
{  i_preunify		,"preunify"	,2		},
{  i_array_assign	,"array_assign"	,2		},
{  i_array_glod		,"array_glod"	,2		},
{  i_array_gsto		,"array_gsto"	,2		},
/*------------------------------------------------------------*/
/* Instruction files used by Sptheo - END */
/*------------------------------------------------------------*/
};

int nr_instr = sizeof(instr_table)/sizeof(instr_tab);
