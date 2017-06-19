/******************************************************
 *    S E T H E O                                     *
 *                                                    *
 * FILE: iand.c
 * VERSION:
 * DATE:                14.5.93
 * AUTHOR: Dagmar Holzinger
 * NAME OF FILE:
 * DESCR:
 *     
 * MOD:
 ******************************************************/

#ifdef STATPART

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "defaults.h"
#include "codedef.h"
#include "symbtab.h"
#include "disp.h"
#include "sps.h"

/* IAbegin */
/*********************************************************/
/* print_bitstr() gibt einen Bitstring am Bildschirm aus */
/*********************************************************/

print_bitstr(bitstr, len)
long    bitstr;
int     len;
{
  int i;
  long bitmask;

  bitmask = 1;
  for (i = 0; i < len; i++)
  {
    if (bitstr & bitmask)
     putchar('1');
    else
      putchar('0');
    bitmask <<= 1;
  }
  putchar('\n');
}


/******************************************************************/
/* get_iand_statistic() berechnet die Werte f"ur die IA-Statistik */
/******************************************************************/

get_iand_statistic(max_subtask_id)
{
  int	i, indp_subgs; /* zaehlt die unabh. Subgoals */

  indp_subgs = 0; /* Initialisierung */
  for (i = 0; i < MAXLITS; i++)
  {
    if (ia_result[ornode_counter][i] == -2)
      break;
    if (ia_result[ornode_counter][i] == 0)
      ++indp_subgs;
  }
  num_of_tasks += (indp_subgs + max_subtask_id);
  if ((max_subtask_id + indp_subgs) > 1)
  {
    ++cnt_indp_or;
    cnt_indp_grp += (max_subtask_id + indp_subgs);
  }
}

  
/*********************************************************/
/*   get_indp_tasks() liest die unabh"angigen Tasks aus  */
/* var_in_node aus und tr"agt das Ergebnis in ia_result ein */
/*********************************************************/

get_indp_tasks(var_in_node, bitstrpos)
long    var_in_node[MAXVAR][MAXBITSTR];
int     bitstrpos;
{
  int 	i, j, k, subtask_id, cnt_lits_in_task, first_lit;
  long	bitmask;
  

  subtask_id = 1;
  for (i = 0; i < num_of_vars; i++)
  {
    cnt_lits_in_task = 0;
    for (j = 0; j <= bitstrpos; j++)
    if (var_in_node[i][j] != 0)
    {
      bitmask = 1;
      for (k = 0; k < 32; k++)
      {
        if (var_in_node[i][j] & bitmask) /* Bit gesetzt */
        {
          if (cnt_lits_in_task == 0)
            first_lit = j + k; /* 1. Literal einer Task zwischenspeichern */
          else
          {
            if (cnt_lits_in_task == 1) /* zweites Literal der Task */
              ia_result[ornode_counter][first_lit] = subtask_id;
            ia_result[ornode_counter][j + k] = subtask_id;
          }
          ++cnt_lits_in_task;
        } 
        bitmask <<= 1;
      }
    }
    if (cnt_lits_in_task > 1)
      ++subtask_id;
  }
  get_iand_statistic(--subtask_id);
}  


/**************************************************************/
/* indp_and() "uberpr"uft die Literale eines Oder-Knotens auf */
/* Unabh"anigkeit und erstellt Gruppen unabh"angiger Literale */
/**************************************************************/

indp_and(var_in_node, bitstrpos, num_of_lits)
long 	var_in_node[MAXVAR][MAXBITSTR];
int	bitstrpos;
int 	num_of_lits;
{
  int 	i, j, k, var_match;
  long	*src, *dst;

  if (num_of_lits ==  1)
    num_of_tasks++;  /* get_iand_statistic(0);   same effect!  CS 3.9.94 */
  else
  {
    for (i = num_of_vars - 1; i > 0; i--)
    {
      src = var_in_node[i]; 
      for (j = i - 1; j >= 0; j--)
      {
        var_match = FALSE;
        dst = var_in_node[j];
        for (k = 0; k <= bitstrpos; k++)
          if (dst[k] & src[k])
          {
            var_match = TRUE;
            break;
          }
        if (var_match)
        {
          for (k = 0; k <= bitstrpos; k++)
          {
            dst[k] |= src[k];
            src[k] = 0;
          }
          break;
        }
      }
    }
    get_indp_tasks(var_in_node, bitstrpos); /* Ergebnis einlesen */
  }
}


/*********************************************************/
/*   get_var_in_arg() erkennt Variablen in Termen        */
/*********************************************************/

get_var_in_arg(bp, argv, bitmask, bitstrpos, var_in_node) 
environ         *bp;
WORD		*argv;
long            bitmask;
int		bitstrpos;
long            var_in_node[MAXVAR][MAXBITSTR];
{
  int i;
  WORD 	*arg, *new_argv; 

  arg = deref (argv, bp);
  switch (GETTAG(*arg))
  {
     case T_FVAR:	/* Variablen in Abh"angigkeitsliste eintragen */
                 	for (i = 0; i < num_of_vars && 
                                    vars[i] != (int) (arg - stack); i++);
                 	if( i == num_of_vars){ /*Var. noch nicht vorhanden*/
                     	    if( num_of_vars == MAXVAR)
			    sam_error("Zu viele Variablen! MAXVAR erhoehen!\n",
			              NULL,2);
                    	    vars[i] = (int) (arg - stack);
                   	    ++num_of_vars;
                 	    }
                        var_in_node[i][bitstrpos] |= bitmask; /* Bit setzen */

                  	break;
    case T_CRTERM:
    case T_NGTERM:	/* T_GTERM: nicht behandelt,darf keine Var. enth.*/
         	 	new_argv = ++arg;
                   	while (GETTAG (*new_argv) != T_EOSTR)
                     	  get_var_in_arg(bp, new_argv++, bitmask, bitstrpos, var_in_node);

                   	break;
    case T_CREF: 	get_var_in_arg(bp, GETVAL (*arg) + code, bitmask, 
                                       bitstrpos, var_in_node);
                 	break;
        default: 	break;
  } /* endswitch */
}


/*********************************************************/
/* get_var_in_subg() erkennt die Variablen von Subgoals  */
/*********************************************************/

get_var_in_subg(lcb, bp, bitmask, bitstrpos, var_in_node) 
literal_ctrl_block      *lcb;        
environ                 *bp;
long			bitmask;
int			bitstrpos;
long			var_in_node[MAXVAR][MAXBITSTR];
{
  int arity;
  int i;

  arity = symbtab[GET_PRED(GETVAL(lcb->ps_symb))].arity;
  for (i = 0; i < arity; i++)
    get_var_in_arg(bp, code + lcb->argv + i, bitmask, bitstrpos, var_in_node);
}


/***************************************************************************/
/* get_open_subg() speichert die Variablen der Subgoals eines Environments */
/***************************************************************************/

get_open_subg(bp, pt_bitmask, pt_lit_nr, pt_bitstrpos, var_in_node)
environ *bp;
long    *pt_bitmask;
int     *pt_lit_nr;  
int	*pt_bitstrpos;
long    var_in_node[MAXVAR][MAXBITSTR];
{
  WORD  *sg_i;
  int   nr_sgls, i;

  sg_i =  (WORD *)bp + SIZE_IN_WORDS(environ)
          + GETNUMBER(bp->lcbptr->nr_vars);

  nr_sgls = GETNUMBER(bp->lcbptr->nr_sgls);

  /* offene Subgoals des aktuellen Environments ausgeben */
  for (i = 0; i < nr_sgls; i++)
  {
    if (sg_i > bp->tp) /* subgoals not yet been touched */
    {
       /* Variablen auslesen */
      get_var_in_subg((literal_ctrl_block *) GETPTR(*sg_i), bp, *pt_bitmask, *pt_bitstrpos, var_in_node);
      ++(*pt_lit_nr); /* Subgoalz"ahler erhoehen */
      if (((*pt_lit_nr) % 32) == 0) /* longinteger wird "uberschritten */
      {
        if( ++(*pt_bitstrpos) == MAXBITSTR)/* Position des Bitstrings erhoehen*/
	    sam_error("Zu viele Literale! MAXLITS und MAXBITSTR erhoehen!\n",
	              NULL,2);
        (*pt_bitmask) = 1;
      }
      else
        (*pt_bitmask) <<= 1; /* Bitmaske verschieben */
    }
    sg_i++;
  }
}


/*********************************************************/
/* store_variables() speichert die Variablen eines Oders */
/*********************************************************/

store_variables(bp, pt_bitstrpos, pt_lit_nr, var_in_node)
environ *bp;
int	*pt_bitstrpos;
int	*pt_lit_nr;  
long	var_in_node[MAXVAR][MAXBITSTR];
{
  int	i;
  long 	bitmask; 

  bitmask = 1;                     /* Initialisierung */
  while (bp >= (environ *) stack)  /* stack = Bottom of Stack */
  {
    get_open_subg(bp, &bitmask, pt_lit_nr, pt_bitstrpos, var_in_node);
    bp = bp->dyn_link;
  }
  /* Datenstruktur ia_result vorbesetzen */
  if( (*pt_lit_nr) > MAXLITS)
      sam_error("Zu viele Literale! MAXLITS und MAXBITSTR erhoehen!\n",NULL,2);
  for (i = 0; i < (*pt_lit_nr); i++)
    ia_result[ornode_counter][i] = 0;
}
/* IAend */


#else
/* ANSI C doesn't like empty files ... */
int dummy_dummy_iand () { return 42; }
#endif /* STATPART */

