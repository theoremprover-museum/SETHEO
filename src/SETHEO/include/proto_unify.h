/*#########################################################################
# File:			proto_unify.h
# SCCS-Info:		11/23/92, @(#)proto_unify.h	1.1
# Author:		Ch. Mueller
# Date:			23.12.87
# Context:		SETHEO
#
# Contents:		This file is a merge of unify.h, unifyred.h, and
#				full.c. Unification and protocol unification are
#				seperated now, using macros PU_**** or function
#				full_proto_unify() now instead of including 
#				file unifyred.h and full_unify().
#
# Modifications:
#	(when)		(who)		(what)
#	11.3.93	Mueller,Goller neue Constraints, Statistics
#	29.6.93		jsc		include-defines
#
# Bugs:
#
# 		--- Forschungsgruppe KI --- Intellektik ---
# Lehrstuhl fuer Rechnerarchitektur, Inst. fuer Informatik, TU Muenchen 
#########################################################################*/

#ifndef PROTO_UNIFY_H
#define PROTO_UNIFY_H

#include "unifyt.h"
#include "full.h"

#define PU_STRVAR(harg, hbp, garg, gbp)\
\
			switch ((unsigned char)(GETTAG(*garg))) {\
\
				case T_CONST:\
\
					trail_var(harg);\
					GENPTR(*harg, garg);\
					break; \
\
\
				case T_STRVAR:\
\
					if (harg == garg) \
							break; \
\
					if (harg > garg)\
					   { \
						trail_var(harg);\
						GENPTR(*harg,garg); \
					   }\
					else\
					   { \
						trail_var(garg);\
						GENPTR(*garg,harg); \
					   }\
					break; \
\
        \
				case T_FVAR:\
\
					trail_var(harg);\
					GENPTR(*harg, garg);\
					break;\
\
\
				case T_GTERM:\
\
					trail_var(harg);\
					GENPTR(*harg,garg); \
					break; \
\
\
				case T_CRTERM:\
\
					if (occ_ck(harg,garg,gbp)) FAIL_U2("%1");\
					trail_var(harg); \
					GENPTR(*harg,garg); \
					break; \
\
\
				case T_NGTERM:\
\
					garg = copy_struct(garg,gbp,harg); \
					if (!garg) FAIL_U2("%2");\
						trail_var(harg);\
					GENPTR(*harg,garg); \
					break; \
\
\
				default:\
					FAIL_U2("%3");\
					break;\
\
				}	/* end of switch tag of goal within case ha == T_STRVAR */\
			break;


#define PU_CONST(harg, hbp, garg, gbp)\
\
			switch ((unsigned char)(GETTAG(*garg))) {\
\
				case T_STRVAR:\
\
					trail_var(garg);\
					GENPTR(*garg, harg);\
					break; \
\
\
				case T_CONST:\
					if (!EQUAL_WORD(*garg,*harg)) \
						FAIL_U2("%4");\
					break; \
\
\
				case T_FVAR:\
\
					c_trail(garg);\
					GENPTR(*garg, harg);\
					break; \
\
\
				default:\
					FAIL_U2("%5");\
					break;\
\
				}	/* end of switch tag of goal within case ha == T_CONST */\
			break;

#define PU_FVAR(harg, hbp, garg, gbp)\
\
			switch ((unsigned char)(GETTAG(*garg))) {\
\
				case T_STRVAR:\
\
					trail_var(garg);\
					GENPTR(*garg, harg);\
					break;\
\
               \
				case T_CONST:\
\
					c_trail(harg);\
					GENPTR(*harg, garg);\
					break; \
\
\
				case T_FVAR:\
\
					if (harg == garg) \
						break; \
\
					if (harg > garg) { \
						c_trail(harg);\
						GENPTR(*harg,garg); \
						} \
					else\
						{ \
						c_trail(garg); \
						GENPTR(*garg,harg); \
						} \
					break; \
\
\
				case T_CRTERM:\
\
					if (occ_ck(harg,garg,gbp)) FAIL_U2("%6");\
					c_trail(harg);\
					GENPTR(*harg,garg); \
					break; \
\
\
				case T_GTERM:\
\
					c_trail(harg);\
					GENPTR(*harg,garg); \
					break; \
\
\
				case T_NGTERM:\
\
					garg = copy_struct(garg,gbp,harg); \
					if (!garg) FAIL_U2("%7");\
					c_trail(harg);\
					GENPTR(*harg,garg); \
					break; \
\
\
				default:\
					FAIL_U2("%8");\
					break;\
\
				}	/* end of switch tag of goal within case ha == T_FVAR */\
			break;


#define PU_CRTERM(harg, hbp, garg, gbp)\
\
			switch ((unsigned char)(GETTAG(*garg))) {\
\
				case T_STRVAR:\
\
					if (occ_ck(garg,harg,hbp)) FAIL_U2("%9");\
					trail_var(garg); \
					GENPTR(*garg,harg); \
					break; \
\
\
				case T_FVAR:\
\
					if (occ_ck(garg,harg,hbp)) FAIL_U2("%0");\
					c_trail(garg); \
					GENPTR(*garg,harg); \
					break; \
\
\
				case T_CRTERM:\
				case T_GTERM:\
				case T_NGTERM:\
\
					if (full_proto_unify(garg,harg,gbp,hbp)) /* do copy */ \
						FAIL_U2("%10"); \
					break; \
\
\
				default:\
					FAIL_U2("%11");\
					break;\
\
				}	/* end of switch tag of goal within case ha == T_CRTERM */\
			break;


#define PU_GTERM(harg, hbp, garg, gbp)\
\
			switch ((unsigned char)(GETTAG(*garg))) {\
\
				case T_STRVAR:\
\
					trail_var(garg); \
					GENPTR(*garg,harg); \
					break; \
\
\
				case T_FVAR:\
\
					c_trail(garg); \
					GENPTR(*garg,harg); \
					break; \
\
\
				case T_CRTERM:\
				case T_GTERM:\
				case T_NGTERM:\
\
					if (full_proto_unify(garg,harg,gbp,hbp)) /* do not copy */ \
						FAIL_U2("%12"); \
					break; \
\
\
				default:\
					FAIL_U2("%12");\
					break;\
\
				}	/* end of switch tag of goal within case ha == T_GTERM */\
			break;


#define PU_NGTERM(harg, hbp, garg, gbp)\
\
			switch ((unsigned char)(GETTAG(*garg))) {\
\
				case T_STRVAR:\
\
					harg = copy_struct(harg,hbp,garg); \
					if (!harg) FAIL_U2("%13") ;\
						trail_var(garg);\
					GENPTR(*garg,harg); \
					break; \
\
\
				case T_FVAR:\
\
					harg = copy_struct(harg,hbp,garg); \
					if (!harg) FAIL_U2("%14") ;\
						c_trail(garg);\
					GENPTR(*garg,harg); \
					break; \
\
\
				case T_GTERM:\
				case T_CRTERM:\
				case T_NGTERM:\
					if (full_proto_unify(garg,harg,gbp,hbp)) /* do copy */ \
						FAIL_U2("%15"); \
					break; \
\
\
				default:\
					FAIL_U2("%16");\
					break;\
\
				}	/* end of switch tag of goal within case ha == T_NGTERM */\
			break;

#endif
