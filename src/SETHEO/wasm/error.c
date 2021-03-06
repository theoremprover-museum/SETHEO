/*#########################################################################
# File:			error.c
# SCCS-Info:		01/16/97, @(#)error.c	7.2
# Author:		J.Schumann
# Date:			23.12.87
# Context:		wasm (SETHEO)
# 
# Contents:		error functions
# Modifications:	
#	28.9.92		jsc
#	26.2.93		jsc	``pass2error''
#	12.1.95		jsc	fatal
#
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
#########################################################################*/


#include <stdio.h>
#include "wasm.h"		/* configuration of wasm		 */


extern int yylineno;
extern char token[];


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ warning
+ - report a warning in a particular line
+
+ special remarks:
+ - none
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void  warning (s)

    char           *s;		/* warning message			 */

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

    printf ("Warning in line %d : %s\n", yylineno, s);

} /*=====================================================================*/




/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ nferror
+ - report a non fatal error
+
+ special remarks:
+ - none
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void  nferror (s)

    char           *s;		/* error message			 */

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

    printf ("Non-fatal error in line %d : %s\n", yylineno, s);

} /*=====================================================================*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ fatal
+ - report a fatal error and exit
+
+ special remarks:
+ - none
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void  fatal (s,exitval)

    char           *s;		/* error message			 */
    int		   exitval;

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

    printf ("wasm: fatal error near line %d : %s\n", yylineno, s);
    exit(exitval);

} /*=====================================================================*/




/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ prttoken
+ - print out token to stdouT
+
+ special remarks:
+ - none
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void  prttoken (token)
char *token;

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

    printf (" %s\n", token);

} /*=====================================================================*/


/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ pass2error
+ - report an error on pass2 of the assembler
+
+ special remarks:
+ - none
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void  pass2error(s1,s2)

char *s1, *s2;

{

printf("Error in Pass2: %s: %s\n",s1,s2);

}

/*#######################################################################*/

