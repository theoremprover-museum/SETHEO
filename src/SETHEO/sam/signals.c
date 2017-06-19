/*********************************************************/
/*    S E T H E O                                        */
/*                                                       */
/* FILE: signals.c                                       */
/* VERSION:		@(#)signals.c	8.1 05 Jan 1996  */
/* DATE: 14.7.88                                         */
/* AUTHOR: J. Schumann                                   */
/* NAME OF FILE:                                         */
/* DESCR:                                                */
/*    Compiles only if signals are defined               */
/* MOD:                                                  */
/*	28.7.88                                          */
/*	1.3.89                                           */
/*	10.5.89                                          */
/*	21.9.94		jsc	sig_alrm, sig_cpulimit   */
/*	27.9.		jsc	sig_alarm                */
/*	12.1.94		jsc	cpu-time < limit+0.5     */
/*	9.3.95		jsc	batchmode                */
/* BUGS:                                                 */
/*********************************************************/

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/times.h>
/**/
#include "signals.h"
#include "itime.h"
#include "deref.h"

#ifndef CLK_TCK
# define CLK_TCK 60
#endif

#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "defaults.h"
#include "unitlemma.h"

#ifdef SIGNALS
#include <signal.h>

#include "my_unistd.h"
#include "sighand.h"

/******************************************************/
/* local MACROS                                       */
/******************************************************/
#define DEBUG(X)


/******************************************************/
/* static variables                                   */
/******************************************************/
extern command  cmds[];



void sighand ()
{
/* invoked by signal SIGINT */
    int             i;
    char            cmd[80];


	/**********************************/
	/* we disable the real-time limit */ 
	/**********************************/
    alarm(0);

    if (batchmode){
	/*************************************************************/
	/* in this case, we do not enter the interactive mode at all */
	/*************************************************************/
	prepare_statistics();
	PRINT_LOG0 ("\n                    ******** INTERRUPT FAILURE ************\n");
	PRINT_LOG0 ("\n");

#	ifdef STATISTICS1
    	disp_statistics(0);
#  	ifdef DEREF_STATISTICS
		print_deref_statistics();
#  	endif
#	endif

	/* close the output file, if necessary */
	if (outputfile != stdout){
		fclose(outputfile);
		}
	
	exit(5);
	}
    else {
    fprintf (stderr, "\n>>>>>>>SETHEO-Interrupt<<<<<<<<<\n");
    if (cputime_limit || realtime_limit){
	fprintf(stderr,"Time-limits disabled\n");
	cputime_limit=realtime_limit=0;
	}
    for (;;) {
	fprintf (stderr, "SETHEO> ");
	gets (cmd);
	for (i = 0; cmds[i].name; i++) {
	    if (!strcmp (cmd, cmds[i].name)) {
		if ((cmds[i].func) ()) {
		    fprintf (stderr, "Continue proof...\n");
		    signal (SIGINT, sighand);
		    return;
		}
		break;
	    }
	}
	if (!cmds[i].name) {
	    fprintf (stderr, "Error\n");
	}
    }}
}


/*******************************************************/
/* sig_alrm                                            */
/*	called when the timer rings                    */
/*******************************************************/
void sig_alrm()
{
float now;
float still_to_go;

if (cputime_limit){
	/*
	** we are heading towards the cpu-time-limit
	*/

	/* get "now" */
    	times (&buffer);
    	now = ((float)(buffer.tms_utime) / (float)CLK_TCK);
	DEBUG(printf("now: %f ov_ %f diff: %f\n",now,overall_time,(float)(cputime_limit)-now - overall_time));
    	still_to_go = (float)cputime_limit - now - overall_time;
    	if (still_to_go > 0.5){
		/*
		** we have more than ONE-HALF cpu-second to go
		*/
		DEBUG(printf("still to go: %d\n",(int)((float)(cputime_limit)- now - overall_time)));
	        signal (SIGALRM, sig_alrm);
		if (still_to_go <1.0){
			alarm(1);
			}
		else {
			alarm((int)still_to_go);
			}
		return;
		}
	}
/*
** we have reached the limit
** i.e., we have to abort
*/

prepare_statistics();

if (realtime_limit){
PRINT_LOG0 ("\n                    ******** REAL TIME FAILURE ************\n");
}
else {
PRINT_LOG0 ("\n                    ******** CPU TIME FAILURE ************\n");
}

PRINT_LOG0 ("\n");

#ifdef STATISTICS1
    disp_statistics(0);
#  ifdef DEREF_STATISTICS
	print_deref_statistics();
#  endif
#endif


#ifdef UNITLEMMA
    if (print_lemmata) {
    /* write all genereted lemmata into outputfile: */

    path_QUERY	                   root;
    POINTER		           ptr;
    unit_lemma_storage_ctrl_block *lemma;
    
    root = path_QueryCreate(unitlemma_index,ALL_MIN,NULL);
    
    while ( (ptr = path_QueryApply(root)) ) {
      lemma = (unit_lemma_storage_ctrl_block *)ptr;
      /* print the lemma */
      if ((lemma->status == lemma_deleted) ||
	  (lemma->status == lemma_deleted_not_used)){
	fprintf(outputfile,"# ");
      }
      print_compiled_fact(outputfile, lemma->ulemma); 
    }
  }
#endif


/* close the output file, if necessary */
if (outputfile != stdout){
	fclose(outputfile);
	}

exit(5);
}


#endif

