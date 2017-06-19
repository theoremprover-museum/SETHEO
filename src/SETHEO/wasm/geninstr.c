/*#########################################################################
# File:			geninstr.c
# SCCS-Info:		06 Apr 1994, @(#)geninstr.c	4.1
# Author:		J. Schumann
# Date:			23.12.87
# Context:		wasm (SETHEO)
#
# Contents:		generate instructions
#
# Modifications:
#	(when)		(who)		(what)
#					with optim.
#	31.10.88
#	22.1.89				new tag values (NTAGS)
#	10.5.89				strings
#	23.10.91	Max Moser	`restyled'
#	28.09.92	jsc
#	26.2.93		jsc 		change in retrive_undef
#					(output of label name)
#	12.1.95		jsc		length of code at beginning of file
#
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
#########################################################################*/


#include <stdio.h>
#include "wasm.h"
#include "tags.h"

#define DEBUG(X)

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ put_instr
+ - put instruction into code area
+   given:
+	opcode,
+	program counter location
+
+ special remarks:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void  put_instr (opcode, pc)
    int             opcode;
    int             pc;

{ /*=====================================================================*/

    DEBUG(printf ("%8.8x      %8.8x\n", pc, opcode));

    if(pc > maxpc) maxpc=pc;
    fprintf (fout, FORMAT_CODE, MEM_CODE, pc, opcode, "%");

} /*=====================================================================*/





/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ put_oper
+ - put operand into code area
+   value
+   pc:    program counter
+   tag:   UNDEF of value not defined or defined)
+
+ special remarks:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
#ifdef __STDC__
void  put_oper (int val,short tag,int pc)
#else
void  put_oper (val, tag, pc)
    int             val;
    short           tag;
    int             pc;
#endif
{ /*=====================================================================*/

    long            lpos,	/* ???					 */
                    ftell ();	/* ???					 */

/*-----------------------------------------------------------------------*/

    if (tag & UNDEF) {
	/* the operand is still undefined				*/

	DEBUG(printf ("%8.8x      UNDEFINED\n", pc));

	/* get the current position in the output file			*/
	lpos = ftell (fout);
	/* and note the not-yet defined position			*/
	enter_nodef (val, pc, lpos, 0);
	/* write out an empty space (to be filled up later)		*/
	fprintf (fout, FORMAT_CODE, MEM_CODE, pc, 0, "%");
    }
    else {	/* is defined */

	DEBUG(printf ("%8.8x      %8.8x\n", pc, val));

	fprintf (fout, FORMAT_CODE, MEM_CODE, pc, val, "%");
    }
    if(pc > maxpc) maxpc=pc;

} /*=====================================================================*/





/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ enter_start
+ - output the start-address into the object file
+   start @ pc
+ special remarks:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void  enter_start (pc)
    int             pc;

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

    DEBUG(printf ("start at %8.8x \n", pc));

    fprintf (fout, FORMAT_CODE, START_CODE, pc, 0, "%");
    if(pc > maxpc) maxpc=pc;

} /*=====================================================================*/





/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ put_tag_word
+ - put a tagged word into the code-area
+   (val,hosttag) at address pc
+   tag is UNDEF of VALUE
+
+ special remarks:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void  put_tag_word (pc, val, tag, hosttag)
    int             pc,
                    val,
                    tag,
                    hosttag;

{ /*=====================================================================*/

    long            lpos,	/* ???					 */
                    ftell ();	/* ???					 */

/*-----------------------------------------------------------------------*/

    if (tag & UNDEF) {

	DEBUG(printf ("%8.8x      UNDEFINED TAGGED\n", pc));

	lpos = ftell (fout);
	enter_nodef (val, pc, lpos, hosttag);

	fprintf (fout, FORMAT_CODE, MEM_CODE, pc, 
		(hosttag << (TAGSTART)) | val, "%");

    }
    else {	/* is defined */

	fprintf (fout, FORMAT_CODE, MEM_CODE, pc,
		(hosttag << (TAGSTART)) | val, "%");

    }
    if(pc > maxpc) maxpc=pc;

} /*=====================================================================*/





/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ put_word
+ - put an untagged word into the code area
+   with value at address pc
+
+ special remarks:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void  put_word (pc, val)
    int             pc,
                    val;

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

    fprintf (fout, FORMAT_CODE, MEM_CODE, pc, val, "%");
    if(pc > maxpc) maxpc=pc;

} /*=====================================================================*/






/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ end_code
+ - do the pass2 of assembling
+
+ special remarks:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void  end_code ()

{ /*=====================================================================*/

    int             rv,		/* ???					 */
                    val,	/* ???					 */
                    pc,		/* ???					 */
                    hosttag;	/* ???					 */
    long            lpos;	/* ???					 */

/*-----------------------------------------------------------------------*/

    currpos = ftell (fout);
    fprintf (fout, FORMAT_CODE, END_CODE, 0, 0, "%");

    /* have to resolve all undefs */
    while ((rv = retrieve_undef (&val, &pc, &lpos, &hosttag)) != 0) {
	if (rv != UNDEF) {

            DEBUG(printf ("Value %8.8x at %8.8x lpos= %lx\n", val, pc, lpos));

	    fseek (fout, lpos, 0);

	    fprintf (fout, FORMAT_CODE, MEM_CODE, pc,
			(hosttag << (TAGSTART)) | val, "%");

	}
    }

    /* write the maximal program counter (maxpc) at the */
    /* beginning of the file */
    fseek(fout,0L,0);
    fprintf (fout, FORMAT_CODE, MAXADDR_CODE, 0,maxpc, "%");
    fprintf (fout, FORMAT_CODE, NRSYMB_CODE, 0, nrsymbols, "%");

} /*=====================================================================*/






/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ start_code
+ - start the object code file
+
+ special remarks:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void  start_code ()

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

	maxpc=0;
	nrsymbols=0;
	fseek (fout, currpos, 0);
        fprintf (fout, FORMAT_CODE, MAXADDR_CODE, 0,maxpc, "%");
        fprintf (fout, FORMAT_CODE, NRSYMB_CODE, 0, nrsymbols, "%");

} /*=====================================================================*/








/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ codestr
+ - copy s1 to s2
+ - convert whitespaces to \0zz
+
+ special remarks:
+ - <e.g. called from>
+ - <side effects, return values>
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void  codestr (s2, s1)
    char           *s1,		/* ???					 */
                   *s2;		/* ???					 */

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

    while ((*s2 = *s1)) {
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

} /*=====================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ put_symbol_code
+ - put a .symb line into the hex-file
+
+ special remarks:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void  put_symbol_code(value1,value2,token)
	int 	value1,value2;	/* type and value of that symbol	*/
	char	*token;

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

	nrsymbols++;
	fprintf(fout,FORMAT_CODE,SYMB_CODE,value1,value2,token);

} /*=====================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ put_clause_code
+ - put a .clause line into the hex-file
+
+ special remarks:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void  put_clause_code(clause_nr,lit_nr)
	int 	clause_nr,lit_nr;

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

	fprintf(fout,FORMAT_CODE,CLAUSE_CODE,clause_nr,lit_nr,"%");

} /*=====================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ put_reduct_code
+ - put a .reduct line into the hex-file
+
+ special remarks:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void  put_reduct_code(pred_nr)
	int	pred_nr;

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

	fprintf(fout,FORMAT_CODE,RED_CODE,pred_nr,0,"%");

} /*=====================================================================*/





/*#######################################################################*/
