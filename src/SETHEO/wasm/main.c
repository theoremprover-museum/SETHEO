/*#########################################################################
# File:			main.c
# SCCS-Info:		01/16/97, @(#)main.c	7.2
# Author:		J.Schumann
# Date:			23.12.87
# Context:		wasm (SETHEO)
# 
# Contents:		main procedure
# Modifications:	
#	(when)		(who)		(what)
# 					optimize
#					undef if numexpr
#	10.10.				PARTHEO instructions
#	16.10.
#	31.10.
#	07.11.  			global vars
#	20.02.89			gen. built ins
#	25.04.89			global load & store
#	10.05.89			strings
#	17.07.89			nalloc
#	27.07.89			include
#	31.07.89			read. & pexec
#	02.08.89			ndealloc
#	24.10.89			filesize
#       11.05.90			HEUMOD   gh
#	30.05.90			force option -d when generating code
#                     			for dynamic heuristic       gh
#       13.07.90			modified option handling    gh
#       27.09.91	Goller		call has got 1 more argument
#	23.10.91	Max Moser	`restyled'
#	28.9.92		jsc
#	8.3.93		jsc		-noopt parameter and verbosity
#	10.3.93		jsc		usage
#	28.2.94		jsc		CLK_TCK
#	6.4.94		jsc		command-line & version
#
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
#########################################################################*/


#include <stdio.h>
#include <sys/types.h>
#include <sys/times.h>
#include <time.h>
#ifndef CLK_TCK
# define CLK_TCK 60
#endif

#include "wasm.h"		/* configuration of wasm		 */
#include "parser.h"
#include "cmdline.h"


/*-------------------------------------------------------------------------
- definition of local and global data
-------------------------------------------------------------------------*/
/***********************************************************************/
/* globals                                                             */
/***********************************************************************/

char            token[TOKENLENGTH];
int             numval;
extern FILE    *yyin;
FILE           *infiles[MAX_INCL]; 	/* files for include mechanism	 */
int             incl_ptr = 0;		/* index for include mechanism	 */


int             curr_pc;

int             errors;

FILE           *fout;

int             optflg = 1;   /* per default set to 1 */

int      opt_incount;
int      opt_outcount;

/***********************************************************************/
/* statics                                                             */
/***********************************************************************/

static	int             verbose = 1;   /* per default set to 1 */

static	char	*logo = "wasm V3.2 Copyright TU Munich (March 25, 1994)";

/*********************************************************************/
/*** TABLE OF PARAMETERS                                           ***/
/*********************************************************************/
#define ARG_OPTIM		0
#define ARG_VERBOSE		1

argentry argtab[] = {
{ ARG_OPTIM,	opt,	"opt",		none,	opt,	"",	0},
{ ARG_VERBOSE,	opt,	"verbose",	none,	opt,	"",	0},
};

int size_argtab = sizeof(argtab)/sizeof(argentry);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ main
+ - test command line arguments
+ - open input and output files
+ - initialise symbol table
+ - if needed initialise optimisation
+ - start processing
+ - start parsing
+ - if needed print out report
+ - quit processing
+ - report errors
+
+ special remarks:
+ - 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int main (argc, argv)

    int             argc;
    char           *argv[];

{

char  fna1[PATHLENGTH],		/* buffer for in file name	 */
      fna2[PATHLENGTH];		/* buffer for output file name	 */
char *av0;		/* intermediate storage for the program name */

struct tms      buffer;
int ti;

int res, iv, neg;
int i;
char *istr;

/*-----------------------------------------------------------------------*/
    /* get the beginning time	*/
    times (&buffer);
    ti = buffer.tms_utime;

    curr_pc = 0;
    currpos = 0;


	/* parsing the parameters */

    if (argc < 2) {
	usage(stderr,argv[0],"file");
	exit (1);
    }

fprintf(stderr,"%s\nCommand line: ",logo);
for (i=0; i<argc;i++){
	fprintf(stderr,"%s ",argv[i]);
	}
fprintf(stderr,"\n");

av0 =argv[0];
argv++;
argc--;

while((res = parsearg(&argv,&argc,&iv,&istr,&neg,"no")) >= 0){
	
	switch(res){
	case ARG_OPTIM:
		optflg = (neg)?0:1;
		break;
	case ARG_VERBOSE:
		verbose = (neg)?0:1;
		break;

		}
	}


switch (res){
	case noarg:
		fprintf(stderr,
		   "%s: no input file\nusage: %s [flags] file\n",
		   av0,av0);
		exit(1);
	case absarg:
		break;
	case argnotfound:
	case argmissing:
	case intexpected:
		fprintf(stderr,"error in argument parsing\n");
		usage(stderr,av0,"");
		exit(1);
	}

/***************************************************************/
/* now open the input and the output file                      */
/***************************************************************/

    sprintf (fna1, "%s.s", argv[0]);
    sprintf (fna2, "%s.hex", argv[0]);

    if ((infiles[0] = fopen (fna1, "r")) == NULL) {
	fprintf (stderr,"%s: Could not open %s\n",av0, fna1);
	exit (2);
    }
    if ((fout = fopen (fna2, "w")) == NULL) {
	fprintf (stderr,"%s: Could not open %s\n",av0, fna2);
	exit (2);
    }

    yyin = infiles[0];

    init_symbtab ();
    if (optflg)
	init_optim ();

    start_proc ();

    yyparse ();

    if (errors){
	fprintf(stderr,"Errors occurred during pass1. Aborting\n");
	exit(1);
	}

    if (optflg) {
 	/* print optimized code */
	optim_out ();
    }

    end_code ();

    if (errors) {
	fprintf (stderr,"%s: %d errors occurred\n",av0, errors);
	fclose (fout);
	exit (1);
    }

#ifdef DEBUGXXX
    if (printsy)
	prt_ref (stdout, printsy);
#endif

    if (optflg) {
	fprintf (stderr,
		"Assembler optimization: %d labels read, %d labels output\n",
		opt_incount, opt_outcount);
    }

    fclose (fout);

    times (&buffer);
    if (verbose){
	fprintf(stderr,
		"%s generated in %5.2f seconds\n",
		fna2, (float)((buffer.tms_utime - ti) * 1000 / 
			(float)CLK_TCK)/1000.0);
	}

    exit (0);

} /*=====================================================================*/

/*#######################################################################*/
