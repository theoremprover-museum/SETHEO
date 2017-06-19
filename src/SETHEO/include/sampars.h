/*******************************************************************/
/*    S E T H E O                                                  */
/*                                                                 */
/* FILE sampars.h                                                  */
/* VERSION		@(#)sampars.h	8.1 05 Jan 1996                                    */
/* DATE 11.11.92                                                   */
/* AUTHOR J. Schumann                                              */
/* NAME OF FILE                                                    */
/* DESCR                                                           */
/* MOD                                                             */
/*		3.0-parameters (no chp *)                          */
/*		29.6.93		jsc		include-defines    */
/*		21.9.94		jsc		realtime cputime   */
/*		9.3.95		jsc		batchmode          */
/* BUGS                                                            */
/*******************************************************************/
#ifndef SAMPARS_H
#define SAMPARS_H

#define ARG_DEPTH			0
#define ARG_CONS			1
#define ARG_FVARS		        2
#define ARG_LPANL                       3
#define ARG_ANL				4
#define ARG_REG				5
#define ARG_TS				6
#define ARG_CS				7
#define ARG_LOCINF			8
#define ARG_INF				9
#define ARG_LOCINFR			10
#define ARG_INFR			11
#define ARG_VERBOSE			12
#define ARG_ITER			13
#define ARG_CODESIZE			14
#define ARG_STACKSIZE			15
#define ARG_TRAILSIZE			17
#define ARG_HEAPSIZE			18
#define ARG_SYMBSIZE			19
#define ARG_SEED			20
#define ARG_REALTIME			21
#define ARG_CPUTIME			22
#define ARG_LEMMATREE			23
#define ARG_BATCHMODE			24
#define ARG_TC	        	        25
#define ARG_SGS                         26
#define ARG_HORNANL                     27
#define ARG_NHORNANL                    28
#define ARG_LPHORNANL                   29
#define ARG_LPNHORNANL                  30
#define ARG_DYNSGREORD                  31
#define ARG_WD                          32
#define ARG_WDR                         33
#define ARG_WD1                         34
#define ARG_WD2                         35
#define ARG_WD3                         36
#define ARG_WD4                         37
#define ARG_WD4_FLAG                    38
#define ARG_SINGLE_DELAY                39
#define ARG_MULTI_DELAY                 40
#define ARG_TCDFLAG	        	41
#define ARG_TCD1	        	42
#define ARG_TCD2	        	43
#define ARG_SGSDFLAG	        	44
#define ARG_SGSD1	        	45
#define ARG_SGSD2	        	46
#define ARG_FVARSDFLAG	        	47
#define ARG_FVARSD1	        	48
#define ARG_FVARSD2	        	49
#define ARG_FORCE_GR	                50
#define ARG_FORCE_PS	                51
#define ARG_FORCE_DL	                52
#define ARG_FORCE_FVAR	                53

#ifdef STATPART
#define ARG_TIMELIMIT                   54
#define ARG_M                           55
#define ARG_PVM                         56
#define ARG_SIM                         57
#define ARG_SPGEN                       58
#define ARG_TSK                         59
#define ARG_RECOMP                      60
#define ARG_IAND                        61
#define ARG_LINRES                      62
#define ARG_EO                          63
#define ARG_NOGEN                       64
#define ARG_SUBGS                       65
#define ARG_FORMEL                      66
#define ARG_FFCHECK                     67
#endif

#define ARG_SPREAD_REDUCTS              68
#define ARG_SIG                         70
#define ARG_LOOKAHEAD                   71
#define ARG_SIGDFLAG	        	72
#define ARG_SIGD1	        	73
#define ARG_SIGD2	        	74
#define ARG_EQ_BY_STE_MOD        	75
#define ARG_DELAY_SWITCH        	76
#define ARG_SHORTCL        	        77
#define ARG_LONGCL        	        78

#define ARG_DEBUG        	        79
#define ARG_ALLTREES                    80
#define ARG_PRINTLEMMATA                81
#define ARG_HIDDEN                	82

#endif
