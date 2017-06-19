/****************************************************
;;; MODULE NAME : cgutil.c
;;;
;;; PARENT      : cgen.c
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        :  @(#)cgutil.c	15.1  01 Apr 1996
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 15.5.91
;;;
;;; DESCRIPTION : utilities for code generation
;;;
;;; REMARKS     :
;;;		12.10.92	bug: symb-->get_codenr
;;;		19.2.93		jsc	unvoid get address of variable
;;;		22.2.93		mayr	str-var-offsets
;;;		1.3.93		jsc	bidirectional weak-unif
;;;					graph (cg_graph)
;;;					failure_clause
;;;		3.3.93		jsc	cg_graph based on parsed_rule_type
;;;		12.3.93		jsc	bug in exist_contrapositiv
;;;		4.6.93	jsc	new structure for built-ins
;;;				cg_includes removed
;;;		1.7.93		jsc	extern.h created, hamdi-sources
;;;					included EXTENDED_CONSTRAINTS
;;;		2.7.93		jsc	merged 8.1.1.1 (setsubgoals)
;;;		2.7.93		jsc	random-reordering
;;;		21.7.93		jsc	debugging
;;;		9.9.93		jsc	debugging
;;;		14.12.93	jsc		purity, fold
;;;		07.06.94	jsc		now, a function symbol
;;;						does no more carry its arity
;;;						(cg_struct)
;;;						neg. symbols = 10000..19999
;;;		28.7.94		jsc	-interactive -searchtree
;;;             22.12.93        Hamdi   some lines removed
;;;		30.03.94	Hamdi	static functions decl.
;;;             9.3.95          jsc     abgespeckter LCB + subgoal-list
;;;
;;; BUGS:
;;;	get_copy_bound ????
****************************************************/

#include "symb.h"
#include "extern.h"

#define DEBUG1(X)
#define DELETED (-9999)

/*****************************************************/
/* static functions                                  */
/*****************************************************/
static int compute();
static int var_p();
static void print_copy_list();

/*****************************************************/
/* functions (codegen/cgutil.c)                      */
/*****************************************************/
int do_it();
int get_copy_bound();
void cg_init_symb();
int adr_var();
void cg_unifs();
int termlab();
void cg_argvectors();
void cg_builtin();
void tl_gen_av();
void cg_lcb_info();
void cg_struct();
void prt_term();
void cg_goals();
void cg_graph();
void cg_choices();
void enter_redlist();
void cg_red();
void cg_red_fact();
void cg_failure_fact();
void cg_randomfact();
void cg_cpybd();
void cg_copy();
void cg_queryvar();
void cg_queryvarlab();
void cg_gen_subgoal_list();

/*****************************************************/
/* statics                                           */
/*****************************************************/
static int cpyoffset = -1;	/* offset for copy bounds		*/
static int a_offset;		/* address offset for variables		*/
static int s_offset;		/* address offset for structural variables */
static int labtrail;		/* number of entries in label trail	*/
static int lab_clnr;		/* number of current clause 		*/
static termtype **ltrail;	/* pointer to array of labels		*/
static symbollist *strvars;     /* structural variables of a clause     */

	/* statics for INFERENCE_RULES					
	 * if one of the following variables is set to 1,
         * then, an according ``pseudo''-fact must be generated
	 * for the inference rule
	 * NOTE: This does NOT hold for the reduction steps
	*/

static int random_reordering_clause_needed = 0;
static int empty_label_list;
static int fail_label_needed;
static int labnr = 0;

/*****************************************************/
/*  do_it                                            */
/*****************************************************/
/* search in compile commands for string             */
/* if found return 1 else 0			     */
int do_it(module,s,type)
modtype *module;
char *s;
int type;
{
modtype   *mp;
claustype *cp;
predtype  *pp;
syel	  *sp;
int symbnr = (sp=(syel *)get_name(s))? sp->symbnr : (-1);

if (!module) 
	return 0;
for (mp=module; mp; mp=mp->prev) {
	if (!(cp = mp->strategy))
	   /* look up in higher module */
	   continue;

	/* search for control/compile directive */
	for ( ; cp; cp=cp->next)
	   for (pp=cp->p_list; pp; pp=pp->next)
	       if (pp->symb == symbnr) {
		  /* strategie is on !! */
		  /* chech surrounding  */
	   	  if (cp->delpos != type)
	      	     warning(s,"wrong control/compile statement");
		  return 1;
		  }
	}
return do_it(module->prev,s,type);
}

/*****************************************************/
/*  get_copy_bound                                   */
/*****************************************************/
/* search in control commands for new bounds         */
/* if found return 1 else 0			     */
int get_copy_bound(module,val)
modtype *module;
int *val;
{
claustype *cp;
predtype *pp;

for ( ; module; module=module->prev) {
	if (!(cp = module->strategy))
	   /* look up in higher module */
	   continue;

	/* search for compile directive */
	for ( ; cp; cp=cp->next)
	      for (pp=cp->p_list; pp; pp=pp->next)
		 if (pp->symb == EVAS_SYMB) {
		    if (pp->t_list->symb == COPI_SYMB) {
		       *val = compute(val,pp->t_list->next);
		       }
		    return 1;
		    }
	}
return inwasm_flags.cpybd;
}

/*****************************************************/
/* compute                                           */
/*****************************************************/
/* compute constant index of global array element    */
static int compute(val,tp)
int *val;
termtype *tp;
{
int erg;

if (tp->type == NUMCONST)
   return tp->value;
else if (tp->symb == COPI_SYMB)
   return *val;
else switch(tp->symb) {
	case PLUS_SYMB:
		erg = compute(val,tp->t_list);
		*val = erg + compute(val,tp->t_list->next);
		break;
	case MULT_SYMB:
		erg = compute(val,tp->t_list);
		*val = erg * compute(val,tp->t_list->next);
		break;
	case MINUS_SYMB:
		erg = compute(val,tp->t_list);
		*val = erg - compute(val,tp->t_list->next);
		break;
	case DIV_SYMB:
		erg = compute(val,tp->t_list);
		*val = erg / compute(val,tp->t_list->next);
		break;
	default:
		warning("","$Copies error");
	}
return *val;
}

/*****************************************************/
/* cg_init_symb()                                    */
/*****************************************************/
/* initialise variables in  symbol table             */
void cg_init_symb()
{
int i;

for (i=nrbuilt; i< nump; i++)
	if (symbtab[i].type == VARIABLE || symbtab[i].type == SVARIABLE){
		symbtab[i].index = (-1);
		symbtab[i].arity = 0;
		}

}

/*****************************************************/
/* adr_var(cl)                                       */
/*****************************************************/
/* get all variables from a clause and determine     */
/* their offset and return their number              */
/**/
int adr_var(cl)

claustype *cl;
{
int cnt;
predtype *pp;
claustype *fann;

cnt = 0;
a_offset = 0;
s_offset = 0;

	/* initialize the list of strvars to be empty */
strvars = NULL;
        /****************************************************/
	/* set var-index & strvar-index:-1 arity:0. This is */
	/* absolutely necessary if the clauses are splitted */
        /****************************************************/
cg_init_symb();

for (pp=cl->p_list; pp; pp=pp->next){
	cnt = cnt + var_p(pp->t_list);
	/* move thru predicates of the current clause */
	if (cl->fann && pp->kp && pp->kp->p_list->litnr == 0) {
	   /* if a fanned clause is splitted at pp */
	   pp = pp->kp->p_list;
	   }
	}

for (pp=cl->constr; pp; pp=pp->next){
	/* move thru constraints of the current clause */
	cnt = cnt + var_p(pp->t_list);
	}

for (fann = cl->fann; fann; fann = fann->next) 
	/* move thru the constraints of all contrapositives 
	   because of strvars */
    for (pp = fann->constr; pp; pp = pp->next)
	    cnt = cnt + var_p(pp->t_list);


	/* init labeltrail for this clause */
labtrail = 0;

	/* fanned clauses ??  take original number */
lab_clnr = cl->p_list->claus->clnr;
if (!ltrail) 
  {
    ltrail = (termtype **)malloc(NRLAB * sizeof(termtype *));

    if (ltrail == NULL)
      {
	fprintf(stderr, "  Error (cgutil.c - adr_var()): Memory allocation error.\n");
	exit(50);
      }
  }

	/* increment str-var-offsets by the number of vars */
for (; strvars; strvars = strvars->next)
  symbtab[strvars->symbol].index += a_offset;

	/* save number of strvar in global variable */
nrstrvars = s_offset;

return cnt;
}

/*****************************************************/
/* count and offset variables in a predicate         */
/*****************************************************/
static int var_p(tl)
termtype *tl;
{
int cnt = 0;
symbollist *newsymb;

while (tl) {
	switch (tl->type) {
	case VARIABLE:
		if ((int)symbtab[tl->symb].index == -1) {
			/* first occurrence */
			tl->grflag = 1;	/* if first occurrence, mark term */
			cnt++;
			symbtab[tl->symb].index = a_offset++;
			symbtab[tl->symb].arity++;
			break;
			}
		/* nth occurrence */
		tl->grflag = 0;
		symbtab[tl->symb].arity++;
		break;
	case SVARIABLE:
		if ((int)symbtab[tl->symb].index == -1) {
			/* first occurrence  */
			tl->grflag = 1;	/* if first occurrence, mark term */
			cnt++;
			symbtab[tl->symb].index = s_offset++;
			symbtab[tl->symb].arity++;

			 /* put strvar into the list of strvars */
			 newsymb = (symbollist *) malloc(sizeof(symbollist));

			 if (newsymb == NULL)
			   {
			     fprintf(stderr, "  Error (cgutil.c - var_p()): Memory allocation error.\n");
			     exit(50);
			   }

			 newsymb->symbol = tl->symb;
			 newsymb->next   = strvars;
			 strvars         = newsymb;
			break;
			}
		/* nth occurrence */
		tl->grflag = 0;
		symbtab[tl->symb].arity++;
		break;
	/* built-ins on termposition : +,-,*,/,[ with subterms */
	case STRINGCONST:
	case CONSTANT:
	case NUMCONST:
	case CONSTSYM:
	case GVARIABLE:
		if (tl->t_list) {
			cnt = cnt + var_p(tl->t_list);
			}
		break;
	default:
		fprintf(stderr,"Illegal type codegen-cg_adr_var");
		}
	tl = tl->next;
	}
return cnt;
}


/*****************************************************/
/* cg_unifs()                                        */
/*****************************************************/
/* gen. code for unifactions			     */
void cg_unifs(pr,nrgoals)
predtype *pr;
int nrgoals;
{
termtype *tl;
int lnr,cnr;
/* move thru terms and make unification */
tl = pr->t_list;

while (tl) {
	switch (tl->type) {
	case VARIABLE:
		if (symbtab[tl->symb].arity == 1) 
			/* variable occurs only once */
			cgin_unvoid(symbtab[tl->symb].index);
		else
			cgin_unvar(symbtab[tl->symb].index);
		break;
	case STRINGCONST:
	case CONSTSYM:
	case NUMCONST:
	case CONSTANT:
		if (tl->t_list) {
			lnr = termlab(tl,&cnr); 
			if (tl->grflag == GROUND)
				cgin_ungterm(cnr,lnr);
			else
				cgin_unngterm(cnr,lnr);
			}
		else
			/* is a constant */
			cgin_unconst(tl);
		break;
	case GVARIABLE:
		cgin_unglobl(symbtab[tl->symb].index);
		break;
	default:
		fprintf(stderr,"Illegal type codegen/cg_unifs\n");
		break;
		}
	tl = tl->next;
	}
}

/******************************************************/
/* termlab()                                          */
/******************************************************/
/* enter term into termlist & return index & lab_clnr */
/* check for doubles */
int termlab(term,lnr)
termtype *term;
int *lnr;
{
int i;

*lnr = lab_clnr;
for (i=0; i<labtrail;i++){
	if (ltrail[i] == term){
		return i;
		}
	}
ltrail[labtrail] = term;
if (labtrail>NRLAB) 
	nferror("labeltrail overflow",0);
return labtrail++;
}


/*****************************************************/
/* cg_argvectors()                                   */
/*****************************************************/
/* generate argvectors for all literals		     */
/* and the literal contol blocks		     */
/*                                                   */
/* NB: generate literal control blocks only, if      */
/* it is not a built-in predicate                    */
/* (unless it is a query)                            */
/*****************************************************/
void cg_argvectors(cl)
claustype *cl;
{
  claustype *cp;
  predtype *pl;
  
  /*****************************************************/
  /* first the lcbs for all old literals in the clause */
  
  for(pl=cl->p_list; pl; pl=pl->next) {
    
    if (!pl->symb || pl->symb >= nrbuilt) {
      if (cl->fann) {
	if (pl->kp && pl->kp->p_list->litnr == 0)
	  pl = pl->kp->p_list;
	else 
	  {
	    cgin_lcblab(cl->clnr,pl->litnr);
	    cg_lcb_info(pl,cl);
	    cgin_avlab(cl->clnr,pl->litnr);
	    tl_gen_av(pl->t_list);
	  }
      }
      if (!cl->fann) {
	cgin_lcblab(cl->clnr,pl->litnr);
	cg_lcb_info(pl,cl);
	cgin_avlab(cl->clnr,pl->litnr);
	tl_gen_av(pl->t_list);
      }
    }
    else
      cg_builtin(pl,cl);
    
  }
  /*************************************************/
  /* now the lcb for a new_p literal in the clause */
  
  if (cl->fann && cl->del != DELETED)
    for(pl=cl->p_list->next; pl; pl=pl->next) 
      if (pl->kp && pl->kp->p_list->litnr == 0) {
        /* generate literal control block */
        cgin_lcblab(cl->clnr,pl->litnr);
        cg_lcb_info(pl,cl);
        /* generate argument vector */
        cgin_avlab(cl->clnr,pl->litnr);
        tl_gen_av(pl->t_list);
      }
  /**************************************************************/
  /* and the lcbs for the new_p literals in the contrapositives */
  
  for(cp=cl->fann; cp; cp = cp->next) 
    if (cp->del != DELETED)
      for(pl=cp->p_list->next; pl; pl=pl->next) 
	if (pl->kp) {
          /* generate literal control block */
	  cgin_lcblab(cl->clnr,pl->litnr);
	  cg_lcb_info(pl,cl);
	  /* generate argument vector */
	  cgin_avlab(cl->clnr,pl->litnr);
	  tl_gen_av(pl->t_list);
	}
}

/*************/
/* built ins */
/*************/
void cg_builtin(pl,cl)
predtype *pl;
claustype *cl;
{
cgin_avlab(cl->clnr,pl->litnr);
if (symbtab[pl->symb].index == 20) {
	/* there is a cut */
	if (symbtab[cl->p_list->next->symb].index != 19) {
	   /* there is no precut */
	   nferror("There is no precut",cl->lineno);
	   }
	cgin_typed(NCONST,nrvars);
	return;
	}
switch (pl->symb) {
	/* no arguments for  terms == numexpr */
	case IS_SYMB:
	case EVAS_SYMB:
	case GREA_SYMB:
	case LESS_SYMB:
	case LEQU_SYMB:
	case GRQU_SYMB:
		break;
	case ASSI_SYMB:
		tl_gen_av(pl->t_list->next);
		break;
	default:
		tl_gen_av(pl->t_list);
		break;
	}
}

/*****************************************************/
/* term descriptions                                 */
/*****************************************************/
void tl_gen_av(tl)
termtype *tl;
{
int lnr,cnr;

while (tl) {
	switch (tl->type) {
	case VARIABLE:
		cgin_typed(VAR_, symbtab[tl->symb].index);
		break;
	case STRINGCONST:
	case CONSTSYM:
	case CONSTANT:
		if (tl->t_list) {
			lnr = termlab(tl,&cnr);
			cgin_dwterm(CREF,cnr,lnr);
			}
		else
			/* is a constant */
			cgin_typed(CONST,tl->symb);
		break;
	case NUMCONST:
		cgin_typed(NCONST, tl->value);
		break;
	case GVARIABLE:
		if (tl->symb<nrbuilt)
		   /* $Depth, $Inf, $Copies */
		   warning(NAM(tl->symb)," on term position not allowed !");
		cgin_typed(GLOBO,symbtab[tl->symb].index);
		break;
	case SVARIABLE:
		/* only in constraints */
		cgin_typed(SVAR, symbtab[tl->symb].index);
		break;
	default:
		fprintf(stderr,"Illegal type codegen-av");
		break;
		}
	tl = tl->next;
	}
}

/*****************************************************/
/* cg_lcb_info()                                     */
/*****************************************************/
/* create the literal control block                  */
/* for a literal                                     */
/* Note: the pseudo-literal-CB for reduction code    */
/* is generated in cg_red_fact			     */
/*						     */
void cg_lcb_info(pl,cl)
predtype *pl;
claustype *cl;
{
static int pos_nr = 0;
int nr_goals;
int d_depth = 0;
int d_inf = 0;

	if (!cl->clnr)
		/* the default query has exactly one call */
		nr_goals = 1;
        else if (pl->kp)
		/* there exists a contrapositive to go to */
	        nr_goals = pl->kp->nr_lits - pl->kp->nr_built - 1;
             else 
		nr_goals = 0;

        if (pl->litnr <= 100 && pl->kp && pl->kp->del != DELETED) {
	   /* there exists a contrapositive to go to */
	   d_depth = pl->kp->ddepth;	
	   d_inf = pl->kp->dinf;	
	}
	else {
	   /* there exists no contrapositive to go to */
	   d_depth = 0;	
	   d_inf = 0;	
	}
	
	if (pl->litnr <= 100 && pl->kp && pl->kp->del != DELETED)
	   /* there exists a contrapositive to go to */
	   cgin_ldw(pl->sign,pl->claus->clnr,pl->litnr); 
	else 
	   /* there exists no contrapositive to go to */
           cgin_fail_label();
	
	cgin_typed(NCONST, (pl->sign == '+')? 
	   get_codenr(pl->symb) : INVERSE(get_codenr(pl->symb)));
	cgin_typed(NCONST, - pl->claus->parsed_rule_type);
	cgin_typed(NCONST, pl->claus->clnr);
	cgin_typed(NCONST, pl->litnr);
	cgin_typed(NCONST, cl->nrvars);
	cgin_typed(NCONST, cl->nrstrvars);
	cgin_typed(NCONST, nr_goals);
	cgin_typed(NCONST, d_depth);
	cgin_typed(NCONST, d_inf);
	cgin_avdw(cl->clnr,pl->litnr);


	
/*********************************/
/* for new LCB with subgoal list */
	if ((pl->litnr <= 100 && pl->kp && pl->kp->del != DELETED)){
	  /*	if there exists the corresponding contrapositive
		then it generates its own subgoallist
		refer to it */
	  cgin_subgoal_list_dw(pl->claus->clnr,pl->litnr);
	}
	else {
		/* there is no corresponding contra-positive and 
		   therefore no subgoal-list
		   just generate an empty label */
		cgin_dw(0);
		}
	if ((pl->claus->clnr == 0) ||
	    (pl->claus->nr_lits - pl->claus->nr_built > 1 && 
	     (pl->claus->fann || pl->litnr > 1)))
		 
		cgin_ordw(pl->claus->clnr,pl->litnr);
	else 
	 	cgin_dw(0);
/*************************************/
	pos_nr += 1;
}


/*****************************************************/
/* cg_struct                                         */
/*****************************************************/
void cg_struct()
{
int i;
termtype *term;

for (i=0; i<labtrail;i++){
	/* label */
	cgin_termlab(lab_clnr,i);
	/* print term */
	term = ltrail[i];
	/* $$123 make a .dw [n]gterm <arity><symbol> */
	cgin_typed((term->grflag == GROUND)? GTERM : NGTERM,
		get_codenr(term->symb));
	prt_term(term->t_list);
	cgin_typed(EOSTR,0);
	}
if (labtrail) cgin_dw(0);  /* end of structure */
}

/*****************************************************/
/* prt_term                                          */
/*****************************************************/
void prt_term(term)
termtype *term;
{
int i;
int clnr;
while (term) {
	switch (term->type) {
	case VARIABLE :
		/* in earlier times there was a void variable */
		/* but victim of rationalisation              */
		cgin_typed(VAR_,symbtab[term->symb].index);
		break;
	case GVARIABLE:
		cgin_typed(GLOBO,symbtab[term->symb].index);
		if (term->t_list)
		    warning(NAM(term->symb)," arrays are not implemented");
		break;
	case SVARIABLE:
		/* only in constraints */
		cgin_typed(SVAR,symbtab[term->symb].index);
		break;
	case STRINGCONST:
	case CONSTSYM:
	case CONSTANT:
		if (term->t_list) {
			/* compound term */
			i = termlab(term,&clnr);
			cgin_dwterm(CREF,clnr,i);
			}
		else 
			/* modified by jsc get_codenr	*/
			cgin_typed(CONST, term->symb);
		break;
	case NUMCONST:
		cgin_typed(NCONST, term->value);
		break;
	default :
		fprintf(stderr,"Illegal type codegen-term\n");
		break;
		}
	term = term->next;
	}
}

/*****************************************************/
/* cg_goals()                                        */
/*****************************************************/
/* call the subgoals 				     */
/* built-in? deterministic call? call orbranch       */
void cg_goals(pp,lreduct,clnr)

predtype *pp;
int lreduct;
int clnr;
{

while (pp) {

	if (pp->symb && pp->symb < nrbuilt) {
		/* generate code for built-ins */
		(symbtab[pp->symb].bfun)(pp);
		}
	else if (!pp->wu) {
		nferror("call: no connection",pp->claus->lineno);
		cgin_fail();
		}
	else {
		orbcal++; 	/* count orbranch labels */
		cgin_call(clnr,pp->litnr,pp); 
		}
	/***************************************/
	/* enter this predsymbol in a list for */
	/* generation of reduction facts later */
	/***************************************/
	if (!inwasm_flags.reduct && lreduct && pp->symb)
	   enter_redlist(redlist,pp->symb);
	pp = pp->next;
	}
}

/*****************************************************/
/* cg_graph()                                        */
/*****************************************************/
/* the weak unification graph with special strategies*/
void cg_graph(module)
modtype *module;
{
claustype *cp,*cpp;
predtype *pp;
char s[30];
int lreduct;
int lpredfail;
int lrandreord;

if (!labnr) {
   labnr = 1;
   if (orbcal) 
	cgin_coment(1); /* WEAK UNIFICATION GRAPH */
   }

while (module) {
	if (module->m_list) cg_graph(module->m_list);

	/********************************************************/
	/* determine, which options on the weak unification     */
	/* graph are to be done, and, accordingly, set the      */
	/* local variables                                      */
	/********************************************************/
	lreduct = 
	  inwasm_flags.reduct || do_it(module,"redsteps",CONTROL);
	lpredfail = 
	  inwasm_flags.predfail || do_it(module,"predfail",CONTROL);
	lrandreord = 
          inwasm_flags.randreord || do_it(module,"randreord",CONTROL);

	fail_label_needed = 0;

	/**************************************************************/
	/* in case of random-reordering, we need the failure label and*/
	/* failure clause in any case                                 */
	/**************************************************************/
	if (lrandreord || inwasm_flags.interactive){
		fail_label_needed = failure_clause_needed = 1;
	}

	/************************************************/
        /* we search for all occurences of old literals */
	/************************************************/
	for (cp=module->c_list; cp; cp=cp->next) {

	/* single literals (except default-query) have no orbranch */
	if (!cp->p_list->next && cp->p_list->symb) {
	      continue;
	}
	   
	/* proceed thru all literals of the clause */
	   for (pp=cp->p_list; pp; pp=pp->next) {

		/* if the current clause is splitted move to the rest */
		if (pp->kp && pp->kp->p_list->litnr == 0) {
		   pp = pp->kp->p_list;
		   continue;
		}

		/* built-ins except default query have no orbranch */ 
		if (pp->symb && (pp->symb <= nrbuilt)) {
		   continue;
		}

		/* heads of contrapositives have no orbranch */
		if (pp->symb && (pp->kp == cp && !cp->fann)) {
		   continue;
		}

		/* if there are no connections and no reduction steps */
		if ((!pp->wu) && !lreduct) {
		   sprintf(s,"literal %d in clause %d",
		   pp->litnr,pp->claus->clnr);
		   warning(s," has no connection");
		   continue;
		}
		/* otherise gen orbranch */
		if (cp->clnr == pp->claus->clnr)
		     /* if the literal belongs to the clause */
		     cg_choices(cp,pp,lreduct,lpredfail,lrandreord);

	  } /* pp */
        } /* cp */
	/**********************************************************/
        /* then the orbranches for all occurences of new literals */
	/**********************************************************/
	for (cp=module->c_list; cp; cp=cp->next) {
	    if (cp->del != DELETED) 
	       for (pp=cp->p_list->next; pp; pp=pp->next) 
	           if (pp->kp && pp->kp->p_list->litnr == 0) 
		      cg_choices(cp,pp,0,lpredfail,lrandreord);

	    for (cpp = cp->fann; cpp; cpp=cpp->next) 
	        if (cpp->del != DELETED)
	           for (pp=cpp->p_list->next; pp; pp=pp->next) 
	               if (pp->kp && pp->kp->p_list->litnr == 0) 
		          cg_choices(cp,pp,0,lpredfail,lrandreord);
        }


	module = module->next;
	} /* module */
}

/*****************************************************/
/* cg_choices()                                      */
/*****************************************************/
void cg_choices (cp,pp,lreduct,lpredfail,lrandreord)

claustype *cp;
predtype *pp;
int lreduct;
int lpredfail;
int lrandreord;
{
 predlist *wp;

 cgin_orlab(pp->claus->clnr,pp->litnr);

 if (!lpredfail)
    cgin_orbr(labnr);
 else
    cgin_por(labnr,pp);

 cgin_opt();
 cgin_lab(labnr++);

 /***********************************************************/
 /* here, we must place code, if we add an INFERENCE_RULE_A */
 /***********************************************************/
 
 /* random reordering */
 if (lrandreord) {
    cgin_rand_reord_label();
    random_reordering_clause_needed = 1;
 }

 /* initialising the interactive mode */
 if (inwasm_flags.interactive){
    cgin_trymode_label();
 }

 if (lreduct && pp->symb) {
    /* reduct is also an alternative       */
    /* no reduction step for default query */
    cgin_rdw(pp);
 }

 /* generate labels for all possible entry points */
 /* we must note, if the resulting list is empty */

 empty_label_list = 1;

 for (wp=pp->wu; wp; wp=wp->next) {
     if (wp->pr->claus->parsed_rule_type == PQUERY && wp->pr->litnr > 1) {
	continue;
     }
     if (wp->pr->claus->parsed_rule_type == PRULE && wp->pr->litnr > 1) {
	continue;
     }
     /* otherise if kp is not deleted: generate the particular labels */
     if (wp->pr->kp && wp->pr->kp->del != DELETED) {
	 cgin_lcbdw(wp->pr);
	 empty_label_list =0;
     }
 }
 /************************************************/
 /* now INFERENCE_RULE_Z is to be added          */
 /************************************************/
 /* 
  * INF_Z: exec_proof_steps 
  */
 if (inwasm_flags.interactive){
    cgin_exec_ps_label();
 }
 /* if label list is still empty we produce a failure-label */ 
 if (empty_label_list || fail_label_needed){
    cgin_fail_label();
 }
 failure_clause_needed = 1;
 cgin_dw(0);
 cgin_noopt();
}

/*****************************************************/
/* enter_redlist                                     */
/*****************************************************/
void enter_redlist(rp,symb)
symbollist *rp;
int symb;
{

if (!rp) 
	redlist = new_symboll(symb,(symbollist *)NULL);
else {
	for ( ; rp; rp = rp->next) {
	   if (rp->symbol == symb)
	      return;
	   }
	redlist = new_symboll(symb,redlist);
	}
}

/*****************************************************/
/* cg_red()                                          */
/*****************************************************/
/* generate reduction "facts" for predicates and their negation */
void cg_red()
{
symbollist *rp;
int i;

cgin_coment(2); /* REDUCTION FACTS */

if (inwasm_flags.reduct) {
	for (i=nrbuilt; i<nump; i++) {
	/* all facts has to be generated */
		if (symbtab[i].type != PREDSYM) 
			continue;
		else {
			cg_red_fact(i,'-');
			cg_red_fact(i,'+');
			}
		}
}
else
	for (rp = redlist; rp; rp = rp->next) {
	/* there are only some facts to generate */
		cg_red_fact(rp->symbol,'-');
		cg_red_fact(rp->symbol,'+');
		}

}


/*****************************************************/
/* cg_red_fact()                                     */
/* generate code for the reduction facts             */
/*****************************************************/
void cg_red_fact(symbol,sign)
int symbol;
int sign;
{

	cgin_dred(sign,symbol);
	cgin_rlab2(sign,symbol);

	if (inwasm_flags.folddown || inwasm_flags.folddownx){
		cgin_ureduct(sign,symbol,symbtab[symbol].arity);
	}
	else {
		cgin_reduct(sign,symbol,symbtab[symbol].arity);
	}
	if (inwasm_flags.interactive){
		cgin_save_reduct();
	}
	cgin_proceed();

	/* here comes the pseudo literal control-block */

	cgin_rlab(sign,symbol);
	cgin_rdw2(sign,symbol);
	cgin_typed(NCONST, (sign == '+')? 
		get_codenr(symbol) : INVERSE(get_codenr(symbol)));
	cgin_typed(NCONST, REDUCTION_TYPE);

	/* note: all other fields of the lcb are not set*/

}

/******************************************************/
/* cg_failure_fact()                                  */
/* create, if necessary, the failure-fact             */
/******************************************************/
void cg_failure_fact()
{
/* is a failure-fact necessary? */
if (failure_clause_needed){
	cgin_fail_clause();
	}
}


/******************************************************/
/* cg_randomfact()                                    */
/* create, if necessary the random-reordering fact    */
/******************************************************/
void cg_randomfact()
{
if (random_reordering_clause_needed){
	cgin_rand_reord_clause();
	}
}

/******************************************************/
/* cg_cpybd()                                         */
/******************************************************/
void cg_cpybd(cp,cpybound)
claustype *cp;
int cpybound;
{

if (cp->clnr>0) {
	/* new copybound for all fanned clauses */
	cpylist = new_symboll(cpybound,cpylist);
	cpyoffset++;
	}

cgin_cpybd(cpyoffset);
}

/******************************************************/
/* cg_copy()                                          */
/******************************************************/
void cg_copy()
{

cgin_coment(5); /* COPY BOUNDS */

cgin_label("cpy");
print_copy_list(cpylist);
cgin_dw(0);
}

/******************************************************/
/* print_copy_list()                                  */
/******************************************************/
static void print_copy_list(list)
symbollist *list;
{

if (!list) return;
else {
	print_copy_list(list->next);
	cgin_dw(list->symbol);
	}
}

/******************************************************/
/* cg_queryvar()                                      */
/******************************************************/
/* the variables of the query should be printed	      */
void cg_queryvar(clnr)
int clnr;
{
int i;

for (i=0; i<a_offset; i++) {
	cgin_qout(0,0);
	cgin_qout(i,clnr);
	}
}

/******************************************************/
/* cg_queryvarlab()                                   */
/******************************************************/
void cg_queryvarlab(clnr)
int clnr;
{
static int nl_label_done = 0;
int i;


if ( ! nl_label_done ) {
	nl_label_done = 1;
	cgin_qvlab(0,0);
		/* can be entered before cg_symbtab() is done */
	cgin_typed(CONST, enter_name(enter_token("\\n"),STRINGCONST,0,0));
	}

for (i=0; i<a_offset; i++) {
	cgin_qvlab(i,clnr);
	cgin_typed(VAR_,i);
	}
}

/*****************************************************/
/*  cg_gen_subgoal_list                              */
/*****************************************************/
/* list the ldb-labels of each subgoal of the current*/
/*	contrapositive                               */
/* pp points to the list of SUBGOALS (not to the head)*/
/**/
void cg_gen_subgoal_list(clnr,pp)
int clnr;
predtype *pp;
{
 
/* old version: pp pointed tpo contrapositive          */
/* cgin_subgoal_list_label(pp->claus->clnr,pp->litnr); */
/* pp = pp->next; */
while (pp) {
	if (pp->symb && pp->symb < nrbuilt) {
		/*
		   this is a built-in: we do not generate
		   a subgoal label for it
		  */
		 pp = pp->next;
		 continue;
		 }
	else if (!pp->wu) {
		/*
		   if we do not have a connection, let the field
		   point to zero 
		  */
		 cgin_dw(0);
		 nferror("call: no connection",pp->claus->lineno);
		 }
	else {
		cgin_sgl_lcbdw(clnr,pp->litnr);
	      	}
	pp = pp->next;
	}
}
									 


/*######################################################################*/

