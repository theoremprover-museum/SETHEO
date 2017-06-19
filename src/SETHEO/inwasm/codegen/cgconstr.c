/****************************************************
;;; MODULE NAME : cgconstr.c
;;;
;;; PARENT      : cgen.c
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)cgconstr.c	15.1 01 Apr 1996
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 5.5.92
;;;
;;; DESCRIPTION : codegeneration for constraints
;;;
;;; REMARKS     :
;;;		14.12.93	jsc		purity, fold
;;;             22.12.93        hamdi           restyled
;;;		17.8.94		jsc		intregration of
;;;						hamdi's file into
;;;						this version
;;;						include optypes removed
;;;              
****************************************************/


#include "types.h"
#include "extern.h"

extern int constrnum;
extern int nrbuilt;
extern FILE *code_file;

/******************************************************/
/* functions (codegen/cgconstr.c)                     */
/******************************************************/
void cg_constraints();
int tsubs_constr();
void cg_constrainterms();
void cg_genregs();

/******************************************************/
/* cg_constraints()                                   */
/******************************************************/
/* generate the constraint instructions               */
/* first original clause, then all contrapositives    */

void cg_constraints(clause,clnr)
claustype *clause;
int clnr;
{
predtype *constr = clause->constr;

while (constr) {

	cgin_constr(clnr,constr);
        constr = constr->next;
      }
}

int tsubs_constr(constr)
predtype *constr; 
{
 while (constr) {
       if (constr->sign != TAUT_CONSTR && constr->sign != SUB_CONSTR &&
           constr->sign != SEM_CONSTR)
 	    constr = constr->next; 
       else 
       	return(1);
 }
 return(0);
}

           

/******************************************************/
/* cg_constrainterms()                                */
/******************************************************/
/* generate the termstructures for the cv.clnr.cnr    */
void cg_constrainterms(clause)
claustype *clause;
{
claustype *contrapositive;
predtype *constr; 

/**************************************************************************
 * search for first contrapositive with tautology or subsumtion constraints 
 */
contrapositive = clause;

if (!tsubs_constr(contrapositive->constr)) { 
  contrapositive = clause->fann;

  while (contrapositive && !tsubs_constr(contrapositive->constr)) 
     contrapositive = contrapositive->next;
}

/*****************************************************************
 * extract tautology subsumtion and semantical constraints from it
 */
 if (contrapositive) {
	constr = contrapositive->constr;

	while (constr) {
    	   if (constr->sign == TAUT_CONSTR || constr->sign == SUB_CONSTR || 
	       constr->sign == SEM_CONSTR) {
       	         cgin_cvlab(clause->clnr, constr->litnr);

       	         /* recursive termdump */
       	         tl_gen_av(constr->t_list);
           }
           constr = constr->next;
        }
 }
/******************************************************
 * collect overlap and symmetry constraints from clause 
 */

contrapositive = clause;
constr = contrapositive->constr;

while (constr) 
 {
      if (constr->sign == OVER_CONSTR || constr->sign == SYMM_CONSTR) {
         cgin_cvlab(clause->clnr, constr->litnr);

         /* recursive termdump */
         tl_gen_av(constr->t_list);
      }
      constr = constr->next;
 }

contrapositive = clause->fann;

/***************************************************************
 * collect overlap and symmetry constraints from contrapositives
 */

while (contrapositive) 
 {
    constr = contrapositive->constr;
    while (constr) {
        if (constr->sign == SYMM_CONSTR || constr->sign == OVER_CONSTR) {
           cgin_cvlab(clause->clnr, constr->litnr);

           /* recursive termdump */
           tl_gen_av(constr->t_list);
        }
        constr = constr->next;
    }
    contrapositive = contrapositive->next;
 }
}

/********************************************************/
/* cg_genregs()                                         */
/********************************************************/
/* generate for every subgoal call a genreg instruction */
void cg_genregs(clnr,subgoal)
int clnr;
predtype *subgoal;
{
int sgs;

sgs = 1;

while (subgoal) {

	if (subgoal->symb > nrbuilt || 
	    (subgoal->kp && subgoal->kp->clnr > clnr)) {
		/* no genregs for built-ins and new predicates */
	   if (inwasm_flags.foldupx){
		cgin_genregx(clnr,subgoal->litnr);
		}
	   else if (inwasm_flags.folddownx){
	   	cgin_genregu(clnr,subgoal->litnr);
		}
	   else {
	   	cgin_genreg(clnr,subgoal->litnr);
		}
	   }

	subgoal = subgoal->next;
	}
}

/*######################################################################*/

