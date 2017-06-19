/******************************************************
 *    S E T H E O                                     *
 *                                                    *
 * FILE: loadcode.c
 * VERSION:		@(#)lcode.c	8.1 05 Jan 1996
 * DATE: 23.12.87
 * AUTHOR: J. Schumann
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *		5.10. parameters
 *		31.10. loading common information
 *              23.7.91 symbtab and code-area overflow added by Goller
 *              13.9.91 pcstart added by Goller
 *				17.9.93	johann	symbol table allocation
 *		12.1.94	jsc	size of code & symbol table comes
 *				with the .hex file
 * BUGS:
 *	Note: if NRSYMB_CODE is given, the size of the symbol
 *	table just large enough to keep all symbols. If a
 *	read is allowed, this size should be incremented
 *
 ******************************************************/



#include <stdio.h>
#include <string.h>

#include "constraints.h"
#include "tags.h"
#include "machine.h"
#include "codedef.h"
#include "symbtab.h"
/**/
#include "lcode.h"
#include "errors.h"
#include "init.h"

#define DEBUGM(X)	
// #define DEBUG

/*********************************************************
 * load the contents of a .hex file into SAM's memory
 *********************************************************/
void  load_code (codefile,default_codesize,default_symb_size)
    FILE           *codefile;
    int            default_codesize;
    int            default_symb_size;
{
    int             pcount,
                    data;
    char            co;
    int             db,
                    curr_addr;
    char            str[M_SYMB_LENG + 4];


    db = curr_addr = 0;


    while (fscanf (codefile, FORM_READ, &co, &pcount, &data, str) == 4) {


#ifdef DEBUG
	printf ("read: :%c:%x:%x:%s:\n", co, pcount, data, str);
#endif
        
	switch (co) {

	  case MAXADDR_CODE:
		/* required size of the memory = max_addr+1 */
		alloc_code(data+1);
		break;

	  case NRSYMB_CODE:
		alloc_symbtab(data);
		break;

	  case MEM_CODE:
	    if (!code) alloc_code(default_codesize);
#ifdef WORD_64
            {
              short tag; long value;

              /* extract the WORD (coded in old format by inwasm) into the new fields:
                 + tag
                 + idflag
                 + value
                 */
              tag = (data & TAGMASK)>> TAGSTART;
              value = data & VALMASK;
              if (tag == T_CONST) {
                /* may or may not be a symbol */
                if (value & IDTAG) {
                  GENSYMBOL(code[pcount],(value&IDMASK),T_CONST);
                } else {
                  GENOBJ(code[pcount],value,T_CONST);
                }
              } else if ((tag==T_GTERM)||(tag==T_NGTERM)||(tag==T_CRTERM)) {
                GENSYMBOL(code[pcount],(value&IDMASK),tag);
              } else if (tag==T_EMPTY) {
                if (value & IDTAG) {
//JSC-092016
		  DEBUGM(printf("%s:%d: pcount=%d\n",__FILE__,__LINE__,pcount));
		  DEBUGM(printf("%s:%d: value=%d\n",__FILE__,__LINE__,value));
		  DEBUGM(printf("%s:%d: value&IDMASK=%d\n",__FILE__,__LINE__,value&IDMASK));
		  DEBUGM(printf("%s:%d: tag=%d\n",__FILE__,__LINE__,tag));
                  GENSYMBOL(code[pcount],(value&IDMASK),tag);
                } else {
		  DEBUGM(printf("%s:%d: code=%lx\n",__FILE__,__LINE__,code));
		  DEBUGM(printf("%s:%d: pcount=%d\n",__FILE__,__LINE__,pcount));
		  DEBUGM(printf("%s:%d: value=%d\n",__FILE__,__LINE__,value));
		  DEBUGM(printf("%s:%d: value&IDMASK=%d\n",__FILE__,__LINE__,value&IDMASK));
		  DEBUGM(printf("%s:%d: tag=%d\n",__FILE__,__LINE__,tag));
		  DEBUGM(fflush(stdout));
                  GENOBJ(code[pcount],(value&IDMASK),tag);
                }
              } else {
		  DEBUGM(printf("%s:%d: pcount=%d\n",__FILE__,__LINE__,pcount));
		  DEBUGM(printf("%s:%d: value=%d\n",__FILE__,__LINE__,value));
		  DEBUGM(printf("%s:%d: tag=%d\n",__FILE__,__LINE__,tag));
		  DEBUGM(fflush(stdout));
		  
                GENOBJ(code[pcount],value,tag);
              }
            }
#else            
            code[pcount] = (WORD) data;
#endif            
	    curr_addr = pcount;
	    if (code + pcount > code_last) {
	       sam_error ("code-area overflow", NULL, 2);
	    }
	    break;

	  case START_CODE:
	    if (!code) alloc_code(default_codesize);
	    pcstart = code + pcount;
	    curr_addr = pcount;
	    break;

	  case SYMB_CODE:
	    if (!symbtab) alloc_symbtab(default_symb_size);
	    if (symbtab + nr_symbols > symbtab_last) {
	       sam_error ("symbtab overflow", NULL, 2);
	    }
	    symbtab[nr_symbols].name = strdup(str);
	    symbtab[nr_symbols].type = pcount;
	    symbtab[nr_symbols++].arity = data;
	    break;

	  case END_CODE:
	  case RED_CODE:
	  case CLAUSE_CODE:
	    break;

	  default:
	    sam_error ("Error on loading", NULL, 2);
	    exit (3);
	}
    }
    pcstart = code;
}

