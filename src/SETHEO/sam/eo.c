#ifdef STATPART


#include <stdio.h>
#include "stdlibwrap.h"
#include "tags.h"
#include "machine.h"
#include "sps.h"

/* task and subtask numbers start by 1 */

/* for a description and a specification of the algorithm,
   see phd thesis c.suttner, around p.76
   for additional documentation, see execordering.c, the one-time 
   master version of this file */

/* tse: (input ) tasksetexpression
 * eo:  (output ) execution ordering: array m x m: eo[1][3] = subtask 1 starts 3
 * start: (output ) subtasks to start at once (children of the virtual root of the 'tree' )
 */

extern int task_nums[MAXOR_NODES][MAX_OR_SUBTASKS]; 
extern short int *execution_ordering[MAX_TASK_BOUND];
extern short int init_set[MAXOR_NODES];

int task_execution_ordering()
/*
short int tse[MAXOR_NODES][MAX_OR_SUBTASKS];
short int *eo[MAX_TASK_BOUND];
short int start[MAXOR_NODES];
*/
{
   int i,j,x;
   int maxtask = 0;
   int maxsubtask = 0;
   int maxlength = 0;  /* largest task (# of subtasks ) =^= number of iterations */
   int niveau_ifreq = 0;
   int niveau_len = 0;
   int flag = 0;
   int levelcnt = 0;
   
   int g;             /* g: the factor to extract from the degenerate subtree */
   /* k = input tse = set of all tasks */
   /* f = working tse = active taskset */

   /* permutted index stuff */
   int task_l, task_r; /* inclusive borders of the set of tasks in consideration, use with permut */
   int set[MAX_TASK_BOUND];       /* 1: start of new field of the permutted task array, 0: otherwise */
   int permut[MAX_TASK_BOUND];    /* permut[x] = y : x is index from task set, y is the index into tasks */

   /* "tree" representation: access via permut (active[i], olength[i], tse[i] ... ) refers to single task */
   int active[MAX_TASK_BOUND];    /* 0: removed tasks, 1: other tasks */
   int olength[MAX_TASK_BOUND];   /* length of product = number of AND nodes ( == subtasks ) originally in a task */
   int pred[MAX_TASK_BOUND];     /* predecessor of the remaining AND nodes of any given task */
   short int *tasks[MAXOR_NODES]; /* [MAX_TASK_BOUND]; expanded tse */   

   /* non-task specific stuff (other permutation indices ) */
   int freq_k[MAX_TASK_BOUND]; /* subtask frequencies index (global ) */
   int freq_f[MAX_TASK_BOUND]; /* subtask frequencies index (taskset ) */
   int lpermut[MAX_TASK_BOUND];   /* task ordering by olength (global, not changing ) */

   int subtasktmp[MAX_TASK_BOUND];   
   int tasktmp[MAX_TASK_BOUND];  

   
/* 1      convert data and generate global information (for k ) */
   maxsubtask = 0;
   maxtask = 0;
   for( i = 1 ; task_nums[i][1] ; i++ )
   {
      for( j = 1 ; task_nums[i][j] ; j++ )
         if (task_nums[i][j] > maxsubtask ) maxsubtask = task_nums[i][j];
      maxtask = i;
   }

   /* 1.1    initialization */
   for( i = 0 ; i <= maxtask+1 ; i++ )
      set[i] = permut[i] = active[i] = olength[i] = pred[i] = lpermut[i] = tasktmp[i] = 0;
   for( j = 0 ; j <= maxsubtask+1 ; j++ ) 
      freq_k[j] = freq_f[j] = subtasktmp[j] = 0;
   for( i = 0 ; i <= maxtask+1 ; i++ ){
	tasks[i] =(short int *) sam_malloc ((maxsubtask+2) * sizeof (short int));
        for( j = 0 ; j <= maxsubtask+1 ; j++ )
             tasks[i][j] = 0;
	}
   for( i = 0 ; i <= maxsubtask+1 ; i++ ){
	execution_ordering[i] = (short int *) sam_malloc ((maxsubtask+2) * sizeof (short int));
        for( j = 0 ; j <= maxsubtask+1 ; j++ ){
             execution_ordering[i][j] = 0;
	     }
        }
   for( j = 0 ; j <= maxsubtask+1 ; j++ ) 
        init_set[j] = 0;

   /* 1.2    unpack tse, global frequencies and task length */
   /*        tse index and task index are always identical */
   maxlength = 0;
   for( i = 1 ; task_nums[i][1] ; i++ ) {
      for( j = 1 ; task_nums[i][j] ; j++ ) {
         if( tasks[i] == NULL || task_nums[i][j] > maxsubtask )
	     PRINT_LOG1("ERROR in execution_ordering() 1.2: tasks[%d]\n", i);
         tasks[i][task_nums[i][j]] = 1;    /* expand tse */
         subtasktmp[task_nums[i][j]] += 1; /* count frequencies */
         olength[i] += 1;             /* count length of task */
         }
      if (olength[i] > maxlength ) maxlength = olength[i];
      }
   sortindex(subtasktmp,freq_k,maxsubtask ); /* sort subtasks by global frequency (never changed ) */
   sortindex(olength,lpermut,maxtask );      /* sort tasks by length of task (changes ) */


/* 2      LOOP: set up level for level processing  */

   /* 2.0.1  initialize permut - initially identity, all tasks active */
   for( i = 1 ; i <= maxtask ; i++ ) {
      permut[i] = i; active[i] = 1;
      }
   /* 2.0.2  set left and right (inclusive ) border, set 1st taskset */
   set[1] = 1;                  /* 1st taskset of interest: 1..maxtask */
   task_l = 1; task_r = maxtask;

   /* 2.1    OUTER LOOP BODY */
   /* tasksets from left to right, loop is executed once for each taskset.
      after last taskset: next level, start with first taskset again.
      each level reduces all active tasks by at least one selected subtask g.
      g is predecessor for the representants of all subtasks st in the next level; 
      st in tasks t containing g; t in the current taskset;
      g is removed from all tasks t
    */

   for( levelcnt = 1 ; levelcnt <= maxlength; ) /* at most maxlength levels see exit 2.6! */
   {
      /* 2.1.1  skip taskset if no active task */
      flag = 0;
      for( i = task_l ; i <= task_r ; i++ )
         if( active[permut[i]] ) flag = 1;
      if (flag ) 
      {
	
         /* 2.2    get freq_set(f ), freq_f is sorted index of subtasks */
         for( j = 1 ; j <= maxsubtask+1 ; j++ ) subtasktmp[j] = 0; /* guarantee 0 at end of array */
         for( i = task_l ; i <= task_r ; i++ )
            if( active[permut[i]] ) 
               for( j = 1 ; j <= maxsubtask ; j++ ){
                    if( tasks[i] == NULL )
	                PRINT_LOG1("ERROR in execution_ordering() 1.2: tasks[%d]\n", permut[i]);
                    if( tasks[permut[i]][j] )
                        subtasktmp[j] += 1;
	            }
         sortindex(subtasktmp,freq_f,maxsubtask );
         /*        niveau_ifreq = first subtask in freq_f after freq_set(f ) */
         for( x = 1 ; x <= maxsubtask ; x++ )
            if( subtasktmp[freq_f[1]] != subtasktmp[freq_f[x]] ) break;
         niveau_ifreq = x;          /* index; x = 0 is not possible here */

         /* 2.3    selection of g */
         /* 2.3.1  get subtasks of smallest tasks of k containing subtasks in freq_set(f )  using 
                   original task length */
         for( j = 1 ; j <= maxsubtask+1 ; j++ ) subtasktmp[j] = 0;
         for( i = maxtask ; i ; i-- )
            if (niveau_len = olength[lpermut[i]] ) break; /* niveau_len = min_length(selected tasks ), != 0 */
         flag = 0;
         for( i = maxtask ; i ; i-- ) {
            if( niveau_len != olength[lpermut[i]] ){
	    /* start of next niveau(plateau ) of task length reached */
                if (flag ) break;                /* all potential g's found */
                niveau_len = olength[lpermut[i]];  
	        /* if no g's found, next level of longer tasks */
                }
            for( j = 1 ; j < niveau_ifreq ; j++ ){ 
	    /* mark subtasks in freq_set(f ) for subtasks in task lpermut[i] */
                 if( tasks[lpermut[i]] == NULL || freq_f[j] > maxsubtask )
	             PRINT_LOG1("ERROR in execution_ordering() 2.5.1: tasks[%d]\n",i);
                 if( tasks[lpermut[i]][freq_f[j]] )
                     flag = subtasktmp[freq_f[j]] = 1; /* subtasktmp is subset of freq_set(f ) */
	        }
         }
         /* 2.3.2  get globally most frequent subtasks g from subtasktmp */
         /*        choose the arbitrarily first one */
         for( j = 1 ; j <= maxsubtask ; j++ )
              if( subtasktmp[freq_k[j]] ) break;

         g = freq_k[j];

         if( !g )
             PRINT_LOG0("ERROR in execution_ordering(): subtask lost in memory!\n");

         /* 2.4    new index for taskset: [task contains g | task does not contain g] */
         /*        any task here *is* active! */
         x = task_l;
         for( i = task_l ; i <= task_r ; i++ ){     /* h */
              if( tasks[permut[i]] == NULL || g > maxsubtask )
	          PRINT_LOG1("ERROR in execution_ordering() 2.4: tasks[%d]\n",permut[i]);
              if( tasks[permut[i]][g] )
                  tasktmp[x++] = permut[i];
	      }
         flag = x - 1;
         set[x] = 1; 
         for( i = task_l ; i <= task_r ; i++ ){     /* r */
              if( tasks[permut[i]] == NULL || g > maxsubtask )
	          PRINT_LOG1("ERROR in execution_ordering() 2.4: tasks[%d]\n",permut[i]);
              if( !tasks[permut[i]][g] )
                  tasktmp[x++] = permut[i];
	      }
         for( i = task_l ; i <= task_r ; i++ )    /* update permutation array */
            permut[i] = tasktmp[i];
         task_r = flag;               /* reduce taskset to tasks containing g */

         /* 2.5    INNER LOOP: process g */
         flag = 1;
         for( i = task_l ; i <= task_r ; i++ )
         {        
            /* 2.5.1  remove g from task, set link in eo and mark g as predecessor for the remaining tasks */
           if( tasks[permut[i]] == NULL || g > maxsubtask )
	       PRINT_LOG1("ERROR in execution_ordering() 2.5.1: tasks[%d]\n",i);
            tasks[permut[i]][g] = 0;
 	    if( olength[permut[i]] == levelcnt ) flag = 0;
	    if( execution_ordering[pred[permut[i]]] == NULL || g > maxsubtask+1 )
	        PRINT_LOG1("ERROR in execution_ordering() 2.5.1: execution_ordering[%d]\n", pred[permut[i]]);
            execution_ordering[pred[permut[i]]][g] = 1;
            pred[permut[i]] = g;
         }
         /* 2.5.2  inactivate set of interest if there is a short cut using the current path */
         if( !flag ) 
	     for( i = task_l ; i <= task_r ; i++ ) 
	          active[permut[i]] = 0;
      }

      /* 2.6    END OF OUTER LOOP: select next taskset; if no more tasksets, start new level */
      task_l = task_r + 1;
      /*
      printf("levelcnt = %d maxlength= %d ",levelcnt, maxlength);
      printf("task_l= %d, maxtask= %d\n", task_l, maxtask);
      */
      if( task_l > maxtask ) {
         /* 2.6.1 set children of eo-DAG root (subtasks without predecessors) */
         if( levelcnt == 1 )
             for( i = 1 ; i <= maxtask ; i++ )
                  init_set[pred[i]] = 1;     /* mark all these subtasks */
         /* 2.6.2 still active tasks ? new level : done */
         for( i = 1 ; i <= maxtask ; i++ )
            if( active[i] ) {
               task_l = 1;
               levelcnt++;
               break;
               }
         if( task_l != 1 )  /* all tasks have been processed */
             break;
         }
      /* 2.6.3  determine last task of taskset */
      set[maxtask+1] = 1; 
      for( task_r = task_l ; task_r <= maxtask ; task_r++ ) 
         if( set[task_r+1] == 1 )
            break;
      }


   /* 3      postprocessing: remove links to subtasks that are within the set "start" */
   for( i = 1 ; i <= maxsubtask ; i++ )
      if( init_set[i] )
         for( j = 1 ; j <= maxsubtask ; j++ ){
              if( execution_ordering[j] == NULL || i > maxsubtask ) 
	          PRINT_LOG1("ERROR in execution_ordering() 3: execution_ordering[%d]\n", j);
              if( execution_ordering[j][i] ) 
	          execution_ordering[j][i] = 0;         
              }

   return;

}


/* create a sorted index highest to lowest from input */
int sortindex( input, output, maxindex )
int *input, *output, maxindex;
{
   int i,j,k,max;

   for( i = 1 ; i <= maxindex ; i++ )
      output[i] = 0;
   max = k = 0;
   for( i = 1 ; i <= maxindex ; i++ )
      if (input[i] > max ) max = input[i];
   for( j = max ; j ; j-- )
      for( i = 1 ; i <= maxindex ; i++ )
         if( input[i] == j ) output[++k] = i;
   return;
}

#else
/* ANSI C doesn't like empty files ... */
int dummy_dummy_eo () { return 42; }
#endif
