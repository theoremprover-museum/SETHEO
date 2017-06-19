/* perform_red_step fuer i_call.c und init_inf_step_area fuer instr.c
   hierher ausgelagert (Wunsch von SETHEO) */


#ifdef STATPART

#include <stdio.h>
#include "tags.h"
#include "machine.h"
#include "symbtab.h"
#include "sps.h"

extern S_Lit* *subsumrel[MAXOR_NODES];
extern int      saved_reg_constr, saved_ts_constr;

static int      spgen;
static int      iand;     /* IAnd */
static int      subsume;  /* IAnd */
static int      taskgen;
static int      subgs;    /* IAnd */
static int      sprecomp;

FILE                   *taskfile;
static int             ptree = FALSE;      /* proof-tree output on/off */
char    *ext;                /* f. Extension der Res.-Datei f.par. Sim.*/
int     tab_pos;
char    prooftask[500];
char    *c_pt;
char    rmcommand[50];

void perform_red_step( lcb )
  literal_ctrl_block  *lcb;
  {
  register environ *pdp;
  register int     i;
  pdp = goal_bp;
  i = node_ptr->red_lvl;
  while( i-- ){ /* get beginning of reduction list */
    pdp = pdp->dyn_link;
    }
  if( !un_red(pdp, lcb->ps_symb, symbtab[lcb->ps_symb].arity) ){
      /* no reduction possible */
      sam_error("ERROR in RECOMPUTATION",NULL,2);  /* EXITS */
      }
  goal_bp->tp = pdp;   /* Achtung!!!! */
  }


void init_inf_step_area(){
  /* Im Fall der Wiederberechnung einer Task muss diese fuer jede zu */
  /* beweisende Tiefe jedesmal vor dem Instruction-Cycle neu in die  */
  /* inf_step_area eingelesen werden. Waehrend der kompletten Dauer  */
  /* des Beweisversuches einer einzelnen Task ist diese in der Daten-*/
  /* struktur tableau[] gespeichert.                                 */
  inf_step *is;
  int      tab_pos;

  is = inf_step_area - 1;
  tab_pos = 0;

  do
    {
    is++;
    is->codeptr = tableau[tab_pos].codeptr;
    is->red_lvl = tableau[tab_pos++].red_lvl;
    }
  while (is->codeptr != -1);

  /* IAbegin */
  if (iand_task_num != -2) /* IA-Task vorhanden */
   {
     now_next_subg = TRUE; /* neu initialisiert */
     num_of_subgs  = 0; /* neu intialisiert */
     do
     {
        is++;
        is->codeptr = tableau[tab_pos++].codeptr;
        is->red_lvl = 0;
     }
     while (is->codeptr != -2);
  }
  /* IAend */

  node_ptr = inf_step_area;
  }


init_spt_it_deep(){
  int i, j;

  ppt = 0;
  prev_env_clbl = 0;
  inf_step_area_start = inf_step_area;
  if( iand_check || subsume_check) {
      for( i = 0; i < MAXOR_NODES; i++)
           for( j = 0; j < MAXLITS; j++)
                ia_result[i][j] = -2;
      for( i = 0 ; i < MAXOR_NODES ; i++ )
           if( subsumrel[i] != NULL ){
               for( j = 0 ; j < MAXLITS ; j++ ){
                    free(subsumrel[i][j]);
                    subsumrel[i][j] = NULL;
                    }
               free(subsumrel[i]);
               subsumrel[i] = NULL;
               }
      num_of_vars = 0;
      for( i = 0; i < MAXVAR; i++)
           vars[i] = -1;
      first_lit = NULL;
      last_lit = NULL;
      cnt_indp_or = 0;
      cnt_indp_grp = 0;
      num_of_tasks = 0;
      }
  ornode_counter = 0;
  }



#else
/* ANSI C doesn't like empty files ... */
int dummy_dummy_spt_aux () { return 42; }
#endif /* STATPART */
