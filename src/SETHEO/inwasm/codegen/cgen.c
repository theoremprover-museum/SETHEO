/****************************************************
;;; MODULE NAME : cgen.c
;;;
;;; PARENT      : main.c
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)cgen.c	15.1  01 Apr 1996
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 15.5.91
;;;
;;; DESCRIPTION : frame of code generation
;;;
;;; REMARKS     : Modified by Mayr 22.2.93    the number of strvars are computed for the clauses  
;;;		4.6.93	jsc	new structure for built-ins
;;;		1.7.93  jsc	places for code-generation for built-ins
;;;				and other options
;;;				Integration of hamdi-code:EXTENDED_CONSTRAINTS
;;;				globals and externals ->extern.h
;;;		2.7.93		jsc	merged 8.1.1.1 (setsubgoals)
;;;		21.7.93		jsc	debugging
;;;		23.7.93		jsc	cgen directly calls cg_fail_fact
;;;		14.12.93	jsc		purity, fold
;;;					- setsubg for partialtree in
;;;					  default query
;;;		28.7.94		jsc	-interactive -searchtree
;;;             22.12.93        hamdi   int constrnum ...
;;;		12.01.94	hamdi	EXTENDED_CONSTR... removed
;;;		17.8.94		jsc	static & integr
;;;             9.3.95          jsc     setsubg is not a instruction
;;;
****************************************************/

#include "types.h"
#include "extern.h"
#include <stdio.h>

#define DELETED (-9999)

/****************************************************/
/* statics                                          */
/****************************************************/
// JSC2007   static int constrnum = 1;

/****************************************************/
/* functions (codegen/cgen.c)                       */
/****************************************************/
void gencode();
void cg_modules();
void cg_module();
void cg_clause();
void cg_start();
void cg_rest();
int check_head();

/****************************************************/
/* gencode()                                        */
/****************************************************/
void gencode(matrix)
modtype *matrix;
{
  
  cgin_init(inwasm_flags.reduct, ishorn);
  
  cg_modules(matrix);
  
  /* weak unification graph is dumped */
  cg_graph(matrix);
  
  /* copy bounds, reduction facts, includes */
  cg_rest();
  
  /* symbtab is dumped */
  cg_symbtab();
  
  /* finish the code-generation */
  cgin_finish();
}

/****************************************************/
/* cg_modules()                                     */
/****************************************************/
void cg_modules(module)
  modtype *module;
{
  while (module) {
    if (module->m_list) {
      cg_modules(module->m_list);
      cgin_nextcoment(1);
    }
    cg_module(module);
    
    module = module->next;
    if (module)
      cgin_nextcoment(1);
  }
}

/****************************************************/
/* cg_module()                                      */
/****************************************************/
void cg_module(module)
  modtype *module;
{
  claustype *clause;
  int lcpybd;
  int lreduct = 0;
  int lcpylimit = inwasm_flags.cpylimit;
  
  /* get local valid strategy */
  lreduct = do_it(module,"redsteps",CONTROL);
  lcpybd  = get_copy_bound(module,&lcpylimit);
  
  if (ishorn && lreduct)
    warning("","Using reduction steps in local horn formula is useless");
  
  /* global reductionsteps ? */
  lreduct |= inwasm_flags.reduct;
  
  clause = module->c_list;
  while (clause) {
    /* assign each variable its offset and n.th occurrence */
    clause->nrvars = nrvars = adr_var(clause);
    
    /* the number of different strvars is saved only at the  */
    /* original clause - the number was computed in adr_var() */
    clause->nrstrvars = nrstrvars;
    
    /* intstructions */
    if (clause == startclause) {
      cg_start(clause,lreduct,lcpybd,lcpylimit);
    }
    else {
      constrnum = 1; /* init constr.-counter */
      cg_clause(clause,clause->clnr,lreduct,lcpybd,lcpylimit);
    }
    
    /* argumentvectors + literal control blocks for all contrapositives */
    cg_argvectors(clause);
    
    /* constraintterms */
    cg_constrainterms(clause);
    
    /* query variable */
    if (inwasm_flags.qvariable && !clause->p_list->symb 
	&& clause->clnr > 0) 
      cg_queryvarlab(clause->clnr);
    
    /* terms with functions */
    cg_struct();
    clause = clause->next;
    if (clause) 
      cgin_nextcoment(0);
  }
}

/**********************************************************/
/* cg_clause()                                            */
/* 	generate code for a contrapositive or a clause    */
/*	Note: If the clause is fanned, this function first*/
/*	generates code for the first contrapositive then  */
/*	it calls cg_clause for clause->fann.              */
/**********************************************************/
void cg_clause(clause,clnr,lred,lcpy,lcpylimit)
  claustype *clause;
int clnr;
int lred;
int lcpy;
int lcpylimit;
{
  claustype *cp;
  predtype *pp = clause->p_list;
  int nrgoal = clause->nr_lits - clause->nr_built - 1;
  int head_nalloc = 0;

  if (nrgoal < 0)
    nrgoal = 0;
  

  if (clause->del != DELETED) {
    
    /***********************************************/
    /* begin a clause:                             */
    /* generate a pseudo operation                 */
    /* 	.clause clnr,litnr                     */ 
    /***********************************************/
    cgin_clause(pp);
    
    /***********************************************/
    /* generate the clause-label                   */
    /* lxxxxxx:                                    */
    /***********************************************/
    new_cllab(pp);
    
    /***********************************************/
    /* do pre-alloc stuff                          */
    /***********************************************/
    
    /* copy bound */
    if (lcpy) 
      cg_cpybd(clause,lcpylimit);
    
    /***********************************************/
    /* generate the [n]alloc instruction           */
    /***********************************************/
    if (inwasm_flags.ignorelits) {
      head_nalloc = check_head(clause);
    } else {
      head_nalloc = 0;
    }
    cgin_alloc(nrvars,nrgoal,pp->claus->clnr,pp->litnr,head_nalloc);
    
    /***********************************************/
    /* do post-alloc, pre-unify stuff              */
    /***********************************************/
    
    
    /***********************************************/
    /* generate the unification instructions       */
    /***********************************************/
    cg_unifs(pp,nrgoal);
    
    /***********************************************/
    /* generate code for the constraints           */
    /***********************************************/
    if (clause->constr){
      cg_constraints(clause,clnr);
    }
    
    /***********************************************/
    /* generate genregs: for every call one genreg */
    /***********************************************/
    if (inwasm_flags.genreg && nrgoal > 0) 
      cg_genregs(clnr,clause->p_list->next);
    
    /***********************************************/
    /* do pre-call, post-unification stuff         */
    /***********************************************/
    
#	ifdef SETSUBGOALS_IS_INSTRUCTION
    /*
     * set subgoal-list for display of partial 
     * tableaux if we have at least one subgoal 
     */
    if (inwasm_flags.set_subgoal_list && (nrgoal > 0)){
      cgin_set_subgoal_list(clnr,pp->litnr);
    }
#	endif
    
    /*
     * stuff for the interactive SETHEO
     */
    if (inwasm_flags.interactive){
      if (nrgoal > 0){
	cgin_save_clause(pp->claus->clnr,pp->litnr);
      }
      else {
	cgin_save_fact();
      }
    }
    
    /***********************************************/
    /* generate code for the subgoals:             */
    /* 	- calls for extension steps            */
    /*	- code for built-ins                   */
    /***********************************************/
    cg_goals(pp->next,lred,clnr);
    
    /***********************************************/
    /* do stuff AFTER solving the current goal     */
    /***********************************************/
    /* 
     * display all variables of the query		
     */
    if (inwasm_flags.qvariable && !pp->symb && clause->clnr > 0) 
      cg_queryvar(clause->clnr);
    
    /* insert fold-up instructions */
    if (inwasm_flags.foldup && (nrgoal > 0)) {
      cgin_foldup();
    }
    if (inwasm_flags.foldupx && (nrgoal > 0)) {
      cgin_foldupx();
    }
    
    /***********************************************/
    /* generate the [n]dealloc instruction         */
    /***********************************************/
    cgin_dealloc(nrgoal);
    
    /***********************************************/
    /* do specific stuff for a contrapositive      */
    /* after its code generation                   */
    /***********************************************/
    /*
     * if we set the subgoal list, we also must add the
     * list of lcb's for the subgoals of the current 
     * contrapositive then generate the label for the list 
     */
    cgin_subgoal_list_label(clnr,pp->litnr);
    if (inwasm_flags.set_subgoal_list && (nrgoal > 0)){
      cg_gen_subgoal_list(clnr,pp->next);
    }
    
  }
  /* generate code for the fanned clauses */
  for (cp=clause->fann; cp; cp=cp->next) 
    cg_clause(cp,clause->clnr,lred,lcpy,lcpylimit);
  
}

/****************************************************/
/* cg_start()                                       */
/****************************************************/
/* generate code for startclause DEFAULTQUERY       */
void cg_start(clause,lred,lcpy,lcpylimit)
  claustype *clause;
int lred;
int lcpy;
int lcpylimit;
{
  int head_nalloc = 0;
  predtype *pp = clause->p_list;
  
  /* Note: this pp points to the list of subgoals of the default */
  /* query */
  
  loc_cpy |= inwasm_flags.cpybd;
  
  /* starting clause: default query */
  /* no real subgoals		  */
  
  /* l[np][0-9]3[0-9]2 */
  new_cllab(pp);
  
  /* .clause clnr,litnr */
  cgin_clause(pp);
  
  if (loc_cpy)
    cgin_cpset();
  
  if (nr_globals) {
    /* there are global variables, allocate space for them */
    cgin_galloc(nr_globals);
  }
  
  cgin_loadfcb();
  
  /* nalloc nrvars, lcb..      */
  if (inwasm_flags.ignorelits) {
    head_nalloc = check_head(clause);
  } else {
    head_nalloc = 0;
  }
  cgin_alloc(nrvars,0,pp->claus->clnr,pp->litnr, head_nalloc);
  
#	ifdef SETSUBGOALS_IS_INSTRUCTION
  /* setsubg xxxxx for the default query */
  if (inwasm_flags.set_subgoal_list){
    cgin_set_subgoal_list(clause->clnr,1);
  }
#	endif	
  
  /* set the save_clause for the default query */
  if (inwasm_flags.interactive){
#	ifdef SETSUBGOALS_IS_INSTRUCTION
    cgin_set_subgoal_list(clause->clnr,1);
#	endif
    cgin_save_clause(clause->clnr,1);
  }
  
  /* subgoals, built-ins */
  cg_goals(pp,lred,0);
  
  /* the default query has no contrapositives !! */
  
  if (inwasm_flags.ptree)
    cgin_disptree();
  
	/*
	 * for display of the search tree
	 */
  if (inwasm_flags.searchtree){
    cgin_label_ortree();
    cgin_fail();
  }
  
  
  if (inwasm_flags.all) 
    cgin_fail();
  else
    cgin_stop();
  
  
  /* if we have the partoial tree, we must create a */
/* one-element list of labels                     */
  /**/
  if (inwasm_flags.set_subgoal_list){
    /* generate the label for the list */
    cgin_subgoal_list_label(clause->clnr,1);
    cg_gen_subgoal_list(clause->clnr,pp);
  }
  
}

/****************************************************/
/* cg_rest()                                        */
/****************************************************/
/* copybounds, reductionfacts, */
/* only if necessary		*/
void cg_rest()
{
  
  if (loc_cpy && !cpylist)
    warning("","$Copies : wrong use");
  
  if (cpylist) 
    cg_copy();
  
  if (inwasm_flags.reduct || redlist) 
    cg_red();
  
  
  /* generate the failure fact, if necessary */
  cg_failure_fact();
  
  /***********************************************/
  /* generate code for additional inference      */
  /* rule, like                                  */
  /*	- random reordering                    */
  /*	- usage of lemmata                     */
  /***********************************************/
  cg_randomfact();
  
  /*
   * additional facts for the interactive mode
   */
  if (inwasm_flags.interactive){
    cgin_trymode_fact();
    cgin_exec_ps_fact();
  }
  
  
  cgin_dwfcb();
  
}


/****************************************************/
/* check_head()                                     */
/****************************************************/
int check_head(clause)
claustype *clause;
{
  int return_value = FALSE;
  predtype *pp = clause->p_list;
  int pos = clause->delpos;  

  /* printf("clnr: %d / delpos: %d\n",clause->clnr, pos); */
  if (pos<0) {
    if (pos == PRULE && pp->literal_flag == 1) {
      return_value = TRUE;
    } else {
      return_value = FALSE;
    }
  } else {
    while (--pos) {
      if (pp->literal_flag == 1) {
	return_value = TRUE;
      }
      pp = pp->next;
    }
  }

  return return_value;
}


/*######################################################################*/
