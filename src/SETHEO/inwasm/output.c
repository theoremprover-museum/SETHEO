/******************************************************************************
;;; MODULE NAME : output.c
;;;
;;; PARENT      :
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)output.c	15.1  01 Apr 1996
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 15.5.91
;;;
;;; DESCRIPTION : print matrix, print_occ, error messages
;;;
;;; REMARKS     :
;;;		2.7.93	jsc	extern.h
;;;		12.7.93	jsc	silent removed
;;;		30.3.94 hamdi	static functions decl.
;;;		2.2.96	jsc	fatal/2	
;;;
******************************************************************************/

#include "symb.h"
#include "extern.h"

/*****************************************************************************/
/* functions (inwasm/output.c)                                               */
/*****************************************************************************/
void print_clause();
void print_term();
void warning();
void nferror();
void fatal();

/*****************************************************************************/
/* local macros                                                              */
/*****************************************************************************/
#define IND i,' '	/* indent */

/*****************************************************************************/
/* statics                                                                   */
/*****************************************************************************/
static int i = 0;
static FILE *fout = NULL; 


/*****************************************************************************/
/* print_clause()                                                            */
/* Description: print clause in standard logic notation on fout              */
/*****************************************************************************/
void print_clause(pp)
predtype * pp;
{

  fprintf(fout,"%*c",IND);
  while(pp) {
    fprintf(fout,"%c%s",pp->sign,NAM(pp->symb));
    if (pp->t_list) {
      fprintf(fout,"(");
      print_term(pp->t_list,fout);
      fprintf(fout,")");
    }
    if (pp->next)
      fprintf(fout," , ");
    pp=pp->next;
  }
}


/*****************************************************************************/
/* print_term()                                                              */
/* Description: print term in prolog like manner on fout2                    */
/*****************************************************************************/
void print_term(tp,fout2)
termtype * tp;
FILE *fout2;
{
  while(tp) {

#ifdef DEBUG
    fprintf(fout2,"%c",(tp->grflag==GROUND)?'g':(tp->grflag==NGROUND)?'u':'!');
#endif

    fprintf(fout2,"%s",NAM(tp->symb));
    if (tp->t_list) {
      fprintf(fout2,"(");
      print_term(tp->t_list,fout2);
      fprintf(fout2,")");
    }
    if (tp->next)
      fprintf(fout2,",");
    tp=tp->next;
  }
}


/*****************************************************************************/
/* warning()                                                                 */
/* Description: print warning on stdout                                      */
/*****************************************************************************/
void warning(t,s)
char *t,*s;
{
  fprintf(stderr,"  Warning : %s %s\n",t,s);
}


/*****************************************************************************/
/* nferror()                                                                 */
/* Description: print non fatal error on stdout                              */
/*****************************************************************************/
void nferror(s,lineno)
char *s;
int lineno;
{
  fprintf(stderr,"Non-fatal error : %s  in line %d\n",s,lineno);
  errors++;
}


/*****************************************************************************/
/* fatal()                                                                   */
/* Description: report a fatal error and exit                                */
/*****************************************************************************/
void fatal (s,exitval)
char *s;		                                    /* error message */
int exitval;
{
  fprintf (stderr,"  Fatal error: %s\n",s);
  exit(exitval);
}

