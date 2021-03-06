/**************************************************/
/* opcodes.h:  -- do not edit by hand             */
/**************************************************/

#define OPCODE_alloc		0
#define OPCODE_nalloc		1
#define OPCODE_dealloc		2
#define OPCODE_ndealloc		3
#define OPCODE_fold_up		4
#define OPCODE_reg_fold_up		5
#define OPCODE_call		6
#define OPCODE_stop		7
#define OPCODE_break		8
#define OPCODE_jmp		9
#define OPCODE_jmpz		10
#define OPCODE_jmpg		11
#define OPCODE_unvoid		12
#define OPCODE_unconst		13
#define OPCODE_unvar		14
#define OPCODE_ungterm		15
#define OPCODE_unngterm		16
#define OPCODE_unglobl		17
#define OPCODE_orbranch		18
#define OPCODE_porbranch		19
#define OPCODE_fail		20
#define OPCODE_reduct		21
#define OPCODE_ureduct		22
#define OPCODE_proceed		23
#define OPCODE_add		24
#define OPCODE_mul		25
#define OPCODE_sub		26
#define OPCODE_div		27
#define OPCODE_lod		28
#define OPCODE_sto		29
#define OPCODE_glod		30
#define OPCODE_gsto		31
#define OPCODE_lconst		32
#define OPCODE_assign		33
#define OPCODE_genreg		34
#define OPCODE_genxreg		35
#define OPCODE_genureg		36
#define OPCODE_cgen		37
#define OPCODE_printtree		38
#define OPCODE_out		39
#define OPCODE_tell		40
#define OPCODE_told		41
#define OPCODE_eqpred		42
#define OPCODE_eq_built		43
#define OPCODE_neq_built		44
#define OPCODE_equ_unif		45
#define OPCODE_is_unifiable		46
#define OPCODE_is_notunifiable		47
#define OPCODE_pre_cut		48
#define OPCODE_cut		49
#define OPCODE_is_var		50
#define OPCODE_isnon_var		51
#define OPCODE_is_number		52
#define OPCODE_is_const		53
#define OPCODE_is_compl		54
#define OPCODE_rand_reorder		55
#define OPCODE_setrand		56
#define OPCODE_galloc		57
#define OPCODE_size		58
#define OPCODE_tdepth		59
#define OPCODE_tdepth3		60
#define OPCODE_getdepth		61
#define OPCODE_getinf		62
#define OPCODE_getmaxinf		63
#define OPCODE_getlocinf		64
#define OPCODE_set_depth		65
#define OPCODE_set_inf		66
#define OPCODE_set_maxinf		67
#define OPCODE_set_locinf		68
#define OPCODE_functor		69
#define OPCODE_arg		70
#define OPCODE_genlemma		71
#define OPCODE_genulemma		72
#define OPCODE_checklemma		73
#define OPCODE_checkulemma		74
#define OPCODE_addtoindex		75
#define OPCODE_checkindex		76
#define OPCODE_uselemma		77
#define OPCODE_dumplemma		78
#define OPCODE_getnrlemmata		79
#define OPCODE_delrange		80
#define OPCODE_path2list		81
#define OPCODE_assume		82
#define OPCODE_init_counters		83
#define OPCODE_getcounter		84
#define OPCODE_set_counter		85
#define OPCODE_get_maxfvars		86
#define OPCODE_set_maxfvars		87
#define OPCODE_get_maxtc		88
#define OPCODE_set_maxtc		89
#define OPCODE_get_maxsgs		90
#define OPCODE_set_maxsgs		91
#define OPCODE_lpo		92
#define OPCODE_lpoeq		93
#define OPCODE_kbo		94
#define OPCODE_sig		95
#define OPCODE_precall		96
#define OPCODE_isempty		97
#define OPCODE_issubset		98
#define OPCODE_ismember		99
#define OPCODE_card		100
#define OPCODE_mkempty		101
#define OPCODE_addtoset		102
#define OPCODE_remfromset		103
#define OPCODE_selnth		104
#define OPCODE_selnext		105
#define OPCODE_postunify		106
#define OPCODE_argsize		107
#define OPCODE_preunify		108
#define OPCODE_array_assign		109
#define OPCODE_array_glod		110
#define OPCODE_array_gsto		111
