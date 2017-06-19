/*****************************************************************************
;;; MODULE NAME : signals.c
;;;
;;; PARENT      : 
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : 
;;;
;;; AUTHOR      : Rudolf Zeilhofer
;;;
;;; DATE        : 06.03.98
;;;
;;; DESCRIPTION : handles SIGALRM Signal called by alarm/signal
;;;
;;; REMARKS     :
******************************************************************************/

#include <stdio.h>
#include <time.h>
#include <sys/types.h>
#include <sys/times.h>
#include <signal.h>
#include "types.h"
#include "extern.h"

#ifndef CLK_TCK
# define CLK_TCK 60
#endif

/*******************************************************/
/* sig_alrm()                                         */
/* Description: called when the timer rings            */
/*******************************************************/
void sig_alrm()
{
  float now;
  float still_to_go;
  struct tms timebuffer;
  int realtime_flag = TRUE;
  long now_time_in_sec;
  
  if (inwasm_flags.cputimelimit) {
    /* we are heading towards the cpu-time-limit */
    
    times(&timebuffer);
    /* get "now" */
    now = ((float)(timebuffer.tms_utime) / (float)CLK_TCK);
    time(&now_time_in_sec);

    still_to_go = (float)cputimelimit - now - overall_time;
    /* printf("STG: %f\n", still_to_go); */
    /* printf("REAL: %d\n", (realtimelimit - (now_time_in_sec - start_time_in_sec))); */
    if (still_to_go > 0.5) {
      /* we have more than ONE-HALF cpu-second to go */
      signal (SIGALRM, sig_alrm);

      /* check if realtimelimit will be reached before cputimelimit */
      if (inwasm_flags.realtimelimit &&
	  ((float)(realtimelimit - (now_time_in_sec - start_time_in_sec) < still_to_go))) {
	alarm((int)(realtimelimit - (now_time_in_sec - start_time_in_sec)));
	inwasm_flags.cputimelimit = 0;
	return;
      }

      if (still_to_go < 1.0){
	alarm(1);
      } else {
	alarm((int)still_to_go);
      }
      return;
    } else {
      /* printf("start_time: %f / now: %f\n", overall_time, now); */
      if ((int)(overall_time - now) < realtimelimit) 
	realtime_flag = FALSE;
    }
  }
  if (inwasm_flags.realtimelimit && realtime_flag) {
    printf("\n");
    fprintf(stderr, "  Error: Real-Time failure. (Real-Time limit %d seconds)\n",
	    realtimelimit);
    exit(91);
  } else if (inwasm_flags.cputimelimit) {
    printf("\n");
    fprintf(stderr, "  Error: CPU-Time failure. (CPU-Time limit %d seconds)\n",
	    cputimelimit);
    exit(92);
  }
  return; 
}

