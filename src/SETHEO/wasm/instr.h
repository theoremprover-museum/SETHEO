/*########################################################################
# Include File:		instr.h
# SCCS-Info:		10/30/91, @(#)instr.h	1.1
# Author:		J. Schumann
# Date:			09.01.88
# Context:		wasm (SETHEO)
# 
# Contents:		definitions of instructions
#
# Modifications:	
#	(when)		(who)		(what)
#	9.1.88
#	11.1.88
#	4.3.88 				funct
#	10.3. 				inference bound
#	6.6. 				special proof-tree reproof
#	31.10. 				break
#	7.11.  				global variables
#	20.2.89 			sets and general built ins
#	25.4.89 			global lod & sto
#	11.5.89 			eqpred
#	17.7.89 			nalloc
#	31.7.89 			pexec & read
#	2.8.89  			ndealloc
#       23.7.91 	Goller		CALL with 3 args now 
#	23.10.91	Max Moser	`restyled'
#
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
########################################################################*/


/*------------------------------------------------------------------------
- definition of instructions
------------------------------------------------------------------------*/

#define INSTRMASK       0x0f00

#define CONTROL         0x800
#define UNIFSIMPL       0x400
#define MISC            0x200

#define CALL            1 | CONTROL
#define CALL_LENG       4       	/* has three args now   Goller  */

#define PROCEED         2 | CONTROL
#define PROCEED_LENG    1
#define FAIL            6 | CONTROL
#define FAIL_LENG       1
#define ALLOCATE        7 | CONTROL
#define ALLOC_LENG      4
#define NALLOC          26 | CONTROL
#define NALLOC_LENG     ALLOC_LENG
#define DEALLOC         8 | CONTROL
#define DEALLOC_LENG    1
#define NDEALLOC	27 | CONTROL
#define NDEALLOC_LENG   DEALLOC_LENG
#define STOP            9 | MISC
#define STOP_LENG       1
#define EXEC            10 | CONTROL
#define EXEC_LENG       3
#define UNIF_VOID       11 | UNIFSIMPL
#define UNIF_CONST      12 | UNIFSIMPL
#define UNIF_TV1        13 | UNIFSIMPL
#define UNIF_TV2        14 | UNIFSIMPL
#define UNIF_VAR        15 | UNIFSIMPL
#define UNIF_GTERM      16 | UNIFSIMPL
#define UNIF_NGTERM     17 | UNIFSIMPL
#define UNIFY_LENG      2
#define OUT             16 | MISC
#define OUT_LENG        2
#define IN              17 | MISC
#define IN_LENG         2
#define BUILT           18 | MISC
#define BUILT_LENG      2
#define CPYBOUND        19 | CONTROL
#define CPY_LENG        2
#define OR_BRANCH	20 | CONTROL
#define OR_LENG		2

#define REDUCT          12 | CONTROL
#define REDUCT_LENG     3

#define GENLEM		22 | CONTROL
#define GENLEM_LENGTH	3

#define USELEMMA	23 | CONTROL
#define USELEMMA_LENG	3

#define GENTASK		24 | CONTROL
#define GENTASK_LENG	1

#define RESTART		25 | CONTROL
#define RESTART_LENG	2

#define OR_P_BRANCH     13 | CONTROL
#define OR_P_LENG       4
#define PTREE    	14 | CONTROL
#define PTR_LENG        2

#define DISP		19 | MISC
#define DISP_LENG	3

#define DISPTREE		999 | MISC
#define DISP_LENG	1

/*------------------------------------------------------------------------
- proof-tree instruction to be used for reproof- subst-display
- arg1:	label
- arg2:	pred-symb+sign
- arg3:	coded: clausenr,litnumber,arity
------------------------------------------------------------------------*/

#define DPTREE    	15 | CONTROL
#define DPTR_LENG       4

#define SETCPY		21 | MISC
#define SETCPY_LENG	2
#define SETDEPTH	22 | MISC
#define SETDEPTH_LENG	2

#define ADD		23 | MISC
#define ADD_LENG	1
#define SUB		24 | MISC
#define SUB_LENG	1
#define MUL		25 | MISC
#define MUL_LENG	1
#define LOD		26 | MISC
#define LOD_LENG	2
#define STO		27 | MISC
#define STO_LENG	2
#define LCONST		28 | MISC
#define LCONST_LENG	2
#define JMPZ		29 | MISC
#define JMPZ_LENG	2
#define JMPG		30 | MISC
#define JMPG_LENG	2
#define OUTNU		31 | MISC
#define OUTNU_LENG	2
#define JMP		32 | MISC
#define JMP_LENG	2
#define JMPD		33 | MISC
#define JMPD_LENG	2
#define JMPCP		34 | MISC
#define JMPCP_LENG	2

#define BREAK		35 | MISC
#define BREAK_LENG	1

#define ASSIGN		36 | MISC
#define ASSIGN_LENG	3
#define ALLOCG		37 | MISC
#define ALLOCG_LENG	2

#define GEN_BUILT	38 | MISC
#define GEN_BUILT_LENG	3
#define STO_G		39 | MISC
#define STO_G_LENG	2
#define LOD_G		40 | MISC
#define LOD_G_LENG	2

#define EQPRED		41 | MISC
#define EQPRED_LENG	4

#define PEXEC		42 | MISC
#define PEXEC_LENG	3

#define READ		43 | MISC
#define READ_LENG	2

#define GENREG          44 | MISC
#define GENREG_LENG     3

/*######################################################################*/
