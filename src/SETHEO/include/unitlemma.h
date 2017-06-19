#ifndef UNITLEMMA_H
#define UNITLEMMA_H

#ifdef extern
#define EXTERN_DEF__
#undef extern
#endif

#include "path/path.h"

#ifdef EXTERN_DEF__
#define extern
#endif

/*------------------------------------------------------------------------
------------------------------------------------------------------------*/

/***********************************************************************/
/* select the kind of memory management                                */
/***********************************************************************/
#ifdef STD_ALLOC
#	define MALLOC(X) malloc(X)
#	define FREE(X,Y) free(X)
#else
#	include "path/newmemory.h"
#	define MALLOC(X)	memory_Malloc(X)
#	define FREE(X,Y) 	memory__Free(X,Y)
#   ifndef UNITLEMMA
#	define MALLOC(X) malloc(X)
#	define FREE(X,Y) free(X)
#   endif
#endif


/***********************************************************************/
/* MACROS
*	* to determine the size of the various memory blocks
*		according to the arity of predicates and functors
*	* determine code- and argv-offsets
***********************************************************************/
	/* size of a term with functor FUNCTOR */
	/* Such a term needs 
	 * ARITY + 3 WORDS for arguments, functor, T_EOSTR, and term_block_ptr
	 * the size is given in Bytes 
	 **/
#define TERMSIZE(FUNCTOR) ((GETARITY(FUNCTOR) + 3)*sizeof(WORD))

	/* size of the compiled fact (without terms) */
	/* the size is given in Bytes 
	 **/
#define FACTSIZE(ARITY) (sizeof(unit_lemma_ctrl_block) +		\
		3 * sizeof(WORD) +		    /* nalloc lcb + ndealloc	*/ \
	    3*ARITY * sizeof(WORD))	/* unif* instructions + argv-entries */

	/* size if the compiled fact without complex terms,
	 * given the fact
	 **/
#define THISFACTSIZE(FACT) FACTSIZE(GETARITY(fact->lcb.ps_symb))

	/* where does the code start wrt. the start of the fact */
	/* The offset is given in *WORDs
	 **/
#define CODE_OFFSET (sizeof(unit_lemma_ctrl_block) / sizeof(WORD *))

	/* where does the argument vector starts wrt. the compiled
	 * fact. The offset is given in *WORDs
	 **/
#define ARGV_OFFSET(ARITY) ((sizeof(unit_lemma_ctrl_block) +				\
		3 * sizeof(WORD) +		/* nalloc lcb and ndealloc */ 			\
		2*ARITY * sizeof(WORD))	/* unif* instructions	*/		\
		/ sizeof(WORD *))

/***********************************************************************/
/* MACROS
***********************************************************************/
#ifdef UNITLEMMA_STATISTICS
#	define UL_STAT(X)	X
#else
#	define UL_STAT(X)	
#endif

/***********************************************************************/
/* Typedefs
***********************************************************************/
	/***************************************************************/
	/* this structure comprises the beginning of a unit-lemma
	 * after this structure, a variable-size part for code
	 * and the argument-vector is placed
	 * the size of the entire unit-lemma (without complex terms)
	 * is determined by THISFACTSIZE
	 **/
typedef struct struct_unit_lemma_ctrl_block {
	WORD			*term_block_ptr;
	literal_ctrl_block	lcb;
	} unit_lemma_ctrl_block;


	/***************************************************************/
	/* enum for the status of a unit lemma  in the FPA index */
typedef enum {
	NONE,
	lemma_valid,			/* lemma can be used 		*/
	lemma_used,			/* lemma has been used		*/
	lemma_deleted,			/* lemma is marked deleted,     */
                                        /* but has been used            */
	lemma_deleted_not_used		/* lemma can be deleted, since  */
                                        /* it has not been used		*/
	} lemma_status;


	/***************************************************************/
	/* this structure contains the control-information for
	 * a lemma kept in the FPA path index
	 **/
typedef struct {
	choice_pt		*chp;		/* smallest chp, when the lemma
						   has been used,
						   NULL otherwise */
	unit_lemma_ctrl_block *ulemma;	/* pointer to the compiled fact */
	WORD			data;	/* in this version, we only
					   allow to keep ONE constant
					   attached to the lemma
					   further releases: WORD *
					 */
	lemma_status	status;		/* status of the lemma */
	} unit_lemma_storage_ctrl_block;

	/***************************************************************/
	/* statistics for unit-lemmata
	 **/
typedef struct struct_unitlemma_statistics {
		/******** gen lemma ********/
	int		genlemma_entered;	/* # of entering genlemma */
	int		genlemma_disabled;	/* number of times, the 
                                                   argument of genlemma is 0 */
	int		genlemma_no_unit_lemma; /* number of times, the lemma 
                                                   is no unit lemma */
	int		gl_gen_index;		/* # of times, a more gen is retrieved*/
	int		gl_gen_found;		/* # of times, a more general was found */
	int		gl_inst_index;
	int		gl_inst_found;
	int		gl_lemma_stored;
	int		gl_failure_exit;
		/******** use lemma ********/
	int		uselemma_entered;	/* # of entering the uselemma instr. */
	int		ul_no_unifiable;	/* # of times, no unifable could be found */
	int		ul_lemma_pushed;	/* # of times, a lemma's lcb */
						/* was pushed on the stack   */
	} unitlemma_statistics;


/***********************************************************************/
/* Functions
***********************************************************************/
unit_lemma_ctrl_block *compile_to_fact();

/***********************************************************************/
/* externals for unit-lemmata
***********************************************************************/
extern path_INDEX unitlemma_index;

extern int	  unitlemma_nr_valid

#ifdef extern
				= 0
#endif
					;

extern int	unitlemma_tree
#ifdef extern
				= 0
#endif
					;

#ifdef UNITLEMMA_STATISTICS
	extern	unitlemma_statistics	ul_stat;
#endif


/***************************************************************
 *  Function-Declarations
 ***************************************************************/

unit_lemma_ctrl_block  *compile_to_fact _ANSI_ARGS_((WORD predsymb,WORD *argptr));
int compile_argument _ANSI_ARGS_((WORD *argptr,WORD **cpp,WORD **avp,unit_lemma_ctrl_block *code_ptr));
void free_compiled_fact _ANSI_ARGS_((unit_lemma_ctrl_block *fact));
void disassemble_compiled_fact _ANSI_ARGS_((FILE *fout,unit_lemma_ctrl_block *fact));
void print_compiled_fact _ANSI_ARGS_((FILE* fout, unit_lemma_ctrl_block* fact));
void print_compiled_term _ANSI_ARGS_((FILE* fout, unit_lemma_ctrl_block* fact, int negated));

#endif
