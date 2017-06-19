/*########################################################################
# NOTE: PORT JSC 09/12/2001: IS_FOLDED_UP 
# Include File:		tags.h
# SCCS-Info:		8/26/92, @(#)tags.h	1.8
# Author:		J. Schumann
# Date:			88
# Context:		wasm (SETHEO)
# 
# Contents:		???
#
# Modifications:	
#	(when)		(who)		(what)
#		 			tags mod'd to fit any address-start
#	13.4.88 			CREF
#       13.4. 				atari V.
#	14.7. 				signals
#	5.10. 				return values
#	7.11. 				global variables
#	30.11. 				hypercube
#	27.12. 				transputer
#	31.1.89 			Makefile
#	14.2.89				IDTAG
#	19.6.89 			ABORT
#	3.10.89 			reordering of return codes
#	10.10.90 			new return codes
#       24.6.91 	Goller		changes to ISVAR
#	23.10.91	Max Moser	`restyled'
#       11.02.92        Markus Bschorer #ifdef CONSTRAINTS
#	   09.03.92		Markus Bschorer New Tag T_STRVAR
#	18.02.93	jsc		T_STRVAR always active
#	25.2.93		Goller		T_CSTRVAR added, T_STRVAR -> 9
#	11.3.93	Mueller,Goller neue Constraints, Statistics
#	29.6.93		jsc		include-defines
#	1.6.94		jsc		new macros
#
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
########################################################################*/
#ifndef SAM_TAGS_H
#define SAM_TAGS_H


/*------------------------------------------------------------------------
- !!!!!MACHINE DEPENDENT !!!!!!!!!!!!!!
------------------------------------------------------------------------*/


#ifdef sun

#define  PTR_MASK	0x00000000	/* mask for pointer     	*/

/*------------------------------------------------------------------------

                          WORD-format:

                4 bits     28 bits 
               +------+----------------------------+
               | TAG  |  VALUE                     |
               +------+----------------------------+
                31  28 27                         0

------------------------------------------------------------------------*/

#define TAGMASK         0xf0000000      /* 4-bit tags 			*/
#define TAGSIZE 	4
#define TAGSTART  	28
#define VALMASK 	0x0fffffff
#define PTRMASK		0x0fffffff  
#define IDMASK          0x000fffff
#define IDTAG		0x00100000      /* if on, is a constant, must
                                           be equal to IDTAG of inwasm	*/
#define NUMMASK		0x0000ffff      /* only short-arithmetic        */
#define MAXNUMBER       32767           /* must correspond to NUMMASK   */
#define MINNUMBER       -32767          /* must correspond to NUMMASK   */
#define WORDLENG        32              /* 32 bit wordlength 		*/
#define MAXPREDSYMB     10000           /* must be equal to inwasm      */

#else /* hpux */

#define  PTR_MASK	0x40000000	/* mask for pointer     	*/
/*------------------------------------------------------------------------
               WORD-format:

    4 bits 4 bits      24 bits
   +------+------+---------------------------+
   | RES  | TAG  |  VALUE                    |
   +------+------+---------------------------+
    31  28 27  24 23                        0

------------------------------------------------------------------------*/

#define TAGMASK         0x0f000000      /* 4-bit tags 			*/
#define TAGSIZE 	4
#define TAGSTART  	24
#define VALMASK 	0x00ffffff
#define PTRMASK		0xf0ffffff
#define IDMASK          0x000fffff
#define IDTAG	        0x00100000	/* if on, is a constant, must
                                           be equal to IDTAG of inwasm	*/
#define NUMMASK		0x0000ffff      /* only short-arithmetic        */
#define MAXNUMBER       32767           /* must correspond to NUMMASK   */
#define MINNUMBER       -32767          /* must correspond to NUMMASK   */
#define WORDLENG        32              /* 32 bit wordlength 		*/
#define MAXPREDSYMB     10000           /* must be equal to inwasm      */


#endif


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#ifdef WORD_32

#ifdef sun
typedef  unsigned int   WORD;           /* 32-bit wordlength 		*/
typedef  unsigned int   WORD_cast;
#else
typedef  unsigned int   WORD;           /* 32-bit wordlength 		*/
typedef  unsigned int   WORD_cast;
#endif

#define GETTAGFORMAT    "%d"
#define GETSYMBOLFORMAT "%d"
#define GETVALFORMAT    "%x"

#define ADDRESS_OUT_OF_RANGE(address) \
        ((long) address) >= ((1L << TAGSTART) | PTR_MASK)

/*----------------------------------------------------------------------*/

#define GETTAG(obj) \
        (((obj) & TAGMASK) >> (TAGSTART))

#define GETTAG2(obj) GETTAG(obj)

#define GETVAL(obj) \
        ((obj) & VALMASK)

#define GETPTR(obj)\
        ((WORD*) ((obj) & PTRMASK))

#define GETNUMBER(OBJECT) \
        ((short) ((OBJECT) & NUMMASK))

#define GETSYMBOL(OBJECT) \
        ((OBJECT) & IDMASK)

#define GETSYMBOLINDEX(OBJECT) \
        (GETSYMBOL(OBJECT) \
         - (ISNEGATED(OBJECT)? MAXPREDSYMB : 0))

#define GETARITY2(INDEX)  \
        symbtab[INDEX].arity

#define GETARITY(OBJECT)  \
        GETARITY2(GETSYMBOLINDEX(OBJECT))

#define GETPRINTNAME(OBJECT)  \
        symbtab[GETSYMBOLINDEX(OBJECT)].name

#define GETSIGN(OBJECT)                 \
        ((ISNEGATED(OBJECT))?-1:1)


/* #define CHANGE_SIGN(OBJECT) \ */
/*         ((ISNEGATED(OBJECT)) ? (OBJECT) - MAXPREDSYMB : \ */
/*                                (OBJECT)+ MAXPREDSYMB) */

#define CHANGE_THE_SIGN(OBJECT) \
        ((OBJECT) += (ISNEGATED(OBJECT) ? (-MAXPREDSYMB):MAXPREDSYMB))

#define ISCOMPLEX(OBJECT) \
        ((GETTAG(OBJECT) == T_CRTERM) || 			\
         (GETTAG(OBJECT) == T_GTERM)  || 			\
         (GETTAG(OBJECT) == T_NGTERM))

#define ISID(OBJECT)               ((OBJECT) & IDTAG)

#define ISNUMBER(OBJECT) \
        ((GETTAG(OBJECT) == T_CONST) && \
        !((OBJECT) & IDTAG))

#define ISCONSTANT(OBJECT) \
        ((GETTAG(OBJECT) == T_CONST) && \
        ((OBJECT) & IDTAG)) 

#define ISSYMBOL(OBJECT) \
        (ISCONSTANT(OBJECT) || ISCOMPLEX(OBJECT) )

#define ISVAR(OBJECT) \
        (GETTAG(OBJECT) & T_ISVAR)

#define ISLITERAL(OBJECT) \
        (   (ISCONSTANT(OBJECT) || ISCOMPLEX(OBJECT)) \
         && symbtab[GETSYMBOLINDEX(OBJECT)].type == SYMB_P)

#define ISNEGATED(OBJECT) \
        (GETSYMBOL(OBJECT) >= MAXPREDSYMB)

#define CONSTCELLISNUMBER(OBJECT) \
        !((OBJECT) & IDTAG)

#define CONSTCELLISSYMBOL(OBJECT) \
        ((OBJECT) & IDTAG)

#define INDEXTOSYMBOL(OBJECT) \
        (OBJECT | IDTAG)

#define EQUAL_WORD2(OB1,OB2) \
        ((OB1) == (OB2))

#define EQUAL_WORD(OBJ1,OBJ2)  \
        (  (GETVAL(OBJ1) == GETVAL(OBJ2)) && (ISID(OBJ1) == ISID(OBJ2)))

/* Generate arbitrary cells: */
#define GENOBJ(where,val,tag) \
        ((where)  =  ((WORD) (val) | ((tag) << (TAGSTART))))

#define GENCONST(where,val) \
        GENOBJ(where,val,T_CONST)

/* Generate cells for numeric values (remember: only short-arithmetic !): */
#define GENNUMBER(where,val) \
        GENOBJ(where, (((WORD) (val)) & NUMMASK), T_CONST)

/* Generate cells for symbols (predicate, function, constant symbols): */
#define GENSYMBOL(where,val,tag) \
        GENOBJ(where, ((((WORD) (val)) & IDMASK) | IDTAG), tag)

/* Generate cells for pointers: */
#define GENPTR(where,val) \
        GENOBJ(where, (WORD) (val), T_BVAR)

/* Generate cell for code: */
#define GENCODE(where,val) \
        GENOBJ(where, (WORD) (val), T_CODE)



/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/


#else /* WORD_64 */

typedef  struct {
  unsigned long tag;
  unsigned long value;
} WORD;

typedef unsigned long WORD_cast;

#define GETTAGFORMAT    "%ld"
#define GETSYMBOLFORMAT "%ld"
#define GETVALFORMAT    "%lx"

#define ADDRESS_OUT_OF_RANGE(address) \
        FALSE

#ifndef _ANSI_ARGS_
#if (defined(__STDC__) && !defined(NO_PROTOTYPE)) || defined(__cplusplus) || defined(USE_PROTOTYPE)
#   define _USING_PROTOTYPES_ 1
#   define _ANSI_ARGS_(x)       x
#else
#   define _ANSI_ARGS_(x)       ()
#endif
#endif

/*----------------------------------------------------------------------*/

#define GETTAG(OBJECT)             ((OBJECT).tag&IDMASK)
#define GETTAG2(OBJECT)            ((OBJECT).tag)
#define GETVAL(OBJECT)             (OBJECT).value
#define GETPTR(OBJECT)             (void*)(OBJECT).value
#define GETNUMBER(OBJECT)          ((short)(OBJECT).value & NUMMASK)
#define GETSYMBOL(OBJECT)          (OBJECT).value
#define GETSYMBOLINDEX(OBJECT)     (GETSYMBOL(OBJECT) \
                                   - (ISNEGATED(OBJECT)? MAXPREDSYMB : 0))
#define GETARITY(OBJECT)           GETARITY2(GETSYMBOLINDEX(OBJECT))
#define GETARITY2(INDEX)           symbtab[INDEX].arity
#define GETPRINTNAME(OBJECT)       symbtab[GETSYMBOLINDEX(OBJECT)].name
#define GETSIGN(OBJECT)            ((ISNEGATED(OBJECT))?-1:1)
/* #define CHANGE_SIGN(SYMBOL)        ((SYMBOL)>= MAXPREDSYMB  ? (SYMBOL - MAXPREDSYMB) :( SYMBOL + MAXPREDSYMB)) */
#define CHANGE_THE_SIGN(OBJECT)    (((OBJECT).value) += (ISNEGATED(OBJECT) ? (-MAXPREDSYMB):MAXPREDSYMB))
#define ISID(OBJECT)               ((OBJECT).tag & IDTAG)
#define ISCOMPLEX(OBJECT)          ((GETTAG(OBJECT) == T_CRTERM) || 			\
                                    (GETTAG(OBJECT) == T_GTERM)  || 			\
                                    (GETTAG(OBJECT) == T_NGTERM))
#define ISNUMBER(OBJECT)           ((GETTAG(OBJECT) == T_CONST) && !ISID(OBJECT))
#define ISCONSTANT(OBJECT)         ((GETTAG(OBJECT) == T_CONST) && ISID(OBJECT))
#define ISSYMBOL(OBJECT)           (ISCONSTANT(OBJECT) || ISCOMPLEX(OBJECT) )
#define ISVAR(OBJECT)              (GETTAG(OBJECT) & T_ISVAR)
#define ISLITERAL(OBJECT)           \
                                   ( (ISCONSTANT(OBJECT) || ISCOMPLEX(OBJECT)) \
                                       && symbtab[GETSYMBOLINDEX(OBJECT)].type == SYMB_P)
#define ISNEGATED(OBJECT)          (GETSYMBOL(OBJECT) >= MAXPREDSYMB)
#define CONSTCELLISNUMBER(OBJECT)  (!ISID(OBJECT))
#define CONSTCELLISSYMBOL(OBJECT)  ISID(OBJECT)
#define INDEXTOSYMBOL(OBJECT)      ((OBJECT).tag | IDTAG)
#define EQUAL_WORD2(OBJ1,OBJ2)     ( ((OBJ1).tag == (OBJ2).tag) && ((OBJ1).value == (OBJ2).value))
#define EQUAL_WORD(OBJ1,OBJ2)      (  (GETVAL(OBJ1) == GETVAL(OBJ2)) && (ISID(OBJ1) == ISID(OBJ2)))
#define GENOBJ(WHERE,VAL,TAG)     {(WHERE).value=(VAL);(WHERE).tag=(TAG);}
#define GENNUMBER(WHERE,VAL)      {(WHERE).value=(VAL);(WHERE).tag=T_CONST;}
#define GENSYMBOL(WHERE,VAL,TAG)  {(WHERE).value=(VAL);(WHERE).tag=(TAG);(WHERE).tag|=IDTAG;}
#define GENPTR(WHERE,VAL)         {(WHERE).value=(unsigned long)(VAL);(WHERE).tag=T_BVAR;}
#define GENCODE(WHERE,VAL)        {(WHERE).value=(VAL);(WHERE).tag=T_CODE;}
#define GENCONST(WHERE,VAL)       {if (ISID(VAL)) { GENSYMBOL(WHERE,GETSYMBOL(VAL),T_CONST) } else { GENOBJ(WHERE,GETVAL(VAL),T_CONST) } }


#endif

#define SIZE_IN_WORDS(TYPE) (int)(((float)sizeof(TYPE))/sizeof(WORD) +2.0)

/*------------------------------------------------------------------------
- Tags
------------------------------------------------------------------------*/

#define T_EMPTY         0               /* empty cell or nil    	*/
#define T_CONST         1               /* Constant             	*/
#define T_GTERM         2               /* Ground term, no copy 	*/
#define T_NGTERM        3               /* non-ground, code-area	*/
#define T_CRTERM        4               /* term in heap         	*/
#define T_EOSTR         5               /* end of structure     	*/
#define T_CREF		6		/* reference in code ar.	*/

#define T_ISVAR         8               /* bitpos of VAR-types 		*/
#define T_VAR           11              /* free variable        	*/
#define T_BVAR          12              /* bound var == REF     	*/
#define T_FVAR		13		/* variable cell, no inf	*/

#define T_GLOBL		10		/* Global variable	        */
					/* T_GLOBAL is a tag similar to 
					   T_FVAR, not an offset as
					   defined in Johann's Diss.
					   Goller                       */

#define T_HREF		14		/* reference to heap	        */
					/* used for globals	        */

#define T_CSTRVAR	8
#define T_STRVAR        9               /* tag for structure-variables  */
                                        /* neccessary for handling sub- */
                                        /* sumption- and antilemma-     */
                                        /* constraints.                 */
                                        /* added by bschorer            */

#define T_MARK          T_EMPTY         /* Tag for marking cells (used 
					   for hornanl, nonhornanl, 
					   tc_bound, dynsgreord)        */
#define T_MARKED_FVAR   7               /* Tag for marking cells (used 
					   for delay of subgoals)       */

#define T_CODE		T_EMPTY		/* Tag for code (used in code
                                           area)                        */

/*------------------------------------------------------------------------
- Signal handling
------------------------------------------------------------------------*/

#define SIGNALS		/**/

/*------------------------------------------------------------------------
- general purpose
------------------------------------------------------------------------*/

#ifndef TRUE
#define TRUE		1
#define FALSE		0
#endif

#ifdef WORD_64
#define NVAL		(environ *)((-1))
#else
#define NVAL		(environ *)((-1) & PTRMASK)
#endif

/*------------------------------------------------------------------------
- invariable symbol table indexes
------------------------------------------------------------------------*/

#define DEFAULT_QUERY   0	/* index of default query in symbtab	*/
#define LISTFUNCT	1	/* index of list functor in symbtab	*/
#define EMPTYLIST	2	/* index of empty list in symbtab	*/


/*------------------------------------------------------------------------
- instr() return values
------------------------------------------------------------------------*/
typedef enum {
	success =0,
	failure,
	sam_abort,
	error,
	mem_overflow,
	totfail,
        boundfail,
	stop_cycle		= 4096
	} instr_result;

#define REMODE		(-7)
#define NORMAL		(-8)


/*------------------------------------------------------------------------
- tags for Regularity-Constraints
------------------------------------------------------------------------*/

#ifdef CONSTRAINTS

#define REGULARITY      1     /* Tag for regulartity-constr.  */
#define TAUTOLOGIE      2     /* Tag for tautologic-constr.   */
#define SUBSUMPTION     3     /* Tag for subsupmtion-constr.  */
#define ANTILEMMA       4     /* Tag for antilemma-constr.    */

#endif



/*------------------------------------------------------------------------
- inference types used as tags for environ->tp
------------------------------------------------------------------------*/

typedef enum {
	EXTENSION = 0,
	REDUCTION,
	FACTORIZATION_PESS,
	FACTORIZATION_OPT,
	LEMMA,
	NOT_SOLVED
	} inf_types;



/*------------------------------------------------------------------------
- types of alternatives at a choicepoint
------------------------------------------------------------------------*/

typedef enum {
    declarative,     /* declarative rule */
    proc_fact,       /* procedural fact */
    proc_query,      /* procedural query */
    proc_rule,       /* procedural rule */
    reduct           /* reduction step */
} alt_types;


/*------------------------------------------------------------------------
- Semantics of environ->flags  
-----------------------------------------------------------------------------*/

/* In order to preserve compatibility with Antilemmata (everything on the
trail has to be tagged) flags are tagged with T_EMPTY */

#ifdef hpux

/*----------------------------------------------------------------------------
           folded_up (bit 28)
           |
-          |                                             0 
-  +--------+---------+-----------------------------------+
-  |        | T_EMPTY |        inference-number           |
-  +--------+---------+-----------------------------------+
-

The inference-number is set in i_ptree 
-----------------------------------------------------------------------------*/

#ifdef FOLD_UP
#define NOT_FOLDED_UP       0xE0FFFFFF
#define FOLDED_UP           0x10000000
	/* this macro is necessary, because GETVAL cuts of this bit
	 * in hp_ux mode
	 **/
#define IS_FOLDED_UP(X)		((X) & FOLDED_UP)
#endif

#else  /* if not hpux */

/*----------------------------------------------------------------------------
              folded_up (bit 27)
              |
-             |                                          0 
-  +---------+--------------------------------------------+
-  | T_EMPTY |                 inference-number           |
-  +---------+--------------------------------------------+
-

The inference-number is set in i_ptree 
-----------------------------------------------------------------------------*/

#ifdef FOLD_UP
#define NOT_FOLDED_UP       0x07FFFFFF
#define FOLDED_UP           0x08000000
#define IS_FOLDED_UP(X)		(GETVAL(X) & FOLDED_UP)
#endif

#endif  /* if not hpux */

#define MAXPREDSYMB     10000           /* must be equal to inwasm       */

#endif

/*######################################################################*/
