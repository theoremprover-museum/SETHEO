/****************************************************
;;; MODULE NAME : instr.c
;;;
;;; PARENT      : cgen.c
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)instrs.c	18.4  03/12/98
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 15.5.91
;;;
;;; DESCRIPTION :
;;;
;;; REMARKS     :
;;;		19.2.93		jsc	unvoid gets address of variable
;;;		1.3.93		jsc	cgin_fail_label,
;;;		1.3.93		jsc	cgin_fail_clause,
;;;		26.3.93		jsc	changed format for termlabel
;;;		4.6.93	jsc	new structure for built-ins
;;;		1.7.93		jsc	extern.h
;;;		2.7.93		jsc	merged 8.1.1.1 (setsubgoals)
;;;		14.12.93	jsc		purity, fold
;;;		24.3.94		jsc	genreg* arguments
;;;		7.6.94		jsc	T_*TERM are with ID_TAG
;;;					sign
;;;		28.7.94		jsc	interactive, search tree instrs
;;;             9.3.95          jsc     cgin_subgoal_list_dw
;;;             23.6.95          jsc     cgin_loadfcb
;;;             19.1.96          jsc     clean-up
;;;             24.1.96          jsc     hexout
;;;
******************************************************/

/*****************************************************/
/* includes                                          */
/*****************************************************/
#include "types.h"
#include "extern.h"
#include "hexout.h"
#include "opcodes.h"
#include "uopcodes.h"

/*****************************************************/
/* functions (codegen/instrs.c)                      */
/*****************************************************/

/*****************************************************/
/* cgin_init()	                                     */
/*****************************************************/
/* initialise code generation                        */
/*****************************************************/
void cgin_init(reduct,ishorn)
  int reduct,ishorn;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,
	    "/* The time has come, the walross said to talk of many things */\n");
    fprintf(code_file,
	    "/* * * * %s H O R N  * * * * * %s reductionsteps  * * */\n",
	    (ishorn)?"* * ":" non",(reduct)?"* * with":" without");
  }
  else {
    hex_out_init();
  }
}


/*****************************************************/
/* cgin_finish()	                             */
/* finish code generation                            */
/*****************************************************/
void cgin_finish()
{
  if (inwasm_flags.hex_out) {
    hex_out_finish();
  }
}


/*****************************************************/
/* cgin_nextcoment()                                 */
/* generate comment for clauses/modules		     */
/*****************************************************/
void cgin_nextcoment(is_module)
  int is_module;
{
  if (!inwasm_flags.hex_out){
    if (is_module)
      fprintf(code_file,
	      "/* * * * * * * * * *  starting next module  * * * * * * * * * */\n");
    else
      fprintf(code_file,
	      "/* * * * * * * * * *  starting next original clause * * * * * */\n");
  }
}


/*****************************************************/
/* cgin_coment()                                     */
/* generate comment for clauses/modules		     */
/*****************************************************/
void cgin_coment(type)
  int type;
{
  if (!inwasm_flags.hex_out){
    switch (type) {
    case 1: fprintf(code_file,
		    "/* * * * * * * * * * * WEAK UNIFICATION GRAPH * * * * * * * * */\n");
    break;
    case 2: fprintf(code_file,
		    "/* * * * * * * * * * * REDUCTION FACTS  * * * * * * * * * * * */\n");
    break;
    case 3: fprintf(code_file,
		    "/* * * * * * * * * * * WASM INCLUDES  * * * * * * * * * * * * */\n");
    break;
    case 4: fprintf(code_file,
		    "/* * * * * * * * * * * RANDOM REORDERING  * * * * * * * * * * */\n");
    break;
    case 5: fprintf(code_file,
		    "/* * * * * * * * * * * COPY BOUNDS  * * * * * * * * * * * * * */\n");
    break;
    default:
      /* no coment */
      break;
    }
  }
}


/*****************************************************/
/* new_cllab()                                       */
/* generate a new label for clause                   */
/* 	lnxxx: or lpxxx:                             */
/*****************************************************/
void new_cllab(pr)
  predtype *pr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"l%c%3.3d%2.2d:\n",
	    (pr->sign == '-')? 'n' : 'p',
	    pr->claus->clnr,pr->litnr);
  }
  else {
    hex_out_enter_label((pr->sign == '-')? 
			label_type_cl_label_neg :
			label_type_cl_label_pos,
			pr->claus->clnr,pr->litnr);
  }
}


/*****************************************************/
/* cgin_lcblab(clnr,litnr)                           */
/* generate a new label for literal control block    */   
/*	lcbxx:                                       */
/*****************************************************/
void cgin_lcblab(clnr,litnr)
  int clnr,litnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"lcb%3.3d%3.3d:\n",clnr,litnr);
  }
  else {
    hex_out_enter_label(label_type_lcb_label,clnr,litnr);
  }
}


/*****************************************************/
/* cgin_avlab(clnr,litnr)                            */
/* generate a new label for arg-vect		     */
/* 	avxxx:                                       */
/*****************************************************/
void cgin_avlab(clnr,litnr)
  int clnr,litnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"av%3.3d%3.3d:\n",clnr,litnr);
  }
  else {
    hex_out_enter_label(label_type_av_label,clnr,litnr);
  }
}


/*****************************************************/
/* cgin_cvlab(clnr,litnr)                            */
/* generate a new label for constraint-vect	     */
/*	cvxxx:                                       */
/*****************************************************/
void cgin_cvlab(clnr,constrnr)
  int clnr,constrnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"cv%3.3d%3.3d:\n",clnr,constrnr);
  }
  else {
    hex_out_enter_label(label_type_cv_label,clnr,constrnr);
  }
}


/*****************************************************/
/* cgin_orlab()                                      */
/* generate a orlabel 				     */
/*	orxxx:                                       */
/*****************************************************/
void cgin_orlab(clnr,litnr)
  int clnr,litnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"or%3.3d%3.3d:\n", clnr,litnr);
  }
  else {
    hex_out_enter_label(label_type_or_label,clnr,litnr);
  }
}


/*****************************************************/
/* cgin_rlab(sign,symb)                              */
/* generate a new label for reduce		     */
/* literal control-block			     */
/*	r1xxx: or r0xxx:                             */
/*****************************************************/
void cgin_rlab(sign,symb)
  char sign;
int symb;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"r%c%*.*d:\n",
	    (sign == '+')? '0' : '1',INV,INV, get_codenr(symb));
  }
  else {
    hex_out_enter_label((sign == '+')? 
			label_type_reduct_label_pos:
			label_type_reduct_label_neg,
			get_codenr(symb),0);
  }
}


/*****************************************************/
/* cgin_rlab2(sign,symb)                             */
/* generate a new label for reduce(code)	     */
/*	rr1xxx: or rr0xxx:                           */
/*****************************************************/
void cgin_rlab2(sign,symb)
  char sign;
  int symb;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"rr%c%*.*d:\n",
	    (sign == '+')? '0' : '1',INV,INV, get_codenr(symb));
  }
  else {
    hex_out_enter_label((sign == '+')? 
			label_type_reduct2_label_pos:
			label_type_reduct2_label_neg,
			get_codenr(symb),0);
  }
}


/*****************************************************/
/* cgin_termlab(ar1,ar2)                             */
/* generate a label for a term instruction           */
/*	tlxxx:                                       */
/*****************************************************/
void cgin_termlab(ar1,ar2)
  int ar1,ar2;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"tl%3.3d%3.3d:\n",
	    ar1,ar2);
  }
  else {
    hex_out_enter_label(label_type_tl_label,ar1,ar2);
  }
}


/*****************************************************/
/* cgin_lab()                                        */
/* generate a label                                  */
/* 	labxxx:                                      */
/*****************************************************/
void cgin_lab(labelnr)
  int labelnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"lab%3.3d:\n", labelnr);
  }
  else {
    hex_out_enter_label(label_type_lab_label,labelnr,0);
  }
}


/*****************************************************/
/* cgin_qvlab()                                      */
/* generate a query variable label                   */
/*****************************************************/
void cgin_qvlab(index,clnr)
  int index;
  int clnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"qv%3.3d%2.2d:\n",clnr,index);
  }
  else {
    hex_out_enter_label(label_type_qv_label,index,clnr);
  }
}


/*****************************************************/
/* cgin_subgoal_list_label(clnr,litnr)               */
/* generate a new label for the subgoal list         */
/*****************************************************/
void cgin_subgoal_list_label(clnr,litnr)
  int clnr,litnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"sl%3.3d%3.3d:\n",clnr,litnr);
  }
  else {
    hex_out_enter_label(label_type_sl_label,clnr,litnr);
  }
}


/*****************************************************/
/* cgin_blab                                         */
/*****************************************************/
void cgin_blab(addr)
  int addr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"blab%2.2d:\n",addr);
  }
  else {
    /* hex_out_enter_label(label_type_blab_label,0,0); */
    hex_out_enter_label(label_type_blab_label,addr,0);
  }
} 


/*****************************************************/
/* cgin_label()                                      */
/* generate generic label			     */
/*	name:                                        */
/* NOTE: not supported in hex_out                    */
/*	(and probably not used anyway)               */
/*****************************************************/
void cgin_label(name)
  char name[];
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"%s:\n",name);
  }
  else {
    fprintf(stderr,"code generation: general labels not supported: %s\n",
	    name);
  }
}



/*****************************************************/
/*****************************************************/
/*  PUT WORDS                                        */
/*****************************************************/
/*****************************************************/


/*****************************************************/
/* cgin_typed()                                      */
/* generate a typed word definition                  */
/*	.dw	tag	value                        */
/*****************************************************/
void cgin_typed(type,symb)
  int type,symb;
{
  char *typ = NULL;
  
  if (!inwasm_flags.hex_out){
    switch (type) {
    case CONST:
      typ = "const";
      symb = get_codenr(symb) | IDTAG;
      break;
    case VAR_:
      typ = "var";
      break;
    case GTERM:
      typ = "gterm";
      symb = symb | IDTAG;
      break;
    case NGTERM:
      typ = "ngterm";
      symb = symb | IDTAG;
      break;
    case CRTERM:	/* never used ?? */
      typ = "crterm";
      symb = symb | IDTAG;
      break;
    case EOSTR:
      typ = "eostr";
      break;
    case CREF:
      typ = "cref";
      break;
    case GLOBO:
      typ = "global";
      break;
    case SVAR:
      typ = "cstrvar";
      break;
    case NCONST:
      typ = "const";		/* numeric constant */
      break;
    default:
      fprintf(stderr,"Illegal type codegen-typed\n");
    }
    fprintf(code_file,"	.dw	%s	%d\n", typ,symb);
  }
  else {
    hex_out_put_word_tagged(type,symb);
  }
}


/*****************************************************/
/* cgin_dwterm()                                     */
/* generate a dw for a term                          */ 
/*	.dw	cref	tlxxx                        */
/*****************************************************/
void cgin_dwterm(ar1,ar2,ar3)
  int ar1,ar2,ar3;
{
  char *typ = NULL;
  
  if (!inwasm_flags.hex_out){
    switch (ar1) {
    case CREF:
      typ = "cref";
      break;
    default:
      fprintf(stderr,"Illegal type codegen-dwterm\n");
    }
    fprintf(code_file,"	.dw	%s	tl%3.3d%3.3d\n",
	    typ,ar2,ar3);
  }
  else {
    hex_out_use_label_tagged(label_type_tl_label,ar2,ar3,CREF);
  }
}


/*****************************************************/
/* cgin_dw(ar1)                                      */
/*	generate a generic untagged word             */
/*	.dw	number                               */
/*****************************************************/
void cgin_dw(number)
  int number;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	.dw	%d\n", number);
  }
  else {
    hex_out_put_word(number);
  }
}


/*****************************************************/
/* cgin_ldw()                                        */
/* generate a orbranch entry                         */
/*	.dw	lpxxx  or .dw lnxxx                  */
/*****************************************************/
void cgin_ldw(sign,clnr,litnr)
  int clnr, litnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	.dw	l%c%3.3d%2.2d\n",
	    (sign == '-')? 'n' : 'p', clnr, litnr);
  }
  else {
    hex_out_use_label((sign == '-')?
		      label_type_cl_label_neg:
		      label_type_cl_label_pos,
		      clnr,litnr);
  }
}


/*****************************************************/
/* cgin_avdw()                                       */
/* generate an argument vector entry                 */
/*	.dw	avxxx                                */
/*****************************************************/
void cgin_avdw(clnr,litnr)
  int clnr, litnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	.dw	av%3.3d%3.3d\n",
	    clnr,litnr);
  }
  else {
    hex_out_use_label(label_type_av_label,clnr,litnr);
  }
}


/*****************************************************/
/* cgin_ordw()                                       */
/* generate a orbranch entry                         */ 
/*	.dw	orxxx                                */
/*****************************************************/
void cgin_ordw(clnr,litnr)
  int clnr, litnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	.dw	or%3.3d%3.3d\n",
	    clnr,litnr);
  }
  else {
    hex_out_use_label(label_type_or_label,clnr,litnr);
  }
}


/*****************************************************/
/* cgin_lcbdw()                                      */
/* generate a lcb reference                          */
/*	.dw	lcbxxx                               */
/*****************************************************/
void cgin_lcbdw(pp)
  predtype *pp;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	.dw	lcb%3.3d%3.3d\n",
	    pp->claus->clnr, pp->litnr);
  }
  else {
    hex_out_use_label(label_type_lcb_label,pp->claus->clnr, pp->litnr);
  }
}


/*****************************************************/
/* cgin_sgl_lcbdw()                                  */
/* generate a lcb reference (given clnr,litnr)       */
/*	.dw	lcbxxx                               */
/*****************************************************/
void cgin_sgl_lcbdw(clnr,litnr)
  int clnr, litnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	.dw	lcb%3.3d%3.3d\n",clnr,litnr);
  }
  else {
    hex_out_use_label(label_type_lcb_label,clnr,litnr);
  }
}


/*****************************************************/
/* cgin_rdw()                                        */
/* generate a .dw for the weak-unification graph     */
/*	.dw	r1xx	or 	.dw	r0xx         */
/*****************************************************/
void cgin_rdw(pp)
  predtype *pp;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	.dw	r%c%*.*d\n", 
	    (pp->sign == '+')? '0' : '1', INV, INV ,get_codenr(pp->symb));
  }
  else {
    hex_out_use_label((pp->sign == '+')? 
		      label_type_reduct_label_pos:
		      label_type_reduct_label_neg,
		      get_codenr(pp->symb),0);
  }
}


/*****************************************************/
/* cgin_rdw2()                                       */
/* generate a .dw for the literal control-block entry*/
/* for a reduction step				     */
/*	.dw	rr1xx	or .dw	rr0xx                */
/*****************************************************/
void cgin_rdw2(sign,symbol)
  int sign,symbol;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	.dw	rr%c%*.*d\n", 
	    (sign == '+')? '0' : '1', INV, INV ,get_codenr(symbol));
  }
  else {
    hex_out_use_label((sign == '+')? 
		      label_type_reduct2_label_pos:
		      label_type_reduct2_label_neg,
		      get_codenr(symbol),0);
  }
}


/*****************************************************/
/* cgin_fail_label()                                 */
/* generate a orbranch entry for the special failure */
/* clause                                            */
/*****************************************************/
void cgin_fail_label()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	.dw	lcbfailure\n");
  }
  else {
    hex_out_use_label(label_type_lcbfailure,0,0);
  }
}


/*****************************************************/
/* cgin_rand_reord_label()                           */
/*	.dw	lcbrreord                            */
/*****************************************************/
void cgin_rand_reord_label()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	.dw	lcbrreord\n");
  }
  else {
    hex_out_use_label(label_type_lcbrreord,0,0);
  }
}


/*****************************************************/
/* cgin_exec_ps_label()                              */
/* generate a orbranch entry for the special         */
/* exec_proof_steps clause                           */
/*****************************************************/
void cgin_exec_ps_label()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\t.dw\tlcbexec\n");
  }
  else {
    hex_out_use_label(label_type_lcbexec,0,0);
  }
}


/*****************************************************/
/* cgin_trymode_label()                              */
/* generate a orbranch entry for the special init.   */
/* trymode clause                                    */
/*****************************************************/
void cgin_trymode_label()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\t.dw\tlcbtrymode\n");
  }
  else {
    hex_out_use_label(label_type_lcbtrymode,0,0);
  }
}


/*****************************************************/
/* cgin_subgoal_list_dw(clnr,litnr)                  */
/* generate a .dw for the subgoal list               */
/*****************************************************/
void cgin_subgoal_list_dw(clnr,litnr)
  int clnr,litnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\t.dw\tsl%3.3d%3.3d\n",clnr,litnr);
  }
  else {
    hex_out_use_label(label_type_sl_label,clnr,litnr);
  }
}



/*****************************************************/
/*****************************************************/
/* INSTRUCTIONS                                      */
/*****************************************************/
/*****************************************************/


/*****************************************************/
/* cgin_galloc()	                             */
/* allocate space for global variables               */
/*****************************************************/
void cgin_galloc(number)
  int number;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	galloc	%d\n",number);
  }
  else {
    hex_out_put_word(OPCODE_galloc);
    hex_out_put_word(number);
  }
}


/*****************************************************/
/* cgin_cpset()                                      */
/* generate a setcopy instruction		     */
/* NOTE: not supported                               */
/*****************************************************/
void cgin_cpset()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	setcopy	cpy\n");
  }
  else {
    fprintf(stderr,"code generation: setcpy not supported:\n");
    hex_out_put_word(OPCODE_break);
  }
}


/*****************************************************/
/* cgin_alloc()                                      */
/* generate a alloc instruction                      */ 
/*	alloc LITERAL_CNTRL_BLOCK		     */
/*****************************************************/
void cgin_alloc(nrvars,nrgoal,clnr,litnr,head_nalloc)
  int nrvars,nrgoal;
  int clnr,litnr,head_nalloc;
{
  if (!inwasm_flags.hex_out){
    if (nrgoal && !head_nalloc) {
      fprintf(code_file,"	%salloc	lcb%3.3d%3.3d\n",
	      "",clnr,litnr);
    } else {
      fprintf(code_file,"	%salloc	lcb%3.3d%3.3d\n",
	      "n",clnr,litnr);
    }
  }
  else {
    if (nrgoal && !head_nalloc) {
      hex_out_put_word(OPCODE_alloc);
    }
    else {
      hex_out_put_word(OPCODE_nalloc);
    }
    hex_out_use_label(label_type_lcb_label,clnr,litnr);
  }
}


/*****************************************************/
/* cgin_unvar(offset)                                */
/* generate a unvar instruction                      */
/*****************************************************/
void cgin_unvar(offset)
  int offset;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	unvar	%d\n",offset);
  }
  else {
    hex_out_put_word(OPCODE_unvar);
    hex_out_put_word(offset);
  }
}


/*****************************************************/
/* cgin_unvoid(offset)                               */
/* generate a unvoid instruction                     */
/*	unvoid offset                                */
/* NOTE: offset needed for proof-tree                */
/*****************************************************/
void cgin_unvoid(offset)
int offset;
{
if (!inwasm_flags.hex_out){
	fprintf(code_file,"	unvoid	%d\n",offset);
	}
else {
	hex_out_put_word(OPCODE_unvoid);
	hex_out_put_word(offset);
	}
}


/*****************************************************/
/* cgin_unglobl(offset)                              */
/* generate a unglobl instruction                    */
/*****************************************************/
void cgin_unglobl(offset)
  int offset;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	unglobl	%d\n",offset);
  }
  else {
    hex_out_put_word(OPCODE_unglobl);
    hex_out_put_word(offset);
  }
}


/*****************************************************/
/* cgin_unconst(tl)                                  */
/* generate a unconst instruction                    */
/*****************************************************/
void cgin_unconst(tl)
  termtype *tl;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	unconst	%d\n",
	    (tl->type == NUMCONST)?
	    tl->value:
	    (get_codenr(tl->symb) | IDTAG)
	    );
  }
  else {
    hex_out_put_word(OPCODE_unconst);
    hex_out_put_word(
		     (tl->type == NUMCONST)?
		     tl->value:
		     (get_codenr(tl->symb) | IDTAG)
		     );
  }
}


/*****************************************************/
/* cgin_ungterm()                                    */
/* generate a ungterm instruction                    */
/*****************************************************/
void cgin_ungterm(clnr,labelnr)
  int clnr,labelnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	ungterm	tl%3.3d%3.3d\n",
	    clnr,labelnr);
  }
  else {
    hex_out_put_word(OPCODE_ungterm);
    hex_out_use_label(label_type_tl_label,clnr,labelnr);
  }
}


/*****************************************************/
/* cgin_unngterm()                                   */
/* generate an unngterm instruction                  */
/*****************************************************/
void cgin_unngterm(clnr,labelnr)
  int clnr,labelnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	unngterm	tl%3.3d%3.3d\n",
	    clnr,labelnr);
  }
  else {
    hex_out_put_word(OPCODE_unngterm);
    hex_out_use_label(label_type_tl_label,clnr,labelnr);
  }
}


/*****************************************************/
/* cgin_cpybd(offset)                                */
/* generate a cpybd instruction                      */
/* NOTE: currently not supported                     */
/*****************************************************/
void cgin_cpybd(offset)
  int offset;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	cpybd	%d\n",offset);
  }
  else {
    fprintf(stderr,"code generation: cpybd not supported:\n");
    hex_out_put_word(OPCODE_break);
  }
}


/*****************************************************/
/* cgin_genreg()                                     */
/* generate a genreg instruction                     */
/*****************************************************/
void cgin_genreg(clnr,litnr)
  int clnr,litnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tgenreg \tlcb%3.3d%3.3d\n",clnr,litnr);
  }
  else {
    hex_out_put_word(OPCODE_genreg);
    hex_out_use_label(label_type_lcb_label,clnr,litnr);
  }
}


/*****************************************************/
/* cgin_genregx()                                    */
/* generate a genxreg instruction for ext. reg. for  */
/* fold-up                                           */
/*****************************************************/
void cgin_genregx(clnr,litnr)
  int clnr,litnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tgenxreg \tlcb%3.3d%3.3d\n",clnr,litnr);
  }
  else {
    hex_out_put_word(OPCODE_genxreg);
    hex_out_use_label(label_type_lcb_label,clnr,litnr);
  }
}


/*****************************************************/
/* cgin_genregu()                                    */
/* generate a genureg instruction for fold-down      */
/*****************************************************/ 
void cgin_genregu(clnr,litnr)
  int clnr,litnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tgenureg \tlcb%3.3d%3.3d\n",clnr,litnr);
  }
  else {
    hex_out_put_word(OPCODE_genureg);
    hex_out_use_label(label_type_lcb_label,clnr,litnr);
  }
}


/*****************************************************/
/* cgin_assign()                                     */
/* generate a assign instruction                     */
/*****************************************************/
void cgin_assign(index,clnr,litnr)
  int index, clnr, litnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tassign	%d, av%3.3d%3.3d\n",
	    index, clnr, litnr);
  }
  else {
    hex_out_put_word(OPCODE_assign);
    hex_out_put_word(index);
    hex_out_use_label(label_type_av_label,clnr,litnr);
  }
}


/*****************************************************/
/* cgin_eqpred()                                     */
/* generate a eqpred instruction                     */
/*****************************************************/
void cgin_eqpred(clnr,litnr,sign,symb,arity)
  int clnr,litnr,sign,symb,arity;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\teqpred	av%3.3d%3.3d, %c%*.*d, %d\n",
	    clnr, litnr,
	    (sign == '-')? '0' : '1', 
	    INV,INV,
	    get_codenr(symb), arity);
  }
  else {
    hex_out_put_word(OPCODE_eqpred);
    hex_out_use_label(label_type_av_label,clnr,litnr);
    hex_out_put_word((sign == '-')? 
		     get_codenr(symb):
		     10000+get_codenr(symb)
		     );
    hex_out_put_word(arity);
    /*
      fprintf(stderr,"code generation: non-portable-code: eqpred\n");
      */
  }
}


/*****************************************************/
/* cgin_argsize()                                    */
/* generate an argsize instruction                   */
/*****************************************************/
void cgin_argsize(clnr,litnr,thislitnr)
  int clnr,litnr,thislitnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\targsize	lcb%3.3d%3.3d, av%3.3d%3.3d\n",
	    clnr, litnr,
	    clnr, thislitnr);
  }
  else {
    hex_out_put_word(OPCODE_argsize);
    hex_out_use_label(label_type_lcb_label,clnr,litnr);
    hex_out_use_label(label_type_av_label,clnr,thislitnr);
  }
}


/*****************************************************/
/* cgin_dcall()                                      */
/*****************************************************/
/* generate a determinate call instruction           */
/*	call	LABEL, LITCNTRL-BLOCK		     */
void cgin_dcall(clnr,litnr,pred)
  int clnr,litnr;
  predtype *pred;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	call	l%c%3.3d%2.2d, lcb%3.3d%3.3d\n",
	    (pred->sign == '-')? 'n' : 'p',
	    pred->claus->clnr, pred->litnr,
	    clnr, litnr);
  }
  else {
    fprintf(stderr,"code generation: deterministic call not supported:\n");
    hex_out_put_word(OPCODE_break);
  }
}


/*****************************************************/
/* cgin_call()                                       */
/* generate a call instruction                       */
/*****************************************************/
void cgin_call(clnr,litnr,ppclnr)
  int clnr, litnr, ppclnr;
{
  /*
    fprintf(code_file,"	call	or%3.3d%3.3d, lcb%3.3d%3.3d\n",
    ppclnr, litnr, clnr, litnr);
    */
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	call\n");
  }
  else {
    hex_out_put_word(OPCODE_call);
  }
}


/*****************************************************/
/* cgin_constr()                                     */
/* generate constraint instructions                  */
/*****************************************************/
void cgin_constr(clnr,constr)
  int clnr;
  predtype *constr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tcgen \tcv%3.3d%3.3d\n", clnr,constr->litnr);
  }
  else {
    hex_out_put_word(OPCODE_cgen);
    hex_out_use_label(label_type_cv_label,clnr,constr->litnr);
  }
}


/*****************************************************/
/* cgin_dealloc()                                    */
/* generate a dealloc                                */
/*****************************************************/
void cgin_dealloc(nrgoal)
  int nrgoal;
{
  if (nrgoal){
    if (!inwasm_flags.hex_out){
      fprintf(code_file,"	dealloc\n");
    }
    else {
      hex_out_put_word(OPCODE_dealloc);
    }
  }
  else {
    if (!inwasm_flags.hex_out){
      fprintf(code_file,"	ndealloc\n");
    }
    else {
      hex_out_put_word(OPCODE_ndealloc);
    }
  }
}


/*****************************************************/
/* cgin_orbr(labelnr)                                */
/* generate a orbranch instruction                   */
/*****************************************************/
void cgin_orbr(labelnr)
  int labelnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	orbranch	lab%3.3d\n", labelnr);
  }
  else {
    hex_out_put_word(OPCODE_orbranch);
    hex_out_use_label(label_type_lab_label,labelnr,0);
  }
}


/*****************************************************/
/* cgin_por()                                        */
/* generate a orbranch instruction with predfail     */
/*****************************************************/
void cgin_por(labelnr,pp)
  int labelnr;
  predtype *pp;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	porbranch	lab%3.3d, %c%*.*d, %d\n",
	    labelnr, (pp->sign == '-')? '0' : '1',
	    INV, INV, get_codenr(pp->symb), pp->arity);
  }
  else {
    fprintf(stderr,"code generation: porbranch NYI\n");
    hex_out_put_word(OPCODE_break);
  }
}


/*****************************************************/
/* cgin_reduct()                                     */
/* generate a reduct instruction                     */
/*****************************************************/
void cgin_reduct(sign,symb,arity)
  char sign;
  int symb,arity;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	reduct	%c%*.*d, %d\n",
	    (sign == '+')? '0' : '1', INV, INV, get_codenr(symb), arity);
  }
  else {
    hex_out_put_word(OPCODE_reduct);
    hex_out_put_word((sign == '+')? 
		     get_codenr(symb):
		     10000+get_codenr(symb)
		     );
    hex_out_put_word(arity);
    /*
      fprintf(stderr,"code generation: non-portable-code: reduct\n");
      */
  }
}


/*****************************************************/
/* cgin_ureduct()                                    */
/* generate a ureduct instruction                    */
/* for folding down                                  */
/*****************************************************/
void cgin_ureduct(sign,symb,arity)
  char sign;
  int symb,arity;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	ureduct	%c%*.*d, %d\n",
	    (sign == '+')? '0' : '1', INV, INV, get_codenr(symb), arity);
  }
  else {
    hex_out_put_word(OPCODE_ureduct);
    hex_out_put_word((sign == '+')? 
		     get_codenr(symb):
		     10000+get_codenr(symb)
		     );
    hex_out_put_word(arity);
    /*
      fprintf(stderr,"code generation: non-portable-code: ureduct\n");
      */
  }
}


/*****************************************************/
/* cgin_stop()                                       */
/* generate a stop-command                           */
/* NOTE: Although present as a built-in, it is used  */
/*	 internally in the code-generation           */
/*****************************************************/
void cgin_stop()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tstop\n");
  }
  else {
    hex_out_put_word(OPCODE_stop);
  }
}


/*****************************************************/
/* cgin_fail()                                       */
/* generate a fail-command                           */
/*	Although present as a built-in, it is used   */
/*	internally in the code-generation            */
/*****************************************************/
void cgin_fail()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tfail\n");
  }
  else {
    hex_out_put_word(OPCODE_fail);
  }
}


/*****************************************************/
/* cgin_proceed()                                    */
/* generate a proceed				     */
/*****************************************************/
void cgin_proceed()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	proceed\n");
  }
  else {
    hex_out_put_word(OPCODE_proceed);
  }
}


/*****************************************************/
/* cgin_clause()                                     */
/* generate a clause directive 			     */
/*****************************************************/
void cgin_clause(pr)
  predtype *pr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	.clause %d,%d\n",
	    pr->claus->clnr,pr->litnr);
  }
}


/*****************************************************/
/* cgin_dred()                                       */
/*****************************************************/
void cgin_dred(sign,symb)
  int sign,symb;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	.reduction %c%*.*d\n",
	    (sign == '+')? '0' : '1',INV, INV, get_codenr(symb));
  }
}


/*****************************************************/
/* cgin_opt()                                        */
/*****************************************************/
void cgin_opt()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	.optim\n");
  }
  else {
    hex_out_optimize(1);
  }
}


/*****************************************************/
/* cgin_noopt()                                      */
/*****************************************************/
void cgin_noopt()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	.noopt\n");
  }
  else {
    hex_out_optimize(0);
  }
}


/*****************************************************/
/* cgin_qout()                                       */
/* generate a query variable output instruction      */
/*****************************************************/
void cgin_qout(index,clnr)
  int index;
  int clnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"	out	qv%3.3d%2.2d\n",
	    clnr,index);
  }
  else {
    hex_out_put_word(OPCODE_out);
    hex_out_use_label(label_type_qv_label,clnr,index);
  }
}


/*****************************************************/
/* cgin_disptree()                                   */
/* generate the proof-tree instruction               */
/*****************************************************/
void cgin_disptree()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tprinttree\n");
  }
  else {
    hex_out_put_word(OPCODE_printtree);
  }
}


/*****************************************************/
/* cgin_save_clause(clnr,litnr)                      */
/* generate a ``save_clause slxxx                    */
/*****************************************************/
void cgin_save_clause(clnr,litnr)
  int clnr,litnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tsave_clause\tsl%3.3d%3.3d\n",clnr,litnr);
  }
  else {
    hex_out_put_word(OPCODE_save_clause);
    hex_out_use_label(label_type_sl_label,clnr,litnr);
  }
}


/*****************************************************/
/* cgin_set_subgoal_list(clnr,litnr)                 */
/* generate a ``setsubg slxxx                        */
/*****************************************************/
void cgin_set_subgoal_list(clnr,litnr)
  int clnr,litnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tsetsubg\tsl%3.3d%3.3d\n",clnr,litnr);
  }
  else {
    hex_out_put_word(OPCODE_setsubg);
    hex_out_use_label(label_type_sl_label,clnr,litnr);
  }
}


/*****************************************************/
/* cgin_foldup()                                     */
/* generate a fold_up instruction for folding up     */
/*****************************************************/
void cgin_foldup()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tfold_up\n");
  }
  else {
    hex_out_put_word(OPCODE_fold_up);
  }
}


/*****************************************************/
/* cgin_foldupx()                                    */
/* generate a reg_fold_up instruction for folding up */
/* extended reg                                      */
/*****************************************************/
void cgin_foldupx()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\treg_fold_up\n");
  }
  else {
    hex_out_put_word(OPCODE_reg_fold_up);
  }
}


/*****************************************************/
/* cgin_save_fact()                                  */
/* generate a save_fact instr. for interactive setheo*/
/*****************************************************/
void cgin_save_fact()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tsave_fact\n");
  }
  else {
    hex_out_put_word(OPCODE_save_fact);
  }
}


/*****************************************************/
/* cgin_save_reduct()                                */
/* generate a save_reduct instr. for interactive     */
/* setheo                                            */
/*****************************************************/
void cgin_save_reduct()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tsave_reduct\n");
  }
  else {
    hex_out_put_word(OPCODE_save_reduct);
  }
}


/*****************************************************/
/* cgin_label_ortree()                               */
/* generate a label_ortree instr. for interactive    */
/* setheo                                            */
/*****************************************************/
void cgin_label_ortree()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tlabel_ortree\n");
  }
  else {
    hex_out_put_word(OPCODE_label_ortree);
  }
}


/*****************************************************/
/* cgin_loadfcb                                      */
/*****************************************************/
void cgin_loadfcb()
{
#ifdef LOADFCB_ACTIVE
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tloadfcb\tfcb\n");
  }
  else {
    hex_out_put_word(OPCODE_loadfcb);
    hex_out_use_label(label_type_fcb,0,0);
  }
#endif
}


/*****************************************************/
/* cgin_glod                                         */
/*****************************************************/
void cgin_glod(addr)
  int addr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tglod\t%d\n",addr);
  }
  else {
    hex_out_put_word(OPCODE_glod);
    hex_out_put_word(addr);
  }
}


/*****************************************************/
/* cgin_gsto                                         */
/*****************************************************/
void cgin_gsto(addr)
  int addr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tgsto\t%d\n",addr);
  }
  else {
    hex_out_put_word(OPCODE_gsto);
    hex_out_put_word(addr);
  }
}


/*****************************************************/
/* cgin_lod                                          */
/*****************************************************/
void cgin_lod(addr)
  int addr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tlod\t%d\n",addr);
  }
  else {
    hex_out_put_word(OPCODE_lod);
    hex_out_put_word(addr);
  }
}


/*****************************************************/
/* cgin_sto                                          */
/*****************************************************/
void cgin_sto(addr)
  int addr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tsto\t%d\n",addr);
  }
  else {
    hex_out_put_word(OPCODE_sto);
    hex_out_put_word(addr);
  }
}


/*****************************************************/
/* cgin_lconst                                       */
/*****************************************************/
void cgin_lconst(addr)
  int addr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tlconst\t%d\n",addr);
  }
  else {
    hex_out_put_word(OPCODE_lconst);
    hex_out_put_word(addr);
  }
}


/*****************************************************/
/* cgin_setdepth                                     */
/*****************************************************/
void cgin_setdepth(addr)
  int addr;
{
  fprintf(stderr,"code generation: stedepth obsolete\n");
  
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tsetdepth\t%d\n",addr);
  }
  else {
    fprintf(stderr,"code generation: setdepth obsolete\n");
    hex_out_put_word(OPCODE_break);
  }
}


/*****************************************************/
/* cgin_jmpz_blab                                    */
/*****************************************************/
void cgin_jmpz_blab(addr)
  int addr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tjmpz\tblab%2.2d\n",addr);
  }
  else {
    hex_out_put_word(OPCODE_jmpz);
    hex_out_use_label(label_type_blab_label,addr,0);
  }
}


/*****************************************************/
/* cgin_jmpg_blab                                    */
/*****************************************************/
void cgin_jmpg_blab(addr)
  int addr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tjmpg\tblab%2.2d\n",addr);
  }
  else {
    hex_out_put_word(OPCODE_jmpg);
    hex_out_use_label(label_type_blab_label,addr,0);
  }
}


/*****************************************************/
/* cgin_sub                                          */
/*****************************************************/
void cgin_sub()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tsub\n");
  }
  else {
    hex_out_put_word(OPCODE_sub);
  } 
}


/*****************************************************/
/* cgin_add                                          */
/*****************************************************/
void cgin_add()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tadd\n");
  }
  else {
    hex_out_put_word(OPCODE_add);
  }
}


/*****************************************************/
/* cgin_mul                                          */
/*****************************************************/
void cgin_mul()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tmul\n");
  }
  else {
    hex_out_put_word(OPCODE_mul);
  }
}


/*****************************************************/
/* cgin_div                                          */
/*****************************************************/
void cgin_div()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tdiv\n");
  }
  else {
    hex_out_put_word(OPCODE_div);
  }
}



/*****************************************************/
/*****************************************************/
/* BUILT-INS                                         */
/*****************************************************/
/*****************************************************/


/*****************************************************/
/* cgin_generic_built_in()                           */
/* generate a generic built instruction              */
/* with no arguments                                 */
/*****************************************************/
void cgin_gen_built_in_noargs(name,opcode)
  char *name;
  int opcode;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\t%s\n", name);
  }
  else {
    hex_out_put_word(opcode);
  }
}


/*****************************************************/
/* cgin_generic_built_in()                           */
/*****************************************************/
/* generate a generic built instruction              */
void cgin_gen_built_in_args(name,opcode,clnr,litnr)
  char *name;
  int opcode;
  int clnr, litnr;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\t%s\tav%3.3d%3.3d\n", name, clnr, litnr);
  }
  else {
    hex_out_put_word(opcode);
    hex_out_use_label(label_type_av_label,clnr,litnr);
  }
}


/*****************************************************/
/* cgin_b_gen_lcb_t()                                */
/*****************************************************/
/* generate a generic built instruction              */
/* NAME   LCB,argv                                   */
void cgin_b_gen_lcb_t(name,opcode,litnr_lcb,clnr,litnr)
  char *name;
  int opcode;
  int clnr, litnr, litnr_lcb;
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\t%s\tlcb%3.3d%3.3d,av%3.3d%3.3d\n",
	    name,clnr, litnr_lcb, clnr, litnr);
  }
  else {
    hex_out_put_word(opcode);
    hex_out_use_label(label_type_lcb_label,clnr,litnr_lcb);
    hex_out_use_label(label_type_av_label,clnr,litnr);
  }
}


/*****************************************************/
/* cgin_prepare_ndbuilt()                            */
/*****************************************************/
/* generate a preparation instruction for            */
/* non-determinstic built-ins                        */
void cgin_prepare_ndbuilt()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\tprepare_ndbuilt\tlcbfailure\n");
  }
  else {
    hex_out_put_word(OPCODE_prepare_ndbuilt);
    hex_out_use_label(label_type_lcbfailure,0,0);
  }
}


/*****************************************************/
/* NOTE:                                             */
/* code-generation functions for INFERENCE_RULE_*    */
/* 1) generate a label  cgin_*_label()               */
/* 2) generate the code for a dummy lcb and the fact */
/*	cgin_*_clause()                              */
/*****************************************************/ 



/*****************************************************/
/*****************************************************/
/* ENTIRE CLAUSES                                    */
/*****************************************************/
/*****************************************************/


/*****************************************************/
/* cgin_fail_clause()                                */
/* generate a special failure clause                 */
/* NOTE:                                             */
/*	lcbfailure can possibly be too short         */
/*	(it does not contain any entries except      */
/*	the jump-label                               */
/*****************************************************/
void cgin_fail_clause()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"failure:\n");
    fprintf(code_file,"\tfail\n");
    fprintf(code_file,"lcbfailure:\n");
    fprintf(code_file,"\t.dw\tfailure\n");
  }
  else {
    hex_out_enter_label(label_type_failure,0,0);
    hex_out_put_word(OPCODE_fail);
    hex_out_enter_label(label_type_lcbfailure,0,0);
    hex_out_use_label(label_type_failure,0,0);
  }
}


/*****************************************************/
/* cgin_trymode_fact()                               */
/* generate a special clause for interactive mode    */
/*	lcbtrymode can possibly be too short         */
/*	(it does not contain any entries except      */
/*	the jump-label                               */
/*****************************************************/
void cgin_trymode_fact()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"lbltrymode:\n");
    fprintf(code_file,"\tinitialize_trymode\n");
    fprintf(code_file,"lcbtrymode:\n");
    fprintf(code_file,"\t.dw\tlbltrymode\n");
  }
  else {
    hex_out_enter_label(label_type_lbltrymode);
    hex_out_put_word(OPCODE_initialize_trymode);
    hex_out_enter_label(label_type_lcbtrymode);
    hex_out_use_label(label_type_lbltrymode,0,0);
  }
}


/*****************************************************/
/* cgin_exec_ps_fact()                               */
/* generate a special clause for interactive mode    */
/* generate a special failure clause                 */
/*	lcbexec can possibly be too short            */
/*	(it does not contain any entries except      */
/*	the jump-label                               */
/*****************************************************/
void cgin_exec_ps_fact()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"lblexec:\n");
    fprintf(code_file,"\texec_proof_steps\n");
    fprintf(code_file,"lcbexec:\n");
    fprintf(code_file,"\t.dw\tlblexec\n");
  }
  else {
    hex_out_enter_label(label_type_lblexec);
    hex_out_put_word(OPCODE_exec_proof_steps);
    hex_out_enter_label(label_type_lcbexec);
    hex_out_use_label(label_type_lblexec,0,0);
  }
}


/*****************************************************/
/* cgin_rand_reord_clause()                          */
/*	lcbrreord can possibly be too short          */
/*	(it does not contain any entries except      */
/*	the jump-label                               */
/*****************************************************/
void cgin_rand_reord_clause()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"rrord:\n");
    fprintf(code_file,"\trand_reorder\n");
    fprintf(code_file,"lcbrreord:\n");
    fprintf(code_file,"\t.dw\trrord\n");
  }
  else {
    hex_out_enter_label(label_type_rrord);
    hex_out_put_word(OPCODE_rand_reorder);
    hex_out_enter_label(label_type_lcbrreord);
    hex_out_use_label(label_type_rrord,0,0);
  }
}


/*****************************************************/
/* cgin_dwfcb                                        */
/*****************************************************/
void cgin_dwfcb()
{
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"fcb:\n");
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.nr_clauses);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.nr_literals);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.nr_connections);
  }
  else {
    hex_out_enter_label(label_type_fcb,0,0);
    hex_out_put_word_tagged(NCONST,fcb.nr_clauses);
    hex_out_put_word_tagged(NCONST,fcb.nr_literals);
    hex_out_put_word_tagged(NCONST,fcb.nr_connections);
  }
  
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.nr_const);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.nr_fsymb);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.nr_predsymb);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_arity_fsymb);
  }
  else {
    hex_out_put_word_tagged(NCONST,fcb.nr_const);
    hex_out_put_word_tagged(NCONST,fcb.nr_fsymb);
    hex_out_put_word_tagged(NCONST,fcb.nr_predsymb);
    hex_out_put_word_tagged(NCONST,fcb.max_arity_fsymb);
  }
  
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.nr_queries);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_lit_queries);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.sum_lit_queries);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_vars_queries);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_termdepth_queries);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_termsize_queries);
  }
  else {
    hex_out_put_word_tagged(NCONST,fcb.nr_queries);
    hex_out_put_word_tagged(NCONST,fcb.max_lit_queries);
    hex_out_put_word_tagged(NCONST,fcb.sum_lit_queries);
    hex_out_put_word_tagged(NCONST,fcb.max_vars_queries);
    hex_out_put_word_tagged(NCONST,fcb.max_termdepth_queries);
    hex_out_put_word_tagged(NCONST,fcb.max_termsize_queries);
  }
  
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.nr_punits);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_vars_punits);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_termdepth_punits);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_termsize_punits);
  }
  else {
    hex_out_put_word_tagged(NCONST,fcb.nr_punits);
    hex_out_put_word_tagged(NCONST,fcb.max_vars_punits);
    hex_out_put_word_tagged(NCONST,fcb.max_termdepth_punits);
    hex_out_put_word_tagged(NCONST,fcb.max_termsize_punits);
  }
  
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.nr_nunits);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_vars_nunits);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_termdepth_nunits);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_termsize_nunits);
  }
  else {
    hex_out_put_word_tagged(NCONST,fcb.nr_nunits);
    hex_out_put_word_tagged(NCONST,fcb.max_vars_nunits);
    hex_out_put_word_tagged(NCONST,fcb.max_termdepth_nunits);
    hex_out_put_word_tagged(NCONST,fcb.max_termsize_nunits);
  }
  
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.nr_defclauses);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_lit_defclauses);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.sum_lit_defclauses);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_vars_defclauses);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_termdepth_defclauses);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_termsize_defclauses);
  }
  else {
    hex_out_put_word_tagged(NCONST,fcb.nr_defclauses);
    hex_out_put_word_tagged(NCONST,fcb.max_lit_defclauses);
    hex_out_put_word_tagged(NCONST,fcb.sum_lit_defclauses);
    hex_out_put_word_tagged(NCONST,fcb.max_vars_defclauses);
    hex_out_put_word_tagged(NCONST,fcb.max_termdepth_defclauses);
    hex_out_put_word_tagged(NCONST,fcb.max_termsize_defclauses);
  }
  
  if (!inwasm_flags.hex_out){
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.nr_indefclauses);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_lit_indefclauses);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.sum_lit_indefclauses);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_vars_indefclauses);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_termdepth_indefclauses);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_termsize_indefclauses);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.max_plit_indefclauses);
    fprintf(code_file,"\t.dw\tconst %d\n",fcb.sum_plit_indefclauses);
  }
  else {
    hex_out_put_word_tagged(NCONST,fcb.nr_indefclauses);
    hex_out_put_word_tagged(NCONST,fcb.max_lit_indefclauses);
    hex_out_put_word_tagged(NCONST,fcb.sum_lit_indefclauses);
    hex_out_put_word_tagged(NCONST,fcb.max_vars_indefclauses);
    hex_out_put_word_tagged(NCONST,fcb.max_termdepth_indefclauses);
    hex_out_put_word_tagged(NCONST,fcb.max_termsize_indefclauses);
    hex_out_put_word_tagged(NCONST,fcb.max_plit_indefclauses);
    hex_out_put_word_tagged(NCONST,fcb.sum_plit_indefclauses);
  }
}
	
