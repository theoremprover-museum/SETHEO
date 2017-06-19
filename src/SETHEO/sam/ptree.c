/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: ptree.c 
 * VERSION:
 * DATE:		29.10.92
 * AUTHOR:
 * NAME OF FILE:
 * DESCR:
 *	print the entire proof-tree
 *	to a given file
 *	
 *	this instruction always succeeds
 * MOD:
 *	22.2.93:	sgi->tp < bp then red.step changed to <= (jsc)
 *	11.3.93	Mueller,Goller neue Constraints, Statistics
 *	03.6.94	johann	new macros
 *	07.6.94	johann	negative pred-symbs have higher index
 *	09.3.95	johann	FROM->TO
 *	22.3.95	johann	FROM->TO
 *	28.4.97	jsc	visible
 *	1.7.97	jsc	bugfixes in -hide
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include <string.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "codedef.h"
#include "symbtab.h"
/**/
#include "disp.h"
#include "ptree.h"


#define INDENT_STEP		2

#define DEBUG(X)
#define DEBUG2(X) 

/* exported variable */
long ptree_inf_counter;

static	int must_hide(environ 	*lbp);

/******************************************************
 *	eoln
 *	make a \n to the file
 ******************************************************/
static void eoln(filep)
FILE 		*filep;
{
  fputc('\n',filep);
}

/******************************************************
 *	do_indent
 *	indent ''indent'' blanks
 ******************************************************/
static void do_indent(filep,indent)
FILE 		*filep;
int		indent;
{
  if (indent)
    while(indent--){ 
      fputc(' ',filep);
    }
}

/******************************************************/
/*	print_factorization_info2                     */
/******************************************************/
void print_factorization_info2(filep,bpto,bpfrom,sg_ptr)
FILE 		*filep;
environ 	*bpto;
environ 	*bpfrom;
WORD		*sg_ptr;
{
  literal_ctrl_block *lcb_from;

  DEBUG(printf("print_red_info: bpfrom=%lx bpto=%lx\n",bpfrom,bpto));
  
  lcb_from = (literal_ctrl_block*)(code + GETVAL(*sg_ptr));
  
  fprintf(filep," [ %ld , fac__(%d.%d, %d ) ]",
	  ptree_inf_counter,
	  GETNUMBER(lcb_from->clause_nr), 
	  GETNUMBER(lcb_from->lit_nr),
	  0);
}

/******************************************************/
/*	print_factorization_info                      */
/******************************************************/
void print_factorization_info(filep,bpto,bpfrom,sg_ptr)
FILE 		*filep;
environ 	*bpto;
environ 	*bpfrom;
WORD		*sg_ptr;
{
  literal_ctrl_block *lcb_from;
  
  DEBUG(printf("print_red_info: bpfrom=%lx bpto=%lx\n",bpfrom,bpto));
  lcb_from = (literal_ctrl_block*)(code + GETVAL(*sg_ptr));
  

  fprintf(filep," [ %ld , fac__(%d.%d,"GETSYMBOLFORMAT" ) ]",
	  ptree_inf_counter,
	  GETNUMBER(lcb_from->clause_nr), 
	  GETNUMBER(lcb_from->lit_nr),
	  GETVAL(bpto->flags));
}

/******************************************************
 *	print_reduction_info
 *	
 *	print the current reduction step.
 *	the level can be obtained by searching
 *	the dynamic link, starting at bp, until the
 *	bp->dyn* == sg_i->env
 *	sg_i->env points to the environment, from where the
 *	information about the predicate symbol and the arguments
 * 	can be obtained
 ******************************************************/
void print_reduction_info(filep,bpto,bpfrom,sg_ptr)
FILE 		*filep;
environ 	*bpto;
environ 	*bpfrom;
WORD		*sg_ptr;
{
  literal_ctrl_block *lcb_from;
  
  DEBUG(printf("print_red_info: bpfrom=%lx bpto=%lx\n",bpfrom,bpto));
  
  lcb_from = (literal_ctrl_block*)(code + GETVAL(*sg_ptr));
  
  fprintf(filep," [ %ld , red__(%d.%d,"GETSYMBOLFORMAT" ) ]",
	  ptree_inf_counter,
	  GETNUMBER(lcb_from->clause_nr), 
	  GETNUMBER(lcb_from->lit_nr),
	  GETVAL(bpto->flags));
}

/******************************************************
 *	print_lit_info
 *
 *	print the info about the literal + the global
 *	inference counter
 * 	the environment
 *	pointer is given
 *	all output is to be made into the file ``filep''
 *
 ******************************************************/
void print_lit_info(filep,bp,bp_from,sg_ptr)
FILE		*filep;
environ 	*bp;
environ 	*bp_from;
WORD		*sg_ptr;
{

  literal_ctrl_block *lcb_from;
  
#ifdef FACT_NOT_RULE
  char *type;
  if (!GETNUMBER(bp->lcbptr->nr_sgls)){
    /* noch nicht so type = "fact"; */
    type = "ext";
  }
  else {
    type = "ext";
  }
#endif
  
  lcb_from = (literal_ctrl_block*)(code + GETVAL(*sg_ptr));
  
  fprintf(filep,"[ %ld , %s(%d.%d,%d.%d) ] ",
	  ptree_inf_counter,
	  "ext__",
	  GETNUMBER(lcb_from->clause_nr), 
	  GETNUMBER(lcb_from->lit_nr),
	  GETNUMBER(bp->lcbptr->clause_nr), 
	  GETNUMBER(bp->lcbptr->lit_nr)); 
}


/******************************************************
 *	print_literal
 *
 *	print the head-literal of a clause. Its environment
 *	pointer is given
 *	all output is to be made into the file ``filep''
 *
 ******************************************************/
void print_literal(filep,bp,negated)
FILE		*filep;
environ 	*bp;
int		negated;
{
  WORD psymb;
  int arity;
  int i;
  
  /* calculate the correct sign	*/
  psymb = bp->lcbptr->ps_symb;
  if (negated){
    GENSYMBOL(psymb,GETSYMBOL(psymb),GETTAG(psymb));
    CHANGE_THE_SIGN(psymb);
  }
  
  if (ISNEGATED(psymb)){
    /* print a ~ */
    fputc('~',filep);
  }
  
  DEBUG2(printf("ps_symb= %lx\n",bp->lcbptr->ps_symb));
  DEBUG2(printf("GETSYMBOL(ps_symb)= %lx\n",GETSYMBOL(bp->lcbptr->ps_symb)));
  DEBUG2(printf("ISNEGATED(GETSYMBOL(ps_symb))= %lx\n",ISNEGATED(GETSYMBOL(bp->lcbptr->ps_symb))));
  /* print the predicate symbol	*/
  fprintf(filep,"%s",GETPRINTNAME(bp->lcbptr->ps_symb));
  
  /* are there terms ? */

  arity = GETARITY(bp->lcbptr->ps_symb);
  
  if (arity > 0){
    fputc('(',filep);
    
    for (i=0; i<arity;i++){
      /* display the current term */
      disp_(filep,
	    code + GETVAL(bp->lcbptr->argv) +i,
	    bp);
      /* separate the items with a ',' */
      if (i < arity-1){
	fputc(',',filep);
      }
    }
    
    fputc(')',filep);
  }
  
}


/******************************************************
 *	disp_proof_tree
 *	= treelist
 ******************************************************/
void disp_proof_tree(filep,bp,indent)
environ		*bp;
FILE		*filep;
int 		indent;
{

  environ     *lbp;
  WORD        *sg_i;
  WORD        *sg_i_help;
  WORD        *sg_list;
  int 		nr_sgls;
  int 		i;
  int 		hide;
  
  
  DEBUG(printf("disp_proof_tree: bp=%lx\n",bp);)
    

  /* print the head literal, unless it is the query */
  if (bp != (environ *)stack) {
    do_indent(filep,indent);
    fprintf(filep,"[ ");
    print_literal(filep,bp,0);
    fprintf(filep," ]");
  }

  nr_sgls = GETNUMBER(bp->lcbptr->nr_sgls);

  /* check, if there are subgoals */
  if (!nr_sgls){
    /* there are no subgoals of this clause	*/
    
    DEBUG(printf("no subgoals\n");)
	
    eoln(filep);
    return;
  }

  if (bp != (environ *)stack) {
    eoln(filep);
  }

  /* proceed thru all subgoals and display them (if they are open) */
  /* get the beginning of the list of subgoal-infos */
  sg_i =  (WORD *)bp + SIZE_IN_WORDS(environ) 
    + GETVAL(bp->lcbptr->nr_vars);
  
  
  
  DEBUG(printf("%d subgoals\n",nr_sgls);)
  DEBUG(printf("sg_i=%lx\n",sg_i);)
  DEBUG(printf("(environ*) GETPTR(*sg_i)=%lx\n",(environ*) GETPTR(*sg_i));)
  DEBUG(printf("tp=%lx\n",bp->tp);)


  sg_list = code + GETVAL((bp->lcbptr)->subgoal_list);
  for (i=0; i< nr_sgls; i++){
    hide=0;
    /* process subgoal #i */
    if (sg_i <= bp->tp){
      /* then we have a subgoal which has been processed
       * already	*/
      do_indent(filep,indent);
      
      switch(GETTAG(*sg_i)) {
	
      case EXTENSION:
	
	/* this was an extension step	*/
	lbp = (environ*) GETPTR(*sg_i);
	hide=must_hide(lbp);
	if (hide){
		break;
		}
	
 	if (bp != (environ *)stack) {
		fprintf(filep,", [");
		}
	else {
		fprintf(filep,"[");
		}

	/* print the negatedliteral with clause/lit info */
	print_literal(filep,lbp,1);
	fprintf(filep,",");
	print_lit_info(filep,lbp,bp,sg_list);
        /* 	lbp->flags = (lbp->flags & ~VALMASK) | GETVAL(ptree_inf_counter) ; */
        GENOBJ(lbp->flags,ptree_inf_counter,T_EMPTY);
	
	fprintf(filep,",[");
	
	eoln(filep);
	
	/* this was an inference		*/
	ptree_inf_counter++;
	
	/* call the printing routine recursively */
	disp_proof_tree(filep,lbp,indent+INDENT_STEP);
	
	/* close the brackets again */
	do_indent(filep,indent);
	fprintf(filep,"] ]"); 
	
	break;
	
	
      case REDUCTION:
	
	/* this was a reduction step	 */
#ifdef STATISTICS1
	reductions_in_proof++;
#endif
	
	lbp = (environ*) GETPTR(*sg_i);
	DEBUG(printf("reduction\n");)
	fprintf(filep,", [");
	print_literal(filep,lbp,0);
	
	fprintf(filep,",");
	
	print_reduction_info(filep,lbp,bp,sg_list);
	fprintf(filep,"]");
	
	/* this was an inference		*/
	ptree_inf_counter++;
	
	break;

      case FACTORIZATION_PESS:
	
	/* this was a factorization step	 */
#ifdef STATISTICS1
	folding_statistics.factorizations_in_proof++;
#endif
	
	lbp = (environ*) GETPTR(*sg_i);
	DEBUG(printf("factorization\n");)
	fprintf(filep,", [");
	print_literal(filep,lbp,1);
	
	fprintf(filep,",");
	
	print_factorization_info(filep,lbp,bp,sg_list);
	fprintf(filep,"]");
	
	/* this was an inference		*/
	ptree_inf_counter++;
	
	break;
	
      case FACTORIZATION_OPT:
	
	/* this was a factorization step	 */
#ifdef STATISTICS1
	folding_statistics.factorizations_in_proof++;
#endif
	
	/* we have to look for the last folded-down literal in a chain
	   of repeated applications of opt. factorization steps */
	
	sg_i_help = (WORD*)(GETPTR(*sg_i));
	while( GETTAG(*sg_i_help) == FACTORIZATION_OPT )
	  sg_i_help = (WORD*)(GETPTR(*sg_i_help));
	lbp = (environ*) GETPTR(*sg_i_help);
	
	DEBUG(printf("factorization\n");)
	fprintf(filep,", [");
	print_literal(filep,lbp,1);
	
	fprintf(filep,",");
	
	print_factorization_info2(filep,lbp,bp,sg_list);
	fprintf(filep,"]");

	/* this was an inference		*/
	ptree_inf_counter++;
	
	break;
        }
    }
    else {
      /* this is a subgoal which has not yet been touched */
      
      fprintf(filep, ", not yet touched\n");
      
      /* we only print these subgoals, if a full tree
       * is requested					*/
      
    }
    sg_i++;
    sg_list++;
    
    
    eoln(filep);
  }
  eoln(filep);
}



/******************************************************
 *	must_hide
 ******************************************************/
static	int must_hide(lbp)
environ 	*lbp;
{
t_hidden_preds	*thp;

if (!hidden_preds){
	return 0;
	}

for (thp=hidden_preds;thp;thp=thp->next){
	if (!strcmp(GETPRINTNAME(lbp->lcbptr->ps_symb),thp->name)){
		return 1;
		}
	}
return 0;
}
