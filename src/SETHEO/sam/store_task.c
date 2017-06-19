/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: store_task.c                                 */
/* VERSION:                                           */
/* DATE:                24.6.93                       */
/* AUTHOR: Michael Huber, Dagmar Holzinger, Christian Suttner */
/* NAME OF FILE:                                      */
/* DESCR:                                             */
/*      store the or-nodes of the searchfront         */
/*      store the or and independent-and tasks        */
/*                                                    */ 
/* MOD:                                               */
/******************************************************/



#ifdef STATPART


#include <stdio.h>
#include <signal.h>
#include "stdlibwrap.h"
#include <sys/times.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "codedef.h"
#include "symbtab.h"
#include "disp.h"
#include "defaults.h"
#include "sps.h"
#include "my_unistd.h"

#define  CPUTIME_TERMINATED 5

extern int    ti;
extern float  actime;  /* time for all it-deep. cycles so far */
extern int    fails, bfails;
extern S_Lit* *subsumrel[MAXOR_NODES];
extern int     cputimelimit;  /* CS 1.10.94 */
extern int    skipped_lr_redstep_cnt, skipped_ia_redstep_cnt;

int           ornode_litcnt[MAXOR_NODES];
int           ornode_subtask_cnt[MAXOR_NODES];
int           task_nums[MAXOR_NODES][MAX_OR_SUBTASKS];
int           orlist[MAX_TASK_BOUND][MAX_AFFECTED_ORNODES];
int           task_lits[MAX_TASK_BOUND][MAXLITS];
char          *task_set[MAX_TASK_BOUND];
char          curr_task[500];
int           tsi;
int           task_counter;
int           or_nr;
int cnt_subsumed;    /* IAnd */

int           inf_step_cnt = 0;    /* initialize for 1. call of store_or_task */

extern int   task_execution_ordering();
short int          *execution_ordering[MAX_TASK_BOUND];
short int          init_set[MAXOR_NODES];

/******************************************************************************/
/* store_or_task speichert die allocate-Adressen und Reduktion-Level einer    */
/* Task (bis auf das momentane env) auf Datenstruktur tableau ab.             */
/******************************************************************************/
store_or_task(bp)
environ *bp;
{
 int nr_sgls;
 int i;
 WORD *sg_i;


 if( inf_step_cnt >= MAX_TABLEAU_SIZE ){
    PRINT_LOG0("Generation aborted! Task too big! Increase MAX_TABLEAU_SIZE\n");
     /*sam_error("Task too big! Increase MAX_TABLEAU_SIZE\n", NULL, 2);*/
     create_task = FALSE;
     inf_step_cnt = 0;
     return;
     }
 tableau[inf_step_cnt].codeptr = bp->lcbptr->clauselbl;
 tableau[inf_step_cnt++].red_lvl = -1;  

 nr_sgls = GETNUMBER(bp->lcbptr->nr_sgls);
 if( !nr_sgls ){
      return;
      }

 sg_i = (WORD *)bp + SIZE_IN_WORDS(environ)
                   + GETNUMBER(bp->lcbptr->nr_vars);

for( i=0; i < nr_sgls; i++){
     if( sg_i <= bp->tp){            /* treated subgoal, otherwise untouched */
        switch(GETTAG(*sg_i)) {

            case EXTENSION:
             store_or_task((environ*) GETPTR(*sg_i));
               break;

            case REDUCTION:
	     /* bei red-step zeigt codeptr jetzt auf die alte Klausel !! */
             /*tableau[inf_step_cnt].codeptr = sg_i->env->lcbptr->argv;*/
             tableau[inf_step_cnt].codeptr = ((environ*) GETPTR(*sg_i))->lcbptr->argv;
	     /* red-step soll ja nicht explizit durchgefuehrt werden, sondern 
	        "von Hand" mit entsprechendem Argument ueberprueft werden!*/
             tableau[inf_step_cnt++].red_lvl = 
                                            calc_reduction((environ*) GETPTR(*sg_i),bp);
               break;

            case FACTORIZATION_PESS:
            case FACTORIZATION_OPT:
            case LEMMA:
            case NOT_SOLVED:
printf("AAAAHHHH: DYING due to unexpected option in i_ptree.c ...\n");
               break;
           }
	   }
       else{
           tableau[inf_step_cnt].codeptr = -1;
	   inf_step_cnt = 0;            /* set counter back for next call */
	   return;
           }
   sg_i++;                       
   }
}

 
void cp_or_task()
{
int i;
inf_step *is;

i  = 1;                             /* omit default query label */
is = inf_step_area_start;
while( tableau[i].codeptr != -1){
       is->codeptr = tableau[i].codeptr;
       is->red_lvl = tableau[i++].red_lvl;
       is++;
       }
is->codeptr         = -1;
inf_step_area_start = ++is;
}


/***************************************************************************/
/* Funktion calc_reduction ermittelt das Reduction-Level des letzten Infe- */
/* renzschrittes, falls dieser ein Reduktionsschritt war.                  */
/***************************************************************************/

calc_reduction(bpfrom,bpto)
environ *bpfrom;
environ *bpto;
{
  int red_lvl;

  red_lvl = 0; 
  while( bpfrom != bpto ){
         red_lvl++;
         bpto = bpto->dyn_link;
         }

  return(red_lvl);
}

void print_runtime( buffer )
struct tms buffer;
{
float runtime;

runtime = ((float)(buffer.tms_utime - ti) / (float)CLK_TCK) - actime;
if( runtime < 0.01 ){
    PRINT_LOG0("time=    <0.01 sec");
#ifdef STATPART
    if( recomp )
        PRINT_RES0("   <0.01 sec");
#endif
    }
else{
    PRINT_LOG1("time= %8.2f sec", runtime);
#ifdef STATPART
    if( recomp ){
        PRINT_RES1("%8.2f sec", runtime);
        }
#endif
    }
}


#ifndef HP
spsighand_XCPU(){ spsighand( SIGXCPU); }
#endif

#ifdef HP
spsighand_USR1(){ spsighand(_SIGUSR1); }
#endif
#ifndef HP
spsighand_USR1(){ spsighand(SIGUSR1); }
#endif

spsighand_ALRM(){ spsighand(SIGALRM); }

spsighand(catched_signal)      
int catched_signal;
{
float time_left;
int tfails;
int bound_fails;
char s[15];

/* sigignore(catched_signal); */

times( &buffer );

if( catched_signal == SIGALRM ){ /* check if cputimelimit is already reached */
    if( !cputimelimit )
        sam_error("Catched SIGALRM, but cputimelimit == 0!\n", NULL, 2);
    time_left = (float)cputimelimit - 
	        ((float)(buffer.tms_utime) / (float)CLK_TCK) - overall_time;
    if( time_left > 0.5 ){
        signal (SIGALRM, spsighand_ALRM);
	if( time_left < 1.0 )
            alarm(1);
        else
            alarm((int)time_left);
        return;
        }
    sprintf(s,"%s","CPUTIME-LIMIT");
    if( pvm )
        pvm_task_error(CPUTIME_TERMINATED);
    }
#ifdef HP
else if( catched_signal == _SIGUSR1 )
#endif
#ifndef HP
else if( catched_signal == SIGUSR1 )
#endif

         sprintf(s,"%s","PVM-KILLED");
     else sprintf(s,"%s","UNKNOWN_SIGNAL");

print_runtime( &buffer );
evaltime = ((float)buffer.tms_utime / (float)CLK_TCK) - evaltime;
overall_time = ((float)buffer.tms_utime / (float)CLK_TCK) - overall_time;
bound_fails = depth_fails + inf_fails + local_inf_fails; 
tfails = c_reg_fails + c_ts_fails + c_anl_fails + unif_fails + bound_fails;

PRINT_LOG3(" inferences= %8d  fails= %8d  %s\n", total_inf, tfails, s);
PRINT_LOG2("Total:  time=  %7.2f sec inferences= %8d  ", evaltime,
            global_tot_inf + total_inf);
PRINT_LOG2("fails= %8d  T_all= %6.2f\n\n",tfails + fails, overall_time);

if( recomp /*&& !pvm*/ ){
    fprintf(resfile," %10d %10d %8d  CPUTIME-LIMIT\n", 
                     total_inf, tfails, bound_fails );
    fprintf(resfile,"Total:     %6.2f sec %10d %10d %8d  T_all= %6.2f ",
                     evaltime, global_tot_inf + total_inf, 
		     tfails + fails, 
		     bound_fails + bfails, overall_time);
    if( iand_task_num != -2 && (skipped_lr_redstep_cnt || skipped_ia_redstep_cnt) )
        if( linres )
            fprintf(resfile, "lr: %7d", skipped_lr_redstep_cnt);
        else
	    fprintf(resfile, "ia: %7d", skipped_ia_redstep_cnt);
    fprintf(resfile, "\n");
    }
exit(EXIT_FAILURE);
}


void sps_output()
{
  int cnt_subsumers;   /* IAnd */
  int cnt_subsumed_or; /* IAnd */

  if( create_task){
      PRINT_LOG1("OR-Nodes                           : %8d\n", ornode_counter);

  if( iand_check ){
      PRINT_LOG1("OR-Nodes with Real IAnd-Tasks      : %8d\n", cnt_indp_or);
      PRINT_LOG1("IAnd Tasks                         : %8d\n", num_of_tasks);
      PRINT_LOG1("Real IAnd Tasks                    : %8d\n", cnt_indp_grp);
      if( linres ){
          get_subs_statistic(&cnt_subsumed, &cnt_subsumers, &cnt_subsumed_or);
          if( cnt_subsumed ){
              PRINT_LOG1("Subsuming IAnd Tasks               : %8d\n", 
	                  cnt_subsumers);
              PRINT_LOG1("Subsumed  IAnd-Tasks               : %8d\n", 
	                  cnt_subsumed);
              PRINT_LOG1("Subsumed  Or-Nodes                 : %8d\n",
	                  cnt_subsumed_or);
              PRINT_LOG1("\nCreated Tasks                      : %8d\n", 
	                  num_of_tasks - cnt_subsumed );
              }
          }
      }
  if( !count_task ){
      if( !pvm ) fprintf(taskfile,"\n# Tasklist:\n");
      write_tasks_to_file();   /* Ausgabe der Tasks ! */
      }
   }
}

write_iand_task(or_nr, pt_lit_nr, iandtask_nr, pt_grp_nr)
int or_nr; /* Nummer des momentanen Oder-Knotens */
int *pt_lit_nr; /* Nummer des zuletzt bearbeitenden Literals */
int iandtask_nr; /* Nummer der Task */
int *pt_grp_nr; /* Nummer der IA-Gruppe, die zuletzt erfa"st wurde */
{
 int	i, j; 
 int    task_lit_nr = 0;

 for( i = 0 ; i < MAXLITS && ia_result[or_nr][i+1] != -2 ; i++ )
      if( ia_result[or_nr][i] == 0 || 
          ia_result[or_nr][i] != ia_result[or_nr][i+1] ) 
          break;
 if( ia_result[or_nr][i+1] == -2 || ia_result[or_nr][1] == -2 ){  
     /* all subgoals dependent  or  only 1 subgoal in ornode */
     ++(*pt_grp_nr);
     ++(*pt_lit_nr);
     j = 0;
     while(j < MAXLITS){
        if( ia_result[or_nr][j] == -2) /* alle Literale bearbeitet */
	    break;
        task_lits[task_counter][++task_lit_nr] = j;
	j++;
	}
     task_lits[task_counter][0] = task_lit_nr;  
     return;
     }

 sprintf(&curr_task[tsi], "%d ", iandtask_nr);   /* Nummer der IA-Task */
 tsi += strlen(&curr_task[tsi]);
 if( ia_result[or_nr][*pt_lit_nr] == 0){ /* Literal ist unabh"angig */
     task_lits[task_counter][0] = 1;  
     task_lits[task_counter][1] = *pt_lit_nr;
     sprintf(&curr_task[tsi], "%d ", (*pt_lit_nr));/*Litnummer(beg. bei 0)*/
     tsi += strlen(&curr_task[tsi]);
     }
 else if( ia_result[or_nr][*pt_lit_nr] > (*pt_grp_nr)++ ){ /* Gruppe abh. Lits*/
          i = (*pt_lit_nr);
          while(i < MAXLITS){
             if( ia_result[or_nr][i] == -2) /* alle Literale bearbeitet */
                 break;
             if( ia_result[or_nr][i] == (*pt_grp_nr)){/*Lit. geh"ort z. Gruppe*/
	         task_lits[task_counter][++task_lit_nr] = i;
		 sprintf(&curr_task[tsi], "%d ", i);   
		 tsi += strlen(&curr_task[tsi]);
		 }
             ++i;
             }
             task_lits[task_counter][0] = task_lit_nr;  
          }
 ++(*pt_lit_nr);
}
    
/***************************************************************************/
/* Funktion write_or_task liest den Inhalt der Datenstruktur inf_step_area */
/* auf ein File mit dem Namen <Theorem>.sp aus. Dies geschieht am Ende der */
/* initialen Explorationsphase oder bevor die inf_step_area waehrend der   */
/* initialen Explorationsphase ueberlaeuft.                                */
/***************************************************************************/

write_or_task( is, or_num )
inf_step **is; 
int or_num;
{      /*if((*is) == inf_step_area || ((*is)-1)->codeptr == -1)Anfang OrTask*/
 if( pvm )
     sprintf(&curr_task[tsi], "%d ", task_counter); 
 else
     sprintf(&curr_task[tsi], "%d ", or_num);
 tsi += strlen(&curr_task[tsi]);
 while( (*is) < inf_step_area_start ){
    if( (*is)->codeptr != -1 ){
	 sprintf(&curr_task[tsi], "%d,%d ",(*is)->codeptr, (*is)->red_lvl);
	 tsi += strlen(&curr_task[tsi]);
         (*is)++;
	 }
    else{
	 sprintf(&curr_task[tsi], "%d ", (*is)->codeptr);
	 tsi += strlen(&curr_task[tsi]);
         (*is)++;
	 return(1); /* Ende der Oder-Task */                                    
	}                                     
    }
}


/***************************************************************************/
/* Funktion write_tasks_to_file liest den Inhalt der Datenstruktur         */
/* inf_step_area auf ein File mit dem Namen <Theorem>.sp aus. Dies         */ 
/* geschieht am Ende der initialen Explorationsphase oder bevor die        */ 
/* inf_step_area waehrend der initialen Explorationsphase ueberlaeuft.     */
/***************************************************************************/

write_tasks_to_file()
{
inf_step *first_inf_step, *is;
int 	 lit_nr, iand_grp_nr, iand_task_nr;
int      i,j, k, l, first, flag;
S_Lit    *srel_pt;
clock_t  start, end;

first_inf_step = inf_step_area; /* erster Oder-Knoten */

or_nr        = 1;
task_counter = 0;

while( first_inf_step < inf_step_area_start){ /* inf_step_area_start =  
                                                  Ende der Inf_step-Area + 1 */
   if( iand_check || subsume_check){ /* es gibt IA-Tasks */
       lit_nr = 0;
       iand_task_nr = 0;
       iand_grp_nr = 0;
       ornode_subtask_cnt[or_nr] = 0;
       ornode_litcnt[or_nr] = 0;     
              /*printf("\nia_result[%2d][...]= ", or_nr);fflush(stdout);*/
       while( lit_nr < MAXLITS){ /* IAnd-Tasks ausgeben */
              /*printf("%2d ", ia_result[or_nr][lit_nr]);fflush(stdout);*/
          if( ia_result[or_nr][lit_nr] == -2){/* Literale bereits abgearbeitet*/
	      ornode_litcnt[or_nr] = lit_nr; 
              break;
              }
          else if( (ia_result[or_nr][lit_nr] == -1) || /* Literal redundant */
                  ((ia_result[or_nr][lit_nr] > 0) &&  /* Lit schon erfasst */
                   (ia_result[or_nr][lit_nr] <= iand_grp_nr)) ){
		   if( ia_result[or_nr][lit_nr] == -1 )
		       ornode_subtask_cnt[or_nr]++;
                   ++lit_nr;
                   continue;
                   }
               else{ /* es gibt eine IAnd-Task */
                   is = first_inf_step; /* Anfang des Oder-Knotens */
		   task_counter++;
		   orlist[task_counter][0] = or_nr;
		   orlist[task_counter][1] = 0;
		   ornode_subtask_cnt[or_nr]++;
                   tsi = 0;     
                   write_or_task(&is, or_nr);
                   write_iand_task(or_nr,&lit_nr, ++iand_task_nr, &iand_grp_nr);
                   sprintf(&curr_task[tsi], "%s", "-2");
                   task_nums[or_nr][iand_task_nr] = task_counter;
                   task_nums[or_nr][iand_task_nr+1] = 0;
		   do_curr_task(curr_task);
                   }          
          } /* endwhile */
       if( is == first_inf_step){         /* Oder enth"alt keine IAnd-Task */
           /*printf("Skipping subsumed OR-node\n");*/
	   while( (is++)->codeptr != -1 && is < inf_step_area_start );
	   }
       first_inf_step = is;
       }
   else{
       task_counter++;
       orlist[task_counter][0] = or_nr;
       orlist[task_counter][1] = 0;
       tsi = 0;     
       ornode_subtask_cnt[or_nr] = 1;
       task_nums[or_nr][1] = task_counter;
       task_nums[or_nr][2] = 0;
       write_or_task(&first_inf_step, or_nr);
       sprintf(&curr_task[tsi], "%s", "-2");
       do_curr_task(curr_task);
       }
/*printf("\n");*/
   ++or_nr;
   } /* endwhile */
   or_nr--; /* undo last increment */

  if( iand_check && (! cnt_indp_grp) )
      return;

  if( cnt_indp_grp ){ /* es gibt Real IA-Tasks */
/*
for( i = 1 ; i <= or_nr ; i++ )
     for( j = 0 ; j < MAXLITS ; j++ )
          if( subsumrel[i] != NULL && subsumrel[i][j] != NULL ){
              printf("subsumrel[%2d][%d]= ", i, j);
              for( srel_pt = subsumrel[i][j]; srel_pt != NULL ; 
                   srel_pt = srel_pt->next )
                   printf("(%d, %d) ", srel_pt->or_nr, srel_pt->lit_nr);
              printf("\n");
              }
*/
/*
for( i = 1 ; i <= task_counter ; i++){
     printf("task_lits[%d] (%d elems)= ", i, task_lits[i][0]);
     for( j = 1 ; j <= task_lits[i][0] ; j++ )
          printf(" %d", task_lits[i][j]);
     printf("\n");
     }
*/

      for( i = 1 ; i <= task_counter ; i++){
	 if( subsumrel[orlist[i][0]] != NULL ) /* task i may subsume other tasks */
           for( j = 1 ; j <= task_lits[i][0] ; j++ ){ /* for each literal in task i */
	     if(subsumrel[orlist[i][0]][task_lits[i][j]] != NULL){  /* if current literal subsumes */
	       k = 1;
	       for(srel_pt = subsumrel[orlist[i][0]][task_lits[i][j]] ;   
	                   srel_pt != NULL ; srel_pt = srel_pt->next ){
		   flag = FALSE;
	           for( l = 1 ; task_nums[srel_pt->or_nr][l] != 0 ; l++ )
		        if( task_nums[srel_pt->or_nr][l] == i )
			    flag = TRUE;
                   if( !flag ){     /* add only if not already there */
		       orlist[i][k++] = srel_pt->or_nr;
		       task_nums[srel_pt->or_nr][l++] = i; /* add subsuming literal to ornode */ 
		       task_nums[srel_pt->or_nr][l]   = 0;      
		       }
		   else ornode_subtask_cnt[srel_pt->or_nr]--; 
	           }
	       orlist[i][k] = 0;
	       }
	     }
           }
  /* print # of tasks for OR-nodes with subsumed parts */
  if( !pvm && linres && cnt_subsumed ){ 
      fprintf(taskfile, "\nOR-node info: "); 
      for( i = 1 ; i <= or_nr ; i++ ){
           first = 1;
	   for( j = 1 ; j <= task_counter && first ; j++ ){
                for( k = 1 ; orlist[j][k] != i && orlist[j][k] != 0 ; k++ );
	        if( orlist[j][k] == i && first ){
		    first = 0;
	    	    /* d.h. eine andere Task subsumiert Teil des ornodes */
                    fprintf(taskfile,"(%d,%d) ", i, ornode_subtask_cnt[i]);
		    }
                }
           }
      fprintf(taskfile, "\n\n");
      for( i = 1 ; i <= task_counter ; i++){
	   first = 1;
	   for( j = 1 ; orlist[i][j] != 0 ; j++ ){
		if( first ){
		    first = 0;
		    fprintf(taskfile, "Task: %4d OR-nodes: ", i);
		    }
		fprintf(taskfile, "%d ", orlist[i][j]);
		}
	   if( ! first )
	       fprintf(taskfile, "\n");
	   }
      }

/*
for( i = 1 ; i <= task_counter ; i++){
     for( j = 1 ; task_nums[i][j] != 0 ; j++ ){
          printf("tasknums[%d][%d] = %d\n", i, j, task_nums[i][j]);
	  }
     printf("tasknums[%d][%d] = 0\n", i, j);
     }
printf("tasknums[%d][%d] = 0\n", i, 1);
*/

    if( use_eo ){
	times( &buffer );
        start = (float)buffer.tms_utime;
        PRINT_LOG0("\nComputing ExecutionOrdering ..."); 
        task_execution_ordering();/*(task_nums, execution_ordering, init_set);*/
	times( &buffer );
        end = (float)buffer.tms_utime;
	PRINT_LOG1(" took %5.2f seconds\n", (end - start)/(float)CLK_TCK);

/*
    printf("\nOutput ExecutionOrdering - Array[x][y]!=0 -> x starts y\n");
    for( i=1 ; i<=MAX_OR_SUBTASKS ; i++ ){
         for( j=1 ; j<=MAX_OR_SUBTASKS ; j++ )
              printf(" %d ", execution_ordering[i][j]);
         printf("\n");
         }
*/
    PRINT_LOG0("\nSubtasks to start at once: ");
    for( i = 1 ; i <= task_counter ; i++ )
         if( init_set[i] )  
             PRINT_LOG1(" %d ", i);
    PRINT_LOG0("\n\n");

    for( i = 1 ; i <= task_counter ; i++ ){
         k = 0;
         for( j = 1 ; j <= task_counter ; j++ )
              if( execution_ordering[i][j] ){
                  if( !k ){
	              PRINT_LOG1("Links from %2d to ", i);
		      k = 1;
		      }
                  PRINT_LOG1("%d ", j);
		  }
	 if( k )
	     PRINT_LOG0("\n");
         }
    }
  }
  else use_eo = FALSE;


#ifdef PVM
if( pvm )
    pvm_master();
#endif
}

#else
/* ANSI C doesn't like empty files ... */
int dummy_dummy_store_task () { return 42; }
#endif /* STATPART */
