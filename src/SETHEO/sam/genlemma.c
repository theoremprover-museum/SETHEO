/*****************************************************************/
/*    S E T H E O                                                */
/*                                                               */
/* FILE: genlemma.c                                              */
/* VERSION:		@(#)genlemma.c	13.2 04/17/97                                  */
/* DATE:		6.6.94                                   */
/* AUTHOR:                                                       */
/* NAME OF FILE:                                                 */
/* DESCR:		utility functions for generate-lemmata   */
/*				originates from i_genlemma.c 5.2 */
/* MOD:                                                          */
/*		19.6.94		new printout of statistics       */
/*		7.11.94		print proof-tree for             */
/*				lemma                            */
/*		9.3.95	jsc	lemma-tree                       */
/*              6.4.97  jsc     bug fixed in match_lemma_inst/gen: */
/*                              did not reset tr                   */
/* BUGS:                                                         */
/*****************************************************************/

#include <stdio.h>
#include "path/path.h"
#include "unitlemma.h"
#include "symbtab.h"
#include "disp.h"
/**/
#include "genlemma.h"
#include "ptree.h"
#include "unitlemma.h"
#include "deref.h"
#include "match.h"


#ifdef UNITLEMMA
/***************************************************
 * statics
 ***************************************************/
static match_lemma_gen();
static match_lemma_inst();
static WORD *umlemma_pseudo_alloc();

/***************************************************
 * local macros
 ***************************************************/
/*#define DEBUG(X)	X; fflush(stdout); */
#define DEBUG(X)
#define VERBOSE(X)	X

/***************************************************
 * head_is_unit_lemma
 *
 * returns: 1 if true, 0 otherwise
 * Note:	FOLD_UP must be defined
 *			otherwise, 1 is returned always
 ***************************************************/
int head_is_unit_lemma()
{
#ifdef FOLD_UP
  environ *pdp;
  
  /* search for minimal context of lemma */
  pdp = bp->dyn_link;
  while(pdp && pdp != (environ *) stack && pdp->red_marking < bp)
    pdp = pdp->path_link;
  
  /* do we have a unit lemma ? */
  if(pdp != (environ *) stack ){
    /* no, it is no unit lemma */
    return 0;
  }
#endif
  
  return 1;
}


/***************************************************
 * more_general_in_index
 *
 * returns: 1 if for the given literal in ga, more
 *				general unit-lemmata exist
 *			0 otherwise
 *
 * Note:	the given literal is matched with those
 *			unit-lemmata only which are NOT marked
 *			deleted
 * Note:
 *		updates the statistics
 ***************************************************/
int more_general_in_index(ga)
WORD *ga;
{
  path_QUERY root;
  POINTER path_ptr;
  
  unit_lemma_storage_ctrl_block *lemma;
  
  
  DEBUG(printf("more_general_in_index:...>>\n"));
  DEBUG(disp_(stdout,ga,bp));
  DEBUG(printf("<<\n"));
  
  /* generate the query-tree */
  root = path_QueryCreate(unitlemma_index,GENERALIZATION_MIN,ga);
  
  if ( (path_ptr = path_QueryApply(root)) ) {
    /* yes, there is already at least one in the index */
    DEBUG(printf("more_general_in_index: more general lemma(s) present\n"));
    DEBUG(printf("match thru the found terms\n"));
    while (path_ptr){
      /* and match , if the lemma is not deleted */
      lemma= (unit_lemma_storage_ctrl_block*)(path_ptr);
      DEBUG(print_compiled_fact(stdout,lemma->ulemma));
      
      if ((lemma->status != lemma_deleted) && 
	  (lemma->status != lemma_deleted_not_used)) {
	/* the lemma is not deleted */
	UL_STAT(ul_stat.gl_gen_index++);
	if (match_lemma_gen(ga,lemma->ulemma)) {
	  /* yes, there is a true more general one in the index */
	  DEBUG(printf("a true more general one found\n"));
	  UL_STAT(ul_stat.gl_gen_found++);
	  path_QueryDelete(root);
	  return 1;
	}
      }
      /* that one was not more general */
      /* we have to try the others */
      path_ptr = path_QueryApply(root);
    }
  }
  /* no ``more general'' lemma could be found */
  path_QueryDelete(root);
  return 0;
}

/***************************************************
 * delete_instances_in_index
 *
 * given a literal, all instances thereof are
 * deleted
 * Returns:
 *		minimum of choice-points of the deleted
 *		lemmata, NULL otherwise
 *
 * Note:
 *		updates the statistics
 *		and unitlemma_nr_valid
 ***************************************************/
choice_pt * delete_instances_in_index(ga)
WORD *ga;
{
  path_QUERY root;
  POINTER path_ptr;
  choice_pt *min_chp;
  unit_lemma_storage_ctrl_block *lemma;
  
  /* up to now, no used lemma deleted */
  min_chp = NULL;  
  
  root = path_QueryCreate(unitlemma_index,INSTANCE_MIN,ga);
  
  if ((path_ptr = path_QueryApply(root)) != NULL){
    /* there is at least one instance */
    /* we proceed thru the list of retrieved lemmata */
    DEBUG(printf("delete_instances_in_index: instantiation(s) present\n"));
    while (path_ptr){
      /* proceed thru all INSTANCE-lemmata found in the index */
      lemma=(unit_lemma_storage_ctrl_block*)(path_ptr);
      DEBUG(print_compiled_fact(stdout,lemma->ulemma));
      
      /* we take only those, which are NOT already deleted */
      if ((lemma->status != lemma_deleted) && 
	  (lemma->status != lemma_deleted_not_used)) {
	/* the lemma is not deleted */
	UL_STAT(ul_stat.gl_inst_index++);
	if (match_lemma_inst(ga,lemma->ulemma)) {
	  /* yes, there is a true instance in the index */
	  DEBUG(printf("a true instance found\n"););
	  UL_STAT(ul_stat.gl_inst_found++);
	  
	  /* mark it as ``deleted''           */
	  /* and update the number of entries */
	  unitlemma_nr_valid--;
	  
	  if (lemma->status == lemma_used){
	    /* mark the lemma as deleted               */
	    /* and remember its choice_point           */
	    /* if it is smaller than the previous ones */
	    lemma->status = lemma_deleted;
	    if (!min_chp) min_chp = lemma->chp;
	    else if (min_chp > lemma->chp)
	      min_chp = lemma->chp;
	    DEBUG(printf("delete_inst..x: new min_chp=\lx",min_chp));
	  }
		    
	  if (lemma->status == lemma_valid)
	    lemma->status = lemma_deleted_not_used;
	  
	}
      }
      /* try the next one(s) */
      path_ptr = path_QueryApply(root);
    }
  }
  
  /* clean up the query tree */
  path_QueryDelete(root);
  
  return min_chp;
}


/******************************************************
 * match lemma gen
 *
 * matches lemma as RHS 
 * and given term as LHS
 * returns:
 *	1 if RHS is more general than LHS
 *	0 otherwise
 *
 ******************************************************/
static int match_lemma_gen(ga,ulemma)
WORD *ga;
unit_lemma_ctrl_block *ulemma;
{
  s_trail *ltr;
  WORD *ha;
  int rv, arity;
  /* prepare for matching, save trail-pointer */
  ltr=tr;
  /* prepare the variables of the stored term */
  umlemma_pseudo_alloc(ulemma);
  
  /*
   * now, proceed thru all terms of the
   * predicate and match
   */
  arity = GETARITY(*ga);
  rv = 1;
  /* ga now points to first argument
   */
  ga++;
  /* ha now points to first argument (argv[0]) of lemma
   */
  ha = (WORD*)ulemma + ARGV_OFFSET(arity);
  
  while (arity--){
    
    DEBUG(printf("match_lemma: comparing arguments:\n"));
    DEBUG(disp_(stdout,ga,bp));
    DEBUG(printf("\n<->\n"));
    DEBUG(disp_(stdout,ha,(environ *)sp));
    DEBUG(printf("\n"));
    if (!(rv = match(ga,bp,ha,(environ*)sp))){
      /*
       * no match 
       */
      DEBUG(printf("match_lemma: no match\n"));
      break;
    }
    ga++;
    ha++;
  }
  DEBUG(printf("match_lemma:clean up\n"));
  untrail(ltr);
  tr=ltr;
  
  DEBUG(printf("match_lemma_gen: returns %d\n",rv));
  return rv;
}


/******************************************************
 * match lemma inst
 *
 * matches lemma as RHS 
 * and given term as LHS
 * returns:
 *	1 if RHS is more instantiated than LHS
 *	0 otherwise
 *
 ******************************************************/
static int match_lemma_inst(ga,ulemma)
WORD *ga;
unit_lemma_ctrl_block *ulemma;
{
  s_trail *ltr;
  WORD *ha;
  int rv, arity;
  /*
   * prepare for matching
   *
   * save trail-pointer
   */
  ltr=tr;
  /*
   * prepare the variables of the stored term
   */
  umlemma_pseudo_alloc(ulemma);

  /*
   * now, proceed thru all terms of the
   * predicate and match
   */
  arity = GETARITY(*ga);
  rv = 1;
  /* ga now points to first argument
   */
  ga++;
  /* ha now points to first argument (argv[0]) of lemma
   */
  ha = (WORD*)ulemma + ARGV_OFFSET(arity);
  
  while (arity--){
    
    DEBUG(printf("match_lemma_inst: comparing arguments:\n"));
    DEBUG(disp_(stdout,ga,bp));
    DEBUG(printf("\n<->\n"));
    DEBUG(disp_(stdout,ha,(environ *)sp));
    DEBUG(printf("\n"));
    if (!(rv = match(ha,(environ*)sp,ga,bp))){
      /*
       * no match 
       */
      DEBUG(printf("match_lemma_inst: no match\n"));
      break;
    }
    ga++;
    ha++;
  }
  DEBUG(printf("match_lemma_inst:clean up\n"));
  untrail(ltr);
  tr=ltr;
  
  DEBUG(printf("match_lemma_inst: returns %d\n",rv));
  return rv;
}

/*****************************************************
 * uemma_pseudo_alloc
 *
 * allocate space for variables of the given lemma
 * (only T_FVAR cells without CONSTRAINT pointers)
 * on the stack after the current sp.
 * returns the new stack-pointer
 * Note: Does NOT modify sp
 *
 *****************************************************/
static WORD *umlemma_pseudo_alloc(fact)
unit_lemma_ctrl_block *fact;
{
  WORD *lsp = sp;
  int nr_vars;

  /* proceed over the normal environment
   */
  lsp += SIZE_IN_WORDS(environ);
  
  nr_vars = GETNUMBER(fact->lcb.nr_vars);
  
  while (nr_vars--){
    GENOBJ(*lsp,0,T_FVAR);
    lsp++;
  }
  return lsp;
}


/******************************************************
 * unitlemma_statistics_init
 ******************************************************/
void unitlemma_statistics_init()
{
#ifdef UNITLEMMA_STATISTICS
	ul_stat.genlemma_entered =
	ul_stat.gl_gen_found	 =
	ul_stat.gl_gen_index	 =
	ul_stat.gl_inst_found	 =
	ul_stat.gl_inst_index	 =
	ul_stat.gl_lemma_stored  =
	ul_stat.gl_failure_exit	 =
	ul_stat.uselemma_entered =
	ul_stat.ul_no_unifiable  =
	ul_stat.ul_lemma_pushed  = 0;
#endif
}

/******************************************************
 * unitlemma_statistics_print
 ******************************************************/
void  unitlemma_statistics_print()
{

#ifdef UNITLEMMA_STATISTICS
	
    if (ul_stat.genlemma_entered  ||  ul_stat.genlemma_no_unit_lemma)
	PRINT_LOG2("Genlemma entered/no-unitlemma      : %8d/%8d\n",
		   ul_stat.genlemma_entered, ul_stat.genlemma_no_unit_lemma);
    if (    ul_stat.gl_gen_index   ||  ul_stat.gl_gen_found
	||  ul_stat.gl_inst_index  ||  ul_stat.gl_inst_found)
	PRINT_LOG4("    - GEN/w.match/INST/w.match     : %8d/%8d/%8d/%8d\n",
		   ul_stat.gl_gen_index,ul_stat.gl_gen_found, 
		   ul_stat.gl_inst_index,ul_stat.gl_inst_found);
    if (ul_stat.gl_lemma_stored  ||  unitlemma_nr_valid)
	PRINT_LOG2("    - stored/Nr. in Index          : %8d/%8d\n",
		   ul_stat.gl_lemma_stored,unitlemma_nr_valid);
    if (    ul_stat.uselemma_entered  ||  ul_stat.ul_no_unifiable
	||  ul_stat.ul_lemma_pushed)
	PRINT_LOG3("Uselemma entered/no/Nr. pushed     : %8d/%8d/%8d\n",
		   ul_stat.uselemma_entered,ul_stat.ul_no_unifiable,
		   ul_stat.ul_lemma_pushed);
    
    if (verbose){
	memory_Print();
    }
#endif
}

/***************************************************
 *	ul_print_tree
 *	print proof tree for the given unitlemma
 *	into a given file.
 *	filep:	where to write
 *	lemma:	the lemma (already compiled)
 *	bp:	root-bp for that subtree
 ***************************************************/
void ul_print_tree(filep,lemma,lbp)
FILE *filep;
unit_lemma_storage_ctrl_block *lemma;
environ *lbp;
{
  fprintf(filep, "# /*-%d (%d) -*/ ", GETNUMBER(lemma->ulemma->lcb.clause_nr),
	  GETNUMBER(lemma->data));
  
  fprintf(filep, "\n[[ ");
  /* print negated lemma */
  print_compiled_term(filep,lemma->ulemma,1);
  fprintf(filep, ",[0, lemma__(%d.1,%d)],[\n", 
	  GETNUMBER(lemma->ulemma->lcb.clause_nr),
	  GETNUMBER(lemma->data));
  disp_proof_tree(filep,lbp,0);
  fprintf(filep, "]]].\n");
}

#endif
