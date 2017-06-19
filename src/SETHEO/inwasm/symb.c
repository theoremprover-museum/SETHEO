/*****************************************************************************
;;; MODULE NAME : symb.c
;;;
;;; PARENT      : pars.y  
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)symb.c	15.1 01 Apr 1996 
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 15.5.91
;;;
;;; DESCRIPTION : create, enter & output of symboltable
;;;
;;; REMARKS     :
;;;		4.6.93	jsc	new structure for built-ins
;;;		2.7.93	jsc	extern.h
;;;		24.3.94	jsc	anonymous variables
;;;		11.9.95	jsc	predicate_nr gets global
;;;             13.9.95 jsc     different usage of nr_*
;;;		22.1.96	jsc	hex-out
;;;
******************************************************************************/

#include "hsearch.h"
#include "symb.h"
#include "extern.h"

/*****************************************************************************/
/* extern variables                                                          */
/*****************************************************************************/
extern ENTRY *hsearch();


/*****************************************************************************/
/* statics                                                                   */
/*****************************************************************************/
static int constant_nr  = 1;	/* counters codenr ->get_codenr */
static int anonymous_var_cnt = 1; /* counter for anonmous variables */

/*****************************************************************************/
/* functions (inwasm/symb.c)                                                 */
/*****************************************************************************/
char *get_name();
int get_symbnr();
void init_symb();
int enter_token();
int enter_name();
int enter_built();
int enter_globvar();
void enter_globarray();
void enter_occ();
int get_codenr();
void printsytab();
void count_codenr();


/*****************************************************************************/
/* get_name()                                                                */
/* Description: look for s in hash table and return pointer to symboltable   */
/*              if found, otherwise NULL                                     */
/*****************************************************************************/
char *get_name(s)
char *s;
{
  ENTRY e;
  ENTRY *ep;

  e.key = s;
  e.data = NULL;
  ep = hsearch(e,FIND);

  if (ep) {

#ifdef DEBUG
    fprintf(stderr,"found %s at %x\n",ep->key,ep);
#endif

    return ep->data ;
  }
  else
    return NULL;
}		


/*****************************************************************************/
/* get_symbnr()                                                              */
/* Description:  return symbolnumber of built_ins/head of token list         */
/*****************************************************************************/
int get_symbnr(token)
char token[NAMELENGTH];
{
  syel *sp;

  sp = (syel *)get_name(token);
  if (!sp) {
    nferror("token not defined",yylineno);
    return 0;
  }
  else
    return sp->symbnr;
}


/*****************************************************************************/
/* init_symb()                                                               */
/* Description: initialze the symbol table                                   */
/*****************************************************************************/
void init_symb()
{
  
  /* initialize symbol table */
  
  if ((symbtab  = (syel *)cnt_malloc( (unsigned)MAXSYMB*sizeof(syel))) == NULL) {
    fprintf(stderr, "  Error (symb.c - init_symb()): Memory allocation error.\n");
    exit(50);
  }

  /* initialize hash table */
  (void) hcreate(MAXSYMB); 

  /* enter built-ins */
  enter_built_ins();
}


/*****************************************************************************/
/* enter_token()                                                             */
/* Description: first entry or lookup token  return index of token list      */
/*              (bucket)                                                     */
/*****************************************************************************/
int enter_token(token)
char * token;
{
  syel *sp;
  ENTRY e;
  
  /* strip off '^'-char at the beginning of the token */
  if (*token == '^') {
    /* DEBUG:
       printf("Before: New entry 'enter token': %s (%d)\n",token, ignorelits_flag); */ 
    ignorelits_flag = 1;
    token++;
    /* DEBUG:
       printf("After: New entry 'enter token': %s (%d)\n",token, ignorelits_flag); */
  } else if (*token == '@') {
    if ((token[1]) == '$') {
      subgoal_flag = 1;
    } else {
      token++;
      subgoal_flag = 1;
    }
  }
  /* printf("New entry 'enter token': %s\n",token); */
  
  /* look if it is already there */
  if ((sp = (syel *)get_name(token)) == NULL) {
    
#ifdef DEBUG
    fprintf(stderr,"New entry 'enter token': %s\n",token);
#endif
    
    /* first occurrence of name    new entry */
    sp = symbtab + nump;
    
    /* check, if it is an anonymous variable */
    if ((*token == '_') && !*(token+1)) {
      /* yes, it is one */
      sprintf(sp->name,"__%d",anonymous_var_cnt++);
    }
    else {
      strncpy(sp->name,token,NAMELENGTH);
    }
    sp->symbnr	= nump++;
    sp->arity	= -1;
    sp->index	= 0;
    sp->codenr	= 0;
    sp->bfun	= NULL;
    sp->loclist	= NULL;
    sp->next	= NULL;
    e.key = sp->name;
    e.data = (char *)sp;

    if (sp->symbnr > MAXSYMB-1) { 
	fprintf(stderr,"  Symboltable too small. Size: %d\n", MAXSYMB); 
	exit(52); 	
    }

    hsearch(e,ENTER);
    return sp->symbnr;
  }
  else 
    return sp->symbnr;
}


/*****************************************************************************/
/* enter_name()                                                              */
/* Description: look up name in symboltable  return index                    */
/*              arity==-1 <=> name has to be updated                         */
/*****************************************************************************/
int enter_name(head,type,arity,modunr)
int head;
int type;
int arity;
int modunr;
{
  char text[30];
  syel *sp;
  syel *s = NULL;

  /* check for built-ins */
  if (head < nrbuilt) {

    /* arity check */
    if (arity != -1 && arity != symbtab[head].arity)
      nferror("wrong arity (built-in)",yylineno);
    if (type == CONSTANT) 
      /* count arithmetic functions */
      symbtab[head].index += 1;    
    return head;
  }

  /* search in list */
  sp = symbtab+head;
  while (sp) {
    if (sp->arity == -1) {
      /* update occurrence of name in symbtab */
      symbtab[sp->symbnr].type	= type;
      symbtab[sp->symbnr].arity	= arity;
      symbtab[sp->symbnr].modunr	= modunr;
      switch (type) {
      case GVARIABLE:
	symbtab[sp->symbnr].index    = nr_globals++;
	break;
      case NUMCONST:
	symbtab[sp->symbnr].index    += 1;
	nr_numbers++;
	break;
      case STRINGCONST:
	symbtab[sp->symbnr].index    += 1;
	nr_constants++;
	break;
      case CONSTANT:
	symbtab[sp->symbnr].index    += 1;
	if (arity) {
	  nr_functions++;
	  if (arity > max_arity_fsymb) {
	    max_arity_fsymb=arity;
	  }
	}
	else 
	  nr_constants++;
	break;
      case CONSTSYM:
	symbtab[sp->symbnr].index    += 1;
	break;
      case PREDSYM:
	symbtab[sp->symbnr].codenr   =  predicate_nr++;
	nr_predicates++;
	break;
      case BUILTIN:
	symbtab[sp->symbnr].codenr   =  predicate_nr++;
	break;
      default:
	/* count nothing */
	break;
      }
      return (sp->symbnr);
    }
    else if (sp->modunr == modunr) {
      /* symbol found  n th occurency */
      if ((arity != -1) && (sp->type != type)) {
	/* do not print this warning for          */
	/* sp->type = PREDSYM and type = CONSTANT */
	if ((sp->type != PREDSYM) || (type != CONSTANT)){
	  sprintf(text," different types %c/%c",sp->type,type);
	  warning(NAM(head),text);
	}
      }
      
      if (arity != -1 
	  && (sp->arity != arity) 
	  && arity				/* jsc */
	  && sp->type != GVARIABLE){
	sprintf(text,"%s with wrong arity",sp->name);
	nferror(text,yylineno);
      }
      
      /* count occurencies of constants+functions */
      if (type==CONSTANT || type==STRINGCONST || 
	  type==NUMCONST || type==CONSTSYM)
	symbtab[sp->symbnr].index    += 1;
      
      return (sp->symbnr);
    }
    /* next entry ? */
    s = sp;
    sp = sp->next;
  }
  
  /* not found in list new entry 		  */
  /* only GVARIABLE + PREDSYM can be nested */
  sp = symbtab + nump;
  strncpy(sp->name,NAM(head),NAMELENGTH);
  sp->modunr      = modunr;
  sp->symbnr      = nump++;
  sp->type        = type;
  sp->arity       = arity;
  sp->index       = (type == GVARIABLE)? nr_globals++ : 0;
  sp->bfun	= NULL;
  sp->next        = NULL;
  s->next = sp;
  
  if (type==PREDSYM || type==BUILTIN)
    symbtab[sp->symbnr].codenr   =  predicate_nr++;
  
  return sp->symbnr;
}


/*****************************************************************************/
/* enter_built()                                                             */
/* Description: new entry of predfined symbols: built-ins,+,-,$Inf,$Copies,  */
/*              .....                                                        */
/*              enterd with modunr 0 !                                       */
/*****************************************************************************/
int enter_built(token,type,built_type,arity,bfun,instr_name,opcode)
char *token;
int type;
built_in_type	built_type;
int  arity;
int  (*bfun)();
/* const */ char *instr_name;
int opcode;
{
  syel *sp;
  ENTRY e;

  if (!(sp = (syel *)get_name(token))) {
    sp = symbtab + nump;
    strncpy(sp->name,token,NAMELENGTH);
    sp->modunr      = 0;
    sp->symbnr      = nump++;
    sp->type        = type;
    sp->arity       = arity;
    sp->bfun	= bfun;
    sp->loclist     = NULL;
    sp->next        = NULL;
    sp->index      	= 0;
    sp->b_instr_name= instr_name;
    sp->b_type      = built_type;
    sp->b_opcode    = opcode;
    if (type==BUILTIN)
      sp->codenr = predicate_nr++;
    else if (type==CONSTANT) {
      /* do nothing: was adjust nr_* */
      ;
    }
    else 
      /* $Depth $Copies $Inf  und DEFAULTQUERY !! */
      sp->codenr = 0;
    
    /* enter in hash table */
    e.key = sp->name;
    e.data = (char *)sp;
    hsearch(e,ENTER); 
    
    nrbuilt += 1;
  }
  else 
    warning(sp->name," built-in declared twice");
  
  return (sp->symbnr);
}


/*****************************************************************************/
/* enter_globvar()                                                           */
/* Description: enter global variable into symbol table;  check interface    */
/*****************************************************************************/
int enter_globvar(symb,module,inter)
int symb;
int inter;
modtype *module;
{
  syel *slp;
  modtype *mlp;

#ifdef DEBUG
  fprintf(stderr,"enter globvar: %s (inter: %d)\n",NAM(symb),inter);
#endif

  if (inter)  /* global variable occur in interface, look up with modunr */
    return enter_name(symb,GVARIABLE,0,module->modunr);

  if (symbtab[symb].arity == (-1)) /* gl var 1.occurence, enter global */
    return enter_name(symb,GVARIABLE,0,0);

  for (mlp = module; mlp->prev; mlp = mlp->prev) {
    /* look in all static predecessors for local definition */
    slp = symbtab + symb; 	/* start with first entry */
    for ( ;slp; slp = slp->next) 
      if (slp->modunr == mlp->modunr)	/* globvar found */
	return slp->symbnr;
  }

  /* not found, look up global (modunr=0) */
  return enter_name(symb,GVARIABLE,0,0);
}


/*****************************************************************************/
/* enter_globarray()                                                         */
/* Description: enter and check index of global array                        */
/*****************************************************************************/
void enter_globarray(tp)
termtype *tp;
{
  termtype *gvar;

#ifdef DEBUG
  fprintf(stderr,"enter_globarray: %s %d \tnr_globals:%d\n",
	  NAM(tp->next->symb),tp->value,nr_globals);
#endif

  if (!(gvar = tp->next)) 
    return;
  if (gvar->type != GVARIABLE)
    warning(NAM(gvar->symb),"wrong type in mkglob/2");
  else if (! tp->type == NUMCONST)
    warning(NAM(gvar->symb),"first argument of mkglob/2 must be a number");
  else if (!tp->value)
    warning(NAM(gvar->symb),"array with size 0 useless");
  else if (symbtab[gvar->symb].arity)
    warning(NAM(gvar->symb),"array already defined");
  else {
    symbtab[gvar->symb].arity = tp->value;
    /* array[0]...array[value-1] 		*/
    /* array[0] already counted => -1	*/
    nr_globals += (tp->value-1);
  }
}


/*****************************************************************************/
/* enter_occ()                                                               */
/* Description: enter predicate occurrence into symbol-table                 */
/*****************************************************************************/
void enter_occ(pr)
predtype * pr;
{
  predlist * loelt;
  
  if ((loelt  = (predlist *)cnt_malloc(sizeof(predlist))) == NULL) {
    fprintf(stderr, "  Error (symb.c - enter_occ()): Memory allocation error.\n");
    exit(50);
  }

  loelt->pr = pr;
  loelt->next = symbtab[pr->symb].loclist; 	/* append to list */
  symbtab[pr->symb].loclist = loelt;      	/* at beginning */
}


/*****************************************************************************/
/* get_codenr()                                                              */
/* Description: return the codenr for symbtab in *.s file                    */
/*****************************************************************************/
int get_codenr(symb)
int symb;
{
  int type;
  
  if (!symb) 
    /* default query is not transformed */
    return (symb);

  type = symbtab[symb].type;
  if (type==CONSTANT || type==STRINGCONST || type==CONSTSYM)
    return (symbtab[symb].codenr);
  else if (type==PREDSYM || type==BUILTIN)
    return (symbtab[symb].codenr+constant_nr-1);
  else
    /* NUMCONST *VARIABLE */
    return (symb);
}


/*****************************************************************************/
/* printsytab()                                                              */
/* Description: print formatted content of symboltable                       */
/*****************************************************************************/
void printsytab(anf)
int anf;
{
  int i;

  for (i=anf; i<nump;i++) {
    printf("entry #%d %-10s ",i,NAM(i));
    printf("typ:%c mod:%d ari:%d ",symbtab[i].type,symbtab[i].modunr
	   ,symbtab [i].arity);
    if (symbtab[i].index>0) printf("ind:%2d ",symbtab[i].index);
    if (symbtab[i].codenr>0) printf("cod:%2d ",symbtab[i].codenr);

#ifdef DEBUG
    if (symbtab[i].bfun) printf("\tbfun:%x ",symbtab[i].bfun);
#endif

    if (symbtab[i].next) printf("\tnxt:%d\n",symbtab[i].next->symbnr);
    else printf("\n");
  }
}


/*****************************************************************************/
/* count_codenr()                                                            */
/* Description: count the correct number to use in *.s File                  */
/*****************************************************************************/
void count_codenr()
{
  int i;

  for (i=0; i<nump; i++) 
    switch (symbtab[i].type) {
    case STRINGCONST:
    case CONSTANT:
    case CONSTSYM:
      symbtab[i].codenr   =  constant_nr++;
      break;
    default:
      break;
    }
}
