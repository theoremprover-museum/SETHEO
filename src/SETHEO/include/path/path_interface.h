/*########################################################################
# Include File:		path_interface.h
# SCCS-Info:		@(#)path_interface.h	6.1 20 Sep 1995
# Author:		J. Schumann
# Date:			16.9.93
# Context:		sam
# 
# Contents:		definitions for unit-lemma generation
#
# Modifications:	
#	(when)		(who)		(what)
#
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
########################################################################*/
#ifndef SAM_INTERFACE_
#define SAM_INTERFACE_


#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
/**/
#include "deref.h"

/***************************************************************/
/* Define Types                                                */
/***************************************************************/
#define user__TERM                   WORD *
#define user__ARG                    WORD *
#define user__SYMBOL                 WORD

/***************************************************************/
/* Macros (with data type given)                               */
/***************************************************************/
#define /* SYMBOL:: */ user__SymbolSpecial()          (-1)
#define /* SYMBOL:: */ user__TermTop(T)               (GETVAL(*T))
#define /* BOOL::*/ user__TermIsVariable(T)      (ISVAR(*T))
#define /* BOOL::*/ user__TermIsConstant(T)      (GETTAG(*T)==T_CONST)

/***************************************************************/
/* functions (see sam_interface.c)                             */
/***************************************************************/
/* The next function returns a boolean value which is TRUE if
   all the arguments of a term are variables. */
#define user__TermAllArgsAreVar(T)     term_with_all_variables(T)



/* The last four functions provide the scanning of a terms
   arguments. */

#define /* ARG_LIST:: */ user__ArgList(T) (((GETTAG(*T)==T_CONST)|| \
					  (ISVAR(*T)))?NULL:(T+1))

#define /* ARG_LIST:: */ user__ArgCdr(L)  (L+1)

#define /* TERM:: */ user__ArgCar(L)      (deref(L,bp))
#define /* BOOL:: */ user__ArgEmpty(L)    ((!L)||(GETTAG(*L)==T_EOSTR))

int term_with_all_variables();
/* int term_with_all_variables(WORD* T); */


#endif
