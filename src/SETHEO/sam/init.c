/******************************************************
 *    S E T H E O                                     *
 *                                                    *
 * FILE: 	init.c
 * VERSION:	@(#)init.c	8.1 05 Jan 1996
 * DATE: 	5.10.88
 * AUTHOR: J. Schumann
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *		7.11.	global variables
 *		16.3.89 bug init chp
 *		25.4.89 statistcis
 *		22.10.89 stack > heap addresses
 *              27.6.91 STATISTICS-couters  changed   Goller
 *              13.01.92 CONSTRAINTS added by bschorer
 *	11.3.93	Mueller,Goller neue Constraints, Statistics
 *
 * 		16.9.93	jsc	unitlemma
 *	17.9.93		jsc		remove of old lemma stuff, tell,told
 *					copy-binding
 *	5.10.94		jsc	size check for data areas
 *	12.1.95		jsc	code & symb-size now with .hex file
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "defaults.h"
#include "codedef.h"
#include "symbtab.h"
/**/
#include "init.h"
#include "errors.h"

#ifdef STATPART
#include "sps.h"
#endif

#ifdef UNITLEMMA
#include "unitlemma.h"
#endif

void alloc_code();
void alloc_symbtab();

#define DEBUGM(X)

/******************************************************
 * init_setheo_memories                               *
 *
 *	allocate space for the memory areas of the SAM
 *	Note: 
 *      1) all areas into which tagged pointers are 
 *      pointing (stack,heap,constrain_stack), must
 *	start on addresses in the area
 *      between 0x00000000 .. 0x0fffffff for sun or 
 *      between 0xT0000000 .. 0xT0ffffff for hpux 
 *      where T is the PTR_MASK set in tags.h
 *      (this requirement is checked by ADDRESS_OUT_OF_RANGE(...) )
 *	2) the addresses in heap must be LOWER than that
 *	on the stack
 ******************************************************/
void  init_seth_mem (
#ifdef CONSTRAINTS
               c_stack_size, pu_trail_size,
#endif
	       stacksize,
	       heapsize,
	       codesize,
	       trailsize,
	       symb_size
#ifdef STATPART
	       , inf_step_size
#endif
)

#ifdef CONSTRAINTS
    int             c_stack_size;
    int             pu_trail_size;
#endif
    int             stacksize,
                    heapsize,
                    codesize,
                    trailsize,
                    symb_size;
#ifdef STATPART
    int	            inf_step_size;
#endif
{

    /********************************************************/
    /***      allocate memory for heap                    ***/   
    /*		TAGGED MEMORY                               */
    /********************************************************/
    //JSC-092016 heap = (WORD *) sam_malloc (heapsize);
    heap = (WORD *) malloc (heapsize*sizeof(WORD));
    //JSC hplast = heap + heapsize / sizeof (WORD);
    hplast = heap + heapsize;
    if(!heap)
       sam_error("heap-area not created", NULL, 2);

    DEBUGM(fprintf(stderr,"INIT: heap = %lx\n",heap));
    DEBUGM(fprintf(stderr,"INIT: heap = %ld\n",heap));
    DEBUGM(fprintf(stderr,"INIT: PTR_MASK=%lx, TAGSTART=%d\n",PTR_MASK,TAGSTART));
    DEBUGM(fprintf(stderr,"INIT: EOADDRSPACE=%lx\n",(1L <<TAGSTART)|PTR_MASK));
    DEBUGM(fprintf(stderr,"INIT: %d\n",(1L <<TAGSTART)|PTR_MASK));
    DEBUGM(fprintf(stderr,"INIT: hp=%d\n",(long)heap));
    DEBUGM(fprintf(stderr,"INIT: res=%d\n", (long)heap >= ((1L <<TAGSTART)|PTR_MASK)));

    if (ADDRESS_OUT_OF_RANGE(heap)) {
	sam_error("address for heap out of range",heap,3);
    }

    /********************************************************/
    /***      allocate memory for stack                   ***/   
    /*		TAGGED MEMORY                               */
    /********************************************************/
    //JSC-092016 stack = (WORD *) sam_malloc(stacksize); 
    stack = (WORD *) malloc(stacksize*sizeof(WORD)); 
    //JSC-092016 up_stack = stack + stacksize / sizeof (WORD);
    up_stack = stack + stacksize;
    if(!stack)
       sam_error("stack-area not created", NULL, 2);
    DEBUGM(fprintf(stderr,"INIT: stack = %lx\n",stack));
    if (!(stack > heap)) 
	sam_error ("FATAL:Addresses out of order", NULL, 2);

    DEBUGM(fprintf(stderr,"INIT: stack = %lx\n",stack));
 
    if (ADDRESS_OUT_OF_RANGE(stack)) {
	sam_error("address for stack out of range",stack,3);
    }

    /********************************************************/
    /*** allocate space for code (if not already done)    ***/
    /********************************************************/
    if (!code){
	alloc_code(codesize);
	pcstart=code;
    }
    /*************************************************************/
    /*** allocate space for symbol table (if not already done) ***/
    /*************************************************************/
    if (!symbtab){
	alloc_symbtab(symb_size);
    }

#ifdef CONSTRAINTS
    /********************************************************/
    /***      allocate memory for C-stack                 ***/   
    /***		TAGGED MEMORY                     ***/
    /********************************************************/
    //JSC-092016c_stack      = (WORD *) sam_malloc(c_stack_size);
    c_stack      = (WORD *) malloc(c_stack_size*sizeof(WORD));
    //JSC-092016 c_stack_end  = c_stack + c_stack_size / sizeof(WORD); 
    c_stack_end  = c_stack + c_stack_size; 
    if (!c_stack)
        sam_error("c-stack-area not created", NULL, 2);

    DEBUGM(fprintf(stderr,"INIT: c_stack = %lx\n",c_stack));

    if (ADDRESS_OUT_OF_RANGE(c_stack)) {
	sam_error("address for c_stack out of range",c_stack,3);
    }

#ifdef STATPART
/********************************************************/
/***      allocate memory to store Or-Nodes           ***/   
/********************************************************/
//JSC-092016    inf_step_area = (inf_step *) sam_malloc (inf_step_size);
    inf_step_area = (inf_step *) malloc (inf_step_size*sizeof(inf_step));
//JSC    inf_step_area_last = inf_step_area + inf_step_size / sizeof(inf_step);
    inf_step_area_last = inf_step_area + inf_step_size;
    inf_step_area_start = inf_step_area;
    if(!inf_step_area)
      sam_error("inference-step-area not created", NULL, 2);
#endif
#endif

    /********************************************************/
    /***      allocate memory for trail                   ***/   
    /********************************************************/
		/* jsc */
    //JSC-092016trail = (s_trail *) sam_malloc (trailsize+pu_trail_size);
    trail = (s_trail *) malloc ((trailsize+pu_trail_size)*sizeof(s_trail));
    //JSC trlast = trail + trailsize / sizeof (s_trail);
    trlast = trail + trailsize;
    if (!trail)
        sam_error("trail-area not created", NULL, 2);
    DEBUGM(fprintf(stderr,"INIT: trail = %lx\n",trail));

#ifdef CONSTRAINTS
    /***********************************************************/
    /***      allocate memory for Protocol-Unification-Trail ***/
    /***********************************************************/
	/*
    pu_trail      = (s_trail *) sam_malloc( pu_trail_size);
*/
	//JSC-092016 pu_trail = trail + trailsize/sizeof(s_trail);
	pu_trail = trail + trailsize;
    //JSC-092016 pu_tr_last    = (s_trail *) pu_trail + pu_trail_size / sizeof(s_trail);
    pu_tr_last    = (s_trail *) pu_trail + pu_trail_size;
    if (!pu_trail)
        sam_error("pu-trail-area not created", NULL, 2);
    DEBUGM(fprintf(stderr,"INIT: pu_trail = %lx\n",pu_trail));
#endif

#ifdef UNITLEMMA
    /* NOTE:
     *	addresses in this area must be HIGHER
     *	than those in the code-area
     *	unitlemmata
     */
    memory_Init();
    unitlemma_index = path_Create();
#endif

}



/******************************************************/
/* initialize setheo registers                        */
/******************************************************/
void  init_reg ()
{
  int     i;

    chp = (choice_pt *) stack - 1;
    bp = goal_bp = NULL;
    tr = trail;
    hp = heap;
    sp = stack;
    ra = code;
    pc = pcstart;
    boundflag = FALSE;
#ifdef STATPART
    s_boundfail = FALSE;
#endif
#ifdef STATISTICS1
    total_inf = 0;
#endif

#ifdef CONSTRAINTS
#ifdef ANTILEMMATA
    c_sp = c_stack;

    DEBUGM(printf("initializing delay...\n"));
    delayed_by_inf_bound = (c_list**) c_stack;
    c_sp += DEF_DELAYMAX * SIZE_IN_WORDS(c_list*);
    DEBUGM(printf("delay: %lx  cstack-end: %lx...\n",delayed_by_inf_bound + maxinf, c_stack_end));
    if ((WORD *)delayed_by_inf_bound + maxinf > c_stack_end){
	sam_error("FATAL: c-stack too small",NULL,2);
	}
    for ( i=0 ; i<maxinf ; i++ )
        if ((WORD *)(delayed_by_inf_bound + i) < c_stack_end){
		delayed_by_inf_bound[i] = NULL;
		}
    DEBUGM(printf("delay initialized.\n"));

    c_counter_sp = (c_counter *) c_sp;
    c_sp += SIZE_IN_WORDS(c_counter);

    c_counter_sp->length = -1;
    c_counter_sp->chp = chp;
    c_counter_sp->infcount = infcount;
    GENPTR(c_counter_sp->next, NULL);

    pu_tr = pu_trail;
#endif
#endif
}


/********************************************************/
/***      allocate memory for code-area               ***/   
/***	  size = size in WORDs                        ***/
/********************************************************/
void alloc_code(codesize)
int codesize;
{
//JSC-092016    code = (WORD *) sam_malloc (codesize *sizeof(WORD));
    code = (WORD *) malloc (codesize *sizeof(WORD));
    code_last = code + codesize;
    if(!code)
       sam_error("code-area not created", NULL, 2);
    DEBUGM(fprintf(stderr,"INIT: code = %lx\n",code));
    DEBUGM(fprintf(stderr,"INIT: codesize = %d\n",codesize));
}


/********************************************************/
/***      allocate memory for symbol-table            ***/   
/***      size in syel's                              ***/
/********************************************************/
void alloc_symbtab(size)
int size;
{
    //JSC-092016 symbtab = (syel *) sam_malloc (size * sizeof (syel));
    symbtab = (syel *) malloc (size * sizeof (syel));
    symbtab_last = symbtab + size;
    if(!symbtab)
       sam_error("symbol table not created", NULL, 2);
    DEBUGM(fprintf(stderr,"INIT: symbtab = %lx\n",symbtab));
    DEBUGM(fprintf(stderr,"INIT: symbtabsize = %d\n",size));
}
