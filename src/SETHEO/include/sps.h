#ifndef SPS_H
#define SPS_H

/****************************************************************/
/*** defaults for Static Partitioning                           */
/****************************************************************/
#ifdef  STATPART
#define MAX_TABLEAU_SIZE   100  /* max. number of stored inferences  */
                                /* for one single Or-Node            */
#define MAXVAR             100  /* IAnd: max. number of variables    */
#define MAXBITSTR            4  /* IAnd: max. number of literals per */
                                /* or_node = MAXBITSTR * 32          */
#define MAXOR_NODES       5000  /* IAnd: max. number of or-nodes     */
#define MAXLITS            128  /* IAnd: max. number if literals per or-node */
#define MAX_TASK_BOUND    5000  /* max. Anzahl von Tasks die gen werden soll */
#define MAX_OR_SUBTASKS     20
#define MAX_AFFECTED_ORNODES 100

/******************************************************************/
/*** Def. v. fprintf-Makros f. Ausgabe in Dateien m. para. Simul. */
/******************************************************************/
#define PRINT_RES0(A) {fprintf(resfile,A);fflush(resfile);}
#define PRINT_RES1(A,B) {fprintf(resfile,A,B);fflush(resfile);}
#define PRINT_RES2(A,B,C) {fprintf(resfile,A,B,C);fflush(resfile);}
#define PRINT_MOD0(A) {fprintf(modfile,A);fflush(modfile);}
#define PRINT_MOD1(A,B) {fprintf(modfile,A,B);fflush(modfile);}


/***************************************************************/
/* structure , to store one single inference-step              */
/***************************************************************/
typedef struct pt_inf_step {
        WORD codeptr;           /* clauselbl to alloc */
        int red_lvl;            /* Reduction-Level    */
        } inf_step;

/* IA_begin */
/***************************************************************/
/* structure, to store one term                                */
/***************************************************************/
typedef struct term_ds
  {
        int            typ;            /* type of term         */
        int             symb_nr;        /* symbol-identifier    */
        struct term_ds  *arg;           /* pointer to arg-list  */
        struct term_ds  *next_term;     /* pointer to next term */
  } Lit_arg;

/***************************************************************/
/* structure, to store one literal                             */
/***************************************************************/
typedef struct literal_ds
  {
        int                or_nr;       /* number of or-node   */
        int                lit_nr;      /* number of literal   */
        int                pred;        /* predicate           */
        int                subs;        /* subsumer            */
        struct term_ds     *arg;        /* pointer to arg-list */
        struct literal_ds  *next_lit;   /* pointer to next lit */
  } Lit;

typedef struct covered_literals
  {
        int                or_nr;             /* number of or-node   */
        int                subtask_nr;        /* number of literal   */
        long               or_list[MAXOR_NODES];
  } Cov_Lit;

typedef struct subsumed_literals
  {
        int                      or_nr;       /* number of or-node   */
        int                      lit_nr;      /* number of literal   */
        struct subsumed_literals *next;
  } S_Lit;
/* IAend */

extern int      pvm;
extern char     lopfilename[50];
extern int      cputimelimit;    /* f. Extension der Res.-Datei f.par. Sim.*/

extern int s_boundfail;     /* True, if boundfail reached         */
extern int create_task;   /* True, if task-generating           */
extern int count_task;    /* True, if task-gen. verbose-mode    */
extern int ornode_counter;/* zaehlt die generierten Tasks       */
extern int pt,ppt;        /* speich.Anz.Task voriger Suchfronten */
extern int t_generate;    /* true, wenn Tasks generiert werden  */
extern int t_proving;     /* true, solange Task-Recomputation   */
extern int t_depth;       /* Tiefe(auch Inf.u.Lok.Inf Tiefe) ei-*/
                          /* ner Task bei Recomputation         */
extern float task_level;  /* (m_tasks - pt)/(1 - ppt/pt)       */
extern int formula;       /* Zur Diff., mit welcher Formel die  */
                          /* autom. Task-Gen. getan werden soll */
extern int env_clbl;      /* Speichervar.f.Task-Gen.m.Forel 2   */
extern int prev_env_clbl; /* Speichervar.f.Task-Gen.m.Forel 2   */
extern int num_of_subgs;  /* zaehlt die Subgs einer Suchfront   */
extern int cnt_indp_or;   /* zaehlt Oder-Knoten mit mehr als    */
                          /* einer unabh. Gruppe                */
extern int cnt_indp_grp;  /* zaehlt echt unabh. Gruppen         */
extern int num_of_tasks;  /* Anzahl der unabh. Aufgaben         */
extern int num_of_vars;   /* Anzahl der Variablen               */
extern int vars[MAXVAR];  /* Identif. der Variablen             */
extern int ia_result[MAXOR_NODES][MAXLITS];
                          /* Ergebnismatrix                     */
extern Lit *first_lit;    /* Zeiger auf erstes Lit. der Liste   */
extern Lit *last_lit;     /* Zeiger auf letztes Lit. der Liste  */
extern int m_tasks;       /* max. number of tasks to generate   */
extern int recomp;        /* true, if recomp_mode to set;       */
extern int pre_recomp;    /* true, wenn Ausgabe f.mod. Setheo   */
extern int recomp_mode;   /* true, if recomputation of an Or-Node */
extern int or_numb;       /* ODER-Nr. der akt. recomp. Task     */
extern int iand_task_num; /* IAnd-Nr. der akt. recomp. Task     */
extern WORD *next_adr;    /* Return-Adresse bei Call            */
extern int now_next_subg; /* Subgoals werden bei der Abarbeitung*/
                          /* der IA-Task ber"ucks. oder nicht   */
extern int iand_check;    /* true, wenn IA-Ueberpruefung        */
extern int subsume_check; /* true, wenn Subsumptions-Uberpruefg.*/
extern int linres;        /* linear resolvents statt Tableaus   */
extern int use_eo;        /* execution ordering                 */
extern int print_subgs;   /* true, wenn offene Subgoals zu schr.*/
extern inf_step *node_ptr; /* pointer to one inference-step     */
extern int pvm;
extern int task_counter;
extern int spsim;
extern inf_step tableau[MAX_TABLEAU_SIZE];  /* store inference-steps*/
extern inf_step *inf_step_area;
extern inf_step *inf_step_area_start;
extern inf_step *inf_step_area_last;

extern environ *saved_bp;  /* iAND */

#define READ_TASK \
  c_pt = prooftask; \
  sscanf(c_pt, "%d ",   &or_numb); \
  while( *(c_pt++) != ' ' ); \
 \
  /* Einlesen einer einzelnen Task auf die Datenstruktur tableau[] */ \
  tab_pos = -1; \
  do { \
  sscanf(c_pt,"%d,%d ",&(tableau[++tab_pos].codeptr),&(tableau[tab_pos].red_lvl)); \
  while( *(c_pt++) != ' ' ); \
     } \
  while ( ! ( *c_pt == '-' && *(c_pt+1) == '1' && *(c_pt+2) == ' ') ); \
  sscanf(c_pt,"%d ", &(tableau[++tab_pos].codeptr)); \
  while( *(c_pt++) != ' ' ); \
 \
  tableau[tab_pos].red_lvl = 0; \
 \
  /* iAND: Einlesen der IAnd-Task, falls vorhanden */ \
  sscanf(c_pt, "%d ", &(iand_task_num)); /* IAnd-Task# einlesen */ \
  while( *(c_pt++) != ' ' ); \
  if( iand_task_num != -2){ /* IAnd-Task vorhanden */ \
          saved_reg_constr = reg_constr; \
           reg_constr = FALSE; \
      if( linres ){ \
          saved_ts_constr  = ts_constr; \
           ts_constr  = FALSE; \
           } \
    do{ \
      /* Subgoalnummern einlesen */ \
      sscanf(c_pt, "%d ", (WORD) &(tableau[++tab_pos].codeptr)); \
      while( *(c_pt++) != ' ' ); \
      tableau[tab_pos].red_lvl = 0; \
    } \
    while (tableau[tab_pos].codeptr != -2); \
  } \
  printf("Task (%d,%d): ", or_numb, iand_task_num); \
  for( i = 0 ; i <= tab_pos ; i++ ) \
       printf("%d,%d ", tableau[i].codeptr, tableau[i].red_lvl); \
  printf("\n"); 


#endif

#endif /* SPS_H */

