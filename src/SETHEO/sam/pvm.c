/* SPTHEO            */
/* pvm.c             */
/* 25./26./27.8.94   */
/* Christian Suttner */

#ifdef STATPART


#include <stdio.h>
#include <sys/times.h>
#include <signal.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "pvm3.h"
#include "sps.h"

#define  SUCCESS            1
#define  FAILURE            2
#define  TASK_ERROR         3
#define  BAD_TASK_ERROR     4
#define  CPUTIME_TERMINATED 5
#define  NO_TASKS_RUNNING   6
#define  CHECK(A)         {if( A < 0 ) PRINT_LOG1("PVM error: A = %d\n", A)}

extern   char      lopfilename[50];

extern   int       ornode_litcnt[MAXOR_NODES];
extern   int       ornode_subtask_cnt[MAXOR_NODES];
extern   int       orlist[MAX_TASK_BOUND][MAX_AFFECTED_ORNODES];
extern   char      *task_set[MAX_TASK_BOUND];
extern   int       task_counter;
extern   int       or_nr;
extern   clock_t   start_time;
extern   int       task_nums[MAXOR_NODES][MAX_OR_SUBTASKS];
extern   short int *execution_ordering[MAX_TASK_BOUND];
extern   short int init_set[MAXOR_NODES];
extern   int       cputimelimit; 

char    *s_task_set[MAX_TASK_BOUND];
int     tids[MAX_TASK_BOUND];        /* array of task id  */
char    tskstring[9], linres_flag[5], time_limit_string[4], time_limit[9];
char    *call_params[7];
float   dist_time[MAX_TASK_BOUND];
int     running_tasks = 0;
int     immediately_sent = 0; /*13;*/
clock_t dist_begin_time;

/*****************************************************************/
/* init_pvm_processing() : enroll in PVM, initialize call_params */
/*****************************************************************/
init_pvm_processing()
{
 int mytid, i;

 sprintf(tskstring, "%s", "-tsk");

 PRINT_LOG0("\nEntering PVM! Start Parallel Processing...\n");
 mytid = pvm_mytid();    /* enroll in pvm */
 if( pvm_parent() == PvmNoParent )                    /* I am the parent */
     tids[0] = mytid;
 else
     sam_error("Error: I'm not the master\n", NULL, 2);
 
 call_params[0] = tskstring;
 i = 2;
 if( cputimelimit ){
     if( cputimelimit > 999 )
         sam_error("Time limit too large; see pvm.c\n", NULL, 2);
     sprintf(time_limit_string, "%s", "-tl");
     sprintf(time_limit, "%d", cputimelimit);
     call_params[i++] = time_limit_string;
     call_params[i++] = time_limit;
     printf("Calling tasks with timelimit: %s\n", time_limit);
     }
 if( linres ){
     sprintf(linres_flag, "%s", "-lr");
     call_params[i++] = linres_flag;
     }
 call_params[i++] = lopfilename;
 call_params[i] = NULL;
}


/********************************************************************/
/* do_curr_task(curr_task) : prepare PVM task or write task to file */
/********************************************************************/
do_curr_task(curr_task)
char curr_task[];
{
 struct tms buffer;

 if( pvm ){
     call_params[1] = curr_task;
     if( task_counter == 1 ){ 
	 init_pvm_processing();
 	 if( use_eo || iand_check )
	     immediately_sent = 0;
         else if( immediately_sent > 0 ){
	          dist_begin_time = times(&buffer);
	          PRINT_LOG0("Spawned tasks:");
		  }
	 }
     if( use_eo || (task_counter > immediately_sent) ){
         task_set[task_counter] = (char *) sam_malloc(200 * sizeof(char));
	 s_task_set[task_counter] = task_set[task_counter];
         sprintf(task_set[task_counter], "%s", curr_task);
         }
     else{
         spawn_task(task_counter);
         }
     }
 else fprintf(taskfile, "%s\n", curr_task);
}

/********************************************************/
/* spawn_task() : spawn one task stored in task_set     */
/********************************************************/
spawn_task(tsk_nr)
int tsk_nr;
{
 int numt;
 struct tms buffer;
 clock_t start, end;

 start = times(&buffer);
/* numt = pvm_spawn("nice +10 msam", call_params, 0, "", 1, &tids[tsk_nr]); */
 numt = pvm_spawn("msam", call_params, 0, "", 1, &tids[tsk_nr]);
 end = times(&buffer);
 dist_time[tsk_nr] = (end - start)/(float)CLK_TCK;
 if( numt < 1 ){
     PRINT_LOG0("\n");
     while( numt < 1 ){
       PRINT_LOG2("Error task: %2d (spawned %d instead", tsk_nr, numt);
       PRINT_LOG1(" of 1) code: %2d ... RETRYING ...\n", tids[tsk_nr]);
       numt = pvm_spawn("msam", call_params, 0, "", 1, &tids[tsk_nr]);
       }
     PRINT_LOG0("Spawned tasks: ");
     }
 task_set[tsk_nr] = NULL;    /* mark task as being started */
 running_tasks++;
 PRINT_LOG1(" %2d", tsk_nr);
 fflush(stdout);
}

/********************************************************/
/* spawn_tasks() : spawn PVM tasks stored in task_set   */
/********************************************************/
int spawn_tasks()
{
 int numt, bufid, i, j, task_result;

 if( immediately_sent == 0 )
     PRINT_LOG0("Spawned tasks:");
 for( i = j = immediately_sent+1 ; i <= task_counter ; i++ ){
    if( task_set[i] != NULL ){
       call_params[1] = task_set[i];
       spawn_task(i);
       if( i % 5 == 0 && i < task_counter ){
           bufid = pvm_probe( -1, -1 );
           if( bufid ){
               PRINT_LOG0("\n");
               if( process_task_msg(i) == SUCCESS )
	           return SUCCESS;
               else
                   if( i < task_counter ) 
		       PRINT_LOG0("Spawned tasks:");
	       j = 1;
               }
           }
       if( (j++ % 22) == 0 && i < task_counter )
           PRINT_LOG0("\nSpawned tasks:");
       }
    }
 PRINT_LOG0("\n");
 return FAILURE;
}


/******************************************************************/
/* pvm_master() : control parallel execution                      */
/******************************************************************/
pvm_master()
{
 int        i, j, k;
 struct tms buffer;
 clock_t    dist_end_time; 

 int  status, rmsgtag, rlen;
 int  tasks_result = FAILURE;
 float min, max, average;
 
 min = 1000;  /* arbitrary, high value */
 max = average = 0;

 if( immediately_sent == 0 ) 
     dist_begin_time = times(&buffer);

 if( ! use_eo ){
     tasks_result  = spawn_tasks();
     }
 else{
     PRINT_LOG0("Spawned tasks: ");
     for( i = 1 ; i <= task_counter ; i++ )
          if( init_set[i] && task_set[i] != NULL ){
              call_params[1] = task_set[i];
	      spawn_task(i);
              }
     PRINT_LOG0("\n");
     }
 dist_end_time = times(&buffer);

 /*
for( i = 1 ; i <= or_nr ; i++ ) {
     printf("task_nums[%2d]= %d", i, task_nums[i][1]);
     for( j = 2 ; task_nums[i][j] != 0 ; j++ )
          printf(", %2d", task_nums[i][j]);
     printf("\n");
     }
     printf("ornode_subtask_cnt[%2d]= %2d\n", i, ornode_subtask_cnt[i]);
for( i = 1 ; i <= task_counter ; i++ ){
     printf("orlist[%2d]= %2d ", i, orlist[i][0]);
     for( j = 1 ; j <= or_nr && orlist[i] != 0 ; j++ )
          printf("%2d ", orlist[i][j]);
     printf("\n");
     }
 */

 if( tasks_result != SUCCESS )
     PRINT_LOG0("Now waiting for a proof ...\n\n");

 if( ! use_eo )
     PRINT_LOG1("Task distribution time (wall-clock): %.2f seconds\n", 
                (dist_end_time - dist_begin_time)/(float)CLK_TCK);
 PRINT_LOG0("Task spawning times    (wall-clock): ");
 for( i = 1 ; i <= task_counter ; i++ ){
      if( dist_time[i] < min )
          min = dist_time[i];
      else if( dist_time[i] > max )
               max = dist_time[i];
      average += dist_time[i];
      }
 average /= task_counter;
 PRINT_LOG3("min= %.2f max= %.2f average= %.2f seconds\n\n", min, max, average);
 if( spsim )
     PRINT_LOG0("Parallel Simulation run, no termination after SUCCESS!!\n")

 while( tasks_result == FAILURE ){
    tasks_result = process_task_msg(task_counter);
    }

 if( spsim ){
     while( running_tasks ){
        tasks_result = process_task_msg(task_counter);
        }
     }
 else if( running_tasks ){
          PRINT_LOG0("Killing tasks ");
          for( i = 1 ; i <= task_counter ; i++ )
               if( tids[i] != 0 ){        /* kill all active tasks */
	           PRINT_LOG1("%d ", i);
                   status = pvm_sendsig( tids[i], SIGUSR1 );
	           CHECK(status);
	           }
	  PRINT_LOG0("\n");
          }
 /*     else if( ! tasks_result == SUCCESS )... */
 PRINT_LOG0("No more tasks running -> master terminates.");
 PRINT_LOG0("\n\n");
 pvm_exit();
 exit();
}

int process_task_msg()
{
 short int msg[2];
 int  killed_flag, success_flag;
 int  task_nr, tsknr, a_ornr;
 int  rtid = 0;
 int  rmsgtag = 0;
 int  rlen = 0;
 int  status;
 int  i, j, k;

 status = pvm_precv( -1, -1, msg, 2, PVM_SHORT, &rtid, &rmsgtag, &rlen );
 running_tasks--;
 CHECK(status);
 task_nr = msg[0];
 tids[task_nr] = 0;
 /*printf("task_nr= %d, subtask_nr= %d\n", task_nr, msg[1]);*/
 if( rmsgtag == SUCCESS ){   
     if( (! iand_check) || msg[1] == 0 ){  /* task represents full ornode */
	 PRINT_LOG1("PROOF MSG RECEIVED (Task %2d) after ", task_nr);
	 print_pvmruntime();
         return SUCCESS;
	 }
     else{                              /* task represents partial ornode */
	  PRINT_LOG1("Task %3d (iand-task) successful ...", task_nr);
	  PRINT_LOG1(" solves part of OR-node %3d", orlist[task_nr][0]);
	  if( ornode_subtask_cnt[orlist[task_nr][0]] > 1 )
	      PRINT_LOG1(" (%d tasks left)",
	                  ornode_subtask_cnt[orlist[task_nr][0]] - 1);
          PRINT_LOG0("\n");
	  success_flag = FALSE;
	  for( i = 0 ; orlist[task_nr][i] != 0 ; i++ ){
	       if( ! --ornode_subtask_cnt[orlist[task_nr][i]] ){
	           if( i )
		     PRINT_LOG1("\t\t\t\t... solves part of OR-node %3d\n",
		                   orlist[task_nr][i]);
		   PRINT_LOG0("\t\t\t\t... concludes proof for OR-node");
		   PRINT_LOG1(" %d !!\n", orlist[task_nr][i]);
	           PRINT_LOG1("PROOF COMPLETED (Tasks %d",
		                 task_nums[orlist[task_nr][i]][1]);
		   for( j = 2 ; task_nums[orlist[task_nr][i]][j] != 0 ; j++ )
		        PRINT_LOG1(", %d", task_nums[orlist[task_nr][i]][j]);
		   PRINT_LOG0(") after ");
		   print_pvmruntime();
		   if( spsim )
		       success_flag = TRUE;
                   else
                       return SUCCESS;
		   }
	       else if( i ){
	               PRINT_LOG1("\t\t\t\t... solves part of OR-node %3d",
		                   orlist[task_nr][i]);
	               PRINT_LOG1(" (%d tasks left)\n",
		                   ornode_subtask_cnt[orlist[task_nr][i]]);
	               }
	       }
	 if( success_flag )
	     return SUCCESS;
	 if( use_eo )
	     start_successors(task_nr);
	 }
     }
 else if( rmsgtag == FAILURE ){ /* failure message */
 	  PRINT_LOG1("Task %3d ", task_nr);
	  if( ! msg[1] )  /* task is an OR-task */
	      PRINT_LOG0("( OR-task ) ")
	  else
	      PRINT_LOG0("(iand-task) ");
	  PRINT_LOG0("unprovable ");
	  for( i = 0 ; orlist[task_nr][i] != 0 ; i++ ){
	       a_ornr = orlist[task_nr][i];
	       for( j = 1 ; j <= task_counter ; j++ )
	  	    for( k = 1 ; orlist[j][k] != 0 ; k++ )/*still required? */
	                 if( orlist[j][k] == a_ornr ){
	  		     if( orlist[j][k+1] == 0 )
	  		         orlist[j][k] = 0;
                             else
			         orlist[j][k] = 0;
                             }
	       /* which other task belongs to ornode a_ornr ? */
	       killed_flag = FALSE;
	       for( j = 1 ; task_nums[a_ornr][j] != 0 ; j++ ) {
	            tsknr = task_nums[a_ornr][j];
		    for( k = 0 ; orlist[tsknr][k] != 0 ; k++ )
		         if( orlist[tsknr][k] != 0 )
		  	     break; /* if break occurs -> still required */
                    if( orlist[tsknr][k] == 0 && task_set[tsknr] != NULL ){ 
		        if( killed_flag )
			    PRINT_LOG0("\n\t\t                     ")
                        else
			    killed_flag = TRUE;
	                PRINT_LOG1(" ... therefore task %d ", j);
                        if( tids[tsknr] != 0 ){ /* task not yet terminated */
                            task_set[tsknr] = NULL;
	                    PRINT_LOG0("will not be started");
                            }
                        else{
	                    status = pvm_sendsig( tids[tsknr], SIGUSR1 ); 
			    CHECK(status);
	                    PRINT_LOG0("is killed");
                            }
			}
		    }
	        }
		PRINT_LOG0("\n");
	 }
      else if( rmsgtag == TASK_ERROR ){
	       /* tids[task_nr] = 0; */
         PRINT_LOG1("SAM Error occurred for task %d ... restarting ", task_nr);
	       if( s_task_set != NULL ){
                   call_params[1] = s_task_set[task_nr];
	           spawn_task(task_nr);
		   PRINT_LOG0("\n");
		   }
	       else
		   PRINT_LOG0("Sorry, restart not possible\n");
               }
           else if( rmsgtag == BAD_TASK_ERROR )
                    PRINT_LOG1("SAM Bad error occurred for task %d\n", task_nr)
                else if( rmsgtag == CPUTIME_TERMINATED ){
			 PRINT_LOG1("Task %3d CPUTIME-LIMIT\n", task_nr);
			 /*killed after ", task_nr); print_pvmruntime();*/
		         }
                     else PRINT_LOG1("Error: received message tag = %d\n", rmsgtag);
if( running_tasks <= 0 )
    return NO_TASKS_RUNNING;
else
    return FAILURE;
}

/******************************************************************/
/* pvm_task_success() : return success message to master          */
/******************************************************************/
pvm_task_success()
{
 int  i;
 int  mytid;        /* my task id        */
 int  ptid, status;
 short int msg[2];

 mytid = pvm_mytid();    /* enroll in pvm */
 ptid  = pvm_parent();
 if( ptid == PvmNoParent )                     /* I am the parent */
     PRINT_LOG0("PVM error: Supposed to be a task process, not parent!\n");
 msg[0] = or_numb;  /* or_numb ist eigentlich task_nr !! */
 if( iand_task_num == -2 )
     msg[1] = 0;
 else
     msg[1] = iand_task_num;
 status = pvm_psend( ptid, SUCCESS, msg, 2, PVM_SHORT ); 
 CHECK(status);
 pvm_exit();
}

/******************************************************************/
/* pvm_task_failure() : return failure message to master          */
/******************************************************************/
pvm_task_failure()
{
 int  i;
 int  mytid;        /* my task id        */
 int  ptid, status;
 short int msg[2];

 msg[0] = or_numb;  /* or_numb ist eigentlich task_nr !! */
 if( iand_task_num == -2 )
     msg[1] = 0;
 else
     msg[1] = iand_task_num;
 mytid = pvm_mytid();    /* enroll in pvm */
 ptid  = pvm_parent();
 if( ptid == PvmNoParent )                     /* I am the parent */
     PRINT_LOG0("PVM error: Supposed to be a task process, not parent!\n");
 status = pvm_psend( ptid, FAILURE, msg, 2, PVM_SHORT ); 
 CHECK(status);
 pvm_exit();
}

/******************************************************************/
/* pvm_task_error() : return error message to master          */
/******************************************************************/
pvm_task_error(error_id)
int error_id;
{
 int  i;
 int  mytid;        /* my task id        */
 int  ptid, status;
 short int msg[2];

 msg[0] = or_numb;  /* or_numb ist eigentlich task_nr !! */
 msg[1] = iand_task_num;
 mytid = pvm_mytid();    /* enroll in pvm */
 ptid  = pvm_parent();
 if( ptid == PvmNoParent ) /* I am the parent */
     PRINT_LOG0("PVM error: Supposed to be a task process, not parent!\n");
 status = pvm_psend( ptid, error_id, msg, 2, PVM_SHORT );
 CHECK(status);
 pvm_exit();
}

/******************************************************************/
/* start_successors() : */
/******************************************************************/
start_successors(task_nr)
int task_nr;
{
 int  i, successor;

 if( execution_ordering[task_nr] != NULL ){
     PRINT_LOG1("Spawning succesors of task %2d : ", task_nr);
     for( i = 1 ; i <= task_counter ; i++ ){
          if( execution_ordering[task_nr][i] ){
	      successor = i;
              if( successor && task_set[successor] != NULL ){
                  call_params[1] = task_set[successor];
                  spawn_task(successor);
                  }
              }
          }
    PRINT_LOG0("\n");
    }
}

int print_pvmruntime()
{
 clock_t end_time;

 end_time = times(&buffer);
 if( end_time == -1 )
     PRINT_LOG0("Error in time measurement!\n");
 PRINT_LOG1("%4.1f seconds wall-clock time\n", (end_time - start_time)/(float)CLK_TCK);
}

/* int print_sender_hostname()
{
 int    i;
 int    status, nhost, narch, rtid, dtid;
 struct pvmhostinfo *hostp;

 status = pvm_config( &nhost, &narch, &hostp );
 CHECK(status);

 dtid = pvm_tidtohost( rtid );
 for( i = 0, dtid = pvm_tidtohost( rtid ) ;
      (*hostp).hi_tid != dtid && i < task_counter ; hostp++, i++ );

 if( i <= task_counter )
     PRINT_LOG1(" from %s!\n", (*hostp).hi_name);
} */

#else
/* ANSI C doesn't like empty files ... */
int dummy_dummy_pvm () { return 42; }
#endif /* STATPART */
