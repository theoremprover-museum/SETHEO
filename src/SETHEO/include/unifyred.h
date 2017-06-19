/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: unifyred.h
/* VERSION:This is a modification of unify.h, adapted for a call from unifred
/*         The tag T_GLOBL for GA is erased, because globals in NH-formula
/*         are not supported by the current version of SETHEO. All combinations
/*         with tag T_NGTERM for the head have been added, because they are
/*         necessary for reduction-steps. ( see bug redngt2.lop )
/* DATE: 2.5.88
/* AUTHOR: J. Schumann
/* NAME OF FILE: 
/* DESCR: 
/*	definition of unify
/* MOD: 
/* 	10.5.88 occur check
/*	2.11.88   debugging
/*	7.11.88		global variables
/*	30.11. no macro
/*	31.1.89 (char)
/*      24.6.91 Goller
/*      24.01.92 CONSTRAINTS added by bschorer
/*      06.03.92 restyled by bschorer
/*      10.03.92 structure-variables added by bschorer
/*	11.3.93	now obsolete
/* BUGS: 
/* REMARKS:
/******************************************************/
