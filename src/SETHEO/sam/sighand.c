/******************************************************
 *    S E T H E O                                     *
 *                                                    *
 * FILE: sighand.c
 * VERSION:
 * DATE: 23.12.87
 * AUTHOR: J. Schumann
 * NAME OF FILE:
 * DESCR:
 *	command interface for signal handler
 * MOD:
 *	10.5.89 ch == NULL changed
 * BUGS:
 ******************************************************/


#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "defaults.h"
/**/
#include "debug.h"
#include "itime.h"
#include "stdlibwrap.h"

#ifdef STATPART
#include <signal.h>
#include "sps.h"
#endif

#ifdef SIGNALS
#include "sighand.h"

static int      disp_stack (), disp_tree (), cont (), help (), reproof ();
static int      disp_stat(), disp_heap (), disp_trail (), quit ();
static int      shell (), disp_ch (), disp_curr (), disp_link ();

#ifdef PVM
extern int  tids[MAX_TASK_BOUND];
#endif


/******************************************************
 * commands of setheo debugger
 ******************************************************/
command         cmds[] = {
    {"reg", dispreg, "Display Registers"},
    {"r", dispreg, "Display Registers"},
    {"stat", disp_stat, "Display Statistics"},
    {"!", shell, "Shell escape"},
    {"stack", disp_stack, "Display stack"},
    {"tree", disp_tree, "Display proof-tree"},
    {"cont", cont, "Continue"},
    {"help", help, "Help"},
    {"?", help, "Help"},
    {"re", reproof, "Reproof"},
    {"trail", disp_trail, "Display trail"},
    {"heap", disp_heap, "Display Heap"},
    {"choice", disp_curr, "Display current choices"},
    {"todo", disp_ch, "Display what is to be done"},
    {"links", disp_link, "Display Links"},
    {"quit", quit, "Quit Setheo"},

{NULL, NULL, NULL}};

/******************************************************
 * command routines of setheo debugger
 * return <>0 if continue proof
 ******************************************************/

/******************************************************
 * disp_stat 
 ******************************************************/
static int      disp_stat ()
{
    disp_statistics(1);
    return 0;
}

/******************************************************
 * disp_stack
 ******************************************************/
static int      disp_stack ()
{
    WORD           *i;

    printf ("STACK @ %lx HEAP @ %lx CODE @ %lx\n", (unsigned long)stack, (unsigned long)heap, (unsigned long)code);

    for (i = stack; i < sp; i++) {
	printf ("STACK: %lx:", (unsigned long)i);
	disp_word (i);
	printf ("\n");
    }
    return 0;
}

/******************************************************
 * disp_heap
 ******************************************************/
static int      disp_heap ()
{
    WORD           *i;

    printf ("STACK @ %lx HEAP @ %lx CODE @ %lx\n", (unsigned long)stack, (unsigned long)heap, (unsigned long)code);

    for (i = heap; i < hp; i++) {
	printf ("HEAP: %lx :", (unsigned long)i);
	disp_word (i);
	printf ("\n");
    }
    return 0;
}

/******************************************************
 * disp_tree
 ******************************************************/
static int      disp_tree ()
{
#ifdef REPROOF_MODE
    p_tree         *j;

    printf ("STACK @ %x HEAP @ %x CODE @ %x\n", stack, heap, code);
    for (j = pt_area; j < pt_ptr; j++) {
	printf ("PROOF_TREE: %d : ", j);
	prt_clause (j->codeptr);
	printf (" Level %d\n", j->red_lvl);
    }
#endif
    return 0;
}


/******************************************************
 * disp_trail
 ******************************************************/
static int      disp_trail ()
{
    s_trail        *j;

    printf ("STACK @ %lx HEAP @ %lx CODE @ %lx\n", 
	    (unsigned long)stack, 
	    (unsigned long)heap, 
	    (unsigned long)code);
    for (j = trail; j < tr; j++) {
#ifdef WORD_64
	printf ("TRAIL: %lx : %lx %lx old value: %lx %lx\n", 
		(unsigned long)j, 
		j->ptr->l1, j->ptr->l2,
		j->oval.l1, j->oval.l2);
#else
	printf ("TRAIL: %lx : %lx old value: %lx\n", 
		(unsigned long)j, 
		(unsigned long)j->ptr, 
		j->oval);
#endif
    }
    return 0;
}

/******************************************************
 * cont
 ******************************************************/
static int      cont ()
{
    return 1;
}

/******************************************************
 * help
 ******************************************************/
static int      help ()
{
    int             i;

    for (i = 0; cmds[i].name; i++) {
	fprintf (stderr, "%s \t\t%s\n", cmds[i].name, cmds[i].descr);
    }
    return 0;
}

/******************************************************
 * reproof
 ******************************************************/
static int      reproof ()
{
#ifdef REPROOF_MODE
    int             pid;

    if (pid = fork ()) {
	/* father has nothing to do */
	wait (NULL);
    }
    else {
	maxinf = DEF_INFCOUNT;
	depth = DEF_DEPTH;
	pmode = 1;
	pt_max = pt_ptr;
	init_reg ();
	printf ("******END OF REPROOF****** (%d)\n",
		instr_cycle ());
	exit (0);
    }
#endif
    return 0;
}


/******************************************************
 * quit
 ******************************************************/
static int      quit ()
{
#ifdef PVM
  int i;
  if( pvm ){
    fprintf(stderr, "Killing all running tasks:");
    for( i = 1 ; i <= task_counter ; i++ )
      if( tids[i] != 0 ){
	pvm_sendsig( tids[i], SIGUSR1 );
	fprintf(stderr, " %3d", i);
      }
    pvm_exit();
    fprintf(stderr, "\n");
  }
#endif
  fprintf (stderr, "Quit...\n");
  exit (0);
}

/******************************************************
 * shell
 ******************************************************/
static int      shell ()
{
    system ("/bin/sh");
    return 0;
}

/******************************************************
 * disp_curr
 ******************************************************/
static int      disp_curr ()
{
 /*display current choices */
    disp_chp (chp);
    return 0;
}

/******************************************************
 * disp_ch
 ******************************************************/
static int      disp_ch ()
{
 /*display choices */
    choice_pt      *ch;

    for (ch = chp; ch >= (choice_pt *) stack; ch = ch->ch_link) {
	disp_chp (ch);
    }
    return 0;
}

/******************************************************
 * disp_link
 ******************************************************/
static int      disp_link ()
{
    dispchoice (chp, bp);
    return 0;
}
#endif
