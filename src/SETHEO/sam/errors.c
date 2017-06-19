/****************************************************************/
/*    S E T H E O                                               */
/*                                                              */
/* FILE:   errors.c                                             */
/* VERSION:                                                     */
/* DATE: 09.05.89                                               */
/* AUTHOR: J. Schumann                                          */
/* NAME OF FILE:                                                */
/* DESCR:                                                       */
/* MOD:                                                         */
/*		29.5.	partheo                                 */
/*		6.6.    this file is not to be used in partheo  */
/* BUGS:                                                        */
/****************************************************************/

#include <stdio.h>
#include <sys/types.h>

#include "my_unistd.h"
#include "constraints.h"
#include "tags.h"
#include "machine.h"

#ifdef SIGNALS
#include <signal.h>
#endif

/**/
#include "errors.h"

/******************************************************/
/* sam_error                                          */
/******************************************************/
/* display string and object, go into monitor         */
/* severe:                                            */
/* 0: non-fatal no object                             */
/* 1: non-fatal object                                */
/* 2: fatal no object                                 */
/* 3: fatal with object                               */
/******************************************************/

void  sam_error (s, obj, severe)
char           *s;
WORD           *obj;
int             severe;
{
  fprintf (stderr,"SETHEO ERROR: %s\n", s);
  
  if (severe & 1) {
    /* display object */
    fprintf(stderr,
	    "Object @ %lx tag ("GETTAGFORMAT") value("GETVALFORMAT")\n", 
	    (long) obj, GETTAG(*obj), GETVAL(*obj));
  }
  fflush(stderr);
  if (severe >= 2) {
    
#ifdef SIGNALS
    kill (getpid (), SIGINT);
#else
    exit (ERROR);
#endif
  } 
}
