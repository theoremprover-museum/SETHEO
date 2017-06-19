/****************************************************
;;; MODULE NAME : hexout.c
;;;
;;; PARENT      : codegen
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)hexout.c	17.2 01/08/97
;;;
;;; AUTHOR      : J. Schumann
;;;
;;; DATE        : 3.1.96
;;;
;;; DESCRIPTION :
;;;
;;; REMARKS     :
;;;
****************************************************/

/*****************************************************/
/* includes                                          */
/*****************************************************/
#include "types.h"
#include "hexout.h"
#include "extern.h"
#include "tags.h"
#include "codedef.h"
#include "optim.h"

/*****************************************************/
/* defines                                           */
/*****************************************************/
#define	NR_TABLISTS_PER_CHUNK	10000
#define T_CODE_SYMBTAB_SIZE	199999 /* must be prime because of hash table */
#define DEBUG(X)	X
#define DEBUG_OPT(X)	X
#define DEBUG7(X)		X
#define HEX_OUT_STATISTICS

#ifdef HEX_OUT_STATISTICS
#	define H_O_STAT(X)	X
#else
#	define H_O_STAT(X)	
#endif

//JSC-092016
#define HEX_OUT_LINEAR_SEARCH

/*****************************************************/
/* statics                                           */
/*****************************************************/
static	int	pc;
static	int	nr_lablist_elements_used;
static	t_lablist	*lablist_store;
static	t_lablist	*lablist_head;
static	t_lablist	*lablist_tail;
static	t_code_symbtab_element *code_symbtab;
static  int	nr_code_symbtab_entries;
static  int	max_pc;
static  int	hex_out_nr_symbols;
static  int	hex_out_opt	=0;

#ifdef HEX_OUT_STATISTICS
static	int	nr_colls = 0;
static	int	nr_access = 0;
#endif

/*****************************************************/
/* functions (codegen/hexout.c)                      */
/*****************************************************/
static t_lablist *lablist_malloc();
void hex_out_init();
void hex_out_enter_label();
void hex_out_use_label_tagged();
int resolve_undefined();
void hex_out_put_word();
void hex_out_put_word_tagged();
void put_word_tagged();
void hex_out_optimize();
void hex_out_finish();
int get_n_set_label_info();
void put_word();
void hex_out_put_symbol();
void hex_out_start_put_symbol();
void hex_out_finish_put_symbol();
void hex_out_codestr ();
void hex_out_optim_pnam();
void hex_out_optim_plab();
void hex_out_finish_optim();
extern int calloc();

/*****************************************************/
/* lablist_malloc()                                  */
/* allocate a undef'd reference to be resolved in    */
/* pass2                                             */
/*****************************************************/
static t_lablist *lablist_malloc()
{
  if (nr_lablist_elements_used < NR_TABLISTS_PER_CHUNK){
    DEBUG(printf("lablist-malloc: case 1: return %lx\n",lablist_store + nr_lablist_elements_used));
    return lablist_store + (nr_lablist_elements_used++);
  }
  nr_lablist_elements_used = 1;
  lablist_store = (t_lablist *)malloc(NR_TABLISTS_PER_CHUNK*sizeof(t_lablist));
  
  DEBUG7(printf("lablist-malloc: case 2: return %lx\n",lablist_store));
  return lablist_store;
}


/*****************************************************/
/* hex_out_init()                                    */
/* initialize the code-output                        */
/*****************************************************/
void hex_out_init()
{
  pc = max_pc = 0;
  
  /* initialize list of usage for undefined labels */
  lablist_store = (t_lablist *)malloc(NR_TABLISTS_PER_CHUNK*sizeof(t_lablist));
  nr_lablist_elements_used = 0;
  lablist_head = NULL;
  lablist_tail = NULL;
  
  /* initialize symbol table */
  code_symbtab = (t_code_symbtab_element *)
    malloc(T_CODE_SYMBTAB_SIZE*sizeof(t_code_symbtab_element));
//JSC-092016    calloc(T_CODE_SYMBTAB_SIZE,sizeof(t_code_symbtab_element));
  nr_code_symbtab_entries = 0;
  DEBUG7(printf("%s: %d: nr_code_symbtab_entries=%d\n",__FILE__,__LINE__,nr_code_symbtab_entries));
  DEBUG7(printf("%s: %d: T_CODE_SYMBTAB_SIZE=%d\n",__FILE__,__LINE__,T_CODE_SYMBTAB_SIZE));
  DEBUG7(printf("%s: %d: code_symbtab=%lx\n",__FILE__,__LINE__,code_symbtab));


  /* write an empty space at the beginning of the hex-file */
  fprintf (code_file, FORMAT_CODE, MAXADDR_CODE, 0,0, "%");
  fprintf (code_file, FORMAT_CODE, NRSYMB_CODE, 0, 0, "%");
  
  
  /* initialize the optimizer */
  if (inwasm_flags.hex_out_opt){
    DEBUG_OPT(printf("init_optim()\n");)
      init_optim();
      opt_incount=opt_outcount=0;
  }
}


/*****************************************************/
/* define a label (xxx:)                             */
/*	given its type(+arguments)                   */
/*	its value is the current pc                  */
/*****************************************************/
void hex_out_enter_label(lt,arg1,arg2)
  t_hexout_label_type	lt;
  int		arg1;
  int		arg2;
{
  t_code_symbtab_element *code_symbtab_element;
  
  DEBUG(printf("enter label: %d %d %d at %d\n",lt,arg1,arg2,pc));
  
  get_n_set_label_info(lt,arg1,arg2,&code_symbtab_element);
  if (code_symbtab_element->defined){
    /* this should not happen */
    fprintf(stderr,
	    "internal error: hexout: label multiple defined: %d %d %d @ %lx [hex]\n"
	    ,lt,arg1,arg2,pc);
  }
  else {
    /* enter the label's value */
    
    if (hex_out_opt){
      /* enter the definition into the optimizer */
      /* 1st argument = index into symbol table  */
	      
		DEBUG_OPT(printf("o_name(%d,%d): [%d %d %d]\n",
				 code_symbtab_element-code_symbtab,
				 OPT_LABELTAG_TAG(T_EMPTY),
				 lt,arg1,arg2);)
		  o_name(code_symbtab_element - code_symbtab,
			 OPT_LABELTAG_TAG(T_EMPTY));
    }
    else {
      code_symbtab_element->value = pc;
    }
    code_symbtab_element->defined = 1;
  }
}


/*****************************************************/
/* use a label (.....\t xxx)                         */
/*	label can be defined or undefined            */
/*****************************************************/
void hex_out_use_label_tagged(lt,arg1,arg2,tag)
  t_hexout_label_type	lt;
  int		arg1;
  int		arg2;
  int		tag;
{
  t_code_symbtab_element	*code_symbtab_element;
  t_lablist	*tl;
  
  DEBUG(printf("use label: %d %d %d at %d\n",lt,arg1,arg2,pc));
  
  get_n_set_label_info(lt,arg1,arg2,&code_symbtab_element);
  
  /* are we in optimizer mode? */

  if (hex_out_opt){
    /* then, we give the symbol into the optimizer */
    
    DEBUG_OPT(printf("enter_o_list(%d,%d): [%d %d %d]\n",
		     code_symbtab_element-code_symbtab,OPT_LABELTAG_TAG(tag),
		     lt,arg1,arg2);)
      enter_o_list(code_symbtab_element-code_symbtab,OPT_LABELTAG_TAG(tag));
  }
  else {
    if (!code_symbtab_element->defined){
      /* the operand is still undefined	    */
      /* note this fact                     */
      DEBUG(printf("use label: not yet defined\n"));
      tl = lablist_malloc();
      if (!tl){
	fprintf(stderr,"  Error: Out of memory.\n");
	exit(101);
      }
      
      if (!lablist_head) lablist_head = tl;
      if (!lablist_tail) {
	lablist_tail = tl;
      }
      else {
	lablist_tail->next = tl;
	lablist_tail = tl;
	tl->next = NULL;
      }
      tl->where = ftell(code_file);
      tl->pc = pc;
      tl->symbol = code_symbtab_element;
      tl->tag = tag;
      
      /* write out an empty space (to be filled up later)		*/
      /* at current pc                                                */
      hex_out_put_word(0);
    }
    else {	/* is defined */
      DEBUG(printf("use label: label defined [val=%d]\n",code_symbtab_element->value));
      if (tag){
	hex_out_put_word_tagged(tag,code_symbtab_element->value);
      }
      else {
	hex_out_put_word(code_symbtab_element->value);
      }
    }
  }
}


/*****************************************************/
/* resolve any previously undefined labels           */
/*	(2nd pass)                                   */
/*	returns: 0 OK, <>0: labels still undefined   */
/*****************************************************/
int resolve_undefined()
{
  while(lablist_head){
    DEBUG(printf("resolve_undefined: [symb=%d %d %d val=%d place=%lx]\n", 
		 lablist_head->symbol->hexout_label_type,
		 lablist_head->symbol->arg1, 
		 lablist_head->symbol->arg1, 
		 lablist_head->symbol->value,
		 lablist_head->where));
    if (!(lablist_head->symbol)->defined){
      /* brr... label still not defined */
      fprintf(stderr,
	      "error: hexout: label not defined: %d %d %d @ %lx [hex]\n",
	      lablist_head->symbol->hexout_label_type,
	      lablist_head->symbol->arg1,
	      lablist_head->symbol->arg1,
	      lablist_head->pc);
      return 1;
    }
    /* move to the position in file */
    fseek (code_file, lablist_head->where, 0);	
    if (lablist_head->tag){
      /* the label is tagged:       */
      /* must enter the correct tag */

      put_word_tagged(lablist_head->pc,
		      lablist_head->tag,
		      lablist_head->symbol->value);
    }
    else {
      put_word(lablist_head->pc,lablist_head->symbol->value);
    }
    
    /* move down the list */
    /* don't dealloc      */
    lablist_head = lablist_head->next;
  }
  return 0;
}


/*****************************************************/
/* put an opcode at the current address              */
/*	typed and untyped version                    */
/* Note:                                             */
/*	we increment the pc unless we are in         */
/*	optimizing mode, where we store the          */
/*	words in the optimizer                       */
/*****************************************************/
void hex_out_put_word(val)
  int val;
{
  if (hex_out_opt){
    enter_o_list(val,OPT_VALUETAG_TAG(T_EMPTY));
    DEBUG_OPT(printf("enter_o_list(%d,%d): value\n",val,OPT_VALUETAG_TAG(T_EMPTY));)
      }
  else {
    put_word(pc,val);
    pc++;
  }
}


/*****************************************************/
/* hex_out_put_word_tagged()                         */
/*****************************************************/
void hex_out_put_word_tagged(type,symb)
  int	type;
  int	symb;
{
  if (hex_out_opt){
    enter_o_list(symb,OPT_VALUETAG_TAG(type));
  }
  else {
    put_word_tagged(pc,type,symb);
    pc++;
  }
}


/*****************************************************/
/* put_word_tagged(pc,type,symb)                     */
/*****************************************************/
void put_word_tagged(pc,type,symb)
  int	pc;
  int	type;
  int	symb;
{
  int hex_out_tag = 0;
  
  switch (type) {
  case CONST:
    hex_out_tag = T_CONST;
    symb = get_codenr(symb) | IDTAG;
    break;
  case VAR_:
    hex_out_tag = T_VAR;
    break;
  case GTERM:
    hex_out_tag = T_GTERM;
    symb = symb | IDTAG;
    break;
  case NGTERM:
    hex_out_tag = T_NGTERM;
    symb = symb | IDTAG;
    break;
  case EOSTR:
    hex_out_tag = T_EOSTR;
    break;
  case CREF:
    hex_out_tag = T_CREF;
    break;
  case GLOBO:
    hex_out_tag = T_HREF;  /* WAS T_GLOBAL */
    break;
  case SVAR:
    hex_out_tag = T_CSTRVAR;
    break;
  case NCONST:
    hex_out_tag = T_CONST;
    break;
  default:
    fprintf(stderr,"Illegal type codegen-typed %d\n",type);
  }

  /*
   * put the tagged word at current address
   * corresponds to:
   *	.dw	tag	symb
   **/
  put_word(pc,((hex_out_tag << TAGSTART) | symb)); 
}


/*****************************************************/
/* toggle optimising                                 */
/*****************************************************/
void hex_out_optimize(onoff)
  int	onoff;
{
  if (inwasm_flags.hex_out_opt){
    if (hex_out_opt && !onoff){
     
      /* we encountered a .noopt */
	  
      DEBUG_OPT(printf("list2tree()\n");)
	list2tree();
    }
    hex_out_opt = onoff;
  }
}


/*****************************************************/
/* hex_out_finish                                    */
/*****************************************************/
void hex_out_finish()
{
  /* do we have to print the trees stored in the optimizer ? */

  if (inwasm_flags.hex_out_opt){
    hex_out_finish_optim();
  }
  
  max_pc = pc;
  
  if (resolve_undefined()){
    /* there have been errors in pass2 */
    fprintf(stderr, "  Error: Errors in code generation pass 2.\n");
    exit(61);
  }
  
  /* write the maximal program counter (maxpc) at the */
  /* beginning of the file                            */
  fseek(code_file,0L,0);
  fprintf (code_file, FORMAT_CODE, MAXADDR_CODE, 0,max_pc, "%");
  fprintf (code_file, FORMAT_CODE, NRSYMB_CODE, 0, hex_out_nr_symbols, "%");
  
  H_O_STAT(fprintf(stderr,"codegen: %d [%d] labels, %d colls [%d accesses]\n",
		   nr_code_symbtab_entries,T_CODE_SYMBTAB_SIZE,nr_colls,nr_access));
  
  if (inwasm_flags.hex_out_opt){
    fprintf(stderr,"Assembler optimization: %d labels read, %d labels output\n",
	    opt_incount, opt_outcount);
  }
}


/*****************************************************/
/*****************************************************/
/*  low-level routines concerning the symbol table   */
/*****************************************************/
/*****************************************************/

/*****************************************************/
/* get_n_set_label_info                              */
/*	ask for a label definition, or               */
/*	enter the label (as undefined) if it         */
/*	is not yet in the symbol table               */
/* returns: pointer to that element (lats arg)       */
/*	0 : new definition                           */
/*	1 : old definition                           */
/*	-1: symbol table overflow                    */
/*****************************************************/
int get_n_set_label_info(lt,arg1,arg2,code_symbtab_element)
  t_hexout_label_type	lt;
  int			arg1;
  int			arg2;
  t_code_symbtab_element	**code_symbtab_element;
{
  int i;
  t_code_symbtab_element	*cs;
  
  DEBUG(printf("get_n: search for: %d %d %d\n",lt,arg1,arg2));
  H_O_STAT(nr_access++);
#ifdef HEX_OUT_LINEAR_SEARCH
  /* V0.0: linear search */
  for (i=0, cs = code_symbtab; 
       i< nr_code_symbtab_entries; i++,cs++){
    
    DEBUG(printf("get_n: @i %d: %d %d %d [val: %d, defs:%d]\n",i,cs->hexout_label_type,cs->arg1,cs->arg2,cs->value,cs->defined));
    /* are the elements equal ?*/
    if (CODE_SYMB_EQUAL(lt,arg1,arg2,cs)){
      /* gotcha */
      DEBUG(printf("get_n: got it\n"));
      *code_symbtab_element = cs;
      return 1;
    }
  }
  
  /* it is a new definition */
  DEBUG(printf("get_n: new definition\n"));
  if (nr_code_symbtab_entries == T_CODE_SYMBTAB_SIZE){
    fprintf(stderr,"  Error: Code generation - Symbol table overflow.\n");
    exit (62);
  }
  DEBUG7(printf("."));
  cs = code_symbtab + nr_code_symbtab_entries++;
  DEBUG7(printf("%s: %d: nr_code_symbtab_entries=%d\n",__FILE__,__LINE__,nr_code_symbtab_entries));
  DEBUG7(printf("%s: %d: cs=%lx\n",__FILE__,__LINE__,cs));
  cs->hexout_label_type = lt;
  cs->arg1 = arg1;
  cs->arg2 = arg2;
  cs->defined = 0;
  *code_symbtab_element = cs;
  
  return 0;
}

#else /*!HEX_OUT_LINEAR_SEARCH*/

/* i= ((lt | (arg1 << 20) | (arg2 << 4 )) % T_CODE_SYMBTAB_SIZE); ~~     */
/* i= (arg1*(19*arg2)*lt) % T_CODE_SYMBTAB_SIZE; bad                     */
/* i= (arg1*arg2*lt*197) % T_CODE_SYMBTAB_SIZE; worse                     */
/* i= (((arg1 << 13) | (arg2 << 9) | (lt << 4 )) % T_CODE_SYMBTAB_SIZE); 
   bad */
/* i= (((arg1 << 12) | (arg2 << 8) | (lt << 3 )) % T_CODE_SYMBTAB_SIZE); 
	~55% */
/* i= (((arg1 << 12) | (arg2 << 8) | (lt << 4 )) % T_CODE_SYMBTAB_SIZE); 
	~15% */
/* i= (((arg1 << 12) | (arg2 << 5) | (lt << 0 )) % T_CODE_SYMBTAB_SIZE); 
	~10x accesses */

i = (((arg1 << 12) | (arg2 << 8) | (lt << 4 )) % T_CODE_SYMBTAB_SIZE); 

cs = code_symbtab + i;

DEBUG(printf("get_n: hashcode=%d for: %d %d %d\n",i,lt,arg1,arg2));

while (1){
  if (CODE_SYMB_EQUAL(lt,arg1,arg2,cs)){
    /* gotcha */
    DEBUG(printf("get_n: got it\n"));
    *code_symbtab_element = cs;
    return 1;
  }
  if (!cs->hexout_label_type){
    /* new definition */
    DEBUG(printf("get_n: new definition\n"));
    DEBUG7(printf("."));
    nr_code_symbtab_entries++;
    cs->hexout_label_type = lt;
    cs->arg1 = arg1;
    cs->arg2 = arg2;
    *code_symbtab_element = cs;
    
    if (nr_code_symbtab_entries == T_CODE_SYMBTAB_SIZE-1){
      fprintf(stderr,"  Error: Code generation - Symbol table overflow.\n");
      exit (63);
    }
    return 0;
  }
  /* we got a collision */
  DEBUG(printf("get_n: collision\n"));
  H_O_STAT(nr_colls++);
  if (i == T_CODE_SYMBTAB_SIZE-1){
    DEBUG7(printf("+"));
    i= 0;
    cs= code_symbtab;
  }
  else {
    i++;
    cs++;
  }
}
}
#endif


/*****************************************************/
/*****************************************************/
/*  low-level routines concerning the HEX-file       */
/*****************************************************/
/*****************************************************/
/* put a word into memory at current address         */
/*****************************************************/
void put_word(pc,val)
  int pc;
  int val;
{
  DEBUG(printf ("%8.8x: %8.8x\n", pc, val));
  fprintf(code_file, FORMAT_CODE, MEM_CODE,pc,val,"%");
}


/*****************************************************/
/* print a symbol-table entry                        */
/*****************************************************/
void hex_out_put_symbol(name,type,arity)
  char *name;
  int type;
  int arity;
{
  static char s[100];

  hex_out_codestr(s,name);
  fprintf(code_file,FORMAT_CODE,SYMB_CODE,type,arity,s);
}


/*****************************************************/
/* hex_out_start_put_symbol();                       */
/*****************************************************/
void hex_out_start_put_symbol()
{
hex_out_nr_symbols = 0;
}


/*****************************************************/
/* hex_out_finish_put_symbol(nr_symbols);            */
/*****************************************************/
void hex_out_finish_put_symbol(nr_symbols)
  int nr_symbols;
{
  hex_out_nr_symbols = nr_symbols;
}


/*****************************************************/
/* hex_out_codestr                                   */
/* - copy s1 to s2                                   */
/* - convert whitespaces to \0zz                     */
/*****************************************************/
void hex_out_codestr (s2, s1)
  char *s2;
  char *s1;	
{ 
  while (*s2 = *s1) {
    if (*s1 == ' ') {
      *s2++ = '\\';
      *s2++ = '0';
      *s2++ = '4';
      *s2 = '0';
    }
    else if (*s1 == '\t') {
      *s2++ = '\\';
      *s2++ = '0';
      *s2++ = '1';
      *s2 = '1';
    }
    s1++;
    s2++;
  }
}


/*****************************************************/
/*****************************************************/
/*  interface for the optimizer                      */
/*****************************************************/
/*****************************************************/
/*
NOTE:
	Since the optimizer always takes only a pair
	(value,tag), we have to distinguish between
	labels and values.
	Therefore, we have the following macros:
	OPT_IS_VALUE(tag)
	OPT_IS_LABEL(tag)
	OPT_TAG_LABELTAG(tag)       tag -> tag of label
	OPT_TAG_VALUETAG(tag)       tag -> tag of value
	OPT_VALUETAG_TAG(tag)       tag of value -> tag
	OPT_LABELTAG_TAG(tag)       tag of label -> tag
	*/
/*****************************************************/

/*****************************************************/
/* interface routine:                                */
/*	from optimizer to enter_label                */
/*	val contains index of label                  */
/*	define it as current pc                      */
/*****************************************************/
void hex_out_optim_pnam(val,tag)
  int	val;
  int	tag;
{
  if (OPT_IS_VALUE(tag)){
    DEBUG_OPT(printf("hex_out_optim_pnam(%d,%d): value\n",
		     val,tag);)
      fatal("optimizer internal: must be a label\n",9);
  }
  
  DEBUG_OPT(if(!((code_symbtab+val)->defined))printf("pnam: label %d not defined\n",
						     val);)
    
  DEBUG_OPT(printf("hex_out_optim_pnam(%d,%d) [%d %d %d]\n",
		   val,tag,
		   (code_symbtab + val)->hexout_label_type,
		   (code_symbtab + val)->arg1,
		   (code_symbtab + val)->arg2);)
    
  (code_symbtab + val)->value = pc;
}


/*****************************************************/
/* plab:                                             */
/* interface routine from optimizer to               */
/*	use label                                    */
/* Note:                                             */
/*	val = index of label or value                */
/*            (if OPT_IS_VALUE(tag))                 */
/*	tag = tag                                    */
/*	we recalculate the index for better          */
/*      code-reuse                                   */
/*****************************************************/
void hex_out_optim_plab(val,tag)
  int	val;
  int	tag;
{
  if (OPT_IS_LABEL(tag)){  
    /* we want to output a label(reference) */
	
    DEBUG_OPT(printf("hex_out_optim_plab(%d,%d): [%d %d %d] @ %d\n",
		     val,tag,
		     (code_symbtab + val)->hexout_label_type,
		     (code_symbtab + val)->arg1,
		     (code_symbtab + val)->arg2,pc);)
      hex_out_use_label_tagged(
			       (code_symbtab + val)->hexout_label_type,
			       (code_symbtab + val)->arg1,
			       (code_symbtab + val)->arg2,
			       OPT_TAG_LABELTAG(tag));
  }
  else {
      /* is a (tagged) value */
    
    DEBUG_OPT(printf("hex_out_optim_plab(%d,%x [x]): tagged value @ %d\n",
		     val,tag,pc);)
      if (OPT_TAG_VALUETAG(tag)){
	hex_out_put_word_tagged(OPT_TAG_VALUETAG(tag),val);
      }
      else {
	hex_out_put_word(val);
      }
  }
}


/*****************************************************/
/* hex_out_finish_optim                              */
/*	interface routine for optimizer              */
/*	print all stored trees                       */
/*	using pnam() and plab()                      */
/*****************************************************/
void hex_out_finish_optim()
{
  DEBUG_OPT(printf("hex_out_finish_optim()\n");)
  print_dtree(hex_out_optim_pnam,hex_out_optim_plab);
}
