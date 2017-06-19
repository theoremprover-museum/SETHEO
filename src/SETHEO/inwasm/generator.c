/*****************************************************************************
;;; MODULE NAME : generator.c
;;;
;;; PARENT      : main.c
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)main.c	15.2 01 Apr 1996
;;;
;;; AUTHOR      : Rudolf Zeilhofer
;;;
;;; DATE        : 18.02.97
;;;
;;; DESCRIPTION : parses files and generates weak unification / constraints /..
;;;
;;; REMARKS     :
******************************************************************************/


#include "types.h"
#include "extern.h"

/*****************************************************************************/
/* methods                                                                   */
/*****************************************************************************/
void parse_input_file();
void parse_file();
void check_horn_formula();     
void generate();  
void generate_code();
int  generate_variable_enumeration();
int  generate_var_enum_predsym();
void generate_weak_unification();
void generate_purity();
void generate_sortarray();
void generate_orbranch_reordering();
void generate_tautology_constraints();
void generate_subsumption_constraints();
void generate_fanning();
void generate_subgoal_weights();
void generate_reduction_steps();
void generate_symmetry_constraints();
void generate_overlap_constraints();
void generate_subgoal_reordering();
void constraintSubsumption();
void constraintSubsumptionAfterSymmetry();
void constraintSubsumptionAfterOverlap();
void optimizeConnections();
void set_fcb();


/*****************************************************************************/
/* parse_input_file()                                                        */
/* Description: calls the functions for parsing the input file and checking  */
/*              for horn formulas                                            */
/*****************************************************************************/
void parse_input_file()
{
  parse_file();
  check_horn_formula();
}


/*****************************************************************************/
/* parse_file()                                                              */
/* Description: parses the specified input_file, generate symboltable and    */
/*              matrix                                                       */
/*****************************************************************************/
void parse_file()
{
  printf("  Parsing input-file.\n"); 
  if (yyparse()) {
    fprintf(stderr,"  Error: Too many errors during parsing. Cannot continue.\n");
    exit(30);
  }
  parsed_nr_clauses = nr_clauses;
}


/*****************************************************************************/
/* check_horn_formula()                                                      */
/* Description: checks input file for horn formulas and alters parameters    */
/*              if necessary                                                 */
/*****************************************************************************/
void check_horn_formula()
{
  ishorn = ishorn && negative_goals(matrix->c_list);

  /* horn formula => by default: no fanning */
  if (ishorn && inwasm_flags.fan) {
    inwasm_flags.fan = 0;
  }

  /* horn formula => no reduction steps except for folding up and down */
  if (ishorn && inwasm_flags.reduct) 
    if (!inwasm_flags.foldup && !inwasm_flags.foldupx &&
	!inwasm_flags.folddown && !inwasm_flags.folddownx) {
      inwasm_flags.reduct = 0;
    }
  if (ishorn && !inwasm_flags.reduct) 
    if (inwasm_flags.foldup || inwasm_flags.foldupx ||
	inwasm_flags.folddown || inwasm_flags.folddownx) {
      inwasm_flags.reduct = 1;
    }
  
  if (nump > MAXPREDSYMB-1) {
    warning("","Too many predicate symbols");
  }
}


/*****************************************************************************/
/* generate()                                                                */
/* Description: generates weak unification, constraints, ...                 */
/*****************************************************************************/
void generate()
{
  int afterFanning = FALSE;

  VariableEnum = 0;  
  VariableEnum = generate_variable_enumeration(VariableEnum, afterFanning);

  /* generate the weak unification table */
  generate_weak_unification();

  matrix->c_list->connections = 0;

  generate_purity();
  generate_sortarray();

  generate_orbranch_reordering();

  /* generate constraints before fanning */
  generate_tautology_constraints();
  generate_subsumption_constraints(); 

  /* subsume unnecessary constraints */
  constraintSubsumption();

  /* optimize connections - if a clause is deleted, test for purity again */
  optimizeConnections(); 

  /* set up statistics */
  set_fcb(); 

  /* fann the clauses */
  generate_fanning();

  afterFanning = TRUE;

  /* don't forget to enumerate the fanned clauses */
  VariableEnum = 0;
  VariableEnum = generate_variable_enumeration(VariableEnum, afterFanning);     

  /* generate subgoal weights and insert reduction steps */
  generate_subgoal_weights();
  generate_reduction_steps();

  /* generate symmetry constraints */
  generate_symmetry_constraints(); 
 
  /* test for redundant constraints */
  constraintSubsumptionAfterSymmetry();
 
  /* not yet implemented
  generate_overlap_constraints();
  constraintSubsumptionAfterOverlap();
  */

  /* reorder subgoals */
  generate_subgoal_reordering();
}


/*****************************************************************************/
/* generate_code()                                                           */
/* Description: generates .hex / _pp.lop and .s codes                        */
/*****************************************************************************/
void generate_code()
{
  if (!inwasm_flags.nocode && !errors) {
    printf("  Codegeneration.\n");
    count_codenr();
    gencode(matrix);
  }
  if (inwasm_flags.lop && !errors){
    lop_output(matrix,lopout_file);
  }
}


/*****************************************************************************/
/* generate_variable_enumeration()                                           */
/* Description: generates a diffrent number for each (G/S)-VARIABLE in the   */
/*              clauses. This is necessary for the unification. Eliminates   */
/*              dynamic renaming of variables.                               */
/*****************************************************************************/
int generate_variable_enumeration(pNumber, flag_fanning)
int pNumber;
int flag_fanning;
{
  claustype *clause_list;
  claustype *fann_list;
  predtype *predicate_list;
  predtype *constraint_list;
  termtype *term_list;
  int number = pNumber;  
  int clause_nr = 0;

  for (clause_list = matrix->c_list ; clause_list != NULL; 
       clause_list = clause_list->next) {
    clause_nr = clause_list->clnr;
    for (predicate_list = clause_list->p_list;
	 predicate_list != NULL; 
	 predicate_list = predicate_list->next) {
      for (term_list = predicate_list->t_list; term_list != NULL; 
	   term_list = term_list->next) {
	term_list->clause_number = clause_nr;
	if (term_list->t_list) {
	  number = 
	    generate_var_enum_predsym(clause_nr, number, term_list->t_list);
	}
	else {
	  term_list->variable_number = number; 	            
	  number++;
	}
      }
    }
    if (clause_list->constr) {
      constraint_list = clause_list->constr;
      while (constraint_list) {
	/* Constraints besitzen immer clause_number 1000 */
	number = 
	  generate_var_enum_predsym(1000, number, constraint_list->t_list);
	constraint_list = constraint_list->next;
      }
    }

    /* gefannte Komponenten durchbenennen */
    if (flag_fanning && clause_list->fann) {
      for (fann_list = clause_list->fann; fann_list != NULL; 
	   fann_list = fann_list->next) {
	clause_nr = fann_list->clnr;   
	for (predicate_list = fann_list->p_list; predicate_list != NULL; 
	     predicate_list = predicate_list->next) {
	  for (term_list = predicate_list->t_list; term_list != NULL; 
	       term_list = term_list->next) {
	    term_list->clause_number = clause_nr;
	    if (term_list->t_list) {
	      number = 
		generate_var_enum_predsym(clause_nr, number, term_list->t_list);
	    }
	    else {
	      term_list->variable_number = number; 	  	    
	      number++;
	    }
	  }
	}
	/* in den gefannten Constraints, die clause_numbers aktualisieren */
	if (fann_list->constr) {
	  constraint_list = fann_list->constr;
	  while (constraint_list) {
	    /* Constraints besitzen immer clause_number 1000 */
	    number = 
	      generate_var_enum_predsym(1000, number, constraint_list->t_list);
	    constraint_list = constraint_list->next;
	  }
	}
      }
    }
    /* printf("\n"); */
  }
  return number; 
}

/*****************************************************************************/

int generate_var_enum_predsym(pClause, pNumber, pTerm_list)
int pClause;
int pNumber;
termtype *pTerm_list;
{
  int number = pNumber;
  int clause_nr = pClause;
  termtype *term_list;

  for (term_list = pTerm_list ; 
       term_list != NULL ;
       term_list = term_list->next) {
    term_list->clause_number = clause_nr;
    if (term_list->t_list) {
      number = 
	generate_var_enum_predsym(clause_nr, number, term_list->t_list);
    }
    else {
      term_list->variable_number = number;            
      number++;
    }
  }
  return number;
}


/*****************************************************************************/
/* generate_weak_unification()                                               */
/* Description: generates weak unification prints output (verbose mode)      */
/*****************************************************************************/
void generate_weak_unification()
{
  printf("  Preprocessing: Generating weak-unification.\n");
  genwunif(); 
 
  /* verbose modes */
  if ((inwasm_flags.verbose == PF_PARS) || 
      (inwasm_flags.verbose == PF_ALWAYS)) 
    print_lop_formel(matrix, stdout, 1, 0);
}


/*****************************************************************************/
/* generate_purity()                                                         */
/* Description: generates purity                                             */
/*****************************************************************************/
void generate_purity()
{
  int return_value;

  if (inwasm_flags.purity)  {  
    printf("  Preprocessing: purity");
    return_value  = genpurity(matrix);
    /* return_value <> 0 iff purity removed query */
    printf("\n");
    if (nr_clauses == 0 || return_value) {
      fprintf(stderr, "  Purity removed all clauses from matrix.\n");
      if (!inwasm_flags.nocode) {
	fprintf(code_file, "fail\n");
      }   
      exit(40);
    }
    /* verbose modes */
    if ((inwasm_flags.verbose == PF_PUR) || 
	(inwasm_flags.verbose == PF_ALWAYS)) 
      print_lop_formel(matrix, stdout, 1, 0);
  }
}


/*****************************************************************************/
/* generate_sortarray()                                                      */
/* Description: generates sortarray                                          */
/*****************************************************************************/
void generate_sortarray()
{
  if (inwasm_flags.clausreord || inwasm_flags.linksubs || inwasm_flags.subs) {
    printf("        Message: Making sortarray.\n"); 
    make_sortarray(matrix->c_list->next);
  }
}


/*****************************************************************************/
/* generate_orbranch reordering()                                            */
/* Description: generates orbranch reordering                                */
/*****************************************************************************/
void generate_orbranch_reordering()
{
  if (inwasm_flags.clausreord) {  
    printf("  Preprocessing: orbranch reordering \n");
    orbranch_reorder(matrix->c_list);
    /* verbose modes */
    if ((inwasm_flags.verbose == PF_CLREORD) || 
	(inwasm_flags.verbose == PF_ALWAYS)) { 
	 print_wunif(matrix,stdout,0);
    }
  }  
}

/*****************************************************************************/
/* generate_tautology_constraints()                                          */
/* Description: generates tautology constraints                              */
/*****************************************************************************/
void generate_tautology_constraints() 
{
  if (inwasm_flags.tauto) {
    printf("  Preprocessing: inserting tautology-constraints \n");
    matrix->c_list = gen_taut_constr(matrix->c_list);
    /* verbose modes */
    if (inwasm_flags.verbose == PF_TCONSTR || 
	inwasm_flags.verbose == PF_ALWAYS) 
      print_lop_formel(matrix,stdout,1,0);
  }
}

/*****************************************************************************/
/* generate_subsumption_constraints()                                        */
/* Description: generates subsumption constraints                            */
/*****************************************************************************/
void generate_subsumption_constraints() 
{
  if (inwasm_flags.subs) {
    printf("  Preprocessing: inserting subsumption-constraints\n");
    matrix->c_list = gen_s_constr(matrix->c_list, inwasm_flags.subs);
    /* verbose modes */
    if (inwasm_flags.verbose == PF_SCONSTR || 
	inwasm_flags.verbose == PF_ALWAYS) 
      print_lop_formel(matrix, stdout, 1, 0);
 }
}


/*****************************************************************************/
/* constraintSubsumption()                                                   */
/* Description: subsumes unnecessary constraints                             */
/*****************************************************************************/
void constraintSubsumption() 
{
  if (inwasm_flags.tauto || inwasm_flags.subs) {
    printf("  Preprocessing: removing redundant constraints\n");
    gen_constr_subs(matrix->c_list);
    if (inwasm_flags.verbose == PF_FIRSTDEL || 
	inwasm_flags.verbose == PF_ALWAYS) 
      print_lop_formel(matrix, stdout, 1, 0);
  } 
}


/*****************************************************************************/
/* optimizeConnections()                                                     */
/* Description: test connections                                             */
/*****************************************************************************/
void optimizeConnections() 
{
  int tempReturnValue;

  if (inwasm_flags.linksubs) {
    printf("  Preprocessing: testing connections \n");
   
    opt_connections(matrix->c_list, inwasm_flags.linksubs);
    printf("        Message: %d connections tested", nr_links_tested);
    printf(" and %d deleted.\n",nr_del_conn);

    /* purtity after having deleted connections */
    if (nr_del_conn > 0) {
      printf("  Preprocessing: purity again");
      tempReturnValue = genpurity(matrix);
      /* tempReturnValue <> 0 iff purity removed query */
      printf("\n");
      if (nr_clauses == 0 || tempReturnValue) {
	fprintf(stderr,"  Purity removed all clauses from matrix.\n");
	if (!inwasm_flags.nocode) {	
	  fprintf(code_file,"fail\n");
	}
	exit(41);
      }
      /* verbose modes */
      if (inwasm_flags.verbose == PF_PUR || 
	  inwasm_flags.verbose == PF_ALWAYS) 
	print_lop_formel(matrix,stdout,1,0);
    }
  } 
  nr_clauses = parsed_nr_clauses;
}


/*****************************************************************************/
/* set_fcb()                                                                 */
/* Description:                                                              */
/*****************************************************************************/
void set_fcb()
{
  claustype *cl;

  fcb.nr_clauses = parsed_nr_clauses;
  fcb.nr_const =nr_constants;
  fcb.nr_fsymb =nr_functions;
  fcb.nr_predsymb=nr_predicates;
  fcb.max_arity_fsymb=max_arity_fsymb;

  for (cl=matrix->c_list; cl; cl=cl->next) {
    fcb.nr_connections += cl->connections; 

    if ((cl->p_list->symb == 0) || 
	(cl->parsed_rule_type == PQUERY)) {
      /* we have a query */
      if (cl->nr_lits == 1) {
	/* this is the artificial query, */
	/* we don't count this		 */
	continue;
      }
      fcb.nr_queries++;

      SET_MAX(fcb.max_lit_queries, cl->nr_lits-1);
      fcb.sum_lit_queries +=cl->nr_lits-1;
      fcb.nr_literals += cl->nr_lits-1;
      SET_MAX(fcb.max_vars_queries, cl->nrvars);
      SET_MAX(fcb.max_termdepth_queries, cl->max_termdepth);
      SET_MAX(fcb.max_termsize_queries, cl->max_termsize);
    }
    else {
      fcb.nr_literals += cl->nr_lits;
      if (cl->nr_lits == 1) {
	if (cl->poslits > 0) {
	  fcb.nr_punits++;
	  SET_MAX(fcb.max_vars_punits, cl->nrvars);
	  SET_MAX(fcb.max_termdepth_punits, cl->max_termdepth);
	  SET_MAX(fcb.max_termsize_punits, cl->max_termsize);
	}
	else {
	  fcb.nr_nunits++;
	  SET_MAX(fcb.max_vars_nunits, cl->nrvars);
	  SET_MAX(fcb.max_termdepth_nunits, cl->max_termdepth);
	  SET_MAX(fcb.max_termsize_nunits, cl->max_termsize);
	}
      }
      else {
	if (cl->poslits > 1 ) {
	  /* indefinite clause */
	  fcb.nr_indefclauses++;
	  fcb.sum_lit_indefclauses += cl->nr_lits;
	  fcb.sum_plit_indefclauses += cl->poslits;
	  SET_MAX(fcb.max_lit_indefclauses, cl->nr_lits);
	  SET_MAX(fcb.max_vars_indefclauses, cl->nrvars);
	  SET_MAX(fcb.max_termdepth_indefclauses, cl->max_termdepth);
	  SET_MAX(fcb.max_termsize_indefclauses, cl->max_termsize);
	  SET_MAX(fcb.max_plit_indefclauses, cl->poslits);
	}
	else {
	  /* definite clause */
	  fcb.nr_defclauses++;
	  fcb.sum_lit_defclauses += cl->nr_lits;
	  SET_MAX(fcb.max_lit_defclauses, cl->nr_lits);
	  SET_MAX(fcb.max_vars_defclauses, cl->nrvars);
	  SET_MAX(fcb.max_termdepth_defclauses, cl->max_termdepth);
	  SET_MAX(fcb.max_termsize_defclauses, cl->max_termsize);
	}
      }
    }
  }
}


/*****************************************************************************/
/* generate_fanning()                                                        */
/* Description: fann the clauses                                             */
/*****************************************************************************/
void generate_fanning() 
{
  if (inwasm_flags.fan && !ishorn) {
    printf("  Preprocessing: fanning   ");
    genfanning(matrix); 
    printf("\n");
    /* verbose modes */
    if ((inwasm_flags.verbose == PF_FAN) || 
	(inwasm_flags.verbose == PF_ALWAYS)) 
      print_lop_formel(matrix,stdout,1,0);
  }
}


/*****************************************************************************/
/* generate_subgoal_weights()                                                */
/* Description: generate subgoal weights                                     */
/*****************************************************************************/
void generate_subgoal_weights() {
  if ((inwasm_flags.sgreord) || (inwasm_flags.overlap))
    gen_subgoal_weights(matrix->c_list);
}


/*****************************************************************************/
/* generate_reduction_steps()                                                */
/* Description:  tell that reduct instructions are added                     */
/*****************************************************************************/
void generate_reduction_steps() {
  if (inwasm_flags.reduct) {
    printf("  Preprocessing: inserting reduction steps\n");
    if (inwasm_flags.nocode && lopout_file) {
      fprintf(lopout_file,"\n# inserting reduction steps\n");
    }
  }
}


/*****************************************************************************/
/* generate_symmetry_constraints()                                           */
/* Description: generate symmetry constraints                                */
/*****************************************************************************/
void generate_symmetry_constraints() {
  if ((!ishorn) && 
      (inwasm_flags.fan) &&
      (inwasm_flags.subs)) {
    printf("  Preprocessing: inserting symmetry constraints\n");
      gen_sym_constr(matrix->c_list);
    /* verbose modes */
    if ((inwasm_flags.verbose == PF_SYMCONSTR) || 
	(inwasm_flags.verbose == PF_ALWAYS)) 
      print_lop_formel(matrix,stdout,1,0);
  }
}


/*****************************************************************************/
/* constraintSubsumptionAfterSymmetry()                                      */
/* Description:                                                              */
/*****************************************************************************/
void constraintSubsumptionAfterSymmetry() {
  if ((!ishorn) && 
      (inwasm_flags.subs)) {
    printf("  Preprocessing: removing redundant constraints\n");
    gen_constr_subs(matrix->c_list);
    /* verbose modes */
    if ((inwasm_flags.verbose == PF_SECONDDEL) || 
	(inwasm_flags.verbose == PF_ALWAYS)) 
      print_lop_formel(matrix,stdout,1,0);
  }
}


/*****************************************************************************/
/* generate_overlap_constraints()                                            */
/* Description:                                                              */
/*****************************************************************************/
void generate_overlap_constraints() {
  if (inwasm_flags.overlap) {
    printf("  Preprocessing: inserting overlap constraints\n");
    matrix->c_list = 
      gen_overl_constr(matrix->c_list,
		       overlap_mode,
		       ((inwasm_flags.verbose == PF_OCONSTR) || 
			(inwasm_flags.verbose == PF_ALWAYS)) ? 1 : 0); 
    /* verbose modes */
    if ((inwasm_flags.verbose == PF_OCONSTR) || 
	(inwasm_flags.verbose == PF_ALWAYS)) {
      printf("  Output after removing DELETED-clauses:\n");
      print_lop_formel(matrix,stdout,1,0);
    }
  }
}


/*****************************************************************************/
/* constraintSubsumptionAfterOverlap()                                       */
/* Description:                                                              */
/*****************************************************************************/
void constraintSubsumptionAfterOverlap() {
  if (inwasm_flags.overlap) {
    printf("  Preprocessing: removing redundant constraints\n");
    gen_constr_subs(matrix->c_list);
    /* check verbose modes */
    if ((inwasm_flags.verbose == PF_THIRDDEL) || 
	(inwasm_flags.verbose == PF_ALWAYS)) 
      print_lop_formel(matrix,stdout,1,0);
  }
}


/*****************************************************************************/
/* generate_subgoal_reordering()                                             */
/* Description:                                                              */
/*****************************************************************************/
void generate_subgoal_reordering() {
  if (inwasm_flags.sgreord) {
    printf("  Preprocessing: subgoal reordering\n");
    sg_reordering(matrix->c_list);
    /* check verbose modes */
    if (inwasm_flags.verbose == PF_SGREORD || 
	inwasm_flags.verbose == PF_ALWAYS) 
      print_lop_formel(matrix,stdout,1,0);
  }
}


 
