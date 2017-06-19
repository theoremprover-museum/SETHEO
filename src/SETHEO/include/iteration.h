#define  NONE     0
#define  DEPTH    1
#define  INF      2
#define  LOCINF   3
#define  FVARS    4
#define  TC       5
#define  SGS      6
#define  SIG      7

#ifdef STATPART
#define GET_INFERENCES_AND_FAILS                                            \
                                                                            \
    /* do not count default query */                                        \
    total_inf -= 1;                                                         \
    global_tot_inf += total_inf;                                            \
    lfails = tc_fails + sgs_fails + fvars_fails +                           \
/* #ifdef CONSTRAINTS */                                                    \
	     c_reg_fails + c_ts_fails + c_anl_fails +                       \
/* #endif */                                                                \
             unif_fails + depth_fails + inf_fails + local_inf_fails;        \
    lbfails = depth_fails + inf_fails + local_inf_fails;
#endif

#ifndef STATPART
#define GET_INFERENCES_AND_FAILS                                            \
                                                                            \
    /* do not count default query */                                        \
    total_inf -= 1;                                                         \
    global_tot_inf += total_inf;                                            \
    lfails = tc_fails + sgs_fails + fvars_fails +                           \
/* #ifdef CONSTRAINTS */                                                    \
	     c_reg_fails + c_ts_fails + c_anl_fails +                       \
/* #endif */                                                                \
             unif_fails + depth_fails + inf_fails + local_inf_fails; 
#endif


#ifdef STATPART
#define ITERATION_STATISTICS                                                \
                                                                            \
    times (&buffer);                                                        \
    if ((((float)(buffer.tms_utime - ti) / (float)CLK_TCK) - run_time) < 0.01)\
        PRINT_LOG0("time     < 0.01 sec  ")                                 \
    else                                                                    \
        PRINT_LOG1("time = %8.2f sec  ",                                    \
	           ((float)(buffer.tms_utime - ti) / (float)CLK_TCK) - run_time)\
    run_time = ((float)(buffer.tms_utime - ti) / (float)CLK_TCK);           \
    GET_INFERENCES_AND_FAILS;                                               \
    PRINT_LOG1 ("inferences = %8d  ", total_inf);                           \
    PRINT_LOG1 ("fails = %8d", lfails - fails);                             \
       if( recomp /*&& !pvm*/ ){                                            \
           fprintf(resfile," %10d %10d %8d",                                \
	                     total_inf,                                     \
			     lfails - fails,                  \
	                     lbfails - bfails);     \
           }                                                                \
       if( recomp ) /*&& !t_proving)   at end of recomputation */           \
           t_proving--;                                                     \
    fails = lfails;                                                         \
    bfails = lbfails;                                                       \
    PRINT_LOG0("\n")
#endif

#ifndef STATPART
#define ITERATION_STATISTICS                                                \
                                                                            \
    times (&buffer);                                                        \
    if ((((float)(buffer.tms_utime - ti) / (float)CLK_TCK) - run_time) < 0.01)\
        PRINT_LOG0("time     < 0.01 sec  ")                                 \
    else                                                                    \
        PRINT_LOG1("time = %8.2f sec  ",                                    \
	           ((float)(buffer.tms_utime - ti) / (float)CLK_TCK) - run_time)\
    run_time = ((float)(buffer.tms_utime - ti) / (float)CLK_TCK);           \
    GET_INFERENCES_AND_FAILS;                                               \
    PRINT_LOG1 ("inferences = %8d  ", total_inf);                           \
    PRINT_LOG1 ("fails = %8d", lfails - fails);                             \
    fails = lfails;                                                         \
    PRINT_LOG0("\n")
#endif

















