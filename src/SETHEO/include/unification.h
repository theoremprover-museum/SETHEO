/*#########################################################################
# File:			unification.h
# SCCS-Info:    11/23/92, @(#)unification.h	1.1
# Author:		C. Mueller
# Date:			10.11.92
# Context:		setheo (SETHEO)
#
# Contents:		defines Macros for unification and protocol unification
#
# Modifications:
#	(when)		(who)		(what)
#	11.3.93	Mueller,Goller neue Constraints, Statistics
#	29.6.93		jsc		include-defines
#	1.6.94		jsc		UNIFY-macro
#
# Bugs:
# 		--- Forschungsgruppe KI --- Intellektik ---
# Lehrstuhl fuer Rechnerarchitektur, Inst. fuer Informatik, TU Muenchen 
#########################################################################*/

#ifndef UNIFICATION_H
#define UNIFICATION_H

#include <stdio.h>
#include "deref.h"
#include "univ_constr.h"
#include "unifyt.h"
#include "full.h"

/* Constants must exist permanently for the constraint-check */

#ifdef CONSTRAINTS

#  ifdef COPY_CONST
#define CONSTRAINT_CHECK(subconstr, arg2)\
*hp++ = *arg2;\
if (!c_check(subconstr, hp-1))\
	FAIL_U
#  else
#define CONSTRAINT_CHECK(subconstr, arg2)\
if (!c_check(subconstr, arg2))\
	FAIL_U
#  endif

#else
#define CONSTRAINT_CHECK(subconstr, arg2)  

#endif



#define UN_CONST(harg, hbp, garg, gbp)\
\
			switch (GETTAG(*garg)) {\
\
				case T_CONST:\
\
					if (!EQUAL_WORD2(*garg,*harg))\
						UNIF_FAIL("CONST: diff const")\
					break;\
\
\
				case T_FVAR:\
\
					CONSTRAINT_CHECK(garg, harg)\
					trail_var (garg);\
					*garg = *harg;\
					break;\
\
\
				case T_GLOBL:\
\
					trail_var (garg);\
					*garg = *harg;\
					break;\
\
\
				default:\
					UNIF_FAIL("CONST: default")\
					break;\
\
				}	

/* end of switch garg within case harg == T_CONST */





#define UN_GLOBL(harg, hbp, garg, gbp)\
\
			switch (GETTAG(*garg)) {\
\
				case T_CONST:\
\
					trail_var (harg);\
					*harg = *garg;\
					break;\
\
\
				case T_FVAR:\
\
					trail_var (garg);\
					GENOBJ (*hp, 0, T_FVAR); \
					GENPTR (*garg, hp);\
					trail_var (harg);   \
					GENPTR (*harg, hp); \
					hp++;\
					break;\
\
\
				case T_GTERM:\
\
					trail_var (harg);\
					GENPTR (*harg, garg);\
					break;\
\
\
				case T_NGTERM:\
\
					garg = copy_struct (garg, gbp, NULL);\
					if (!garg)\
						UNIF_FAIL("GLOBAL: !garg")\
					trail_var (harg);\
					GENPTR (*harg, garg);\
					break;\
\
\
				case T_CRTERM:\
\
					trail_var (harg);\
					GENPTR (*harg, garg);\
					break;\
\
\
				case T_GLOBL:\
\
					trail_var (garg);\
					GENOBJ (*hp, 0, T_FVAR);\
					GENPTR (*garg, hp);\
					trail_var (harg);   \
					GENPTR (*harg, hp);    \
					hp++;\
					break;\
\
\
				default:\
					UNIF_FAIL("GLOBAL: default")\
					break;\
\
				}	

/* end of switch garg within case harg == T_GLOBL */





#define UN_FVAR(harg, hbp, garg, gbp)\
\
			switch (GETTAG(*garg)) {\
\
				case T_CONST:\
\
					CONSTRAINT_CHECK(harg, garg)\
					trail_var (harg);\
					*harg = *garg;\
					break;\
\
\
				case T_FVAR:\
\
					if (harg == garg)\
						break;\
\
					if (harg > garg) {\
						CONSTRAINT_CHECK(harg, garg)\
						trail_var (harg);\
						GENPTR (*harg, garg);\
						}\
\
					else {\
						CONSTRAINT_CHECK(garg, harg)\
						trail_var(garg);\
						GENPTR (*garg, harg);\
						}\
\
					break;\
\
\
				case T_CRTERM:\
\
					if (occ_ck (harg, garg, gbp))\
						UNIF_FAIL("FVAR: !occ_ck")\
					CONSTRAINT_CHECK(harg, garg)\
					trail_var (harg);\
					GENPTR (*harg, garg);\
					break;\
\
\
				case T_GTERM:\
\
					CONSTRAINT_CHECK(harg, garg)\
					trail_var (harg); \
					GENPTR (*harg, garg);\
					break;\
\
\
				case T_NGTERM:\
\
					garg = copy_struct (garg, gbp, harg);\
					if (!garg)\
						UNIF_FAIL("FVAR: !garg")\
					CONSTRAINT_CHECK(harg, garg)\
					trail_var (harg); \
					GENPTR (*harg, garg);\
					break;\
\
\
				case T_GLOBL:\
\
					trail_var (garg);\
					GENOBJ (*hp,   0, T_FVAR);\
					GENPTR (*garg, hp);\
					trail_var (harg);   \
					GENPTR (*harg, hp);    \
					hp++;\
					break;\
\
\
				default:\
					UNIF_FAIL("FVAR: default")\
					break;\
\
				}
/* end of switch garg within case harg == T_FVAR */





#define UN_FVAR_NO_CONSTR(harg, hbp, garg, gbp)\
\
			switch (GETTAG(*garg)) {\
\
				case T_CONST:\
\
					trail_var (harg);\
					*harg = *garg;\
					break;\
\
\
				case T_FVAR:\
\
					if (harg == garg)\
						break;\
\
					if (harg > garg) {\
						trail_var (harg);\
						GENPTR (*harg, garg);\
						}\
\
					else {\
						trail_var(garg);\
						GENPTR (*garg, harg);\
						}\
\
					break;\
\
\
				case T_CRTERM:\
\
					if (occ_ck (harg, garg, gbp))\
						UNIF_FAIL("FVAR_NO: !occ_ck")\
					trail_var (harg);\
					GENPTR (*harg, garg);\
					break;\
\
\
				case T_GTERM:\
\
					trail_var (harg); \
					GENPTR (*harg, garg);\
					break;\
\
\
				case T_NGTERM:\
\
					garg = copy_struct (garg, gbp, harg);\
					if (!garg)\
						UNIF_FAIL("FVAR_NO: !garg")\
					trail_var (harg); \
					GENPTR (*harg, garg);\
					break;\
\
\
				case T_GLOBL:\
\
					trail_var (garg);\
					GENOBJ (*hp, 0, T_FVAR);\
					GENPTR (*garg, hp);\
					trail_var (harg);   \
					GENPTR (*harg, hp);    \
					hp++;\
					break;\
\
\
				default:\
					UNIF_FAIL("FVAR_NO: default")\
					break;\
\
				}
/* end of switch garg within case harg == T_FVAR (no constraint_check) */





#define UN_CRTERM(harg, hbp, garg, gbp)\
\
			switch(GETTAG(*garg)) {\
\
				case T_FVAR:\
\
					if (occ_ck (garg, harg, hbp))\
					UNIF_FAIL("CRTERM: FVAR")\
					CONSTRAINT_CHECK(garg, harg)\
					trail_var (garg);\
					GENPTR (*garg, harg);\
					break;\
\
\
				case T_CRTERM:\
				case T_GTERM:\
				case T_NGTERM:\
					if (full_unify (garg, harg, gbp, hbp))\
						UNIF_FAIL("CRTERM: *TERM")\
					break;\
\
\
				case T_GLOBL:\
\
					trail_var (garg);\
					GENPTR (*garg, harg);\
					break;\
\
\
				default:\
					UNIF_FAIL("CRTERM: default")\
					break;\
\
				}		
/* end of switch garg within case harg == T_CRTERM */






#define UN_GTERM(harg, hbp, garg, gbp)\
\
			switch(GETTAG(*garg)) {\
\
				case T_FVAR:\
\
					CONSTRAINT_CHECK(garg, harg)\
					trail_var (garg);\
					GENPTR (*garg, harg);\
					break;\
\
\
				case T_CRTERM:\
				case T_GTERM:\
				case T_NGTERM:\
					if (full_unify (garg, harg, gbp, hbp))\
						UNIF_FAIL("GTERM: *TERM")\
					break;\
\
\
				case T_GLOBL:\
\
					trail_var (garg);\
					GENPTR (*garg, harg);\
					break;\
\
\
				default:\
					UNIF_FAIL("GTERM: default")\
					break;\
\
				}	
/* end of switch garg within case harg == T_GTERM */




#define UN_NGTERM(harg, hbp, garg, gbp)\
\
			switch(GETTAG(*garg)) {\
\
				case T_FVAR:\
\
					harg = copy_struct (harg, hbp, garg);\
					if (!harg)\
						UNIF_FAIL("NGTERM: FVAR")\
					CONSTRAINT_CHECK(garg, harg)\
					trail_var (garg);\
					GENPTR (*garg, harg);\
					break;\
\
\
				case T_CRTERM:\
				case T_GTERM:\
				case T_NGTERM:\
\
					if (full_unify (garg, harg, gbp, hbp))\
						UNIF_FAIL("NGTERM: *TERM")\
					break;\
\
\
				case T_GLOBL:\
\
					harg = copy_struct (harg, hbp, NULL);\
					if (!harg)\
						UNIF_FAIL("NGTERM: GOBL")\
					trail_var (garg);\
					GENPTR (*garg, harg);\
					break;\
\
\
				default:\
					UNIF_FAIL("NGTERM: default")\
					break;\
\
				}
/* end of switch garg within case harg == T_NGTERM */

/* UNIFY: unify two terms wrt. to base-pointers */
#define UNIFY(harg,bp1,garg,bp2) \
	switch(GETTAG(*harg)) {				\
    case T_CONST:					\
        UN_CONST(harg, bp1, garg, bp2)			\
        break;						\
    case T_FVAR:					\
        UN_FVAR(harg, bp1, garg, bp2)			\
        break;						\
    case T_CRTERM:					\
        UN_CRTERM(harg, bp1, garg, bp2)			\
        break;						\
    case T_GTERM:					\
        UN_GTERM(harg, bp1, garg, bp2)			\
        break;						\
    case T_NGTERM:					\
        UN_NGTERM(harg, bp1, garg, bp2)			\
        break;						\
    case T_GLOBL:					\
        UN_GLOBL(harg, bp1, garg, bp2)			\
        break;						\
    default:						\
        FAIL_U;						\
    }						
  
#endif
