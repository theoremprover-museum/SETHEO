#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "symbtab.h"
/**/
#include "termcomplexity.h"
#include "deref.h"
#include "errors.h"

static int            termcomplexity();
static int            dag_termcomplexity();



int  tc_of_all_open_subgoals()
{
    register int         j,k,nr_sgls,arity,tc;
    WORD                *sgls,*args;
    literal_ctrl_block  *sgi;
    environ             *env;
    s_trail             *save_tr;

    /* Save trail pointer: */
    save_tr = tr;

    tc = 0;

    for ( env = bp ; 
          env != NULL  &&  tc <= maxtc ; 
          env = env->dyn_link ) {

        nr_sgls = GETNUMBER(env->lcbptr->nr_sgls);
        sgls =   (WORD*)env + SIZE_IN_WORDS(environ)
	       + GETNUMBER(env->lcbptr->nr_vars);
        for ( k = 0  ; k < nr_sgls ; k++ ) {
            if (GETTAG(*(sgls+k)) == NOT_SOLVED) 
                break;
        }
        /* Instead of for (k=0 ; ...) */
        /*
        k = env->current_sg + 1;    oder mit tp
        */

        for ( j = k ; j < nr_sgls  &&  tc <= maxtc ; j++ ) {
            sgi = (literal_ctrl_block*) GETPTR(*(sgls+j));
            args = code + GETVAL(sgi->argv);
            arity = GETARITY(sgi->ps_symb);
	    tc += dag_termcomplexity(args,arity,env,maxtc-tc);
        }
    }
    
    /* Untrail: */
    untrail(save_tr);
    tr = save_tr;

    return(tc);
}



static int  dag_termcomplexity(ga,arity,env,limit)
WORD     *ga;
int       arity;
environ  *env;
int       limit;
{
    WORD          *gai;
    register int   i,tc;
    register int   sub_tc;        /* tc of subterm */
    register int   sub_arity;     /* arity of subterm */

    tc = 0;

    for ( i = 0 ; i < arity  &&  tc <= limit ; i++ ) {

        gai = deref(ga+i,env);

        switch (GETTAG(*gai)) {

	    case T_CONST:  tc += COMPL_CONST;
                           break;

            case T_FVAR:   tc += COMPL_FVAR;
                           /* Mark seen variables: */
                           trail_var(gai);
                           GENOBJ(*gai,COMPL_DAG,T_MARK);
                           break;

            case T_MARK:   tc += COMPL_DAG;   /* dag complexity of seen */
		                              /* terms/variables        */
                           break;

	    case T_GTERM:  
            case T_NGTERM: 
            case T_CRTERM: sub_arity = GETARITY(*gai);
		           sub_tc = dag_termcomplexity(gai+1,sub_arity,env,
						       limit-tc);
		           tc += COMPL_FSYMB + sub_tc;
	                   /* Mark seen terms: */
		           trail_var(gai);
		           GENOBJ(*gai,COMPL_DAG,T_MARK);
                           break;

	    default:       sam_error("dag_termcomplexity: illegal tag",gai,3);

        }
    }

    return(tc);
}



static int  termcomplexity(ga,arity,env,limit)
WORD     *ga;
int       arity;
environ  *env;
int       limit;
{
    WORD          *gai;
    register int   i,tc;
    register int   sub_tc;        /* tc of subterm */
    register int   sub_arity;     /* arity of subterm */

    tc = 0;

    for ( i = 0 ; i < arity  &&  tc <= limit ; i++ ) {

        gai = deref(ga+i,env);

        switch (GETTAG(*gai)) {

	    case T_CONST:  tc += COMPL_CONST;
                           break;

            case T_FVAR:   tc += COMPL_FVAR;
                           break;

            case T_MARK:   tc += COMPL_FSYMB + GETNUMBER(*gai);
                           break;

	    case T_GTERM:  
            case T_NGTERM: 
            case T_CRTERM: sub_arity = GETARITY(*gai);
		           sub_tc = termcomplexity(gai+1,sub_arity,env,
						   limit-tc);
		           tc += COMPL_FSYMB + sub_tc;
	                   /* Avoid multiple processing: */
		           trail_var(gai);
		           GENOBJ(*gai,sub_tc,T_MARK);
                           break;

	    default:       sam_error("termcomplexity: illegal tag",gai,3);

        }
    }

    return(tc);
}
