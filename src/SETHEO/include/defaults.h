/**************************************************************
;;;*****************************************
;;;MODULE NAME:   defaults.h
;;;
;;;PARENT     :   setheo/mainpart
;;;
;;;PROJECT    :   pppppppppp
;;;
;;;AUTHOR     :   xxxxxxxxxx
;;;
;;;DATE       :   ddddddd
;;;
;;;DESCRIPTION:   xxxxxxxxxxxx
;;;               xxxxxxxxxxxxxx
;;;MODIF      :   xxxxx (when, what, who)
;;;		 tags mod'd to fit any address-start
;;;		29.7.88 debugging
;;;		15.1.89 common
;;;		12.6.89 iterative deepening
;;;		6.9.89  partheo needs >= 10002 words of code mem
;;;             13.01.92 CONSTRAINTS added by bschorer
;;;             25.02.92 Local-Inference-bound added by bschorer
;;;		10.3.93	jsc	V3.0 parameters: new defaults
;;;		11.3.93	Mueller,Goller neue Constraints, Statistics
;;;		29.6.93		jsc		include-defines
;;;		16.9.93		jsc	LITNU_LEMMA
;;;		17.9.93		jsc	remove of old lemma stuff, tell,told
;;;				copy-binding
;;;
;;;REMARKS    :   xxxxxxx
;;;
;;;COMPILING  :   yn
;;;
;;;BUGS       :   never any !!
;;;#****************************************
**************************************************************/
#ifndef DEFAULTS_H
#define DEFAULTS_H

/* size of ... in Bytes (default )  (E) otherwise */

#ifdef STATPART
#define DEF_CODE	200000		/* code area 		*/
#define DEF_STACK	150000		/* stack area 		*/
#else
#define DEF_CODE	100000		/* code area 		*/
#define DEF_STACK	100000		/* stack area 		*/
#endif

#define DEF_HEAP	300000		/* heap area 		*/
#define DEF_TRAIL	200000   	/* trail area 		*/
#define DEF_SYMBSIZE    1000		/* def. symb table size (E)*/


#ifdef CONSTRAINTS 

#ifdef STATPART
#define   DEF_C_STACK   500000       /* general constraint - stack  */
#else
#define   DEF_C_STACK   200000       /* general constraint - stack  */
#endif

#define   DEF_PU_TRAIL   4000        /* trail for protocoll-        */
                                     /* unification                 */
#endif


/****************************************************************/
/*** defaults for SPTHEO                                        */
/****************************************************************/
#ifdef STATPART
#ifdef HP
#define CLK_TCK         100             /* 60 fuer SunOS, 100 fuer Solaris */
#endif
#define DEF_INF_STEP	1000000 		/* inference-step-area */
#endif

/****************************************************************/
/*** defaults for depth bound                                   */
/****************************************************************/
#define DEF_DEPTH	MAXNUMBER    	/* depth first search   */
#define DEF_MIN_DEPTH	2		/* default minimal depth*/
#define DEF_DEPTH_INC	1		/* def. increment	*/

/****************************************************************/
/*** defaults for weighted depth bound                          */
/****************************************************************/
#define DEF_WD1     120       /* default for init constant in % */
#define DEF_WD2     120       /* default for init factor in %   */
#define DEF_WD3     120       /* default for dyn factor in %    */
#define DEF_WD4       0       /* default for loci factor in %   */

/****************************************************************/
/*** defaults for inference bound                               */
/****************************************************************/
#define DEF_INFCOUNT    MAXNUMBER   	/* max nr of inferences */
#define DEF_INF_INC	3		/* def. increment	*/
#define DEF_MIN_INF     5               /* set inference-       */
                                        /* bounding start-value */
                                        /* inference-bounding   */

/****************************************************************/
/*** defaults for localinference bound                          */
/****************************************************************/
#define DEF_LOC_INF_BOUND   MAXNUMBER   /* set default-local-   */
                                        /* inference-bound      */
#define DEF_MIN_LOC_INF     3           /* set local-inference- */
                                        /* bounding start-value */
#define DEF_LOC_INF_INC     1           /* set incrementation   */
                                        /* of iterative local-  */
                                        /* inference-bounding   */

#ifdef UNITLEMMA
/*****************************************************************/
/*** defaults for unit lemmata                                   */
/*****************************************************************/
# define CLNU_LEMMA	20000		/* unit-lemma start with */
					/* that number as clause */
					/* number		 */
#endif

#ifdef ANTILEMMATA
/***********************************************************************/
/*** defaults for antilemmata                                          */
/***********************************************************************/
#define DEF_MINCOST     25              /* Only if at least            */
                                        /* DEF_MINCOST more inferences */
                                        /* are spent on search an      */
                                        /* antilemma is generated.     */
#define DEF_DELAYMAX    1000            /* Size of the delay-array,    */
                                        /* d.h. an antilemma is only   */ 
                                        /* delayed if                  */
                                        /* infcount<DEF_DELAYMAX.      */
#endif

/****************************************************************/
/*** defaults for fvars bound                                   */
/****************************************************************/
#define DEF_MAXFVARS  MAXNUMBER 	/* max nr of free vars  */

/****************************************************************/
/*** defaults for fvars bound (dependent on depth)              */
/****************************************************************/
#define DEF_FVARSD1       150    /* default for fvarsd constant */
#define DEF_FVARSD2        75    /* default for fvarsd factor   */

/****************************************************************/
/*** defaults for tc bound                                      */
/****************************************************************/
#define DEF_MAXTC          MAXNUMBER   	/* max termcomplexity   */

/****************************************************************/
/*** defaults for tc bound (dependent on depth)                 */
/****************************************************************/
#define DEF_TCD1         1000       /* default for tcd constant */
#define DEF_TCD2          100       /* default for tcd factor   */

/****************************************************************/
/*** defaults for sgs bound                                     */
/****************************************************************/
#define DEF_MAXSGS    MAXNUMBER   	/* max nr of open sgls  */

/****************************************************************/
/*** defaults for sgs bound (dependent on depth)                */
/****************************************************************/
#define DEF_SGSD1         150      /* default for sgsd constant */
#define DEF_SGSD2         200      /* default for sgsd factor   */

/****************************************************************/
/*** defaults for signature bound                               */
/****************************************************************/
#define DEF_MAXSIG        MAXNUMBER   /* max value of signature */

/****************************************************************/
/*** defaults for signature bound (dependent on depth)          */
/****************************************************************/
#define DEF_SIGD1         1000     /* default for sigd constant */
#define DEF_SIGD2          100     /* default for sigd factor   */

/****************************************************************/
/*** defaults for dynsgreord                                    */
/****************************************************************/
#define DEF_DYNSGREORD  1   	/* default level of dynsgreord  */

#endif
