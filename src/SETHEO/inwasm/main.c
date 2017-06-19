/******************************************************************************
;;; MODULE NAME : main.c
;;;
;;; PARENT      :
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)main.c	15.2 01 Apr 1996
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 15.5.91
;;;
;;; DESCRIPTION : main program
;;;
;;; REMARKS     :
;;;		22.2.93		jsc	new version
;;;		3.3.93		jsc	LOP output (fast hack)
;;;					-nocode generates file.lopout
;;;		4.3.93		jsc	mod'd functionality of
;;;					print_lop_formel,print_wunif
;;;					LOP output file named F.o.lop
;;;		8.3.93		jsc	output of execution times
;;;		11.3.93		jsc	usage
;;;		26.3.93		jsc	bug: _pp file
;;;						also to .._pp.lop
;;;		04.05.93	jsc	debugging of: ..
;;;		2.7.93		jsc	extern.h
;;;		21.7.93		jsc	clean-up
;;;		14.12.93	jsc	purity, fold
;;;		28.2.94		jsc	CLK_TCK
;;;		24.3.94		jsc	V3.2
;;;		28.7.94		jsc	-searchtree, -interactive
;;;		5.1.94		hamdi	constraints
;;;		9.2.94		hamdi	sg reordering
;;;		25.02.94	hamdi	linksubs,rsubs...
;;;		02.03.94	hamdi	rlinksubs
;;;             16.06.95	PJ	-sgreord, .lop suffix
;;;		11.9.95		jsc	fcb-mod'd
;;;		19.1.96		jsc	hex-code & clean-up
;;;             17.04.97        RZ      split main.c: main.c / generator.c
;;;                                                   filehandler.c
;;;
******************************************************************************/

#include <sys/types.h>
#include <sys/times.h>
#include <signal.h>
#include <time.h>
#include "types.h"
#include "extern.h"

/*****************************************************************************/
/* statics                                                                   */
/*****************************************************************************/
#ident "inwasm: @(#)main.c	15.2 01 Apr 1996"
static char logo[] = "inwasm V4.02b [wasm-less] Copyright TU Munich 1998 (03/30/98)";

/*****************************************************************************/
/* functions (inwasm/main.c)                                                 */
/*****************************************************************************/
int main();
void usage();
void verbose_usage();
void set_inwasm_defaults();
int process_argv();
void check_argument_consistency();
extern int isdigit();


/*****************************************************************************/
/* main()                                                                    */
/* Description: main programm to start the inwasm                            */
/*****************************************************************************/
int main(argc,argv)
int argc;
char *argv[];
{
  struct tms buffer;        /* buffer for storing time informations */
  int arg_nr;               /* variable holds the number of arguments */

  /* store the beginning time in the variable time	                     */
  times(&buffer);
  start_time = buffer.tms_utime;
  time(&start_time_in_sec);

  /* get "overall_time" */
  overall_time = ((float)(buffer.tms_utime) / (float)CLK_TCK);

  /* echo the logo on screen                                                 */
  printf("%s\n", logo);

  /* reset limitation variables */
  memoryused = 0;

  /* check arguments/options                                                 */
  if (argc < 2) {
    usage(argv[0]);
    fprintf(stderr,"  Please specify your argument(s).\n");
    exit(10);
  }
	
  if ((argc == 2) &&
      (!strcmp(argv[1], "-verbose"))) {
    verbose_usage(argv[0]);
    fprintf(stderr,"  Please specify your verbosity level.\n");
    exit(11);
  }

  if (*argv[argc-1] == '-') {
    fprintf(stderr,"  Please specify an input file.\n");
    exit(12);
  }
  	
  /* echo the command line                                                   */
  printf("  Command line: ");
  for (arg_nr = 0; arg_nr < argc; arg_nr++) {
    printf("%s ", argv[arg_nr]);
  }
  printf("\n");

  /* set the inwasm default flags                                            */
  set_inwasm_defaults();

  /* process arguments and options                                           */
  if (process_argv(argv, argc)) {
    exit(13);
  }

  /* check consistency of the arguments                                      */
  check_argument_consistency();

  /* ----------------------------------------------------------------------- */

  /* open input and output files                                             */
  open_io_files(argc, argv);                                /* filehandler.c */

  /* ----------------------------------------------------------------------- */

  /* parse input, generate constraints, ...                                  */
  parse_input_file();
  generate();                                                /* generator.c  */
  
  /* ----------------------------------------------------------------------- */

  /* generate code                                                           */
  generate_code();                                           /* generator.c  */

  /* ----------------------------------------------------------------------- */
  /* print information about compilation (errors found and time)             */

  if (errors) {
    fprintf(stderr,"\n  %d errors found, no code generated.\n",errors);
    exit(60);
  }

  times(&buffer);
  fprintf(stderr,
	  "  %s%s generated in %5.2f seconds\n",
	  filename_base,
	  (inwasm_flags.lop)?"_pp.lop":
	  (inwasm_flags.hex_out)?".hex":
	  ((inwasm_flags.nocode)?" ":".s"),
	  ((float)((buffer.tms_utime - start_time) * 1000 /
		   (float)CLK_TCK))/1000.0);

  /* end of main() - end of programm                                         */
  /* no errors                                                               */
  return 0;
}


/*****************************************************************************/
/* usage()                                                                   */
/* Description: Prints parameter options to screen                           */
/*****************************************************************************/
void usage(name)
  char *name;
{
  fprintf(stderr,"\n  usage: %s\n",name);
  fprintf(stderr,"\t[ -[no]purity]        # pure clauses are [not] removed\n");
  fprintf(stderr,"\t[ -[no]fan]           # axioms and goals are [not] fanned\n");
  fprintf(stderr,"\t[ -[no]reduct]        # reduction steps are [not] inserted\n");
  fprintf(stderr,
	  "\t[ -[no]sgreord [at av ac]]       # subgoal ordering is [not] changed\n");
  fprintf(stderr,"\t[ -[no]clreord]       # clause ordering is [not] changed\n");
  fprintf(stderr,"\t[ -oldclreord]        # use the old clause reordering\n");
  fprintf(stderr,"\t[ -ignorelits]        # don't count ^<name> predicates\n");
  fprintf(stderr,"\t[ -randreord]         # clause ordering is randomized\n");
  fprintf(stderr,"\t[ -all]               # enumerate all proofs\n");
  fprintf(stderr,"  constraint choices:\n");
  fprintf(stderr,"\t[ -eqpred]            # enable equal predecessor fails\n");
  fprintf(stderr,"\t[ -reg]               # generate regularity constraints \n");
  fprintf(stderr,"\t[ -taut]              # generate tautology constraints\n");
  fprintf(stderr,"\t[ -subs]              # generate subsumption constraints\n");
  fprintf(stderr,"\t[ -rsubs]             # subsumtion constraints without facts\n");
  fprintf(stderr,"\t[ -logprogsubs]       # special treaty of clauses with built-ins\n");
  fprintf(stderr,"\t[ -overlap [n]]       # overlap constraints [n]\n");
  fprintf(stderr,"\t[ -overlapx [n]]      # extensive overlap constraints [n]\n");
  fprintf(stderr,"\t[ -cons]              # -reg + -taut + -rsubs\n");
  fprintf(stderr,"\t[ -ocons [n]]         # -reg + -taut + -rsubs -overlap [n]\n");
  fprintf(stderr,"\t[ -oconsx [n]]        # -reg + -taut + -rsubs -overlapx [n]\n");
  fprintf(stderr,"  link deletion:\n");
  fprintf(stderr,"\t[ -linksubs [n]]      # remove redundant connections \n");
  fprintf(stderr,"\t[ -rlinksubs [n]]     # link subsumption between rules \n");
  fprintf(stderr,"  additional inference rules:\n");
  fprintf(stderr,"\t[ -foldup]            # insert folding-up operations\n");
  fprintf(stderr,"\t[ -foldupx]           # folding-up with extended regularity\n");
  fprintf(stderr,"\t[ -folddown]          # insert folding-down operations\n");
  fprintf(stderr,"\t[ -folddownx]         # folding-down with extended regularity\n");
  fprintf(stderr,"  verbostity options:\n");
  fprintf(stderr,"\t[ -verbose [n]]       # verbosity of preproc phase [n]\n");
  fprintf(stderr,"\t[ -nocode]            # generate no hex-file file.hex \n");
  fprintf(stderr,"\t[ -lop]               # generate lop-file file_pp.lop \n");
  fprintf(stderr,"\t[ -scode]             # generate s-file file.s \n");
  fprintf(stderr,"\t[ -noopt]             # do not optimize .hex file\n");
  fprintf(stderr,"\t[ -notree]            # the proof tree is not generated\n");
  fprintf(stderr,"\t[ -searchtree]        # enable displaying the search tree\n");
  fprintf(stderr,"\t[ -interactive]       # enable interactive proof control\n");
  fprintf(stderr,"  cpu-time and memory options:\n");
  fprintf(stderr,
	  "\t[ -cputimelimit [n]]  # limits used cputime to [n] seconds\n");
  fprintf(stderr,
	  "\t[ -realtimelimit [n]] # limits used realtime to [n] seconds\n");
  fprintf(stderr,
	  "\t[ -memorylimit [n]]   # limits used memory space to [n]{M|K} bytes (M=MByte,K=KByte).\n");
  fprintf(stderr,
	  "\t[ -freememory]        # activates deallocation of memory\n");
  fprintf(stderr,"  clause reordering options:\n");
  fprintf(stderr,
	  "\t[ -clstart [n]              # starts clause reordering from clause [n]\n");
  fprintf(stderr,
	  "\t[ -ignoreclstart      # ignores `@ClauseReorderingStartsHere` in lop-file\n");
  fprintf(stderr,"  input formula:\n");
  fprintf(stderr,"\tfile                  # compile input formula from file.lop\n");
  
}


/****************************************************************************/
/* verbose_usage()                                                          */
/* Description: Usage for verbose mode.                                     */
/****************************************************************************/
void verbose_usage(name)
char *name;
{  
  fprintf(stderr,"  Usage of verbose mode in %s: %s -verbose [nu] ...\n\n",
	  name,name);
  fprintf(stderr,"  Print formula in lop syntax on stdout.\n");
  fprintf(stderr,"  'nu' is one of the following (default = 0):\n\n");
  fprintf(stderr,"	%d  : no printing at all\n",PF_NO);
  fprintf(stderr,
	  "	%d  : after parsing (before preprocessing)\n",PF_PARS);
  fprintf(stderr,"	%d  : after tautology constraints\n",PF_TCONSTR);
  fprintf(stderr,"	%d  : after subsumption constraints\n",PF_SCONSTR);
  fprintf(stderr,"	%d  : after constraint deletion\n",PF_FIRSTDEL);
  fprintf(stderr,"	%d  : after purity\n",PF_PUR);
  fprintf(stderr,"	%d  : after orbranch reordering\n",PF_CLREORD);
  fprintf(stderr,"	%d  : after fanning\n",PF_FAN);
  fprintf(stderr,"	%d  : after symmetry constraints\n",PF_SYMCONSTR);
  fprintf(stderr,"	%d  : after overlap constraints\n",PF_OCONSTR);
  fprintf(stderr,"	%d  : after constraint deletion\n",PF_SECONDDEL);
  fprintf(stderr,"	%d  : after subgoal reordering\n",PF_SGREORD);
  fprintf(stderr,"	%d  : always\n",PF_ALWAYS);
}


/****************************************************************************/
/* set_inwasm_defaults()                                                    */
/* Description: set defaults for flags                                      */
/****************************************************************************/
void set_inwasm_defaults()
{
  inwasm_flags.clausreord = 1;
  inwasm_flags.oldclausreord = 0;
  inwasm_flags.sgreord = 1;
  inwasm_flags.ptree = 1;
  inwasm_flags.all = 0;
  inwasm_flags.verbose = 0;
  inwasm_flags.reduct = 1;
  inwasm_flags.fan = 1;
  inwasm_flags.predfail = 0;
  inwasm_flags.ptree = 1;
  inwasm_flags.purity = 1;
  inwasm_flags.randreord = 0;
  inwasm_flags.genreg = 0;
  inwasm_flags.set_subgoal_list = 1;
  inwasm_flags.cpybd = 0;
  inwasm_flags.cpylimit = 0;
  inwasm_flags.foldup = 0;
  inwasm_flags.foldupx = 0;
  inwasm_flags.folddown = 0;
  inwasm_flags.folddownx = 0;
  inwasm_flags.interactive = 0;
  inwasm_flags.searchtree = 0;
  inwasm_flags.overlap = 0;
  inwasm_flags.linksubs = 0;
  inwasm_flags.hex_out = 1;     
  inwasm_flags.hex_out_opt = 1;
  inwasm_flags.lop = 0;        
  inwasm_flags.nocode = 0;
  inwasm_flags.ignorelits = 0;
  inwasm_flags.memorylimit = 0;
  inwasm_flags.cputimelimit = 0;
  inwasm_flags.realtimelimit = 0;
  inwasm_flags.freememory = 0;
  inwasm_flags.logprogsubs = 0;
  inwasm_flags.clstart = 0;
  inwasm_flags.ignoreclstart = 0;

  /* defaults for sgreordering */
  sgreord_at=2.0; 
  sgreord_av=1.0;
  sgreord_ac=1.0;
}
  

/****************************************************************************/
/* process_argv()                                                           */
/* Description: Process the parameters and set the inwasm flags.            */
/****************************************************************************/
int process_argv(argv,ac)
char **argv;
int ac;
{
  int stringlength;
  char memstring [30];
  int byte_flag = FALSE;
  int kilo_flag = FALSE;
  int mega_flag = FALSE;
 
  while ((ac > 1) && 
	 (*argv[1] == '-')) {
    if (!strcmp(argv[1],"-all")) {
      inwasm_flags.all 	 = 1;
    }
    else if (!strcmp(argv[1],"-noclreord")) {
      inwasm_flags.clausreord = 0;
    }
    else if (!strcmp(argv[1],"-oldclreord")) {
      inwasm_flags.oldclausreord = 1;
    }
    else if (!strcmp(argv[1],"-ignorelits")) {
      inwasm_flags.ignorelits = 1;
    }
    else if (!strcmp(argv[1],"-reg")) {
      inwasm_flags.genreg    = 1;
    }
    else if (!strcmp(argv[1],"-purity")) {
      inwasm_flags.purity   = 1;
    }
    else if (!strcmp(argv[1],"-nopurity")) {
      inwasm_flags.purity   = 0;
    }
    else if (!strcmp(argv[1],"-interactive")) {
      inwasm_flags.interactive = 1;
      inwasm_flags.set_subgoal_list = 1;
    }
    else if (!strcmp(argv[1],"-searchtree")) {
      inwasm_flags.searchtree = 1;
      inwasm_flags.interactive = 1;
    }
    else if (!strcmp(argv[1],"-nocode")) {
      inwasm_flags.nocode = 1;
      inwasm_flags.hex_out = 0;
    }
    else if (!strcmp(argv[1],"-lop")) {
      inwasm_flags.lop = 1;
      inwasm_flags.nocode = 1;
      inwasm_flags.hex_out = 0;
    }
    else if (!strcmp(argv[1],"-scode")) {
      inwasm_flags.hex_out = 0;
    }
    else if (!strcmp(argv[1],"-noopt")) {
      inwasm_flags.hex_out_opt = 0;
    }
    else  if (!strcmp(argv[1],"-fan")) {
      inwasm_flags.fan = 1;
    }
    else if (!strcmp(argv[1],"-nofan")) {
      inwasm_flags.fan = 0;
    } 
    else if (!strcmp(argv[1],"-ignoreclstart")) {
      inwasm_flags.ignoreclstart = 1;
    }
    else if (!strcmp(argv[1],"-clstart")) {
      inwasm_flags.clstart = 1;
      if (isdigit(*argv[2])) {
        sscanf(argv[2], "%d", &cl_reord_start_commline);
	if (cl_reord_start_commline < 2) {
	  fprintf(stderr, 
		  "Usage: -clstart [n]. Clause reordering starts with clause [n>=2].\n");
	  exit(17);
	}
	argv++;
	ac--;
      }
      else {
	fprintf(stderr, 
		"Usage: -clstart [n]. Clause reordering starts with clause [n>0].\n");
	exit(14);
      }
    }
    else if (!strcmp(argv[1],"-freememory")) {
      inwasm_flags.freememory = 1;
    }
    else if (!strcmp(argv[1],"-cputimelimit")) {
      inwasm_flags.cputimelimit = 1;
      if (isdigit(*argv[2])) {
        sscanf(argv[2], "%d", &cputimelimit);
	argv++;
	ac--;
 	alarm(cputimelimit);
	signal(SIGALRM, sig_alrm);
      }
      else {
	fprintf(stderr, 
		"Usage: -cputimelimit [n]. Limit CPU-time usage to [n] seconds.\n");
	exit(14);
      }
    }
    else if (!strcmp(argv[1],"-realtimelimit")) {
      inwasm_flags.realtimelimit = 1;
      if (isdigit(*argv[2])) {
        sscanf(argv[2], "%d", &realtimelimit);
	argv++;
	ac--;
	alarm(realtimelimit);
	signal(SIGALRM, sig_alrm);
      }
      else {
	fprintf(stderr, 
		"Usage: -realtimelimit [n]. Limit realtime usage to [n] seconds.\n");
	exit(15);
      }
    }
    else if (!strcmp(argv[1],"-memorylimit")) {
      inwasm_flags.memorylimit = 1;
      if (*argv[2] != NULL) {
	strcpy(memstring, argv[2]);
	stringlength = strlen(memstring);
	if (memstring[stringlength-1] == 'k' ||
	    memstring[stringlength-1] == 'K') {
	  kilo_flag = TRUE;
	} else if (memstring[stringlength-1] == 'm' ||
	    memstring[stringlength-1] == 'M') {
	  mega_flag = TRUE;
	} else if (memstring[stringlength-1] == 'b' ||
	    memstring[stringlength-1] == 'B') {
	  byte_flag = TRUE;
	}
	if (byte_flag || kilo_flag || mega_flag) {
	  memstring[stringlength-1] = '\0';
	}
      }
      if (isdigit(*memstring)) {
        sscanf(argv[2], "%lf", &memorylimit);
	argv++;
	ac--;
	if (kilo_flag) 
	  memorylimit = memorylimit * 1024;
	else if (mega_flag)
	  memorylimit = memorylimit * 1024 * 1024;
      }
      else {
	fprintf(stderr, 
		"Usage: -memorylimit [n]. Limit memory usage to [n]{M|K} bytes (M=MByte,K=KByte).\n");
	exit(16);
      }
    }
    else if (!strcmp(argv[1],"-sgreord")) {
      inwasm_flags.sgreord = 2;
      if (isdigit(*argv[2])) {
	sscanf(argv[2], "%f", &sgreord_at);
	argv++;
	ac--;
      }
      if (isdigit(*argv[2])) {
	sscanf(argv[2], "%f", &sgreord_av);
	argv++;
	ac--;
      }
      if (isdigit(*argv[2])) {
	sscanf(argv[2], "%f", &sgreord_ac);
	argv++;
	ac--;
      }
    }
    else if (!strcmp(argv[1],"-nosgreord")) {
      inwasm_flags.sgreord = 0;
    }
    else if (!strcmp(argv[1],"-eqpred")) {
      inwasm_flags.predfail = 1;
    }
    else if (!strcmp(argv[1],"-reduct")) {
      inwasm_flags.reduct = 1;
    }
    else if (!strcmp(argv[1],"-noreduct")) {
      inwasm_flags.reduct = 0;
    }
    else if (!strcmp(argv[1],"-notree")) {
      inwasm_flags.ptree = 0;
      inwasm_flags.set_subgoal_list = 1; 
    }
    else if (!strcmp(argv[1],"-subs")) {
      inwasm_flags.subs = 1;
    }
    else if (!strcmp(argv[1],"-logprogsubs")) {
      inwasm_flags.logprogsubs = 1;
    }
    else if (!strcmp(argv[1],"-rsubs")) {
      inwasm_flags.subs = 2;
    }
    else if (!strcmp(argv[1],"-taut")) {
      inwasm_flags.tauto = 1;
    }
    else if (!strcmp(argv[1],"-overlap")) {
      if (isdigit(*argv[2])) {
	sscanf(argv[2], "%d", &overlap_mode);
      }
      inwasm_flags.overlap = 1;
    }
    else if (!strcmp(argv[1],"-overlapx")) {
      if (isdigit(*argv[2])) {
	sscanf(argv[2], "%d", &overlap_mode);
      }
      inwasm_flags.overlap = 2;
    }
    else if (!strcmp(argv[1],"-linksubs")) {
      if (isdigit(*argv[2])) {
	sscanf(argv[2], "%d", &deletion_mode);
      }
      inwasm_flags.linksubs = 1;
    }
    else if (!strcmp(argv[1],"-rlinksubs")) {
      if (isdigit(*argv[2])) {
	sscanf(argv[2], "%d", &deletion_mode);
      }
      inwasm_flags.linksubs = 2;
    }
    else if (!strcmp(argv[1],"-cons")) {
      inwasm_flags.tauto   = 1;
      inwasm_flags.subs    = 2;
      inwasm_flags.genreg  = 1;
      inwasm_flags.overlap = 0;
    }
    else if (!strcmp(argv[1],"-ocons")) {
      inwasm_flags.tauto   = 1;
      inwasm_flags.subs    = 2;
      inwasm_flags.genreg  = 1;
      if (isdigit(*argv[2])) {
	sscanf(argv[2], "%d", &overlap_mode);
      }
      inwasm_flags.overlap = 1;
    }
    else if (!strcmp(argv[1],"-oconsx")) {
      inwasm_flags.tauto   = 1;
      inwasm_flags.subs    = 2;
      inwasm_flags.genreg  = 1;
      if (isdigit(*argv[2])) {
	sscanf(argv[2], "%d", &overlap_mode);
      }
      inwasm_flags.overlap = 2;
    }
    else if (!strcmp(argv[1],"-verbose")) {
      if (isdigit(*argv[2])) {
	sscanf(argv[2], "%d", &inwasm_flags.verbose);
	argv++;
	ac--;
      }
      else inwasm_flags.verbose = 0;
    }
    else if (!strcmp(argv[1],"-randreord")) {
      inwasm_flags.randreord  = 1;
    }
    else if (!strcmp(argv[1],"-foldup")) {
      inwasm_flags.foldup  = 1;
      inwasm_flags.reduct = 1;
    }
    else if (!strcmp(argv[1],"-foldupx")) {
      inwasm_flags.foldupx  = 1;
      inwasm_flags.genreg = 1;
      inwasm_flags.set_subgoal_list = 1;
      inwasm_flags.reduct = 1;
    }
    else if (!strcmp(argv[1],"-folddown")) {
      inwasm_flags.folddown  = 1;
      inwasm_flags.set_subgoal_list = 1;
      inwasm_flags.reduct = 1;
    }
    else if (!strcmp(argv[1],"-folddownx")) {
      inwasm_flags.folddownx  = 1;
      inwasm_flags.genreg = 1;
      inwasm_flags.set_subgoal_list = 1;
      inwasm_flags.reduct = 1;
    }
    else {
      warning("Unknown switch: ",argv[1]);
      return 1;
    }
    argv++;
    ac--;
   
  }
return 0;
}

/****************************************************************************/
/* check_argument_consistency()                                             */
/* Description: check consistency of the arguments                          */
/****************************************************************************/
void check_argument_consistency()
{
  if (inwasm_flags.foldup && inwasm_flags.foldupx) {
    warning("","-foldupx overrides -foldup");
    inwasm_flags.foldup = 0;
    inwasm_flags.set_subgoal_list = 1;
  }
  if (inwasm_flags.folddown && inwasm_flags.folddownx) {
    warning("","-folddownx overrides -folddown");
    inwasm_flags.folddown = 0;
    inwasm_flags.set_subgoal_list = 1;
  }
  if (inwasm_flags.interactive && inwasm_flags.randreord) {
    warning("",
	    "-interactive is not compatible with -randreord: -norandreord set");
    inwasm_flags.randreord = 0;
  }
  if (inwasm_flags.interactive){
    inwasm_flags.set_subgoal_list = 1;
  }
  if (inwasm_flags.realtimelimit && inwasm_flags.cputimelimit) {
    /* realtimelimit is always earlier reached than cputimelimit */
    if (realtimelimit < cputimelimit) {
      inwasm_flags.cputimelimit = 0;
      warning("",
	    "-cputimelimit and -realtimelimit set:  using lower realtime limit");
      alarm(realtimelimit);
    } else {
      alarm(cputimelimit);
      warning("",
	      "-cputimelimit and -realtimelimit set:  using lower cputime limit");
    }
  }
}
	
