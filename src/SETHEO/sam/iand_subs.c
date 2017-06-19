/******************************************************
 *    S E T H E O                                     *
 *                                                    *
 * FILE: iand_subs.c
 * VERSION:
 * DATE:                3.6.93
 * AUTHOR: Dagmar Holzinger
 * NAME OF FILE:
 * DESCR:
 *     
 * MOD:
 ******************************************************/

#ifdef STATPART

#include <stdio.h>
#include "stdlibwrap.h"
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "defaults.h"
#include "codedef.h"
#include "symbtab.h"
#include "disp.h"
#include "sps.h"

/* IAbegin */
/* Prototypen */
Lit_arg* get_term();
Lit*     get_literal();

#define  SSUMED_OR_BOUND   400

S_Lit*         *subsumrel[MAXOR_NODES];
extern Cov_Lit *covered_lits[MAX_TASK_BOUND];
extern ornode_subtask_cnt[MAXOR_NODES];

/*********************************************************/
/*                 check_literal()                       */
/*********************************************************/

check_literal(lit)
Lit *lit;
{
  Lit_arg *term;

  /* Inhalt der Datenstruktur "uberpr"ufen */
  printf("\n%d %d\n", lit->lit_nr, lit->or_nr);
  printf("    %d\n", lit->pred);

  term = lit->arg;
  check_terms(term);
}


/*********************************************************/
/*                    check_terms()                      */
/*********************************************************/

check_terms(term)
  Lit_arg *term;
{
  Lit_arg *arg;

  while(term != NULL)
    {
      printf("\n                %c, %d\n", term->typ, term->symb_nr);
      arg = term->arg;
      check_terms(arg);
      term = term->next_term;
    }
}


/****************************************************************************/
/* get_subs_statistic() berechnet die Werte f"ur die Subsumptions-Statistik */
/****************************************************************************/

get_subs_statistic(subsumed, subsumers, subsumed_or)
int	*subsumed;     /* Zaehler fuer subsumierte unabh. Literale */
int	*subsumers;    /* Zaehler fuer subsumierende unabh. Literale */
int	*subsumed_or;  /* Zaehler fuer subsumierte ODER-Knoten*/
{
  int	j, i;
  Lit	*lit;
  int   tmp;

  *subsumed    = 0;
  *subsumers   = 0;
  *subsumed_or = 0;

  /* subsumierte Literale zaehlen */
  for( i = 1; i <= ornode_counter; i++){
       tmp = 0;
       for( j = 0; j < MAXLITS; j++){
            if( ia_result[i][j] == -2)
                break;
            if( ia_result[i][j] == -1)
	        tmp++;
            }
       if( tmp == j )   /* j statt j-1 !  CS 1.9.94 */
           (*subsumed_or)++;
       (*subsumed) += tmp;
       }

  /* subsumierende Literale zaehlen */
  lit = first_lit;
  while (lit != NULL)
  {
    if (lit->subs) /* subsumierendes Literal */
      ++(*subsumers);
    lit = lit->next_lit;
  }
}


/*********************************************************/
/*       equ_term testet, ob zwei Terme gleich sind       */
/*********************************************************/

equ_term(t1, t2)
  Lit_arg *t1;
  Lit_arg *t2;
{
  Lit_arg *t1_arg;
  Lit_arg *t2_arg;

  if ((t1->typ == t2->typ) && (t1->symb_nr == t2->symb_nr))
  {
    if (t1->typ == 'f')
    {
      t1_arg = t1->arg;
      t2_arg = t2->arg;
      while ((t1_arg != NULL) && (t2_arg != NULL))
      {
        if (!equ_term(t1_arg, t2_arg))
          return(0);
        t1_arg = t1_arg->next_term;
        t2_arg = t2_arg->next_term;
      }
    }
    return(1);
  }
  return(0);
}


/*********************************************************/
/*  bind_var() bindet eine Variable, falls sie noch nicht  */
/*                   gebunden ist                        */
/*********************************************************/

bind_var(t1, t2, c)
  Lit_arg       *t1, *t2;
  Lit_arg       *c[MAXVAR];
{
  int index;

  /* Index von t1 suchen */
  for (index = 0; index < MAXVAR; index++)
    if (vars[index] == t1->symb_nr)
      break;

  if ((index != MAXVAR) && (c[index] != NULL))
  {
    /* Bindung existiert schon */
    if (!equ_term(c[index], t2))
        return(0);
      else
        return(1);
  }

  /* Variable noch nicht gebunden */
  c[index] = t2;

  return(1);
}


/*********************************************************/
/*     smatch() testet, ob term1 term2 subsumiert oder    */
/*                   oder umgekehrt                      */
/*********************************************************/

smatch(t1, t2, c)
  Lit_arg       *t1, *t2;
  Lit_arg       *c[MAXVAR];
{
  Lit_arg       *t1_arg, *t2_arg;
  int 		res, old_res;

/* original code
  if (t1->typ == 'v') *//* t1 ist Variable *//*
  {
    if (bind_var(t1, t2, c))
      return(1); *//* Variable t1 bindet t2 oder t1 == t2 *//*
    else
      return(0); *//* Matching scheitert *//*
  }
  if (t2->typ == 'v') *//* t2 ist Variable und t1 nicht *//*
  {
    if (bind_var(t2, t1, c)) *//* Variable t2 bindet t1 *//*
      return(-1);
    else
      return(0); *//* Matching scheitert *//*
  }
*/
/* CS 10.9.94 KEINE ECHTE SUBSUMPTION MEHR! */
  if (t1->typ == 'v' && t2->typ == 'v') /* t1 ist Variable */
  {
    if (bind_var(t1, t2, c))
      return(1); /* Variable t1 bindet t2 oder t1 == t2 */
    else
      return(0); /* Matching scheitert */
  }
  if (t1->typ == 'v' || t2->typ == 'v') /* term ist Variable und d.a. nicht */
      return(0); /* Matching scheitert */


  /* t1 und t2 sind Funktionen oder Konstanten */
  if ((t1->typ != t2->typ) || (t1->symb_nr != t2->symb_nr))
      return(0);
  if (t1->typ == 'f')
  {
    t1_arg = t1->arg;
    t2_arg = t2->arg;
    old_res = 0;
    while (t1_arg != NULL)
    {
      res = smatch(t1_arg, t2_arg, c);
      if (!res || ((res * old_res) == -1))
        return(0); /* Matching scheitert */
      t1_arg = t1_arg->next_term;
      t2_arg = t2_arg->next_term;
    }
    return(res);
  }
  return(1); /* Konstante t1 == t2 */
}


/*********************************************************/
/* subsum() testet, ob literal1 literal2 subsumiert oder */
/*                   oder umgekehrt                      */
/*********************************************************/

subsum(lit1, lit2)
  Lit *lit1;
  Lit *lit2;
{
  Lit_arg       *term1, *term2;
  int           i, res, old_res;
  Lit_arg       *context[MAXVAR];  /* Bindungskontext */

  term1 = lit1->arg;
  term2 = lit2->arg;
  old_res = 0;
  res = 1;               /* CS 8.11.94 fuer Aussagenlogik */

  /* Bindungskontext initialisieren */
  for (i = 0; i < MAXVAR; i++)
    context[i] = NULL;

  while(term1 != NULL)
  {
    res = smatch(term1, term2, context);
/* KEINE ECHTE UNIFIKATION MEHR! NUR IDENTITAET! CS 5.9.94 */
/* jetzt in smatch realisiert!    res = equ_term(term1, term2);*/

    if (!res || ((res * old_res) == -1))
      return(0); /* Subsumption scheitert */
    term1 = term1->next_term;
    term2 = term2->next_term;
  }
  return(res);
}


/*********************************************************/
/*    get_term() liest einen Term in eine dyn. Daten-    */
/*                       struktur                        */
/*********************************************************/

Lit_arg* get_term(bp, argv) 
environ         *bp;
WORD		*argv;
{
  int 		i, new_argc;
  WORD 		*arg, *new_argv; 
  Lit_arg       *term, *new_term, *pred_term;

  /* Speicherplatz alloziieren */
  term = (Lit_arg*) sam_malloc(sizeof(Lit_arg)); 
  arg = deref (argv, bp);
  switch (GETTAG(*arg))
  {
     case T_CONST:	if (GETVAL(*arg) & IDTAG)
#ifdef WORD_64       
#warning "das wird nicht mehr funktionieren"
#endif       
        		{
          		  /* Symbol */
            		  term->typ = (int) 'c';
            		  term->symb_nr = (int) GETSYMBOL(GETVAL(*arg));
          		}
        		else
        		{
          		  /* Zahl */
          		  term->typ = (int) 'n'; /* Konstante ist Zahl */
          		  term->symb_nr = (int) GETSYMBOL(*arg);
        		}
                        term->arg = NULL;
         		term->next_term = NULL;

			break;
     case T_FVAR:	term->typ = (int) 'v';
		      	term->symb_nr = (int) (arg - stack);
			term->arg = NULL;
      			term->next_term = NULL;

                  	break;
    case T_CRTERM:
    case T_GTERM:
    case T_NGTERM:	term->typ = (int) 'f';
			term->symb_nr = (int) GETSYMBOL(*arg);
			term->arg = NULL;
			term->next_term = NULL;
  			/* moegliche Argumente anhaengen */
                        new_argv = ++arg;
                        new_argc = 0;
                   	while (GETTAG (*new_argv) != T_EOSTR)
                        {
                          ++new_argc;
                          new_term = (Lit_arg *) get_term(bp, new_argv++);
                          if (new_argc == 1) /* erstes Arg. */
                            term->arg = new_term;
                          else
                            pred_term->next_term = new_term;
                          pred_term = new_term;
                        }

                   	break;
    case T_CREF: 	get_term(bp, GETVAL (*arg) + code);

                 	break;
        default: 	break;
  } /* endswitch */
  return(term);
}


/*********************************************************/
/*   get_literal() liest Literal in dyn. Datenstruktur   */
/*********************************************************/

Lit* get_literal(lcb, bp, subgcounter) 
literal_ctrl_block      *lcb;        
environ                 *bp;
int			subgcounter;
{
  int 		arity, i;
  Lit_arg	*term, *pred_term;
  Lit		*lit;

  /* Speicherplatz alloziieren */
  lit = (Lit*) sam_malloc(sizeof(Lit));
  lit->or_nr = ornode_counter;
  lit->lit_nr = subgcounter;
  lit->pred = (int) GETVAL(lcb->ps_symb);
  lit->subs = FALSE;
  lit->arg = NULL;
  lit->next_lit = NULL;
  
  arity = symbtab[GET_PRED(GETVAL(lcb->ps_symb))].arity;
  if (arity > 0)
    for (i = 0; i < arity ;i++)
    {
      term = (Lit_arg *) get_term(bp, code +  lcb->argv + i);
      if (i == 0) /* 1.Argument der Literals */
        lit->arg = term;
      else  /* Argumentliste des Literals enthaelt schon mind. 1 Arg. */
        pred_term->next_term = term;
      pred_term = term;
    }
  return(lit);
}


/*********************************************************/
/*    check_indp_subg() testet unabhh. Subgoals auf      */
/*                 Subsumierbarkeit                      */
/*********************************************************/

check_indp_subg(bp, pt_subgcounter)
environ *bp;
int     *pt_subgcounter;  
{
  WORD  *sg_i;
  int   nr_sgls, i, j, r, subsumer, redundant;
  Lit	*curr_lit, *predecessor, *literal;

  sg_i =  (WORD *)bp + SIZE_IN_WORDS(environ)
          + GETNUMBER(bp->lcbptr->nr_vars);

  nr_sgls = GETNUMBER(bp->lcbptr->nr_sgls);

  /* offene Subgoals des aktuellen Environments ausgeben */
  for (i = 0; i < nr_sgls; i++)
  {
    if (sg_i > bp->tp) /* subgoals not yet been touched */
    {
      if (ia_result[ornode_counter][*pt_subgcounter] == 0) 
      /* subgoal is independent */
      {
        curr_lit = (Lit *) get_literal((literal_ctrl_block *) GETPTR(*sg_i), bp, *pt_subgcounter);
        /* Variablen initialisieren */
        subsumer = FALSE;
        redundant = FALSE;
        predecessor = NULL;

        /* Subsumierbarkeit des Literals testen */
        literal = first_lit; /* erstes Element der Literalliste */
        while (literal != NULL)
        {
          if (literal->pred == curr_lit->pred)
          {
            r = subsum(literal, curr_lit);
            if (r == 1) /* literal subsumiert curr_lit */
            {
              literal->subs = TRUE;
              redundant = TRUE;
              enter_subsumrel(literal->or_nr, literal->lit_nr, 
                              curr_lit->or_nr, curr_lit->lit_nr);

              ia_result[curr_lit->or_nr][curr_lit->lit_nr] = -1;
              break;
            }
            else
              if (r == -1) /* curr_lit subsumiert literal */
              {
	      printf("THIS CASE SHOULD NOT OCCUR, SINCE NO REAL SUBSUMPTION\n");
                subsumer = TRUE;
               enter_subsumrel(curr_lit->or_nr,curr_lit->lit_nr,
                               literal->or_nr, literal->lit_nr);
                ia_result[literal->or_nr][literal->lit_nr] = -1;
                /* literal aush"angen */
                if (predecessor == NULL) /* literal ist erstes Element */
                  first_lit = first_lit->next_lit;
                else
                {
                  predecessor->next_lit = literal->next_lit;
                  if (literal == last_lit)
                    last_lit = predecessor;
                }
                free(literal);
                /* Literalliste fortschalten */
                if (predecessor == NULL)
                  literal = first_lit;
                else
                  literal = predecessor->next_lit;
                continue;
              }
          }
          predecessor = literal;
          literal = literal->next_lit;
        } /* endwhile */
        if (redundant) /* curr_lit ist subsumiert worden */
           /* curr_lit l"oschen */
           free(curr_lit);
        else
          if (subsumer) /* curr_lit hat mind. ein Literal subsumiert */
          {
            /* curr_lit am Anfang einf"ugen */
            curr_lit->next_lit = first_lit;
            first_lit = curr_lit;
            curr_lit->subs = TRUE;
          }
        else 
        {
          /* curr_lit am Ende anh"angen */
          if (last_lit != NULL)
              last_lit->next_lit = curr_lit;
            else
              first_lit = curr_lit;
            last_lit = curr_lit;
        }
/*  check_literal(curr_lit);   TEST  */
      }
      ++(*pt_subgcounter); /* Subgoalz"ahler erh"ohen */
    }
    sg_i++;
  }
}


/*********************************************************/
/*      subs_check() testet die unabh. Subgoals eines    */
/*        Oder-Knotens auf Subsumierbarkeit              */
/*********************************************************/

subs_check(bp)
environ *bp;
{
  int	subgcounter;

  subgcounter = 0;
  while (bp >= (environ *) stack)
  {
    check_indp_subg(bp, &subgcounter, subsumrel);
    bp = bp->dyn_link;
  }
}
/* IAend */

int enter_subsumrel(ssuming_or_nr, ssuming_lit_nr, ssumed_or_nr, ssumed_lit_nr)
int ssuming_or_nr, ssuming_lit_nr, ssumed_or_nr, ssumed_lit_nr;
{
 int   i;
 long  bitmask;
 S_Lit *slit_pt;

/*
 printf("enter_subsumrel: [%d][%d][%d][%d]...", ssuming_or_nr, ssuming_lit_nr, 
                                               ssumed_or_nr, ssumed_lit_nr);
					       fflush(stdout);
*/
 ornode_subtask_cnt[ssumed_or_nr]++;
 if( ssumed_lit_nr > 8*sizeof(long) )
     sam_error("bitmask too small!!\n", NULL, 2);
 bitmask = 1;
 bitmask <<= ssumed_lit_nr;
 if( subsumrel[ssuming_or_nr] == NULL ){
     if( ssuming_lit_nr > MAXLITS )
         sam_error("MAXLITS too small!", NULL, 2);
     subsumrel[ssuming_or_nr] = (S_Lit* *)sam_malloc(MAXLITS * sizeof(S_Lit*));
     if( subsumrel[ssuming_or_nr] == NULL )
         sam_error("malloc for subsumrel failed!\n", NULL, 2);
     for( i = 0 ; i < MAXLITS ; i++ )
          subsumrel[ssuming_or_nr][i] = NULL;
     }
 if( subsumrel[ssuming_or_nr][ssuming_lit_nr] == NULL ){
     subsumrel[ssuming_or_nr][ssuming_lit_nr] = 
                             (S_Lit *)sam_malloc(SSUMED_OR_BOUND * sizeof(S_Lit));
     if( subsumrel[ssuming_or_nr][ssuming_lit_nr] == NULL )
         sam_error("malloc for subsumrel failed!\n", NULL, 2);
     subsumrel[ssuming_or_nr][ssuming_lit_nr]->or_nr  = ssumed_or_nr; 
     subsumrel[ssuming_or_nr][ssuming_lit_nr]->lit_nr = ssumed_lit_nr; 
     subsumrel[ssuming_or_nr][ssuming_lit_nr]->next   = NULL; 
     return;
     }

 for( i = 0, slit_pt = subsumrel[ssuming_or_nr][ssuming_lit_nr] ;
      i < SSUMED_OR_BOUND && slit_pt->next != NULL ; 
      i++, slit_pt = slit_pt->next );
 if( i >= SSUMED_OR_BOUND )
     sam_error("SSUMED_OR_BOUND too small!", NULL, 2);
 slit_pt->next   = slit_pt + 1;
 slit_pt++;                       /* goto next free element */
 slit_pt->or_nr  = ssumed_or_nr;
 slit_pt->lit_nr = ssumed_lit_nr;
 slit_pt->next   = NULL;
 /*printf("exit\n");*/
}


#else
/* ANSI C doesn't like empty files ... */
int dummy_dummy_iand_subs () { return 42; }
#endif /* STATPART */

