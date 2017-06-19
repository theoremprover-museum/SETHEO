/*****************************************************************************/
/*    S E T H E O                                                            */
/*                                                                           */
/* FILE: main.c                                                              */
/* VERSION:		@(#)main.c	8.2 27 Feb 1996                      */
/* DATE: 23.12.87                                                            */
/* AUTHOR: J. Schumann                                                       */
/* NAME OF FILE:                                                             */
/* DESCR:                                                                    */
/* MOD:                                                                      */
/*      11.11.92 new parameter parsing                                       */
/*	10.3.93 	jsc	Version 3.0 parameters                       */
/*	17.9.93		jsc		remove of old lemma stuff, tell,told */
/*					copy-binding                         */
/*					+ outputfile                         */
/*	25.2.94		jsc	CLK_TCK                                      */
/*	22.3.94		jsc	CLK_TCK, void sighand()                      */
/*	21.9.94		jsc	time-limits                                  */
/*	27.9.94		jsc	time-limits                                  */
/*	7.11.94		jsc	lemmatree                                    */
/*	13.1.95		jsc	initialization                               */
/*	9.3.95		jsc	batchmode                                    */
/*	18.4.95		wolfa	ctype raus                                   */
/*	6/99		PIL modifications into LINUX Version                 */
/* BUGS:                                                                     */
/*****************************************************************************/

#include <sys/types.h>
#include <sys/times.h>
#include <stdio.h>

#include "my_unistd.h"
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "defaults.h"
#include "codedef.h"
#include "symbtab.h"
#include "cmdline.h"
#include "sampars.h"
#include "unitlemma.h"
#include "iteration.h"
#include "dynsgreord.h"

#ifdef SIGNALS
#include <signal.h>
#endif

#ifdef STATPART
#include <ctype.h>
#include <signal.h>
#include "sps.h"
#endif

#ifndef CLK_TCK
#define CLK_TCK 60
#endif

#ifdef ILF
#include "stdlibwrap.h"
#include "pvm3.h"
#include <time.h>
#define FOUND_TAG 101
#endif

/**/
#include "bounds.h"
#include "cmdline.h"
#include "deref.h"
#include "errors.h"
#include "init.h"
#include "itime.h"
#include "lcode.h"
#include "signals.h"


/* zum testen auf der sun */

/* #define time() 0 */

#undef PVM

/**************************************************************/
/*** sizes of data areas                                    ***/
/**************************************************************/

#ifdef CONSTRAINTS
static int      c_stack_size = DEF_C_STACK;
static int      pu_trail_size = DEF_PU_TRAIL;
#endif

static int      seed;
static int      stacksize = DEF_STACK;
static int      heapsize = DEF_HEAP;
static int      codesize = DEF_CODE;
static int      trailsize = DEF_TRAIL;
static int      symb_size = DEF_SYMBSIZE;

#ifdef STATPART
static int	inf_step_size = DEF_INF_STEP;
extern int      spsighand_XCPU ();
extern int      spsighand_USR1 ();
extern int      spsighand_ALRM ();
extern int      ff_check;
extern int      ff_bound;
extern int      ff_check_mode;
extern int      ff_saved_maxinf;
extern int      saved_reg_constr, saved_ts_constr;
#endif

/*********************************************************************/
/*** static variables for ID of iterative bounding                 ***/
/*********************************************************************/
static int      iterative_bounding = NONE;
static int      min_depth = DEF_MIN_DEPTH;       /* depth */
static int      depth_inc = DEF_DEPTH_INC;       /* depth */
static int      min_inf = DEF_MIN_INF;           /* inferences */
static int      inf_inc = DEF_INF_INC;           /* inferences */
static int      min_loc_inf = DEF_MIN_LOC_INF;   /* local inferences */
static int      loc_inf_inc = DEF_LOC_INF_INC;   /* local inferences */

#ifdef STATPART
/*********************************************************************/
/*** static variables for static partitioning                      ***/
/*********************************************************************/
static int      spgen;
static int      iand;     /* IAnd */
static int      subsume;  /* IAnd */
static int      taskgen;
static int      subgs;    /* IAnd */
static int      sprecomp;
clock_t  start_time;

FILE                   *taskfile;
static int             ptree = FALSE;      /* proof-tree output on/off */
#endif

unsigned long debug_level = 0;

/*********************************************************************/
/*** TABLE OF PARAMETERS                                           ***/
/*********************************************************************/
argentry argtab[] = {
  { ARG_DEPTH,	opt,	"d",		integer,	req,	"",	0},
  { ARG_INF,	opt,	"i",		integer,	req,	"",	0},
  { ARG_LOCINF,	opt,	"cdd",		integer,	req,	"",	0},
  { ARG_LOCINF,	opt,	"loci",		integer,	req,	"",	0},
  { ARG_WD,	opt,	"wd",		integer,	opt,	"",	0},
  { ARG_WD1,	opt,	"wd1",		integer,	opt,	"",	
    DEF_WD1},
  { ARG_WD2,	opt,	"wd2",		integer,	opt,	"",	
    DEF_WD2},
  { ARG_WD3,	opt,	"wd3",		integer,	opt,	"",	
    DEF_WD3},
  { ARG_WD4,	opt,	"wd4",		integer,	opt,	"",	
    DEF_WD4},
  { ARG_WD4_FLAG,opt,	"wd4flag",	none, 	        opt,	"",	1},
  { ARG_TC,	opt,	"tc",	        integer,	req,	"",	0},
  { ARG_TCDFLAG,opt,	"tcd",	        none,	        opt,	"",	0},
  { ARG_TCD1,	opt,	"tcd1",	        integer,	req,	"",	
    DEF_TCD1},
  { ARG_TCD2,	opt,	"tcd2",	        integer,	req,	"",
    DEF_TCD2},
  { ARG_SIG,    opt,    "sig",          integer,        req,    "",     0},
  { ARG_SIGDFLAG,opt,	"sigd",	        none,	        opt,	"",	0},
  { ARG_SIGD1,	opt,	"sigd1",	integer,	req,	"",	
    DEF_SIGD1},
  { ARG_SIGD2,	opt,	"sigd2",	integer,	req,	"",
    DEF_SIGD2},
  { ARG_FVARS,	opt,	"fvars",	integer,	req,	"",	0},
  /*
    { ARG_FVARSDFLAG,opt,"fvarsd",	none,	        opt,	"",	0},
    { ARG_FVARSD1,opt,	"fvarsd1",	integer,	req,	"",	
    DEF_FVARSD1},
    { ARG_FVARSD2,opt,	"fvarsd2",	integer,	req,	"",	
    DEF_FVARSD2},
    */
  { ARG_SGS,	opt,	"sgs",	        integer,	req,	"",	0},
  /*
    { ARG_SGSDFLAG,opt,	"sgsd",	        none,	        opt,	"",	0},
    { ARG_SGSD1,opt,	"sgsd1",	integer,	req,	"",
    DEF_SGSD1},
    { ARG_SGSD2,opt,	"sgsd2",	integer,	req,	"",
    DEF_SGSD2},
    */
  { ARG_ITER,	opt,	"dr",		integer,	opt,	"",	
    DEF_DEPTH_INC},
  { ARG_INFR,	opt,	"ir",		integer,	opt,	"",	
    DEF_INF_INC},
  { ARG_LOCINFR,opt,	"cddr",	        integer,	opt,	"",	
    DEF_LOC_INF_INC},
  { ARG_LOCINFR,opt,	"locir",	integer,	opt,	"",	
    DEF_LOC_INF_INC},
  { ARG_WDR,	opt,	"wdr",		integer,	opt,	"",	
    DEF_DEPTH_INC},
  { ARG_REG,	opt,	"reg",		none,	        opt,	"",	0},
  { ARG_TS,	opt,	"st",		none,		opt,	"",	0},
#ifdef ANTILEMMATA
  { ARG_ANL,	opt,	"anl",	        integer,	opt,	"",	
    DEF_MINCOST},
  { ARG_ANL,	opt,	"lfc",	        integer,	opt,	"",	
    DEF_MINCOST},
  { ARG_HORNANL,opt,	"hornanl",      integer,        opt,	"",	
    DEF_MINCOST},
  { ARG_HORNANL,opt,	"hornlfc",      integer,        opt,	"",	
    DEF_MINCOST},
  { ARG_NHORNANL,opt,	"nhornanl",     integer,        opt,	"",
    DEF_MINCOST},
  { ARG_NHORNANL,opt,	"nhornlfc",     integer,        opt,	"",
    DEF_MINCOST},
  /* { ARG_LPANL,opt,	"lpanl",        integer,        opt,	"",	
     DEF_MINCOST}, */
  /* { ARG_LPHORNANL,opt,"lphornanl",    integer,        opt,	"",	
     DEF_MINCOST}, */
  /* { ARG_LPNHORNANL,opt,"lpnhornanl",   integer,        opt,	"",
     DEF_MINCOST}, */
#endif
  { ARG_CONS,	opt,	"cons",		none,		opt,	"",	0},
  { ARG_DYNSGREORD,opt,	"dynsgreord",	integer,	opt,	"",	
    DEF_DYNSGREORD},
  { ARG_SINGLE_DELAY,opt,"singlealt",	integer, 	opt,	"",	0},
  { ARG_SINGLE_DELAY,opt,"singledelay",	integer, 	opt,	"",	0},
  { ARG_MULTI_DELAY,opt,"multialt",	integer, 	opt,	"",	0},
  { ARG_MULTI_DELAY,opt,"multidelay",	integer, 	opt,	"",	0},
  { ARG_SPREAD_REDUCTS,opt,"spreadreducts",none,      	opt,	"",	0},
  { ARG_SPREAD_REDUCTS,opt,"spred1",	none,      	opt,	"",	0},
  { ARG_FORCE_GR, opt,	"forcegr",	none,      	opt,	"",	0},
  { ARG_FORCE_DL, opt,	"forcedl",	none,      	opt,	"",	0},
  { ARG_FORCE_FVAR,opt,	"forcefvar",	none,      	opt,	"",	0},
  { ARG_FORCE_PS, opt,	"forceps",	none,      	opt,	"",	0},
  { ARG_DELAY_SWITCH,opt, "altswitch",	integer,      	req,	"",	0},
  { ARG_DELAY_SWITCH,opt, "delayswitch",integer,      	req,	"",	0},
  { ARG_SHORTCL,    opt,  "shortcl",	integer,      	req,	"",	0},
  { ARG_LONGCL,     opt,  "longcl",	integer,      	req,	"",	0},
  { ARG_HIDDEN,     opt,  "hide",	string,      	req,	"",	0},
  { ARG_LOOKAHEAD,opt,  "lookahead",    none,           req,    "",     0},
  { ARG_EQ_BY_STE_MOD,opt,"eq",         none,           req,    "",     0},
  { ARG_CODESIZE,   opt,  "code",	integer,	req,	"",	0},
  { ARG_STACKSIZE,opt,	"stack",	integer,	req,	"",	0},
  { ARG_CS,	opt,	"cstack",	integer,	req,	"",	0},
  { ARG_HEAPSIZE,opt,	"heap",		integer,	req,	"",	0},
  { ARG_TRAILSIZE,opt,	"trail",	integer,	req,	"",	0},
  { ARG_SYMBSIZE,opt,	"symbtab",	integer,	req,	"",	0},
  { ARG_SEED,	opt,	"seed",		integer,	req,	"",	0},
  { ARG_VERBOSE,opt,	"verbose",	integer,	opt,	"",	0},
  { ARG_VERBOSE,opt,	"v",		integer,	opt,	"",	0},
  { ARG_REALTIME,opt,	"realtime",	integer,	req,	"",	0},
  { ARG_CPUTIME,  opt,	"cputime",	integer,	req,	"",	0},
  { ARG_ALLTREES, opt,	  "alltrees",	  none,	          opt,	  "",	  0},
#ifdef UNITLEMMA
  { ARG_LEMMATREE,opt,	"lemmatree",	none,		req,	"",	0},
  { ARG_PRINTLEMMATA, opt,"printlemmata", none,	          opt,	  "",	  0},
#endif
  { ARG_BATCHMODE,opt,	"batch",	  none,		  req,	  "",	  0},
  { ARG_DEBUG,	  opt,	"debug",	  integer,	  opt,	  "",	  0},
#ifdef STATPART
  { ARG_TIMELIMIT,opt,    "tl",           integer,        req,    "",     0},
  { ARG_SPGEN,    opt,    "spgen",        integer,        opt,    "", 10000},
  { ARG_M,        opt,    "m",            integer,        opt,    "",     0},
  { ARG_SIM,      opt,    "sim",          integer,        opt,    "", 10000},
  { ARG_IAND,     opt,    "ia",           none,           opt,    "",     0},
  { ARG_LINRES,   opt,    "lr",           none,           opt,    "",     0},
  { ARG_EO,       opt,    "eo",           none,           opt,    "",     0},
  { ARG_NOGEN,    opt,    "taskgen",      none,           opt,    "",     0},
  { ARG_SUBGS,    opt,    "subgs",        none,           opt,    "",     0},
  { ARG_RECOMP,   opt,    "recomp",       none,           opt,    "",     0}, 
  { ARG_TSK,      opt,    "tsk",          none,           opt,    "",     0}, 
  { ARG_PVM,      opt,    "pvm",          integer,        opt,    "",     0}, 
  { ARG_FFCHECK,  opt,    "ffcheck",      integer,        req,    "",     0},
#endif
};

int size_argtab = sizeof(argtab)/sizeof(argentry);


/*****************************************************************************/

int  main (argc, argv)
     int             argc;
     char           *argv[];
{
  
  instr_result       retval;          /* return value of setheo */
  
  char      *argvector[100];          /* store options for log-file */
  int        arg_counter;
  
  char 	*curr_arg;		      /* ptr to currently parsed arg */
  
  char            s[100];	      /* gen. purp for sprintf */
  int             j;
#ifdef STATPART
  int             i;                  /* gen purp */
  int             pos; 
#endif
  
  int		iv,res,neg;
  char		*istr;
  
  FILE           *codefile;
#ifdef ILF
  char    *logo = "SAM V%s ILF-Version (C) Munich University of Technology (%s)";
#else
  char    *logo = "SAM V%s Copyright TU Munich (%s)";
#endif
  
#ifdef ILF
  int mytid, parenttid;   
  int max, i, find;
#endif
  
#ifdef STATPART
  char            *date;
  time_t curr_time;
  char    *ext;                /* f. Extension der Res.-Datei f.par. Sim.*/
  int     tab_pos;
  char    prooftask[500];
  char    *c_pt;
  char    rmcommand[50];
#endif
  
  t_hidden_preds *tmp_hidden_preds;
				/* used for predicate hiding */

#ifdef STATISTICS1
#ifdef STATPART
  start_time = times(&buffer);
#else
  times(&buffer);
#endif
  overall_time = ((float)buffer.tms_utime / (float)CLK_TCK);
#endif
  
  if (argc < 2) {
    usage(stderr,argv[0],"file");
#ifdef STATPART
    if( pvm )
      pvm_task_error(4); /* Bad error */
#endif
    exit (1);
  }
  
#ifdef ILF
  mytid = pvm_mytid();
  parenttid = pvm_parent(); 
  fprintf(stderr,"slave: tid %x enrolled to pvm.\n",mytid);  
#endif
  
  /**************************************************************/
  /*** default values for bounds                              ***/ 
  /**************************************************************/
  depth_bound = FALSE;
  depth = DEF_DEPTH;
  
  wd_bound = FALSE;
  wd1 = DEF_WD1;        /* for weighted depth bound search */
  wd2 = DEF_WD2;        /* for weighted depth bound search */
  wd3 = DEF_WD3;        /* for weighted depth bound search */
  wd4 = DEF_WD4;        /* for weighted depth bound search */
  wd4_flag = FALSE;     /* for weighted depth bound search */
  
  local_inf_bound = DEF_LOC_INF_BOUND;
  
  maxinf = DEF_INFCOUNT;
  inf_bound = FALSE;
  intmd_inf = 0;
  infcount = 0;
  
  maxfvars = DEF_MAXFVARS;
  fvars_bound = FALSE;
  intmd_fvars = 0;
  fvarsd_flag = FALSE;
  fvarsd1 = DEF_FVARSD1;
  fvarsd2 = DEF_FVARSD2;
  
  maxtc = DEF_MAXTC;
  tc_bound = FALSE;
  intmd_tc = 0;
  tcd_flag = FALSE;
  tcd1 = DEF_TCD1;
  tcd2 = DEF_TCD2;
  
  maxsgs = DEF_MAXSGS;
  sgs_bound = FALSE;
  intmd_sgs = 0;
  sgscount = 1;
  sgsd_flag = FALSE;
  sgsd1 = DEF_SGSD1;
  sgsd2 = DEF_SGSD2;
  
  maxsig = DEF_MAXSIG;
  sig_bound = FALSE;
  intmd_sig = 0;
  sigcount = 0;
  sigd_flag = FALSE;
  sigd1 = DEF_SIGD1;
  sigd2 = DEF_SIGD2;
  
  lookahead_flag = 1;
  inf_lookahead = 0;
  
  eq_by_ste_mod = 0;
  
  /**************************************************************/
  /*** default values for subgoal reordering                  ***/
  /**************************************************************/
  dynsgreord = 0;
  
  /**************************************************************/
  /*** default values for subgoal alternation                 ***/
  /**************************************************************/
  delaycount = 0;
  multi_delay = single_delay = delay_switch = FALSE;
  delay_resource = 0;
  force_ground = force_ps = force_delay = force_fvar = FALSE;
  spread_reducts = FALSE;
  switch_length = 0;
  option_for_shortcl = 2;
  option_for_longcl = 7;
  
  /**************************************************************/
  /*** default values for constraint-options                  ***/
  /**************************************************************/
#ifdef ANTILEMMATA
  anlcount = 0;
  mincost = DEF_MINCOST;
  lpanl = FALSE;
  stronganl = FALSE;
  horn = FALSE;
#endif
#ifdef CONSTRAINTS
  anl_constr = reg_constr = ts_constr = constraints = FALSE;
#endif
  
  
  /**************************************************************/
  /*** default values for several global flags                ***/
  /**************************************************************/
  batchmode = TRUE;
  print_all_trees = FALSE;
  print_lemmata = FALSE;
#ifdef REPROOF_MODE
  pmode = FALSE;
#endif
#ifdef STATPART
  ff_check = ff_check_mode = FALSE;
  ff_saved_maxinf = 0;
  s_boundfail      = FALSE;
  ornode_counter = ppt = 0;
  pt = 1;
  t_generate   = TRUE;
  create_task  = FALSE;
  recomp       = FALSE;
  recomp_mode  = FALSE;
  m_tasks      = MAX_TASK_BOUND;
  task_level   = MAX_TASK_BOUND;
  cputimelimit = 0;  /* fuer HP: falls != 0, cputimelimit gesetzt */
  
  print_subgs   = FALSE;
  iand_check    = FALSE;
  subsume_check = FALSE;
  linres        = FALSE;
  use_eo        = FALSE;
  spsim         = FALSE;
  num_of_subgs  = 0; 
  or_numb       = 0;
#endif
  
  hidden_preds  =0; 
  
  /**************************************************************/
  /*** time limits (per default not set)                      ***/
  /**************************************************************/
  cputime_limit = 0;      		
  realtime_limit = 0;
  
  /**************************************************************/
  /*** store options for log-file                             ***/
  /**************************************************************/
  for(j=0; j< argc; j++)
    argvector[j] = (char *)argv[j];
  arg_counter = argc;
  
  /**************************************************************/
  /*** read options                                           ***/
  /**************************************************************/
  argv++;
  argc--;
  curr_arg = argv[0];
  
  while ((res = parsearg(&argv,&argc,&iv,&istr,&neg,"no")) >= 0) {
    
    switch(res){
      
    case ARG_CONS:
#ifdef CONSTRAINTS
      if (!neg){
	constraints = 
	  anl_constr = reg_constr = ts_constr = TRUE;
      }
      else {
	constraints = 
	  anl_constr = reg_constr = ts_constr = FALSE;
      }
#else
      sprintf(s,"The option %s is not available in this version",
	      curr_arg);
      sam_error(s, NULL,0);
#endif
      break;
      
    case ARG_ANL:
#ifdef ANTILEMMATA
      anl_constr = (neg)?FALSE:TRUE;
      mincost = iv;
#else
      sprintf(s,"The option %s is not available in this version",
	      curr_arg);
      sam_error(s, NULL,0);
#endif
      break;
      
    case ARG_LPANL:
#ifdef ANTILEMMATA
      lpanl = TRUE;
      anl_constr = TRUE;
      mincost = iv;
#else
      sprintf(s,"The option %s is not available in this version",
	      curr_arg);
      sam_error(s, NULL,0);
#endif
      break;
      
    case ARG_HORNANL:
#ifdef ANTILEMMATA
      stronganl = TRUE;
      horn = TRUE;
      anl_constr = TRUE;
      mincost = iv;
#else
      sprintf(s,"The option %s is not available in this version",
	      curr_arg);
      sam_error(s, NULL,0);
#endif
      break;
      
    case ARG_NHORNANL:
#ifdef ANTILEMMATA
      stronganl = TRUE;
      horn = FALSE;
      anl_constr = TRUE;
      mincost = iv;
#else
      sprintf(s,"The option %s is not available in this version",
	      curr_arg);
      sam_error(s, NULL,0);
#endif
      break;
      
    case ARG_LPHORNANL:
#ifdef ANTILEMMATA
      lpanl = TRUE;
      stronganl = TRUE;
      horn = TRUE;
      anl_constr = TRUE;
      mincost = iv;
#else
      sprintf(s,"The option %s is not available in this version",
	      curr_arg);
      sam_error(s, NULL,0);
#endif
      break;
      
    case ARG_LPNHORNANL:
#ifdef ANTILEMMATA
      lpanl = TRUE;
      stronganl = TRUE;
      horn = FALSE;
      anl_constr = TRUE;
      mincost = iv;
#else
      sprintf(s,"The option %s is not available in this version",
	      curr_arg);
      sam_error(s, NULL,0);
#endif
      break;
      
    case ARG_REG:
#ifdef CONSTRAINTS
      reg_constr = (neg)?FALSE:TRUE;
#else
      sprintf(s,"The option %s is not available in this version",
	      curr_arg);
      sam_error(s, NULL,0);
#endif
      break;
      
    case ARG_TS:
#ifdef CONSTRAINTS
      ts_constr = (neg)?FALSE:TRUE;
#else
      sprintf(s,"The option %s is not available in this version",
	      curr_arg);
      sam_error(s, NULL,0);
#endif
      break;
      
    case ARG_CS:
#ifdef CONSTRAINTS
      c_stack_size = iv;
#else
      sprintf(s,"The option %s is not available in this version",
	      curr_arg);
      sam_error(s, NULL,0);
#endif
      break;
      
    case ARG_LOCINF:
      local_inf_bound = iv;
      break;
      
    case ARG_INF:
      maxinf = iv;
      inf_bound = TRUE;
      break;
      
    case ARG_LOCINFR:
      loc_inf_inc = iv;
      iterative_bounding = LOCINF;
      break;
      
    case ARG_INFR:
      inf_inc = iv;
      iterative_bounding = INF;
      inf_bound = TRUE;
      maxinf = 0;
      break;
      
    case ARG_FVARS:
      maxfvars = iv;
      fvars_bound = TRUE;
      break;
      
    case ARG_FVARSDFLAG:
      fvarsd_flag = (neg)?FALSE:TRUE;
      fvars_bound = TRUE;
      fvarsd_flag = TRUE;
      break;
      
    case ARG_FVARSD1:
      fvarsd1 = iv;
      fvarsd_flag = TRUE;
      fvars_bound = TRUE;
      break;
      
    case ARG_FVARSD2:
      fvarsd2 = iv;
      fvarsd_flag = TRUE;
      fvars_bound = TRUE;
      break;
      
    case ARG_TC:
      maxtc = iv;
      tc_bound = TRUE;
      break;
      
    case ARG_TCDFLAG:
      tcd_flag = (neg)?FALSE:TRUE;
      tc_bound = TRUE;
      break;
      
    case ARG_TCD1:
      tcd1 = iv;
      tcd_flag = TRUE;
      tc_bound = TRUE;
      break;
      
    case ARG_TCD2:
      tcd2 = iv;
      tcd_flag = TRUE;
      tc_bound = TRUE;
      break;
      
    case ARG_SGS:
      maxsgs = iv;
      sgs_bound = TRUE;
      break;
      
    case ARG_SGSDFLAG:
      sgsd_flag = (neg)?FALSE:TRUE;
      sgs_bound = TRUE;
      break;
      
    case ARG_SGSD1:
      sgsd1 = iv;
      sgsd_flag = TRUE;
      sgs_bound = TRUE;
      break;
      
    case ARG_SGSD2:
      sgsd2 = iv;
      sgsd_flag = TRUE;
      sgs_bound = TRUE;
      break;
      
    case ARG_VERBOSE:
      verbose = iv;
      break;
      
    case ARG_DEPTH:
      depth = (neg)?DEF_DEPTH:iv;
      depth_bound = TRUE;
      break;
      
    case ARG_ITER:
      depth_inc = iv;
      iterative_bounding = DEPTH;
      depth_bound = TRUE;
      break;
      
    case ARG_WD:
      depth = (neg)?DEF_DEPTH:iv;
      wd_bound = TRUE;
      depth_bound = TRUE;
      break;
      
    case ARG_WDR:
      depth_inc = iv;
      iterative_bounding = DEPTH;
      wd_bound = TRUE;
      depth_bound = TRUE;
      break;
      
    case ARG_WD1:
      wd1 = (neg)?DEF_WD1:iv;
      wd_bound = TRUE;
      depth_bound = TRUE;
      break;
      
    case ARG_WD2:
      wd2 = (neg)?DEF_WD2:iv;
      wd_bound = TRUE;
      depth_bound = TRUE;
      break;
      
    case ARG_WD3:
      wd3 = (neg)?DEF_WD3:iv;
      wd_bound = TRUE;
      depth_bound = TRUE;
      break;
      
    case ARG_WD4:
      wd4 = (neg)?DEF_WD4:iv;
      wd_bound = TRUE;
      depth_bound = TRUE;
      break;
      
    case ARG_WD4_FLAG:
      wd4_flag = (neg)?FALSE:TRUE;
      wd_bound = TRUE;
      depth_bound = TRUE;
      break;
      
    case ARG_SIG:
      maxsig = iv;
      sig_bound = TRUE;
      break;
      
    case ARG_SIGDFLAG:
      sigd_flag = (neg)?FALSE:TRUE;
      sig_bound = TRUE;
      break;
      
    case ARG_SIGD1:
      sigd1 = iv;
      sigd_flag = TRUE;
      sig_bound = TRUE;
      break;
      
    case ARG_SIGD2:
      sigd2 = iv;
      sigd_flag = TRUE;
      sig_bound = TRUE;
      break;
      
    case ARG_LOOKAHEAD:
      lookahead_flag = (neg)?FALSE:TRUE;
      break;
      
    case ARG_EQ_BY_STE_MOD:
      eq_by_ste_mod = (neg)?FALSE:TRUE;
      break;
      
    case ARG_CODESIZE:
      codesize = iv;
      printf("codesize = %d\n",codesize);
      break;
      
    case ARG_STACKSIZE:
      stacksize = iv;
      break;
      
    case ARG_TRAILSIZE:
      trailsize = iv;
      break;
      
    case ARG_HEAPSIZE:
      heapsize = iv;
      break;
      
    case ARG_SYMBSIZE:
      symb_size = iv;
      break;
      
    case ARG_SEED:
      seed = iv;
      srand48 (seed);
      break;
      
    case ARG_CPUTIME:
      cputime_limit=iv;
      break;
      
    case ARG_REALTIME:
      realtime_limit=iv;
      break;
      
    case ARG_DYNSGREORD:
      dynsgreord = iv;
      break;
      
    case ARG_SINGLE_DELAY:
      single_delay = (neg)?FALSE:TRUE;
      multi_delay = FALSE;
      delay_resource = iv;
      break;
      
    case ARG_MULTI_DELAY:
      multi_delay = (neg)?FALSE:TRUE;
      single_delay = FALSE;
      delay_resource = iv;
      break;
      
    case ARG_FORCE_GR:
      force_ground = (neg)?FALSE:TRUE;
      break;
      
    case ARG_FORCE_PS:
      force_ps = (neg)?FALSE:TRUE;
      break;
      
    case ARG_FORCE_DL:
      force_delay = (neg)?FALSE:TRUE;
      break;
      
    case ARG_FORCE_FVAR:
      force_fvar = (neg)?FALSE:TRUE;
      break;
      
    case ARG_SPREAD_REDUCTS:
      spread_reducts = (neg)?FALSE:TRUE;
      break;
      
    case ARG_DELAY_SWITCH:
      delay_switch = TRUE;
      switch_length = iv;
      break;
      
    case ARG_SHORTCL:
      option_for_shortcl = iv;
      break;
      
    case ARG_LONGCL:
      option_for_longcl = iv;
      break;
      
    case ARG_BATCHMODE:
      batchmode = (neg)?FALSE:TRUE;
      break;
      
#ifdef UNITLEMMA
    case ARG_LEMMATREE:
      unitlemma_tree = (neg)?FALSE:TRUE;
      break;
#endif

    case ARG_HIDDEN:
	if (neg) {
		sam_error("No -hohide option", NULL,0);
		}
	else {
		tmp_hidden_preds = 
			(t_hidden_preds*)malloc(sizeof(t_hidden_preds));
		tmp_hidden_preds->next = hidden_preds;
		tmp_hidden_preds->name = strdup(istr);
		hidden_preds = tmp_hidden_preds;
		}
	break;
		
      
#ifdef STATPART
    case ARG_SPGEN:
    case ARG_M:
      create_task = TRUE;
      m_tasks = iv;
      break;
      
    case ARG_SIM:
      spsim = TRUE;
      break;
      
    case ARG_TSK:
      /* Set flags -cons -dr */
      constraints  = anl_constr = reg_constr = ts_constr = TRUE;
      iterative_bounding = DEPTH;
      depth_bound = TRUE;
      
      recomp = TRUE;
      t_proving = 1;
      pvm = TRUE;
      sprintf(prooftask,"%s", argv[0]);
      argc--;
      argv++;
      break;
      
    case ARG_RECOMP:
      recomp = TRUE;
      t_proving = 1;
      i = pos = 0;
      while( strcmp(*argv, "-2") && i++ < 200 ){
	for( j = 0 ; j < strlen(*argv) ; j++ )
	  {
	    prooftask[pos++] = argv[0][j]; 
	  }
	prooftask[pos++] = ' ';
	argc--;
	argv++;
      }
      for( j = 0 ; j < strlen(*argv) ; j++ )
	prooftask[pos++] = argv[0][j]; 
      prooftask[pos++] = ' ';
      argc--;
      argv++;
      if( i == 200 )
	sam_error("Prooftask: no end after 200 chars??\n",0,NULL);
      break;
      
    case ARG_FFCHECK:
      ff_check = TRUE;
      ff_bound = iv;
      break;
      
    case ARG_IAND:
      iand_check = TRUE;
      break;
      
    case ARG_LINRES:
      linres = TRUE;
      iand_check = TRUE;
      subsume_check = TRUE;
      break;
      
    case ARG_EO:
      use_eo = TRUE;
      break;
      
    case ARG_NOGEN:
      if (neg)
	count_task = TRUE;
      else
	{
	  printf("\nUnknown Option -taskgen\n");
	  if( pvm )
	    pvm_task_error(4); /* Bad error */
	  exit(2);
	}
      break;
      
    case ARG_SUBGS:
      print_subgs = TRUE;
      break;
      
    case ARG_PVM:
      pvm = TRUE;
      break;
      
    case ARG_TIMELIMIT:   /* CS 18.7.94 */
      cputimelimit = iv;
      break;
#endif
    case ARG_DEBUG:
      debug_level = iv;
      break;

    case ARG_ALLTREES:
      print_all_trees = (neg)?FALSE:TRUE;
      break;

    case ARG_PRINTLEMMATA:
      print_lemmata = (neg)?FALSE:TRUE;
      break;

    default:
      sam_error("internal error",NULL,2);
      exit(9);
      
    } /* switch (res) */
    
    curr_arg = argv[0];
    
  } /* while ((res = parsearg(...)) >= 0) */
  
  
  /***********************************************************/
  /* Initialization of delay option when using delay switch: */
  /***********************************************************/
  if (delay_switch) {
    set_delay_option(option_for_shortcl);
  }
  
  
#ifdef STATPART
  /* CS: Timelimit Handling */
  /*
    #ifndef HP
    uname( &sysinfo );
    if( ! strcmp( sysinfo.release, "5.3" ) ){
    cputimelimit--;  
    printf("CPUTIME LIMIT adjusted (Solaris Bug): %d\n",cputimelimit);
    }        
    rl.rlim_cur = rl.rlim_max = cputimelimit;   
    if( setrlimit(RLIMIT_CPU, &rl) )
    printf("Setting time limit did not work!\n");
    #else
    */
  
#ifdef HP
  setpriority(0,0,10);
#endif
  
  if( cputimelimit && !( pvm && ! recomp ) )
    alarm(cputimelimit);  /* time limit, but not for pvm master */
#endif /* STATPART */
  
  switch (res){
  case noarg:
    /* there should be a file-name */
    sam_error("Code file missing",NULL,2);
    exit(5);
  case absarg:
    /* we have reached the file-name: OK */
    break;
  case argnotfound:
    /* the user typed an unknown parameter */
  case argmissing:
    /* an required argument is missing */
  case intexpected:
    /* an integer was expected */
    sprintf(s,"%s: %s",argv[0],cmd_line_errtxt(res));
    sam_error(s,NULL,0);
    /* usage(stderr,argvector[0],""); */
    exit(1);
  }
  
  
  if (argc < 1) 
    sam_error("no code file", NULL,2);
  
#ifdef CONSTRAINTS
  if (anl_constr || reg_constr || anl_constr || ts_constr){
    constraints = TRUE;
  }
#endif
  
#ifdef STATPART
  if( recomp ){         
    READ_TASK;     /* Einlesen der SPTHEO-Task */
    c_pt = prooftask;
    sscanf(c_pt, "%d ",   &or_numb);
    while( *(c_pt++) != ' ' );
    
    /* Einlesen einer einzelnen Task auf die Datenstruktur tableau[] */
    tab_pos = -1;
    
    do {
      sscanf(c_pt,"%d,%d ",&(tableau[++tab_pos].codeptr),&(tableau[tab_pos].red_lvl));
      while( *(c_pt++) != ' ' );
    }
    while ( ! ( *c_pt == '-' && *(c_pt+1) == '1' && *(c_pt+2) == ' ') );
    sscanf(c_pt,"%d ", &(tableau[++tab_pos].codeptr));
    while( *(c_pt++) != ' ' );
    
    tableau[tab_pos].red_lvl = 0;
    
    /* IAbegin */
    /* Einlesen der IAnd-Task, falls vorhanden */
    sscanf(c_pt, "%d ", &(iand_task_num)); /* IAnd-Task# einlesen */
    while( *(c_pt++) != ' ' );
    if( iand_task_num != -2){ /* IAnd-Task vorhanden */
      saved_reg_constr = reg_constr;
      reg_constr = FALSE;
      if( linres ){
	saved_ts_constr  = ts_constr;
	ts_constr  = FALSE;
      }
      do{
	/* Subgoalnummern einlesen */
	sscanf(c_pt, "%d ", (WORD) &(tableau[++tab_pos].codeptr));
	while( *(c_pt++) != ' ' );
	tableau[tab_pos].red_lvl = 0;
      }
      while (tableau[tab_pos].codeptr != -2);
    }
    printf("Task (%d,%d): ", or_numb, iand_task_num);
    for( i = 0 ; i <= tab_pos ; i++ )
      printf("%d,%d ", tableau[i].codeptr, tableau[i].red_lvl);
    printf("\n");
    /* IAend */
  }
  
  /* open .res-File for parallel Simulation */
  if( create_task || recomp ){
    sprintf(s, "%s.res_%d", argv[0], or_numb); /* CS 16.11.94 */
    if( (resfile = fopen (s,"w")) == NULL ){
      PRINT_LOG1("could not open %s \n",s);
      if( pvm )
	pvm_task_error(4); /* Bad error */
      exit(2);
    }
    if( pvm && or_numb ){
      curr_time = time();
      date = ctime(&curr_time);
      fprintf(resfile,"\n***Prooftask %3d        - started  %s", 
	      or_numb, date);
    }
  }
#endif
  
  /**************************************************/
  /* time-limits are set here                       */
  /**************************************************/
  if (cputime_limit && realtime_limit){
    /*
    ** don't use both
    */
    sam_error("-realtime overrides -cputime",NULL,0);
    cputime_limit = 0;
  }
  
#ifdef SIGNALS
  /*
  ** include the signal handling routine
  */
  signal (SIGALRM, sig_alrm);
#endif
  
  /*
  ** set the alarm clocks
  */
  if (cputime_limit){
    alarm(cputime_limit);
  }
  if (realtime_limit){
    alarm(realtime_limit);
  }
  
  /*************************/
  /* initialize the setheo */
  /* output file to stdout */
  /*************************/
  outputfile = stdout;
  
  /************************/
  /* open setheo-Log-File */
  /************************/
  sprintf (s, "%s.log", argv[0]);
#ifdef STATPART
  if( recomp && pvm ) 
    sprintf (s, "%s.log_%d", argv[argc-1], or_numb );
#endif
  if ((logfile = fopen (s, "w")) == NULL) {
    printf ("cannot open %s\n", s);
    logfile = stdout;
  }
  
  PRINT_LOG2 (logo, ident, version);
  PRINT_LOG0 ("\n");
#ifdef WORD_64
  PRINT_LOG0("  64 bit word format\n");
#else
  PRINT_LOG0("  32 bit word format\n");
#endif    
  PRINT_LOG2 ("  main.c built on %s at %s\n",__DATE__,__TIME__);

#ifdef JSC_092016
  {
    time_t the_time = time(NULL);
    PRINT_LOG1 ("  run %s\n",ctime(&the_time));
  }
#endif
    
  PRINT_LOG0 ("TIMER STARTED...\n");
#ifdef STATISTICS1
  PRINT_LOG0 ("Options : ");
  for(j=1; j<arg_counter; j++)
    PRINT_LOG1 ("%s ", argvector[j])
      PRINT_LOG0 ("\n\n");
#endif
  
  /*************************/
  /* open setheo-TREE-File */
  /*************************/
  /* we use a graphics postprocessor */
#ifndef STATPART 
  /* for STATPART moved into i_ptree, to avoid opening if no proof is found! */
  sprintf (s, "%s.tree", argv[0]);
  if (!(ftree = fopen (s, "w"))) {
    PRINT_LOG1("could not open %s - use stdout\n", s);
    ftree = stdout;
  }
#endif
  
#ifdef CONSTRAINTS
#ifdef STATISTICS1
  if(anl_constr)
    PRINT_LOG0("using antilemma-constraints\n");
  if(reg_constr)
    PRINT_LOG0("using regularity-constraints\n");
  if(ts_constr) {
    PRINT_LOG0("using tautology-constraints\n");
    PRINT_LOG0("using subsumption-constraints\n");
  }
#endif
#endif
  
#ifdef STATPART
  signal(SIGALRM, spsighand_ALRM );
#ifndef HP
  signal(SIGXCPU, spsighand_XCPU );   /* install sptheo signal handler */
#endif
#ifdef HP
#ifdef PVM
  signal(_SIGUSR1, spsighand_USR1 );
#endif
#endif
#ifndef HP
#ifdef PVM
  signal(SIGUSR1, spsighand_USR1 );
#endif
#endif
  
  /* Fehlerbehandlung bei unzureichender Angabe von Aufruf-Parametern */
  
  /*
    if( !create_task ){
    if( !iand_check && !recomp ){
    printf("\nERROR: Flag NOIAND useless without -Generation!\n");
    exit(2);
    }
    if( !subsume_check && !recomp ){
    printf("\nERROR: Flag NOSUBSUME useless without Task-Generation!\n");
    exit(2);
    }
    if( print_subgs ){
    printf("\nERROR: Flag SUBGS useless without Task-Generation!\n");
    exit(2);
    }
    }
    */
  
  if( !create_task && (iand_check || subsume_check) )
    iand_check = subsume_check = FALSE;
  
  /* open .sp-File for Task-Generation     */
  if( create_task && !count_task && !pvm ){
    sprintf (s, "%s.sp", argv[0]);
    
    if( (taskfile = fopen (s,"w")) == NULL) {
      PRINT_LOG1("could not open %s \n", s);
      if( pvm )
	pvm_task_error(4); /* Bad error */
      exit(2);
    }
  }
  
  /* IAbegin */       /*   open .subgs-File   */
  if (print_subgs)
    {
      sprintf (s, "%s.subgs", argv[0]);
      if ((subgsfile = fopen (s, "w")) == NULL) 
	{
	  printf ("cannot open %s\n", s);
	  subgsfile = stdout;
	}
      fprintf (subgsfile,"# Call: sam ");
      for(j = 1; j < arg_counter; j++)
	fprintf (subgsfile,"%s ", argvector[j]);
      fprintf(subgsfile, "\n\n");
    }
  /* IAend */
  
  
  /* initialize global variables for IAnd  */
  /* IAbegin */
  if (iand_check || subsume_check)
    {
      for (i = 0; i < MAXOR_NODES; i++)
	for (j = 0; j < MAXLITS; j++)
	  ia_result[i][j] = -2;
      num_of_vars = 0;
      for (i = 0; i < MAXVAR; i++)
	vars[i] = -1;
      first_lit = NULL;
      last_lit = NULL;
      cnt_indp_or = 0;
      cnt_indp_grp = 0;
      num_of_tasks = 0;
    }
  /* IAend */
#endif
  
  
  /************************/
  /* open setheo-code-File */
  /************************/
  sprintf (s, "%s.hex", argv[0]);
  if ((codefile = fopen (s, "r")) == NULL){ 
    PRINT_LOG1("cannot open %s\n", s);
#ifdef STATPART
    if( pvm )
      pvm_task_error(4); /* Bad error */
#endif
    exit(1);
  }
#ifdef STATPART
  sprintf(lopfilename, "%s", argv[argc-1]);   
  sprintf(rmcommand, "rm %s.log", lopfilename);
#endif

    /********************************************************/
    /***      initialize memory alloc (linux)             ***/   
    /*		                                            */
    /********************************************************/
    sam_memory_init(c_stack_size+
		pu_trail_size+
		stacksize+
		heapsize+
	        codesize+
	        trailsize+
	       symb_size);
  /**************************/
  /* load code with symbtab */
  /**************************/
  
  /**************************/
  /* load code with symbtab */
  /**************************/
  PRINT_LOG0("\nLoading code...\n\n");
  code=NULL;
  symbtab=NULL;
  symbflg = TRUE;
  nr_symbols = 0;
  load_code (codefile,codesize,symb_size);
  fclose (codefile);
  
  
  /**************************/
  /* allocate memory of SAM */
  /**************************/
  PRINT_LOG0("\nInitializing memory...\n\n");
  init_seth_mem (
#ifdef CONSTRAINTS
		 c_stack_size, pu_trail_size,
#endif
		 stacksize, heapsize, codesize, trailsize, symb_size
#ifdef STATPART
		 , inf_step_size
#endif
		 );
  
  
  /*************************************/
  /* start values for setheo registers */
  /*************************************/
  PRINT_LOG0("\nInitializing registers1...\n\n");
#ifdef STATISTICS1
  /* set the statistics register only once */
  /* with exception of fails and total_inf (see init_reg) */
  cnt = global_tot_inf = fails = 0;
  inf_fails = local_inf_fails = unif_fails = depth_fails = 0;
  fvars_fails = tc_fails = sgs_fails = sig_fails = 0;
  total_reduct = 0;
  
  folding_statistics.total_factorizations = 0;
  folding_statistics.foldings = 0;
  folding_statistics.possible_lemmas = 0;
  folding_statistics.one_level_foldings = 0;
#endif
  
#ifdef CONSTRAINTS
#ifdef STATISTICS1
  gen_reg_constrs = gen_ts_constrs = gen_anl_constrs = 0;
  c_reg_fails = c_ts_fails = c_anl_fails = 0;
#endif
#ifdef STATISTICS2
  c_check_count = c_immed_reg_fails = c_immed_ts_fails 
    = c_immed_anl_fails = 0;
#endif
#endif
#ifdef STATPART /* automatische Schrankensuche bei der Task-Generierung */
  if (create_task && depth == DEF_DEPTH && maxinf == DEF_INFCOUNT 
      && local_inf_bound == DEF_LOC_INF_BOUND){
    min_inf = 1;
    inf_inc = 1;
    iterative_bounding = INF;
  }
#endif
  
  /* for signature bound search: */
  //JSC-092016 sig_occurrence = (int*) sam_malloc(nr_symbols * sizeof(int));
  sig_occurrence = (int*) malloc(nr_symbols * sizeof(int));
  if (!sig_occurrence)
    sam_error("main: Could not allocate memory for\
signature occurrence handling.", NULL, 2);
  
  PRINT_LOG0("\nInitializing registers2...\n\n");
  init_reg();
  PRINT_LOG0("\nInitializing bounds...\n\n");
  init_bounds(maxtc,maxsgs,maxfvars,maxsig);
  
  
  /*************************************/
  /* start interrupt handling          */
  /*************************************/
#ifdef SIGNALS
  PRINT_LOG0("\nInitializing signals...\n\n");
  signal(SIGINT, sighand);
#endif
  
  PRINT_LOG0("\nStart proving...\n\n");
  
  retval = instr_time(iterative_bounding, 
		      min_depth, depth_inc,
		      min_loc_inf, loc_inf_inc,
		      min_inf, inf_inc);
  
  PRINT_LOG0("\n");
  
  
  switch (retval) {
    
  case success:
#ifdef ILF
    pvm_initsend(PvmDataDefault);
    pvm_pkint(&mytid, 1, 1);
    pvm_send(parenttid, FOUND_TAG); 
    fprintf(stderr,"slave: tid %x successful.\n",mytid); 
    PRINT_LOG0("ILF-Version: Success message sent to master.\n");
#endif
    PRINT_LOG0("                    ******** SUCCESS ************\n");
    
#ifdef STATISTICS1
    PRINT_LOG0("\n");
    PRINT_LOG1("Number of inferences in proof      : %8d\n", infcount);
#ifdef STATPART
    if (recomp)
      PRINT_RES1("  SUCCESS Proof: %4d\n", infcount);
#endif
    PRINT_LOG4("        - E/R/F/L                  : %8d/%8d/%8d/%8d\n", 
	       (infcount - reductions_in_proof
		- folding_statistics.factorizations_in_proof), 
	       reductions_in_proof,
	       folding_statistics.factorizations_in_proof,
	       0);
    disp_statistics(0);
#ifdef DEREF_STATISTICS
    print_deref_statistics();
#endif
#endif
#ifdef REPROOF_MODE
    if (ptree) {
      /***********************************/
      /* no bounds in reproof-mode       */
      /***********************************/
      depth = DEF_DEPTH;
      local_inf_bound = DEF_LOC_INF_BOUND;
      infcount = 0;
      maxinf = DEF_INFCOUNT;
      
      pt_max = pt_ptr;
      pmode = TRUE;
      init_reg ();
      /* start repoofing   Goller */
      instr_cycle ();
    }
#endif
    fflush (stdout);
    break;
    
    
  case totfail:
#ifdef STATPART
    if (recomp){
      PRINT_LOG0("  FINITE FAILURE\n");
      PRINT_RES0("  FINITE FAILURE\n");
      if( pvm )
	pvm_task_failure();
    }
#endif
    PRINT_LOG0("                    ******** TOTAL FAILURE ************\n");
    PRINT_LOG0("\n");
    
#ifdef STATISTICS1
    disp_statistics(0);
#ifdef DEREF_STATISTICS
    print_deref_statistics();
#endif
#endif
    break;
    
    
  case error:
    PRINT_LOG0("                    ********E R R O R**********\n");
    PRINT_LOG0("\n");
    
#ifdef STATISTICS1
    disp_statistics(0);
#ifdef DEREF_STATISTICS
    print_deref_statistics();
#endif
#endif
    break;
    
    
  case boundfail:
    PRINT_LOG0("                    ******** BOUND FAILURE ************\n");
    PRINT_LOG0("\n");
    
#ifdef STATPART
    if( recomp )
      PRINT_RES0("  BOUND FAILURE\n");
#endif
#ifdef STATISTICS1
    disp_statistics(0);
#ifdef STATPART
    sps_output();
#endif
#ifdef DEREF_STATISTICS
    print_deref_statistics();
#endif
#endif
    break;
    
    
  default:
    break;
    
  } /* switch (retval) */
  
  
  /* close the output file, if necessary */
  if (outputfile != stdout) {
    fclose(outputfile);
  }

  if (print_all_trees) {
    fclose(ftree);
  }
  
#ifdef STATPART  /* Close output files */
  if (print_subgs) /* iAND */ 
    {
      fprintf(subgsfile, "\n# Number of or-nodes: %d\t\tNumber of subgoals: %d\n",
	      ornode_counter, num_of_subgs);
      fclose(subgsfile);
    }
  
  if (recomp )
    fclose(resfile);
  
  if( iand_check && ! cnt_indp_grp ){
    fclose(logfile);
    printf("NO real iAND-tasks: terminating!!\n");
    system(rmcommand);
    fclose(taskfile);
    sprintf(rmcommand, "rm %s.sp", lopfilename);
    system(rmcommand);
  }
#endif
#ifdef PVM
  if( pvm ){
    pvm_exit();
  }
#endif
#ifdef ILF
  fprintf(stderr,"slave: tid %x exiting from pvm.\n",mytid);
  pvm_exit(); 
#endif
  exit (retval);
}
