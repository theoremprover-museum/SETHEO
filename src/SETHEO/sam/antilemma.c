/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: antilemma.c                                  */
/* VERSION:                                           */
/* DATE:                                              */
/* AUTHOR: Ortrun Ibens                               */
/* NAME OF FILE:                                      */
/* DESCR: routines for antilemma handling             */
/* MOD:                                               */
/* MOD:                                               */
/* BUGS:                                              */
/******************************************************/


#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "defaults.h"
#include "machine.h"
#include "symbtab.h"
#include "unitlemma.h"
/**/
#include "antilemma.h"
#include "deref.h"
#include "errors.h"
#include "univ_constr.h"	


#ifdef ANTILEMMATA

static WORD  *copy_right_side();
static WORD  *copy_term();
int   genantilemma();

extern unsigned long debug_level;


choice_pt  *immediate_antilemma_fail() 
{
    choice_pt           *anl_chp,*ch;
    WORD                *sgls;
    int                 nr_sgls;

    /*******************************************************************/
    /* Try to generate an antilemma only if                            */
    /* - anl_constr is switched on                                     */
    /* and                                                             */
    /* - chp is solved                                                 */
    /* and                                                             */
    /* - if lpanl is on, the actual environment must be a fact         */
    /*   or the last inference must be a reduction step                */
    /*******************************************************************/
    if (      anl_constr
	&&  chp->end_trail != NULL
	&&  (!lpanl  ||  chp->head_bp == NULL
	             ||  GETNUMBER(chp->head_bp->lcbptr->nr_sgls) == 0)  ) {
	
	do {
	    /***********************************************************/
	    /* Get choicepoint for which the antilemma is generated:   */
	    /* - if lpanl is on: the actual choicepoint                */
	    /* - otherwise: the root of the maximal solved subtree     */
	    /*              which contains the actual choicepoint      */
	    /***********************************************************/
	    anl_chp = chp;
	    if (!lpanl) {
		while ((ch = anl_chp->goal_bp->chp)  &&  ch->end_trail)
		    anl_chp = ch;
	    }
	    
	    /***********************************************************/
	    /* untrail until end of maximal solved subtree             */
	    /***********************************************************/
	    untrail(anl_chp->end_trail);   
	    tr = anl_chp->end_trail;
	    
	    nr_sgls = GETNUMBER(anl_chp->goal_bp->lcbptr->nr_sgls);
	    sgls =   (WORD*) anl_chp->goal_bp 
		   + SIZE_IN_WORDS(environ)
		   + GETNUMBER(anl_chp->goal_bp->lcbptr->nr_vars);
    
	    /***********************************************************/
	    /* try to generate antilemma                               */
	    /* and handle immediate antilemma fail                     */
	    /***********************************************************/
	    if (anl_chp->goal_bp->tp < sgls+nr_sgls-1) {
	      if ( !(genantilemma(anl_chp)) ) { 
		/* immediate antilemma fail */
#ifdef STATISTICS1
		c_anl_fails++;
#endif
#ifdef STATISTICS2
		c_immed_anl_fails++;
#endif
		return(anl_chp->ch_link);
	      }
	    }
	    
	    anl_chp->end_trail = NULL;
	}
	while (anl_chp != chp);
	
    }

    return(NULL);
}




int  genantilemma(anl_chp)
choice_pt  *anl_chp;
{
    a_list        *antilemma_memory;
    WORD          *antilemma_memory_left,*antilemma_memory_right,*fr_vars;
    WORD          *ptr;
    int            arity,nr_vars;
    register int   i,j;
    s_trail       *save_tr;


    nr_vars = anl_chp->nr_fvars;
    fr_vars = anl_chp->block_fvars;

    arity = 0;
    for ( i = 0 ; i < nr_vars ; i++ ) {
        if (GETTAG(*(fr_vars+i)) == T_BVAR) {
            ptr = GETPTR(*(fr_vars+i));
            if (GETTAG(*ptr) != T_FVAR) 
                arity++;
        }
    }
    if (arity == 0) {
        if (inf_bound  &&  anl_chp->end_infcount > anl_chp->infcount) {
            antilemma_memory = (a_list*) MALLOC(sizeof(a_list));
            if (antilemma_memory==NULL) {
                sam_error("genantilemma: Could not allocate memory for antilemma_memory.",NULL,0); 
                fflush(stdout);
                return(2);
            }  
	    if (ADDRESS_OUT_OF_RANGE(antilemma_memory)) {
	        sam_error("genantilemma: address for antilemma_memory out of range",(WORD*)antilemma_memory,3);
	    }
            antilemma_memory->active = 1;
            antilemma_memory->arity = 0;
            antilemma_memory->left = NULL;
            antilemma_memory->right = NULL;
            antilemma_memory->infcount = anl_chp->end_infcount;
            antilemma_memory->next = anl_chp->anl_list;
            anl_chp->anl_list = antilemma_memory;
#ifdef STATISTICS1
            anlcount++;
#endif
            return(1);
        }
#ifdef STATISTICS1
        anlcount++;
#endif
        return(0);       /* immediate antilemma fail  */
    }

    if (total_inf <= anl_chp->end_total_inf + mincost)
        return(2);       /* not enough inferences spent on search
                            => antilemma not important            */
    
    /*********************************************************************/
    /* Get a separate memory to save the left side of the antilemma:     */
    /* arity WORDS to save pointers to the left side variables of the    */
    /* antilemma.                                                        */
    /*********************************************************************/
    antilemma_memory_left = (WORD*) MALLOC(arity*sizeof(WORD));
    if (antilemma_memory_left == NULL) {
        sam_error("genantilemma: Could not allocate memory for antilemma_memory_left.",NULL,0);
        fflush(stdout);
        return(2);
    }
    if (ADDRESS_OUT_OF_RANGE(antilemma_memory_left)) {
	sam_error("genantilemma: address for antilemma_memory_left out of range",antilemma_memory_left,3);
    }

    /*********************************************************************/
    /* Put pointers to left side variables in the antilemma_memory_left. */ 
    /*********************************************************************/
    j = 0;
    for ( i = 0 ; i < nr_vars ; i++ ) {
        if (GETTAG(*(fr_vars+i)) == T_BVAR) {
            ptr = GETPTR(*(fr_vars+i));
            if (GETTAG(*ptr) != T_FVAR) {
                GENPTR(*(antilemma_memory_left+j),ptr);
                j++;
	    }
	}
    }

    /**********************************************************************/
    /* Process the right side of the antilemma in a similar way:          */
    /* Get a separate memory for the right side of the antilemma and save */ 
    /* the right side. Before calling copy_right_side() save the          */
    /* trail pointer, afterwards untrail up to the saved trail pointer.   */
    /* The reason for this is the handling of free variables in the term  */
    /* (for explanations see the T_FVAR case of copy_term()).             */
    /**********************************************************************/
    save_tr = tr;
    antilemma_memory_right = copy_right_side(antilemma_memory_left,arity,anl_chp);
    untrail(save_tr);
    tr = save_tr;
    if (antilemma_memory_right == NULL) {
        return(2);
    }

    /*********************************************************************/
    /* Get a separate memory to save the antilemma.                      */
    /*********************************************************************/
    antilemma_memory = (a_list*) MALLOC(sizeof(a_list));
    if (antilemma_memory==NULL) {
        sam_error("genantilemma: Could not allocate memory for antilemma_memory.",NULL,0); 
        fflush(stdout);
        return(2);
    }
    if (ADDRESS_OUT_OF_RANGE(antilemma_memory)) {
	sam_error("genantilemma: address for antilemma_memory out of range",(WORD*)antilemma_memory,3);
    } 

    /*********************************************************************/
    /* Put activation marker, arity, pointer to left side, pointer to    */
    /* right side and inference counter in the antilemma_memory and      */
    /* integrate antilemma_memory in the list of antilemmata of anl_chp: */
    /*********************************************************************/
    antilemma_memory->active = 1;
    antilemma_memory->arity = arity;
    antilemma_memory->left = antilemma_memory_left;
    antilemma_memory->right = antilemma_memory_right;
    antilemma_memory->infcount = anl_chp->end_infcount;
    antilemma_memory->next = anl_chp->anl_list;
    anl_chp->anl_list = antilemma_memory;

#ifdef STATISTICS1
    anlcount++;
#endif

    return(1);
}



static WORD  *copy_right_side(left,arity,anl_chp)
WORD       *left;
int         arity;
choice_pt  *anl_chp;
{
    WORD             *memory,*gai,*term;
    WORD             *next[1];
    register int      j;

    /************************************************************************/
    /* Get a separate memory to save the right side of the antilemma:       */
    /* - arity WORDS to save the arguments rsp. pointers to the arguments.  */
    /* - 1 WORD to save a pointer to the next allocated block of the memory */
    /*   (will be usefull for the FREE operation)                           */
    /************************************************************************/ 
    memory = (WORD*) MALLOC((1+arity)*sizeof(WORD));
    if (memory==NULL) {
        sam_error("genantilemma:copy_right_side: Could not allocate memory.",NULL,0);
        fflush(stdout);
        return(NULL);
    }
    if (ADDRESS_OUT_OF_RANGE(memory)) {
	sam_error("genantilemma:copy_right_side: address for memory out of range",memory,3);
    }   

    /**********************************************************************/
    /* Initialize the cell for the pointer to the next allocated block of */
    /* the memory with NULL.                                              */
    /**********************************************************************/
    GENPTR(*(memory+arity), NULL);    
                                            
    /***********************************************************************/
    /* Keep the address for the pointer to the next allocated block of the */
    /* memory.                                                             */
    /***********************************************************************/
    *next = memory+arity;

    /*********************************************************************/
    /* Handle the arguments:                                             */
    /*********************************************************************/
    for ( j=0 ; j<arity ; j++ ) {

        gai = deref(left+j,NULL);

        switch (GETTAG(*gai)) {

            case T_FVAR:   if (gai < (WORD*) anl_chp) {  
                               GENPTR(*(memory+j),gai);
	                   }
                           else {
                               trail_var(gai);
                               GENPTR(*gai,memory+j);
                               GENOBJ(*(memory+j), (WORD_cast)NULL,T_STRVAR);
	        	   }
                           break;

            case T_STRVAR: GENPTR(*(memory+j),gai);
                           break;
  
            /*************************************************************/
            /* If the argument is a constant, just make a copy of it.    */
            /*************************************************************/
	    case T_CONST:  *(memory+j) = *gai;
                           break;

            /*************************************************************/
            /* If the argument is a ground term (in the code area),      */
            /* generate a pointer to it.                                 */
            /*************************************************************/
            case T_GTERM:  GENPTR(*(memory+j),gai);
	                   break;

            /*************************************************************/
            /* If the argument is a non ground term (in the code area)   */
            /* or a copy of a non ground term (on the heap), call        */
            /* copy_term().                                              */
            /*************************************************************/
            case T_NGTERM:

            case T_CRTERM: term = copy_term(gai,next,anl_chp);

                           if (term == NULL)    return(NULL);
                           GENPTR(*(memory+j),term);
	                   break;

	    default:       sam_error("genantilemma:copy_right_side: illegal tag",gai,3);
                           return(NULL);
	}
    }

    return(memory);
}



static WORD  *copy_term(ga,next,anl_chp)
WORD       *ga,**next;
choice_pt  *anl_chp;
{
    int               arity;
    WORD              *memory,*gai,*term;
    register int      i;

    /************************************************************************/
    /* Get a separate memory to save a term:                                */
    /* - 1 WORD to save the function symbol.                                */
    /* - arity WORDS to save the arguments rsp. pointers to the arguments.  */
    /* - 1 WORD for the T_EOSTR tag.                                        */
    /* - 1 WORD to save a pointer to the next allocated block of the memory */ 
    /*   (will be usefull for the FREE operation)                           */
    /************************************************************************/
    arity = GETARITY(*ga);
    memory = (WORD*) MALLOC((3+arity)*sizeof(WORD));
    if (memory==NULL) {
        sam_error("genantilemma:copy_term: Could not allocate memory.",NULL,0); 
        fflush(stdout);
        return(NULL);
    }
    if (ADDRESS_OUT_OF_RANGE(memory)) {
	sam_error("genantilemma:copy_term: address for memory out of range",memory,3);
    }

    /*********************************************************************/
    /* Put function symbol and T_EOSTR tag in the memory.                */
    /*********************************************************************/
    GENSYMBOL(*memory, GETSYMBOL(*ga),T_CRTERM);
    GENOBJ(*(memory+arity+1),0,T_EOSTR);

    /*********************************************************************/
    /* Initialize the cell for the pointer to the next allocated block of*/
    /* the memory with NULL.                                             */
    /*********************************************************************/
    GENPTR(*(memory+arity+2),NULL);    
                         
    /*********************************************************************/
    /* Fill the next-pointer-cell of the last allocated block.           */
    /*********************************************************************/
    GENPTR(**next,memory);                 
            
    /*********************************************************************/
    /* Keep the address for the pointer to the next allocated block of   */
    /* the memory.                                                       */
    /*********************************************************************/
    *next = memory+arity+2;

    /*********************************************************************/
    /* Handle the arguments:                                             */
    /*********************************************************************/
    for ( i=1 ; i<=arity ; i++ ) {

        gai = deref((ga+i),NULL);

        switch (GETTAG(*gai)) {
  
            /*************************************************************/
            /* If the argument is a constant, just make a copy of it.    */
            /*************************************************************/
	    case T_CONST:  *(memory+i) = *gai;
                           break;

            /*************************************************************
             * If the argument is a free variable, generate a structure 
             * variable cell. 
             * REMARK: Of course every appearance of the same free variable 
             * must be assigned to the same structure variable. This is 
             * realized in the following way: If a free variable is referenced 
             * for the first time, it is trailed, a structure variable cell 
             * is generated and a T_BVAR tagged pointer from the free variable
             * cell to the structure variable cell is generated. If the 
             * same free variable cell is referenced again, the deref() 
             * procedure will automatically find the structure variable cell 
             * (because of the T_BVAR tagged pointer). Then only a pointer 
             * to the structure variable cell must be generated (see the 
             * T_STRVAR case). The untrailing will be done in the 
             * genantilemma() procedure. 
             *************************************************************/
            case T_FVAR:   if (gai < (WORD*) anl_chp) {
                               GENPTR(*(memory+i),gai);
	                   }
                           else {
                               trail_var(gai);
                               GENPTR(*gai,memory+i);
                               GENOBJ(*(memory+i),(WORD_cast)NULL,T_STRVAR);
	        	   }
                           break;

            /*************************************************************/
            /* Generate a pointer to the dereferenced structure variable */
            /* cell. For explanations see the T_FVAR case.               */
            /*************************************************************/
            case T_STRVAR: GENPTR(*(memory+i),gai);
                           break;

            /*************************************************************/
            /* If the argument is a ground term (in the code area),      */
            /* generate a pointer to it.                                 */
            /*************************************************************/
            case T_GTERM:  GENPTR(*(memory+i),gai); 
	                   break;

            /**************************************************************/
            /* If the argument is a non ground term (in the code area) or */
            /* a copy of a non ground term (on the heap), call            */
            /* copy_term() recursively.                                   */
            /**************************************************************/
            case T_NGTERM:
            case T_CRTERM: term = copy_term(gai,next,anl_chp);
                           if (term == NULL)    return(NULL);
                           GENPTR(*(memory+i),term); 
	                   break;

	    default:       sam_error("genantilemma:copy_term: illegal tag",gai,3);
                           return(NULL);
	}
    }

    return(memory);
}



void  free_anl_list(list)
a_list  *list;
{   a_list  *anl,*next_anl;
    WORD    *left_side,*right_side,*term,*next_term;
    int      arity;

    anl = list;
    while (anl != NULL) {

        arity = anl->arity;
        if (arity > 0) {
            /***************************************************************/
            /* free left side                                              */
            /***************************************************************/
            left_side = anl->left;
            FREE( (void*)left_side , arity*sizeof(WORD) );

            /***************************************************************/
            /* free top level of right side                                */
            /***************************************************************/
            right_side = anl->right;
            term = GETPTR(*(right_side+arity));
            FREE( (void*)right_side , (arity+1)*sizeof(WORD) );

            /***************************************************************/
            /* free right side terms                                       */
            /***************************************************************/
            while (term != NULL) {
                arity = GETARITY(*term);
                next_term = GETPTR(*((WORD*) (term+arity+2)));
                FREE( (void*)term , (arity+3)*sizeof(WORD) );
                term = next_term;
            }
	}

        /*******************************************************************/
        /* free antilemma and go on with next antilemma                    */
        /*******************************************************************/
        next_anl = anl->next;
        FREE( (void*)anl , sizeof(a_list) );
        anl = next_anl;

    }
}


int  anl_gen_constr() 
{
    a_list              *anl;
    int                  arity;
    WORD                *left,*right;
    c_counter           *anl_counter;
    choice_pt           *ch;
    s_trail             *save_tr;
    c_counter           *pseudo_constr;
    int                  anl_infcount;

    pu_tr = pu_trail;                

    for ( ch = chp ; ch >= (choice_pt*)stack ; ch = ch->goal_bp->chp ) {

        /*******************************************************************/
        /* for every Antilemma of this Choicepoint                         */
        /*******************************************************************/
        for ( anl = ch->anl_list ; anl != NULL ; anl = anl->next ) {

            /***************************************************************/
            /* if antilemma is active                                      */
            /***************************************************************/
            if (anl->active) {

                trail_int(&(anl->active));
                anl->active = 0;
                arity = anl->arity;
                left = anl->left;
                right = anl->right;

                /***********************************************************/
                /* Save trail pointer.                                     */
                /***********************************************************/
                save_tr = tr;

                /***********************************************************/
                /* try protocol unification:                               */
                /* - if successful: generate subconstraint                 */
                /* - else: untrail the protocol unification trail          */
                /***********************************************************/
                if (debug_level&0x4) fprintf(stderr," ^a ");
                if (un_protoco(left,NULL,right,NULL,arity)) {

                    anl_infcount = anl->infcount;

                    /*******************************************************/
                    /* if no bindings were made:                           */
                    /* check if immediate antilemma fail                   */
                    /*******************************************************/
                    if (pu_tr == pu_trail) {
                        if (inf_bound  &&  anl_infcount > infcount) {
                            pseudo_constr = c_gencounter(ANTILEMMA,ch,anl_infcount);
                            delay_anl_constr(pseudo_constr,INF_BOUND);
                            continue;
                        }
                        return(0);     /* immediate antilemma fail */
		    }

                    /*******************************************************/
                    /* generate antilemma constraint                       */
                    /*******************************************************/
                    anl_counter = c_gencounter(ANTILEMMA,ch,anl_infcount);
                    c_gen_optimize(anl_counter);
#ifdef STATISTICS1
		    gen_anl_constrs++;
#endif  
	        }
                else {
		    /*******************************************************/
                    /* Untrail after failed protocol unification.          */
                    /*******************************************************/
                    untrail(save_tr);
                    tr = save_tr;

                    pu_untrail();                    
	        }
	    }
	}
    } 
   
    return(1);       
}



void  delay_anl_constr(constr,bound)
c_counter  *constr;
int         bound;
{
    c_list   *new_to_delay;
    int       i;

    if (constr->infcount <= DEF_DELAYMAX) {

        new_to_delay = (c_list*) c_sp;
        c_sp += SIZE_IN_WORDS(c_list);

        new_to_delay->zaehler = constr;

        switch (bound) {
            case INF_BOUND:  i = constr->infcount;
                             new_to_delay->next = DELAYED_BY_INF_BOUND(i);
                             trail_ptr((void**)&(DELAYED_BY_INF_BOUND(i)));
                             DELAYED_BY_INF_BOUND(i) = new_to_delay;
                             break;
            default:         sam_error("delay_anl_constr: No such bound !",
                                       NULL,0);
        }
    }
}

#endif
