/**************************************************************/
/* ********************************************************** */
/* *                                                        * */
/* *  HEADER:    types.h                                    * */
/* *                                                        * */
/* *  VERSION:   1.0                                        * */
/* *  MODIFIED:  May 2  1993 by Weidenbach                  * */
/* *                                                        * */
/* *  AUTHORS:   P. Graf (graf@mpi-sb.mpg.de)               * */
/* *             C. Weidenbach (weidenb@mpi-sb.mpg.de)      * */
/* *             MPI fuer Informatik                        * */
/* *             Im Stadtwald                               * */
/* *             6600 Saarbruecken                          * */
/* *             Germany                                    * */
/* *                                                        * */
/* ********************************************************** */
/**************************************************************/

#ifndef _TYPES_
#define _TYPES_

/**************************************************************/
/* List                                                       */
/**************************************************************/

typedef struct LIST_HELP {
  struct LIST_HELP *cdr;
  POINTER           car;
} LIST_NODE, *LIST;



/**************************************************************/
/* Symbol                                                     */
/**************************************************************/

/* Symbols' Components */
typedef unsigned int	TYPE;
typedef unsigned int	ARITY;

#define symbol__SYMBOLMAXLEN	32            /* Maximal Length of symbol name*/


/* Symbols */
typedef struct symbol {
  char          *name;       /*The name of the symbol*/
  POINTER       info;        /*Sort of variable, arity of function,predicate*/
  TYPE          type;        /*Type of symbol: variable,function,etc.*/
  POINTER       binding;     /*If variable, then possible binding
			       If basesort, used for conditioned subsort*/
  int           mark;        /*If variable, used for unification; 
			       If basesort used for emptyness*/
  int           time;
  POINTER       link;        /*If variable, used for unification: graphs of symbols;
			       If basesort, used for unconditioned subsort
			          in this case <link> is always a subset of <binding>*/
} SYMBOL_NODE, *SYMBOL;        


/**************************************************************/
/* Basesort and Sort                                          */
/**************************************************************/

typedef SYMBOL  BASESORT;       /* A basesort is a unary predicate symbol*/
typedef LIST    SORT;           /* A sort is an ordered list of basesorts*/

typedef enum {sort__False,sort__True,sort__TrueNoCond,sort__FalseNoCond} STATUS;

/**************************************************************/
/* Term                                                       */
/**************************************************************/

typedef struct term {
  SYMBOL symbol;
  LIST   args;
} *TERM, TERM_NODE;

/**************************************************************/
/* Sort Theory                                                */
/**************************************************************/

typedef struct sortindexextry {
  TERM   declaration;              /* The declaration */
  BOOL   conditions;               /* TRUE iff the declaration is a conditional one*/
} *SORTINDEXENTRY, SORTINDEXENTRY_NODE;

/**************************************************************/
/* Input                                                      */
/**************************************************************/

typedef struct token {
  char   *string;            /* The string of the token */
  int    line;               /* The line where the token occurs */
} *TOKEN, TOKEN_NODE;

#endif
