/****************************************************
;;; MODULE NAME : pars.y
;;;
;;; PARENT      : main.c
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)pars.y	17.2  01/10/97
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 15.5.91
;;;
;;; DESCRIPTION : YACC grammar for mplop
;;;
;;; REMARKS     :
;;;		3.3.93	jsc	parsed_rule_type added
;;;		2.7.93	jsc	extern.h
;;;		14.12.93	jsc		fix of
;;;						bug ''ishorn'':
;;;						Queries contain, by default
;;;						one positive literal
;;;		3012.93		hamdi		ddepth,dinf
;;;		24.3.94		jsc		pred-names also on 	
;;;						term position
;;;		6.4.94		jsc		yyerror into pars.y
;;;						from yaccsed.script
;;;		21.6.95		jsc,klaus	fcb
;;;		9.4.96          klaus		ddepth initialized	
;;;
;;;
****************************************************/

%token ARROW COLMIN QUERY		/* <- :- ?- 		*/
%token ASSIGN EVASSIGN IS RELOP		/* := :is is < >	*/
%token NAME GLOBVAR VAR STRING NUMBER
%token BEMOD BECOM BECON BEIMP
%token ENMOD ENCOM ENCON ENIMP
%token CONSTRSEP CONSTRAIN STRUCVAR 	/* constraints	*/
%token '.' ';' ',' '|' '~'		/* single chars	*/
%token '[' ']' '(' ')'  

%left '+' '-'  				/* priority levels */
%left '*' '/'

%{
#include <stdio.h>
#include <ctype.h>
#include "types.h"
#include "extern.h"

#define NULLTERM (termtype *)NULL
#define NULLLIST (symbollist *)NULL

/*****************************************************************************/
/* type of parser stack */
/*****************************************************************************/
typedef union {
		struct mocl {
		 modtype * modu;
		 claustype * claus;
		}		MC;
		claustype *	C;
		predtype *	P;
		termtype *	T;
		symbollist * 	S;
		intertype 	I;
		int		N;
	} YYSTYPE;

#define YYSTYPE_IS_DECLARED

/*****************************************************************************/
/* statics*/
/*****************************************************************************/
static int	numval;		/* value delivered by scanner */
static int	modcnt	 = 1;	/* module counter */
static int	modunr   = 0;	/* current number of module */
static int      clausenr = 1;   /* current number of clause */
static int	litnr	 = 1;	/* current number of literal in clause */
static int      number_of_lits = 0;
static int	builtnr	 = 1;	/* current number of built_in in clause */
static int	poslitnr = 0;	/* number of positive literals in clause */
static int	aktsign = '+';	/* sign '-' only in tails */
static int	termsize;
static int	termdepth;
static int	max_termdepth;

static modtype    *currmodule = NULL;	/* pointer to static predecessor */
static claustype  *currclause = NULL;	/* pointer to current clause	*/
static predtype   *pp;
static termtype   *tp;
static symbollist *slp;

#define MAXDIFFVARS	1000

static int	varlist[MAXDIFFVARS];
static int	*varlist_ptr;

#define VARUSAGE(T)	\
	if (!(symbtab[T->symb].index)){ \
		*(varlist_ptr++) = T->symb; \
		symbtab[T->symb].index=varlist_ptr-varlist; \
		} 

/*****************************************************************************/
/* function prototypes */
/*****************************************************************************/
termtype *init_term();
symbollist *new_symboll();
importlist *init_import();
static predtype *init_pred();
int count_arity();
claustype * newclause();
modtype *init_mod();
static void prefix_query();

%}

%type <MC> modules module
%type <C>  clause 
%type <P>  binexpr literal predicate head tail synclause 
%type <P>  opt_constraints constraints constraint
%type <T>  numexpr term terms glvar list conslist
%type <N>  GLOBVAR RELOP NAME 
%type <N>  '+' '-' '*' '/'

%start mploprogram

%%

	mploprogram :
		 { 
		 init_symb(); /* initialize symboltable, enter built-ins */
		 matrix = init_mod(enter_token("main_"));
		 }
		modules
		 {
		 if (errors) {
		   fprintf(stderr,"parsing: %d errors occurred\n",errors);
		   return(1);
		   }
		 matrix->c_list  = $<MC>2.claus;
		    /* default query is standard */
		 prefix_query(matrix);
		 nr_clauses = clausenr-1;	/* save in global variable */
		 return(0);
		 }
		;

	modules :
		module 
		| module modules
		 {
		 if ($<MC>1.modu) {
		    $<MC>1.modu->next = $<MC>2.modu;
		    $<MC>1.claus = $<MC>2.claus;
		    }
		 else {
		    $<MC>1.claus->next = $<MC>2.claus;
		    $<MC>1.modu = $<MC>2.modu;
		    }
		 $<MC>$.claus = $<MC>1.claus;
		 $<MC>$.modu = $<MC>1.modu;
		 }
		;

	module :
		clause
		 {
		 $<MC>$.modu	= NULL;
		 $<MC>$.claus 	= $<C>1;
		 }
		;

	clause :
		 { /* syntax rule only for semantic actions for/after clause */
		 $<C>$ = currclause = newclause();
		 $<C>$->modul	= currmodule;
		 $<C>$->clnr	= clausenr;
		 $<C>$->lineno	= yylineno;
		 $<C>$->delpos	= PFACT;	/* default */
		 $<C>$->parsed_rule_type = PFACT;
		 $<C>$->poslits  =0;
		 $<C>$->max_termsize  =0;
		 $<C>$->max_termdepth  =0;
		 $<C>$->order 	= clausenr++;	
		 $<C>$->fann	= NULL;
		 $<C>$->next	= NULL;
		 builtnr	= 1;
		 litnr		= 1;
		 number_of_lits = 0;
		 poslitnr	= 0;
		 max_termdepth	=0;
		 termdepth	=0;
		 termsize	=0;
		 varlist_ptr	= varlist;
		 /* needed for '@ClauseReorderingStartingPoint' */
		 curr_clausenr = clausenr;
		 }
		synclause  
		 {
		 yyerrok;
		 if (litnr > maxlit) maxlit = litnr;
		 if ($<C>1->delpos<0) nofanning = 1;
		 $<C>1->p_list	 = $<P>2;
		 $<P>2->kp	 = $<C>1;
		 $<C>1->nr_built = builtnr-1;
		 $<C>1->nr_lits	 = litnr-1;
		 $<C>1->clr_nr_lits = litnr- 1 - number_of_lits;	   
		 $<C>1->poslits	 = poslitnr;
		 $<C>1->nrvars	 = varlist_ptr - varlist;
		/* clean up the symbol table */
		while (varlist_ptr > varlist){
			symbtab[*(--varlist_ptr)].index=0;
			}

		/* initialize the decrements of inferences and depth */
		$<C>1->dinf     = $<C>1->nr_lits - $<C>1->nr_built - 1;
		if($<C>1->dinf > 0) {
			$<C>1->ddepth = 1;
			}
                else {
		 	$<C>1->ddepth = 0;
			}


		 /* debugging of termdepth-size */
		 /* printf("termdepth=%d termsize=%d nrvars=%d\n",
		 	$<C>1->max_termdepth,
		 	$<C>1->max_termsize,
		 	$<C>1->nrvars); */ /* bitte so lassen */
		 }
		opt_constraints  '.'
		 {
		 $<C>1->constr = $<P>4;	       
		 if ($<C>1->constr != NULL) {
		   $<C>1->flag_constraints = 1;
		 } else {
		   $<C>1->flag_constraints = 0;
		 }
		 if (poslitnr > 1) ishorn = 0;
		 }
		 ;

	opt_constraints :
                 { /* empty */
		 $<P>$ = NULL;
                 }
		| CONSTRSEP constraints
		 {
		 $<P>$ = $<P>2;
		 }
		;

	synclause :
                head ARROW
                 {
                 aktsign = '-';
                 currclause->delpos = litnr;
		 currclause->parsed_rule_type = 0;
                 }
                tail
                 {
                 aktsign = '+';
                 /* last(head) -> first(tail) */
                 for (pp = $<P>1 ; pp->next ; pp = pp->next) ;
                 pp->next = $<P>4;
                 }
                | head ARROW
                 {
                 currclause->delpos = litnr;
		 currclause->parsed_rule_type = 0;
                 }
		| ARROW 
		 {
		 currclause->delpos = litnr;
		 currclause->parsed_rule_type = 0;
		 /* default query has index 0 */
		 $<P>$ = init_pred(NULLTERM,0);
		 poslitnr = 1;
		 aktsign = '-';
		 if (!inwasm_flags.oldclausreord) {
		   /* do not count __query Literal */		   
		   number_of_lits++;
		 }
		 }
		tail 
		 {
		 aktsign = '+';
		 $<P>2->next = $<P>3;
		 $<P>$ = $<P>2;
		 }
		| literal COLMIN 
		 {
		 aktsign = '-';
		 }
		tail 
		 {
		 aktsign = '+';
		 currclause->delpos = PRULE;
		 currclause->parsed_rule_type = PRULE;
		 $<P>1->next = $<P>4;
		 }
		| QUERY
		 {
		 /* default query has index 0 */
		 $<P>$ = init_pred(NULLTERM,0);
		 poslitnr = 1;
		 aktsign = '-';
		 if (!inwasm_flags.oldclausreord) {
		   /* do not count __query Literal */
		   number_of_lits++;
		 }
		 } 
		tail 
		 {
		 aktsign = '+';
		 currclause->delpos = PQUERY;
		 currclause->parsed_rule_type = PQUERY;	     
		 $<P>2->next = $<P>3;
		 $<P>$ = $<P>2;
		 }
		| literal 
		| error 
		 {
		 yyerrok;
		 $<P>$ = NULL;
		 }
		;

         head : 
		literal
		| literal ';' head
		 {
		 $<P>1->next = $<P>3;
		 }
		| literal '|' head
		 {
		 $<P>1->next = $<P>3;
		 }
		| literal error head 
		 {
		 yyerrok;
		 $<P>1->next = $<P>3;
		 }
		| literal ';' error 
		| literal '|' error 
		;
	
        tail : 
		literal
		| literal ',' tail
		 {
		 $<P>1->next = $<P>3;
		 }
		| literal '&' tail
		 {
		 $<P>1->next = $<P>3;
		 }
		| literal error tail 
		 {
		 yyerrok;
		 $<P>1->next = $<P>3;
		 }
		| literal ',' error
		| literal '&' error
		;

	literal :
		predicate
		 {
		 if ($<P>1->sign == '+') poslitnr += 1;
		 if($<P>1->symb<nrbuilt && $<P>1->sign == '+')
		 	nferror("Illegal usage of a built-in",yylineno);
		 }
		| '~' predicate
		 {
		 $<P>2->sign = (aktsign == '+')? '-' : '+';
		 if($<P>2->symb<nrbuilt && $<P>2->sign == '+')
			nferror("Illegal usage of a built-in",yylineno);
		 if ($<P>2->sign == '+') poslitnr += 1;
		 $<P>$ = $<P>2;
		 }
		| VAR IS numexpr
		 {
		 tp = init_term(NULLTERM,$<N>1,VARIABLE,0,NGROUND);
		 tp->next = $<T>3;
		 $<P>$ = init_pred(tp,IS_SYMB);
		 if (aktsign == '+')
		    nferror("Illegal usage of an assignment",yylineno);

		 VARUSAGE(tp);
		 }
		| glvar ASSIGN term
		 {
		 $<T>1->next = $<T>3;
		 $<P>$ = init_pred($<T>1,ASSI_SYMB);
		 if (aktsign == '+')
		    nferror("Illegal usage of an assignment",yylineno);
		 }
		| glvar EVASSIGN numexpr
		 {
		 $<T>1->next = $<T>3;
		 $<P>$ = init_pred($<T>1,EVAS_SYMB);
		 if (aktsign == '+')
		    nferror("Illegal usage of an assignment",yylineno);
		 }
		| binexpr
		;

	predicate :
		NAME 
		 {
		 $<P>$ = init_pred(NULLTERM,$<N>1);
		 if ((symbtab[$<N>1].name[0] == '$' && $<P>$->symb>=nrbuilt) ||
		     (symbtab[$<N>1].name[0] == '@' &&
		      symbtab[$<N>1].name[1] == '$' && $<P>$->symb>=nrbuilt)) {
		    nferror("Unknown built-in",yylineno);    
		 } else if ((symbtab[$<N>1].name[0] == '$') ||
			  (symbtab[$<N>1].name[0] == '@' &&
			   symbtab[$<N>1].name[1] == '$')) {
		   /* do not count builtins */
		   number_of_lits++;
		 }
                 if (ignorelits_flag == 1 && inwasm_flags.ignorelits) {
		   /* do not count ^Literals */
		   number_of_lits++;
		   $<P>$->literal_flag = 1;
		 } else if (subgoal_flag == 1) { 		  
		   $<P>$->subgoal_flag = 1;
		 }
		 subgoal_flag = 0;
		 ignorelits_flag = 0;
		 }
		| NAME '(' 
			inctermdepth
			terms ')'
		 {		
		 $<P>$ = init_pred($<T>4,$<N>1);
		 if ((symbtab[$<N>1].name[0] == '$' && $<P>$->symb>=nrbuilt) ||
		     (symbtab[$<N>1].name[0] == '@' &&
		      symbtab[$<N>1].name[1] == '$' && $<P>$->symb>=nrbuilt)) {
                    nferror("Unknown built-in",yylineno);
		 } else if ((symbtab[$<N>1].name[0] == '$') ||
			  (symbtab[$<N>1].name[0] == '@' &&
			   symbtab[$<N>1].name[1] == '$')) {
		   number_of_lits++;
                 } 
		 if (ignorelits_flag == 1 && inwasm_flags.ignorelits) {
		   number_of_lits++;
		   $<P>$->literal_flag = 1;
		 } else if (subgoal_flag == 1) { 		  
		   $<P>$->subgoal_flag = 1;
		 }
		 subgoal_flag = 0;
		 ignorelits_flag = 0;
		 if (max_termdepth > currclause->max_termdepth) 
		 	currclause->max_termdepth = max_termdepth;
		 if (termsize > currclause->max_termsize) 
		 	currclause->max_termsize = termsize;
		 termdepth=0;
		 max_termdepth=0;
		 termsize=0;
		 if ($<N>1 == MKGLOB_SYMB)
		    enter_globarray($<P>$->t_list);
		 }
		;

	constraints :
		constraint
		| constraint ',' constraints
		 {
		 $<P>1->next = $<P>3;
		 }
		;

	constraint :
		conslist CONSTRAIN conslist
		 {
		 $<T>1->next = $<T>3;
		 $<P>$ = init_pred($<T>1,$<N>2);
		 $<P>$->litnr = ++nr_constraints;
		 $<P>$->claus = currclause;
		 $<P>$->sign = SEM_CONSTR;
		 }
		;

	terms :
		term
		| term ',' terms
		 {
		 $<T>1->next = $<T>3;
		 }
		| term error terms
		 {
		 $<T>1->next = $<T>3;
		 }
		| error
		 {
		 $<T>$ = init_term(NULLTERM,0,CONSTANT,0,GROUND);
		 }
		;

	term :
		VAR
		 { 
		 termsize++;
		 if (max_termdepth < termdepth)
		 	max_termdepth = termdepth;
		 $<T>$ = init_term(NULLTERM,$<N>1,VARIABLE,0,NGROUND);
		 VARUSAGE($<T>$);
		 }
		| NUMBER
		 { 
		 termsize++;
		 if (max_termdepth < termdepth)
		 	max_termdepth = termdepth;
		 $<T>$ = init_term(NULLTERM,$<N>1,NUMCONST,numval,GROUND);
		 }
		| STRING
		 { 
		 termsize++;
		 if (max_termdepth < termdepth)
		 	max_termdepth = termdepth;
		 $<T>$ = init_term(NULLTERM,$<N>1,STRINGCONST,0,GROUND);
		 }
		| glvar
		| STRUCVAR
		 { /* only allowed in constraints => scan.l */
		 $<T>$ = init_term(NULLTERM,$<N>1,SVARIABLE,0,NGROUND);
		 }
		| NAME 
		 { 
		 termsize++;
		 if (max_termdepth < termdepth)
		 	max_termdepth = termdepth;
		 $<T>$ = init_term(NULLTERM,$<N>1,CONSTANT,0,GROUND);
		 }
		| NAME '(' inctermdepth terms ')'
		 {
		 termdepth--;
		 termsize++;
		 $<T>$ = init_term($<T>4,$<N>1,CONSTANT,0,GROUND);
		 }
		| '[' inctermdepth list ']'
		 { 
		 termdepth--;
		 termsize++;
		 $<T>$ = init_term($<T>3,HEAD_SYMB,CONSTANT,0,GROUND);
		 }
		| '[' ']'
		 { 
		 termsize++;
		 if (max_termdepth < termdepth)
		 	max_termdepth = termdepth;
		 $<T>$ = init_term(NULLTERM,NIL_SYMB,CONSTANT,0,GROUND);
		 }
		;

	list :
		term
		 { /* list ends with nil element */
		 $<T>1->next = init_term(NULLTERM,NIL_SYMB,CONSTANT,0,GROUND);
		 }
		| term ',' list
		 {
		 $<T>1->next = init_term($<T>3,HEAD_SYMB,CONSTANT,0,GROUND);
		 }
		| term '|' term
		 {
		 $<T>1->next = $<T>3;
		 }
		;

	conslist:
		'[' terms ']'
		 {
		 $<T>$ = init_term($<T>2,-1,CONSTSYM,0,GROUND);
		 }
		;

	glvar :
		GLOBVAR
		 {
		 termsize++;
		 if (max_termdepth < termdepth)
		 	max_termdepth = termdepth;
		 $<T>$ = init_term(NULLTERM,$<N>1,GVARIABLE,0,NGROUND);
		 }
		| GLOBVAR '[' numexpr ']'
		 {
		 termsize++;
		 if (max_termdepth < termdepth)
		 	max_termdepth = termdepth;
		 $<T>$ = init_term($<T>3,$<N>1,GVARIABLE,0,NGROUND);
		 }
		;
	
	numexpr :
		NUMBER
		 { 
		 $<T>$ = init_term(NULLTERM,$<N>1,NUMCONST,numval,GROUND);
		 }
		| VAR
		 { 
		 $<T>$ = init_term(NULLTERM,$<N>1,VARIABLE,0,NGROUND);
		 VARUSAGE($<T>$);
		 }
		| glvar
		| '(' numexpr ')'
		 {
		 $<T>$ = $<T>2;
		 }
		| numexpr '+' numexpr
		 {
		 $<T>1->next = $<T>3;
		 $<T>$ = init_term($<T>1,$<N>2,CONSTANT,0,GROUND);
		 }
		| numexpr '-' numexpr
		 {
		 $<T>1->next = $<T>3;
		 $<T>$ = init_term($<T>1,$<N>2,CONSTANT,0,GROUND);
		 }
		| numexpr '*' numexpr
		 {
		 $<T>1->next = $<T>3;
		 $<T>$ = init_term($<T>1,$<N>2,CONSTANT,0,GROUND);
		 }
		| numexpr '/' numexpr
		 {
		 $<T>1->next = $<T>3;
		 $<T>$ = init_term($<T>1,$<N>2,CONSTANT,0,GROUND);
		 }
		| '-' numexpr %prec '*'
		 {  /* dont use $<N>1 == MINUS_SYMB  */
		 $<T>$ = init_term($<T>2,UMI_SYMB,CONSTANT,0,GROUND);
		 }
		| error
		 {
		 $<T>$ = init_term(NULLTERM,0,CONSTANT,0,GROUND);
		 }
		;
	
	binexpr :
		numexpr RELOP numexpr
		 { /*  =<  <  >  >=  */
		 $<T>1->next = $<T>3;
		 $<P>$ = init_pred($<T>1,$<N>2);
		 }
		| term CONSTRAIN 
		 {
		  if (max_termdepth+1 > currclause->max_termdepth) 
		 	currclause->max_termdepth = max_termdepth+1;
		  termdepth=0;
		  max_termdepth=0;
		 }
			term
		 { /* same operator for constraint =/= !! */
		 $<T>1->next = $<T>4;
		 $<P>$ = init_pred($<T>1,NQAL_SYMB);
		 if (max_termdepth+1 > currclause->max_termdepth) 
		 	currclause->max_termdepth = max_termdepth+1;
		 if (termsize > currclause->max_termsize) 
		 	currclause->max_termsize = termsize;
		 termdepth=0;
		 max_termdepth=0;
		 termsize=0;
		 }
		| term '=' '=' 
		 {
		  if (max_termdepth+1 > currclause->max_termdepth) 
		 	currclause->max_termdepth = max_termdepth+1;
		  termdepth=0;
		  max_termdepth=0;
		 }
			term
		 {
		 $<T>1->next = $<T>5;
		 $<P>$ = init_pred($<T>1,EQAL_SYMB);
		 if (max_termdepth+1 > currclause->max_termdepth) 
		 	currclause->max_termdepth = max_termdepth+1;
		 if (termsize > currclause->max_termsize) 
		 	currclause->max_termsize = termsize;
		 termdepth=0;
		 max_termdepth=0;
		 termsize=0;
		 }
		| term '=' 
		 {
		  if (max_termdepth+1 > currclause->max_termdepth) 
		 	currclause->max_termdepth = max_termdepth+1;
		  termdepth=0;
		  max_termdepth=0;
		 }
			term
		 {
		 $<T>1->next = $<T>4;
		 $<P>$ = init_pred($<T>1,UNIF_SYMB);
		 if (max_termdepth+1 > currclause->max_termdepth) 
		 	currclause->max_termdepth = max_termdepth+1;
		 if (termsize > currclause->max_termsize) 
		 	currclause->max_termsize = termsize;
		 termdepth=0;
		 max_termdepth=0;
		 termsize=0;
		 }
		;

inctermdepth:
		{ termdepth++;}
%%

#include "lex.yy.c"
#include "init.c"

/***********************************************************************/
/* yyerror and additional functions*/
/* orig. from yaccsed.script*/
/***********************************************************************/
yyerror(s)  /* Verbesserte Fehlermeldungen */
register char *s; {  /*nach Schreiner*/
extern int yynerrs;
static int list = 0;
if (!list) {
  printf("[error %d] ",yynerrs+1); yywhere();
  if (list=s[strlen(s)-1]==':') fputs(s,stdout);
    else puts(s);
  } else if (*s!='\n') putchar(' '),fputs(s,stdout);
    else putchar('\n'),list=0;
}

#ifdef hpux
#ifndef linux
extern unsigned char yytext[];
#endif
#else
//JSC-092016 extern char yytext[];
extern char *yytext;
#endif

//JSC 092016 extern int yyleng,yylineno;
extern yy_size_t yyleng;
extern int yylineno;
yywhere() {  /* Fehlerplatzierung durch Bezug auf yytext */
register int colon=0;  /*nach Schreiner*/
if (yylineno>0) {
  if (colon) fputs(", ",stdout);
   printf("line %d",yylineno-((*yytext=='\n')||(!*yytext)));
   colon=1;
  }
switch (*yytext) {
  case '\0':
  case '\n': break;
  default: if (colon) putchar(' ');
    printf("near \"%.*s\"",yyleng>20 ? 20 :
	   yytext[yyleng-1]=='\n' ? yyleng-1 : yyleng,yytext);
    colon=1;
  }
if (colon) fputs(": ",stdout);
}

