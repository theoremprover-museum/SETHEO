#ifndef _SAM_PROTOTYPES
#define _SAM_PROTOTYPES

/*************************************************************/
/* helpful stuff for compatible prototypes                   */
/*************************************************************/
#ifndef _ANSI_ARGS_
#if ((defined(__STDC__) || defined(SABER)) && !defined(NO_PROTOTYPE)) || defined(__cplusplus) || defined(USE_PROTOTYPE)
#   define _USING_PROTOTYPES_ 1
#   define _ANSI_ARGS_(x)       x
#else
#   define _ANSI_ARGS_(x)       ()
#endif
#endif

/* %%%%%%%%%%%%%%% anl_constr.c %%%%%%%%%%%%%%% */

int ac_gen_constr _ANSI_ARGS_((void));
int ac_create_anl_constr _ANSI_ARGS_((void));

/* %%%%%%%%%%%%%%% antilemma.c %%%%%%%%%%%%%%% */

choice_pt  *immediate_antilemma_fail _ANSI_ARGS_((void));

int  genantilemma _ANSI_ARGS_((choice_pt  *anl_chp));
void  free_anl_list _ANSI_ARGS_((a_list  *list));
int  anl_gen_constr _ANSI_ARGS_((void)); 
void  delay_anl_constr _ANSI_ARGS_((c_counter  *constr,int bound));

/* %%%%%%%%%%%%%%% bounds.c %%%%%%%%%%%%%%% */
int  look_ahead_for_bound_fail _ANSI_ARGS_((void));

int  bound_fail _ANSI_ARGS_((int   depth_value,int   inf_value,int   old_inf_value,int   local_inf_value,int   sgs_value));
int  fvars_bound_fail _ANSI_ARGS_((int   fvars_value));
int  tc_bound_fail _ANSI_ARGS_((int   tc_value));
int  sig_bound_fail _ANSI_ARGS_((int   sig_value));
void  init_bounds _ANSI_ARGS_((int  tc_init,int  sgs_init,int  fvars_init,int  sig_init));
/* %%%%%%%%%%%%%%% cmdline.c %%%%%%%%%%%%%%% */
int parsearg _ANSI_ARGS_((char **argvp[],int *argcp,int *intvalp,char **stringval,int *negated,char *negstring));
void  usage _ANSI_ARGS_((FILE 	*fp,char	*av0,char	*trailer));
/* %%%%%%%%%%%%%%% debug.c %%%%%%%%%%%%%%% */
int dispreg  _ANSI_ARGS_((void));
void dispchoice _ANSI_ARGS_((choice_pt      *ch,environ        *bptr));
void disp_obj _ANSI_ARGS_((WORD           *op));
void  disp_word _ANSI_ARGS_((WORD           *obj));
void disp_chp _ANSI_ARGS_((choice_pt      *ch));
void prt_clause _ANSI_ARGS_((WORD            a));
WORD *disassemble_instruction _ANSI_ARGS_((FILE *fout,WORD *cp));
void disassemble_word _ANSI_ARGS_((FILE		   *fout,WORD           *obj));
/* %%%%%%%%%%%%%%% deref.c %%%%%%%%%%%%%%% */
WORD *deref _ANSI_ARGS_((register WORD  *obj,environ        *lbp));
WORD *ref_head _ANSI_ARGS_((WORD           *obj));
int trail_var _ANSI_ARGS_((WORD   *obj));
int trail_lint _ANSI_ARGS_((long int *obj));
int trail_int _ANSI_ARGS_((int *obj));
int trail_ptr _ANSI_ARGS_((void **obj));
void untrail _ANSI_ARGS_((s_trail        *tpt));

#ifdef DEREF_STATISTICS
void print_deref_statistics _ANSI_ARGS_((void));
#endif

/* %%%%%%%%%%%%%%% disp.c %%%%%%%%%%%%%%% */
void  disp_ _ANSI_ARGS_((FILE           *fout,WORD           *obj,environ        *locbp));
/* %%%%%%%%%%%%%%% dynsgreord.c %%%%%%%%%%%%%%% */
int  select_subgoal _ANSI_ARGS_((int  delay_mode));
void  delay_subgoal _ANSI_ARGS_((void)); 
void  dag_term_check _ANSI_ARGS_((WORD *ga,int arity,int ground_required,sg_result *res));

/* %%%%%%%%%%%%%%% errors.c %%%%%%%%%%%%%%% */
void  sam_error _ANSI_ARGS_((char *s,WORD           *obj,int             severe));
/* %%%%%%%%%%%%%%% full.c %%%%%%%%%%%%%%% */
int full_unify _ANSI_ARGS_((WORD *ga,WORD *ha,environ *bpgoal,environ *bphead));

#ifdef CONSTRAINTS
int full_proto_unify _ANSI_ARGS_((WORD           *ga,WORD           *ha,environ        *bpgoal,environ        *bphead));
#endif

int             occ_ck _ANSI_ARGS_((WORD           *vp,WORD           *st,environ        *base));
/* %%%%%%%%%%%%%%% fvars.c %%%%%%%%%%%%%%% */
WORD  *get_free_variables _ANSI_ARGS_((register choice_pt 	*lcp));
void  mark_irrelevant_fvars _ANSI_ARGS_((register choice_pt    *lcp));
/* %%%%%%%%%%%%%%% genlemma.c %%%%%%%%%%%%%%% */
int head_is_unit_lemma _ANSI_ARGS_((void));
int more_general_in_index _ANSI_ARGS_((WORD *ga));
choice_pt * delete_instances_in_index _ANSI_ARGS_((WORD *ga));
void unitlemma_statistics_init _ANSI_ARGS_((void));
void  unitlemma_statistics_print _ANSI_ARGS_((void));

/* %%%%%%%%%%%%%%% i_add.c %%%%%%%%%%%%%%% */
instr_result i_add _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_addtoindex.c %%%%%%%%%%%%%%% */
instr_result i_addtoindex _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_addtoset.c %%%%%%%%%%%%%%% */
instr_result i_addtoset _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_alloc.c %%%%%%%%%%%%%%% */
instr_result i_alloc _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_allocg.c %%%%%%%%%%%%%%% */
instr_result i_allocg _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_arg.c %%%%%%%%%%%%%%% */
instr_result i_arg _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_argsize.c %%%%%%%%%%%%%%% */
instr_result i_argsize _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_array_assign.c %%%%%%%%%%%%%%% */
instr_result i_array_assign _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_array_glod.c %%%%%%%%%%%%%%% */
instr_result i_array_glod _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_array_gsto.c %%%%%%%%%%%%%%% */
instr_result i_array_gsto _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_assign.c %%%%%%%%%%%%%%% */
instr_result i_assign _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_assume.c %%%%%%%%%%%%%%% */
instr_result i_assume _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_break.c %%%%%%%%%%%%%%% */
instr_result i_break _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_call.c %%%%%%%%%%%%%%% */
instr_result i_call _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_card.c %%%%%%%%%%%%%%% */
instr_result i_card _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_cgen.c %%%%%%%%%%%%%%% */
instr_result i_cgen _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_checkindex.c %%%%%%%%%%%%%%% */
instr_result i_checkindex _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_checklemma.c %%%%%%%%%%%%%%% */
instr_result i_checklemma _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_checkulemma.c %%%%%%%%%%%%%%% */
instr_result i_checkulemma _ANSI_ARGS_((void));
instr_result i_cut _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_dealloc.c %%%%%%%%%%%%%%% */
instr_result i_dealloc _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_delrange.c %%%%%%%%%%%%%%% */
instr_result i_delrange _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_div.c %%%%%%%%%%%%%%% */
instr_result i_div _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_dtree.c %%%%%%%%%%%%%%% */
instr_result i_dtree _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_dumplemma.c %%%%%%%%%%%%%%% */
instr_result i_dumplemma _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_eq_built.c %%%%%%%%%%%%%%% */
instr_result i_eq_built _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_eqpred.c %%%%%%%%%%%%%%% */
instr_result i_eqpred _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_equ_unif.c %%%%%%%%%%%%%%% */
instr_result i_equ_unif _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_fail.c %%%%%%%%%%%%%%% */
instr_result i_fail _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_fold_up.c %%%%%%%%%%%%%%% */
instr_result i_fold_up _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_functor.c %%%%%%%%%%%%%%% */
instr_result i_functor _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_galloc.c %%%%%%%%%%%%%%% */
instr_result i_galloc _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_genlemma.c %%%%%%%%%%%%%%% */
instr_result i_genlemma _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_genreg.c %%%%%%%%%%%%%%% */
instr_result i_genreg _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_genulemma.c %%%%%%%%%%%%%%% */
instr_result i_genulemma _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_genureg.c %%%%%%%%%%%%%%% */
instr_result i_genureg _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_genxreg.c %%%%%%%%%%%%%%% */
instr_result i_genxreg _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_get_maxfvars.c %%%%%%%%%%%%%%% */
instr_result i_get_maxfvars _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_get_maxsgs.c %%%%%%%%%%%%%%% */
instr_result i_get_maxsgs _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_get_maxtc.c %%%%%%%%%%%%%%% */
instr_result i_get_maxtc _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_getcounter.c %%%%%%%%%%%%%%% */
instr_result i_getcounter _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_getdepth.c %%%%%%%%%%%%%%% */
instr_result i_getdepth _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_getinf.c %%%%%%%%%%%%%%% */
instr_result i_getinf _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_getlocinf.c %%%%%%%%%%%%%%% */
instr_result i_getlocinf _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_getmaxinf.c %%%%%%%%%%%%%%% */
instr_result i_getmaxinf _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_getnrlemmata.c %%%%%%%%%%%%%%% */
instr_result i_getnrlemmata _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_glod.c %%%%%%%%%%%%%%% */
instr_result i_glod _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_gsto.c %%%%%%%%%%%%%%% */
instr_result i_gsto _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_init_counters.c %%%%%%%%%%%%%%% */
instr_result i_init_counters _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_is_compl.c %%%%%%%%%%%%%%% */
instr_result i_is_compl _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_is_const.c %%%%%%%%%%%%%%% */
instr_result i_is_const _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_is_nonvar.c %%%%%%%%%%%%%%% */
instr_result i_is_nonvar _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_is_number.c %%%%%%%%%%%%%%% */
instr_result i_is_number _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_is_var.c %%%%%%%%%%%%%%% */
instr_result i_is_var _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_isempty.c %%%%%%%%%%%%%%% */
instr_result i_isempty _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_ismember.c %%%%%%%%%%%%%%% */
instr_result i_ismember _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_isnotunifiable.c %%%%%%%%%%%%%%% */
instr_result i_isnotunifiable _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_issubset.c %%%%%%%%%%%%%%% */
instr_result i_issubset _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_isunifiable.c %%%%%%%%%%%%%%% */
instr_result i_isunifiable _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_jmp.c %%%%%%%%%%%%%%% */
instr_result i_jmp _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_jmpg.c %%%%%%%%%%%%%%% */
instr_result i_jmpg _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_jmpz.c %%%%%%%%%%%%%%% */
instr_result i_jmpz _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_kbo.c %%%%%%%%%%%%%%% */
instr_result i_kbo _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_lconst.c %%%%%%%%%%%%%%% */
instr_result i_lconst _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_lod.c %%%%%%%%%%%%%%% */
instr_result i_lod _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_lpo.c %%%%%%%%%%%%%%% */
instr_result i_lpo _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_mkempty.c %%%%%%%%%%%%%%% */
instr_result i_mkempty _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_mul.c %%%%%%%%%%%%%%% */
instr_result i_mul _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_nalloc.c %%%%%%%%%%%%%%% */
instr_result i_nalloc _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_ndealloc.c %%%%%%%%%%%%%%% */
instr_result i_ndealloc _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_negate.c %%%%%%%%%%%%%%% */
/* %%%%%%%%%%%%%%% i_neq_built.c %%%%%%%%%%%%%%% */
instr_result i_neq_built _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_orbranch.c %%%%%%%%%%%%%%% */
instr_result i_orbranch _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_out.c %%%%%%%%%%%%%%% */
instr_result i_out _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_path2list.c %%%%%%%%%%%%%%% */
instr_result i_path2list _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_porbranch.c %%%%%%%%%%%%%%% */
instr_result i_porbranch _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_postunify.c %%%%%%%%%%%%%%% */
instr_result i_postunify _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_pre_cut.c %%%%%%%%%%%%%%% */
instr_result i_pre_cut _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_precall.c %%%%%%%%%%%%%%% */
instr_result i_precall _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_preunify.c %%%%%%%%%%%%%%% */
instr_result i_preunify _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_proceed.c %%%%%%%%%%%%%%% */
instr_result i_proceed _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_ptree.c %%%%%%%%%%%%%%% */
instr_result i_ptree _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_rand_reorder.c %%%%%%%%%%%%%%% */
instr_result i_rand_reorder _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_reduct.c %%%%%%%%%%%%%%% */
instr_result i_reduct _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_reg_fold_up.c %%%%%%%%%%%%%%% */
instr_result i_reg_fold_up _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_remfromset.c %%%%%%%%%%%%%%% */
instr_result i_remfromset _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_selnext.c %%%%%%%%%%%%%%% */
instr_result i_selnext _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_selnth.c %%%%%%%%%%%%%%% */
instr_result i_selnth _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_set_counter.c %%%%%%%%%%%%%%% */
instr_result i_set_counter _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_set_depth.c %%%%%%%%%%%%%%% */
instr_result i_set_depth _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_set_inf.c %%%%%%%%%%%%%%% */
instr_result i_set_inf _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_set_locinf.c %%%%%%%%%%%%%%% */
instr_result i_set_locinf _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_set_maxfvars.c %%%%%%%%%%%%%%% */
instr_result i_set_maxfvars _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_set_maxinf.c %%%%%%%%%%%%%%% */
instr_result i_set_maxinf _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_set_maxsgs.c %%%%%%%%%%%%%%% */
instr_result i_set_maxsgs _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_set_maxtc.c %%%%%%%%%%%%%%% */
instr_result i_set_maxtc _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_setrand.c %%%%%%%%%%%%%%% */
instr_result i_setrand _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_sig.c %%%%%%%%%%%%%%% */
instr_result i_sig _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_size.c %%%%%%%%%%%%%%% */
instr_result i_size _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_size_struct.c %%%%%%%%%%%%%%% */
instr_result i_size_struct _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_sto.c %%%%%%%%%%%%%%% */
instr_result i_sto _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_stop.c %%%%%%%%%%%%%%% */
instr_result i_stop _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_sub.c %%%%%%%%%%%%%%% */
instr_result i_sub _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_tdepth.c %%%%%%%%%%%%%%% */
instr_result i_tdepth _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_tell.c %%%%%%%%%%%%%%% */
instr_result i_tell _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_told.c %%%%%%%%%%%%%%% */
instr_result i_told _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_unconst.c %%%%%%%%%%%%%%% */
instr_result i_unconst _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_unglobl.c %%%%%%%%%%%%%%% */
instr_result i_unglobl _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_ungterm.c %%%%%%%%%%%%%%% */
instr_result i_ungterm _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_unngterm.c %%%%%%%%%%%%%%% */
instr_result i_unngterm _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_unvar.c %%%%%%%%%%%%%%% */
instr_result i_unvar _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_unvoid.c %%%%%%%%%%%%%%% */
instr_result i_unvoid _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% i_ureduct.c %%%%%%%%%%%%%%% */
#ifdef FOLD_UP
instr_result i_ureduct _ANSI_ARGS_((void));
#endif
/* %%%%%%%%%%%%%%% i_uselemma.c %%%%%%%%%%%%%%% */
instr_result i_uselemma _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% init.c %%%%%%%%%%%%%%% */
void  init_reg  _ANSI_ARGS_((void));
void alloc_code _ANSI_ARGS_((int codesize));
void alloc_symbtab _ANSI_ARGS_((int size));
/* %%%%%%%%%%%%%%% instr.c %%%%%%%%%%%%%%% */
instr_result instr_cycle  _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% itime.c %%%%%%%%%%%%%%% */
int  instr_time _ANSI_ARGS_((int iterative_bounding,int min_d,int d_inc,int min_loc_inf,int loc_inf_inc,int min_inf,int inf_inc));
void  prepare_statistics _ANSI_ARGS_((void));
void  disp_statistics _ANSI_ARGS_((int print_time));
/* %%%%%%%%%%%%%%% lcode.c %%%%%%%%%%%%%%% */
void  load_code _ANSI_ARGS_((FILE *codefile,int            default_codesize,int            default_symb_size));
/* %%%%%%%%%%%%%%% match.c %%%%%%%%%%%%%%% */
int match _ANSI_ARGS_((WORD* ga,environ *bp_ga,WORD *ha,environ *bp_ha));
/* %%%%%%%%%%%%%%% ptree.c %%%%%%%%%%%%%%% */
void print_factorization_info2 _ANSI_ARGS_((FILE 		*filep,environ 	*bpto,environ 	*bpfrom,WORD		*sg_ptr));
void print_factorization_info _ANSI_ARGS_((FILE 		*filep,environ 	*bpto,environ 	*bpfrom,WORD		*sg_ptr));
void print_reduction_info _ANSI_ARGS_((FILE 		*filep,environ 	*bpto,environ 	*bpfrom,WORD		*sg_ptr));
void print_lit_info _ANSI_ARGS_((FILE		*filep,environ 	*bp,environ 	*bp_from,WORD		*sg_ptr));
void print_literal _ANSI_ARGS_((FILE		*filep,environ 	*bp,int		negated));
void disp_proof_tree _ANSI_ARGS_((FILE *filep,environ *bp,int indent));
/* %%%%%%%%%%%%%%% reg_constr.c %%%%%%%%%%%%%%% */
int rc_gen_constr _ANSI_ARGS_((WORD    rc_predicate,WORD   *goal_p));
#ifdef FOLD_UP
int rc_genx_constr _ANSI_ARGS_((WORD    rc_predicate,WORD   *goal_p));
int rc_genu_constr _ANSI_ARGS_((WORD    rc_predicate,WORD   *goal_p));
int rc_genu_help _ANSI_ARGS_((WORD    rc_predicate,WORD   *goal_p,environ  *rc_bp));
#endif
int rc_gen_decide _ANSI_ARGS_((WORD *rc_gp,environ *rc_bp,WORD rc_predicate,WORD *goal_p,int nargs));
/* %%%%%%%%%%%%%%% saved_instr.c %%%%%%%%%%%%%%% */

/* %%%%%%%%%%%%%%% signals.c %%%%%%%%%%%%%%% */
void sighand  _ANSI_ARGS_((void));
void sig_alrm _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% taut_constr.c %%%%%%%%%%%%%%% */
#ifdef CONSTRAINTS
int ta_gen_constr _ANSI_ARGS_((WORD   *gp1,WORD   *gp2));
#endif
/* %%%%%%%%%%%%%%% termcomplexity.c %%%%%%%%%%%%%%% */
int  tc_of_all_open_subgoals _ANSI_ARGS_((void));
/* %%%%%%%%%%%%%%% unifeq.c %%%%%%%%%%%%%%% */
int un_eq _ANSI_ARGS_((environ *lbp, int predsymb, int nargs));
int chk_arg _ANSI_ARGS_((WORD *lhp,environ *gbp,WORD *lgp,environ *lbp));
/* %%%%%%%%%%%%%%% unifred.c %%%%%%%%%%%%%%% */
int un_red _ANSI_ARGS_((environ         *lbp,long            predsymb,int             nargs));
int un_factor _ANSI_ARGS_((environ            *lbp,literal_ctrl_block *lcbptr,long               predsymb,int           nargs));
/* %%%%%%%%%%%%%%% unifyt.c %%%%%%%%%%%%%%% */
WORD *copy_struct _ANSI_ARGS_((WORD           *obj,environ        *bp,WORD	       *var));
/* %%%%%%%%%%%%%%% univ_constr.c %%%%%%%%%%%%%%% */

#ifdef CONSTRAINTS
void  pu_untrail _ANSI_ARGS_((void));              
c_counter *c_gencounter _ANSI_ARGS_((int         tag,choice_pt  *ch,int         infc));
int c_trail _ANSI_ARGS_((WORD *obj));
int un_protoco _ANSI_ARGS_((WORD *gp1,environ *lbp1,WORD *gp2,environ *lbp2,int nargs));
int c_gen_optimize _ANSI_ARGS_((c_counter *zaehler));
int  c_create _ANSI_ARGS_((WORD       *left_gp,WORD       *right_gp,c_counter  *zaehler));
int c_check _ANSI_ARGS_((WORD         *var,WORD         *right_gp));
#endif
/* %%%%%%%%%%%%%%% weighted_depth.c %%%%%%%%%%%%%%% */
int  free_depth _ANSI_ARGS_((int    actual_depth,int    delta_inf));
int  weighted_depth _ANSI_ARGS_((int    inferences));
int  delta_depth _ANSI_ARGS_((int    delta_inf,int    delta_depth));

#endif
