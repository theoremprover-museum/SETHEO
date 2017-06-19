/******************************************************
 * FILE: i_precall.c
 * DATE: 6.5.93, 13.1.94
 * AUTHOR: Dagmar Holzinger, Michael Huber, Christian Suttner
 ******************************************************/






#include <stdio.h>
#include "tags.h"
#include "machine.h"
#include "sps.h"

#define INSTR_LENGTH		1

int ff_check_mode; 
int ff_check; 
int ff_bound; 
int ff_saved_maxinf;
choice_pt *ff_saved_chp;
int ff_bound_cnt;
int task_bnd_reached = FALSE;
int saved_reg_constr, saved_ts_constr;

instr_result i_precall()
{
#ifdef STATPART
  FILE  *outfile;
  int   i, j, lit_nr, bitstrpos; /* IAnd */
  long  var_in_node[MAXVAR][MAXBITSTR]; /*speichert die vorkommenden Variablen*/
/* recomp_mode = true : im momentanen it.-deep. Zyklus in der recomp. phase */
if( recomp_mode ){
    if( node_ptr != inf_step_area && node_ptr->codeptr == -1){
        /* Ende der Task-Wiederberechnung erreicht */
        recomp_mode = FALSE;
 printf("END OF RECOMPUTATION REACHED !!\n"); 
        /* IAbegin */
	if( iand_task_num != -2 ){ /* IA-Task vorhanden */
            node_ptr++; /* Endemarkierung "uberspringen */
	    reg_constr = saved_reg_constr;
	    if( linres ){
	        ts_constr  = saved_ts_constr;
		}
	    }
        /* IAend */
        t_proving--; 
        PRINT_RES1("\nDepth: %2d",t_depth); 
        }
 /* printf("RECOMP_STEP ");fflush(stdout); */
    }

  if (s_boundfail)
  {
    s_boundfail = FALSE;
    ornode_counter++;
    if( ff_check ){     /* ff_check nur bei Generierung mit -ir */
        if( !ff_check_mode ){              /* start finite failure check */
            ff_check_mode = TRUE;
            ff_saved_maxinf = maxinf;
            ff_saved_chp = chp;
	    store_or_task((environ *)stack);
	    calc_gen_bound(bp);
            trail_var(&ff_saved_chp);
            maxinf += 9;
            printf("[");		 
            ff_bound_cnt = 0;
            pc += INSTR_LENGTH;
            return success;
            }
        else{                         /* boundfail during ff-check */
             ff_bound_cnt++;
             if( ff_bound_cnt >= ff_bound ){   /* supposedly not a ff */
	         cp_or_task();
                 printf("]");		 
	         ff_check_mode = FALSE;
	         maxinf = ff_saved_maxinf;
	         ff_saved_maxinf = 0;
	         chp = ff_saved_chp;
	         return failure;              
	         }
             }
        }
    else{
      if( create_task ){ 
          if( ornode_counter > MAXOR_NODES )
              sam_error("Zu viele ODER-Knoten! MAXOR_NODES erhoehen!\n",NULL,2);
          if( !count_task){   /* if count_task: nur zaehlen, nicht generieren */
  	      store_or_task((environ *)stack);
	      cp_or_task();
	      /*disp_proof_tree(ftree,(environ *)stack,0);*/
	      }
          calc_gen_bound(bp);
    
           if( print_subgs ){
               print_or_node(subgsfile, bp);
               }
          /* IAbegin */
           if( iand_check || subsume_check) {
               for (i = 0; i < MAXVAR; i++)  /* Initialisierung der Variablen */
                 for (j = 0; j < MAXBITSTR; j++)
                   var_in_node[i][j] = 0;
               lit_nr = 0;
               bitstrpos = 0;
               /* Abspeichern der Variablen f"ur jedes Literal */
               store_variables(bp, &bitstrpos, &lit_nr, var_in_node);
               indp_and(var_in_node, bitstrpos, lit_nr);  /* IA-"Uberpr"ufung */
               if( subsume_check )
                   subs_check(bp);
               }
          /* IAend */
      }
/*printf("F ");*/
/*printf(") ");fflush(stdout);*/
/*printf("FAIL in precall: s_boundfail=TRUE\n");*/
    return failure;
    } /*end  of "no ff_check" */
  }
  else 
  {
/*printf("- ");*/
    pc += INSTR_LENGTH;
/*XX printf(") ");fflush(stdout); XX*/
    return success;
  }
#endif /* STATPART */
}

#ifdef STATPART

calc_gen_bound(bp)
environ *bp;
{
int diff;
 if( !task_bnd_reached ){
     env_clbl = (bp->dyn_link)->lcbptr->clauselbl;

     /* Wenn der dyn. Vorgaenger ein anderer als der Vorhergehende ist */
     if( prev_env_clbl != env_clbl )
         ppt++;
     prev_env_clbl = env_clbl;
     /*
     printf("pt= %d ppt= %d m_tasks= %d ornode_counter= %d\n", 
             pt, ppt, m_tasks, ornode_counter);
     */

     if( ( ornode_counter >= m_tasks - pt + ppt ) && maxinf > 1 ){
         maxinf--;
         task_bnd_reached = TRUE;
	 printf("! ");
         }
     }
}

#endif /* STATPART */
