/**************************************************/
/* w_parse2.h:  -- do not edit by hand             */
/**************************************************/

	 T_i_alloc  expr 
		{ put_instr(0,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_nalloc  expr 
		{ put_instr(1,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_dealloc 
		{ put_instr(2,curr_pc++);
		}
	|  T_i_ndealloc 
		{ put_instr(3,curr_pc++);
		}
	|  T_i_fold_up 
		{ put_instr(4,curr_pc++);
		}
	|  T_i_reg_fold_up 
		{ put_instr(5,curr_pc++);
		}
	|  T_i_call 
		{ put_instr(6,curr_pc++);
		}
	|  T_i_stop 
		{ put_instr(7,curr_pc++);
		}
	|  T_i_break 
		{ put_instr(8,curr_pc++);
		}
	|  T_i_jmp  expr 
		{ put_instr(9,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_jmpz  expr 
		{ put_instr(10,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_jmpg  expr 
		{ put_instr(11,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_unvoid  expr 
		{ put_instr(12,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_unconst  expr 
		{ put_instr(13,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_unvar  expr 
		{ put_instr(14,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_ungterm  expr 
		{ put_instr(15,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_unngterm  expr 
		{ put_instr(16,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_unglobl  expr 
		{ put_instr(17,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_orbranch  expr 
		{ put_instr(18,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_porbranch  expr ','  expr ','  expr 
		{ put_instr(19,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		put_oper($4.val,$4.tag,curr_pc++);
		put_oper($6.val,$6.tag,curr_pc++);
		}
	|  T_i_fail 
		{ put_instr(20,curr_pc++);
		}
	|  T_i_reduct  expr ','  expr 
		{ put_instr(21,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		put_oper($4.val,$4.tag,curr_pc++);
		}
	|  T_i_ureduct  expr ','  expr 
		{ put_instr(22,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		put_oper($4.val,$4.tag,curr_pc++);
		}
	|  T_i_proceed 
		{ put_instr(23,curr_pc++);
		}
	|  T_i_add 
		{ put_instr(24,curr_pc++);
		}
	|  T_i_mul 
		{ put_instr(25,curr_pc++);
		}
	|  T_i_sub 
		{ put_instr(26,curr_pc++);
		}
	|  T_i_div 
		{ put_instr(27,curr_pc++);
		}
	|  T_i_lod  expr 
		{ put_instr(28,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_sto  expr 
		{ put_instr(29,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_glod  expr 
		{ put_instr(30,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_gsto  expr 
		{ put_instr(31,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_lconst  expr 
		{ put_instr(32,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_assign  expr ','  expr 
		{ put_instr(33,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		put_oper($4.val,$4.tag,curr_pc++);
		}
	|  T_i_genreg  expr 
		{ put_instr(34,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_genxreg  expr 
		{ put_instr(35,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_genureg  expr 
		{ put_instr(36,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_cgen  expr 
		{ put_instr(37,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_ptree 
		{ put_instr(38,curr_pc++);
		}
	|  T_i_out  expr 
		{ put_instr(39,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_tell  expr 
		{ put_instr(40,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_told 
		{ put_instr(41,curr_pc++);
		}
	|  T_i_eqpred  expr ','  expr ','  expr 
		{ put_instr(42,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		put_oper($4.val,$4.tag,curr_pc++);
		put_oper($6.val,$6.tag,curr_pc++);
		}
	|  T_i_eq_built  expr 
		{ put_instr(43,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_neq_built  expr 
		{ put_instr(44,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_equ_unif  expr 
		{ put_instr(45,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_isunifiable  expr 
		{ put_instr(46,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_isnotunifiable  expr 
		{ put_instr(47,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_pre_cut 
		{ put_instr(48,curr_pc++);
		}
	|  T_i_cut 
		{ put_instr(49,curr_pc++);
		}
	|  T_i_is_var  expr 
		{ put_instr(50,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_is_nonvar  expr 
		{ put_instr(51,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_is_number  expr 
		{ put_instr(52,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_is_const  expr 
		{ put_instr(53,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_is_compl  expr 
		{ put_instr(54,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_rand_reorder 
		{ put_instr(55,curr_pc++);
		}
	|  T_i_setrand  expr 
		{ put_instr(56,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_galloc  expr 
		{ put_instr(57,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_size  expr 
		{ put_instr(58,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_tdepth  expr 
		{ put_instr(59,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_tdepth3  expr 
		{ put_instr(60,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_getdepth  expr 
		{ put_instr(61,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_getinf  expr 
		{ put_instr(62,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_getmaxinf  expr 
		{ put_instr(63,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_getlocinf  expr 
		{ put_instr(64,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_set_depth  expr 
		{ put_instr(65,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_set_inf  expr 
		{ put_instr(66,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_set_maxinf  expr 
		{ put_instr(67,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_set_locinf  expr 
		{ put_instr(68,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_functor  expr 
		{ put_instr(69,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_arg  expr 
		{ put_instr(70,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_genlemma  expr 
		{ put_instr(71,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_genulemma  expr ','  expr 
		{ put_instr(72,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		put_oper($4.val,$4.tag,curr_pc++);
		}
	|  T_i_checklemma  expr 
		{ put_instr(73,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_checkulemma  expr ','  expr 
		{ put_instr(74,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		put_oper($4.val,$4.tag,curr_pc++);
		}
	|  T_i_addtoindex  expr 
		{ put_instr(75,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_checkindex  expr 
		{ put_instr(76,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_uselemma  expr 
		{ put_instr(77,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_dumplemma 
		{ put_instr(78,curr_pc++);
		}
	|  T_i_getnrlemmata  expr 
		{ put_instr(79,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_delrange  expr 
		{ put_instr(80,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_path2list  expr 
		{ put_instr(81,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_assume 
		{ put_instr(82,curr_pc++);
		}
	|  T_i_init_counters  expr 
		{ put_instr(83,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_getcounter  expr 
		{ put_instr(84,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_set_counter  expr 
		{ put_instr(85,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_get_maxfvars  expr 
		{ put_instr(86,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_set_maxfvars  expr 
		{ put_instr(87,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_get_maxtc  expr 
		{ put_instr(88,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_set_maxtc  expr 
		{ put_instr(89,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_get_maxsgs  expr 
		{ put_instr(90,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_set_maxsgs  expr 
		{ put_instr(91,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_lpo  expr 
		{ put_instr(92,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_lpoeq  expr 
		{ put_instr(93,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_kbo  expr 
		{ put_instr(94,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_sig  expr 
		{ put_instr(95,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_precall 
		{ put_instr(96,curr_pc++);
		}
	|  T_i_isempty  expr 
		{ put_instr(97,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_issubset  expr 
		{ put_instr(98,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_ismember  expr 
		{ put_instr(99,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_card  expr 
		{ put_instr(100,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_mkempty  expr 
		{ put_instr(101,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_addtoset  expr 
		{ put_instr(102,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_remfromset  expr 
		{ put_instr(103,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_selnth  expr 
		{ put_instr(104,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_selnext  expr 
		{ put_instr(105,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_postunify  expr 
		{ put_instr(106,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_argsize  expr ','  expr 
		{ put_instr(107,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		put_oper($4.val,$4.tag,curr_pc++);
		}
	|  T_i_preunify  expr 
		{ put_instr(108,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_array_assign  expr 
		{ put_instr(109,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_array_glod  expr 
		{ put_instr(110,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
	|  T_i_array_gsto  expr 
		{ put_instr(111,curr_pc++);
		put_oper($2.val,$2.tag,curr_pc++);
		}
