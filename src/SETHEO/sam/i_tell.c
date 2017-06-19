/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: i_tell.c
 * VERSION:			@(#)i_tell.c	8.1
 * DATE:			17.9.93
 * AUTHOR:			J. Schumann
 * NAME OF FILE:
 * DESCR:
 * MOD:
 *					6.12.93  must close old file first
 *					6.6.94	johann	new macros
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "tags.h"
#include "machine.h"
#include "symbtab.h"
/**/
#include "deref.h"
#include "errors.h"

#define INSTR_LENGTH		2


/******************************************************
 * tell/1
 * open named output file
 * failure and error message, if the argument is no constant
 * or if the file could not be opened
 *
 ******************************************************/
instr_result i_tell()
{
    WORD           *ga;

    ga = deref (ARGPTR(1), bp);

    if (ISCONSTANT(*ga)) {
		/* yes, it is a constant */
		/* */
		/* is an output file already open? */
		if (outputfile != stdout){
			/* yes, close it */
			fclose(outputfile);
			}
		/* now, try to open the new file in mode append */
		if (!(outputfile = fopen(GETPRINTNAME(*ga),"a"))){
			/* could not open the output file */
			outputfile = stdout;
			sam_error("Tell: Could not open output file",ga,1);
			return failure;
			}
        pc +=INSTR_LENGTH;
		return success;
    	}
	/* it is NOT a constant */
	sam_error("Tell: Argument must be a constant, but it is: ",ga,1);
    return failure;
}
