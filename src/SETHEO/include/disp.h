/***********************************************************************/
/*    S E T H E O                                                      */
/*                                                                     */
/* FILE: disp.h                                                        */
/* VERSION:                                                            */
/* DATE: 10.6.88                                                       */
/* AUTHOR: J. Schumann                                                 */
/* NAME OF FILE:                                                       */
/* DESCR:                                                              */
/*	macros for ptree instructions                                  */
/* MOD: 20.7.91 I have changed > to >= in GET_PRED and IS_NEG for the  */
/*              default_query  Goller                                  */
/*      22.7.91 IS_CLAUSE CHANGE_SIGN added by Goller                  */
/*	29.6.93		jsc		include-defines                */
/*	01.6.94		jsc		new macros                     */
/* BUGS:                                                               */
/***********************************************************************/

#ifndef DISP_H
#define DISP_H

#define IS_FACT(X) (((X) % 10) == 1)
#define IS_REDUCT(X) (((X) % 10) == 0)
#define IS_CLAUSE(X) (((X) % 10) >= 2)

#define CLAUSE_NR(X) ((X)/100)
#define LIT_NR(X) (((X) / 10) % 10)

#ifdef STATPART
#define GET_PRED(X) (((X) >= MAXPREDSYMB) ? (X)- MAXPREDSYMB:(X))
#define IS_NEG(X) ((X) >=   MAXPREDSYMB)
#endif

/* void  disp_ (); */
/* void  disp_ (FILE* fout, WORD* obj, environ* locbp); */

#endif



















