/* 
 * wasm-prototypes.h
 */

/*
 * This file contains all prototypes for all non-static functions
 * of wasm
 */

#ifndef _WASM_PROTO_H_
#define _WASM_PROTO_H_

/* the following code has been taken from Tcl 7.5 */

/*
 * Definitions that allow this header file to be used either with or
 * without ANSI C features like function prototypes.
 */

#undef _ANSI_ARGS_
#undef CONST

#if ((defined(__STDC__) || defined(SABER)) && !defined(NO_PROTOTYPE)) || defined(__cplusplus) || defined(USE_PROTOTYPE)
#   define _USING_PROTOTYPES_ 1
#   define _ANSI_ARGS_(x)       x
#   define CONST const
#else
#   define _ANSI_ARGS_(x)       ()
#   define CONST
#endif

/* parser */
int yyparse          _ANSI_ARGS_((void));

/* error.c*/
void warning         _ANSI_ARGS_((char *s));
void nferror         _ANSI_ARGS_((char *s));
void fatal           _ANSI_ARGS_((char *s,int exitval));
void prttoken        _ANSI_ARGS_((char *token));
void pass2error      _ANSI_ARGS_((char *s1,char *s2));

/* geninstr.c */
void put_instr       _ANSI_ARGS_((int opcode,int pc));
void put_oper        _ANSI_ARGS_((int val,short tag,int pc));
void enter_start     _ANSI_ARGS_((int pc));
void put_tag_word    _ANSI_ARGS_((int pc,int val,int tag,int hosttag));
void word            _ANSI_ARGS_((int pc,int val));
void end_code        _ANSI_ARGS_((void));
void start_code      _ANSI_ARGS_((void));
void codestr         _ANSI_ARGS_((char *s2,char *s1));
void put_symbol_code _ANSI_ARGS_((int value1,int value2,char *token));
void put_clause_code _ANSI_ARGS_((int clause_nr,int lit_nr));
void put_reduct_code _ANSI_ARGS_((int pred_nr));

/* opt.c */
void pnam           _ANSI_ARGS_((int val,int tag));
void plab           _ANSI_ARGS_((int val,int tag));
void optim_out      _ANSI_ARGS_((void));
void enter_opt      _ANSI_ARGS_((int val,int tag));

/* optim.c */
void init_optim     _ANSI_ARGS_((void));
void enter_o_list   _ANSI_ARGS_((int el,int tag));
void o_name         _ANSI_ARGS_((int name,int tag));
void list2tree      _ANSI_ARGS_((void));
void print_dtree    _ANSI_ARGS_((void (*pna) (),void (*plab) ()));

/* symbtab.c */
void init_symbtab   _ANSI_ARGS_((void));
int  putdef         _ANSI_ARGS_((char *s,int mode,int value));
int  getdef         _ANSI_ARGS_((char *s,int mode,int *defval));
void start_proc     _ANSI_ARGS_((void));
void enter_nodef    _ANSI_ARGS_((int val,int pc,long lpos,int hosttag));
int  retrieve_undef _ANSI_ARGS_((int *valp,int *pcp,long *lposp,int *hosttagp));
void syprint        _ANSI_ARGS_((int val));
void prt_ref        _ANSI_ARGS_((FILE *fout,int type));
int  pdef           _ANSI_ARGS_((int index,int value));



#endif
