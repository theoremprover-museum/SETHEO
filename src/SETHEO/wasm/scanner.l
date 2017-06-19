%{ 
/*#########################################################################
# File:			scanner.l
# SCCS-Info:		01/02/97, @(#)scanner.l	7.1
# Author:		J. Schumann
# Date:			23.12.87
# Context:		wasm (SETHEO)
# 
# Contents:		scanner for SETHEO Assembler tokens
#
# Modifications:	
#	(when)		(who)		(what)
#	25.2.93		Goller		strvar -> cstrvar
#					TSTRVAR -> TCSTRVAR
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
#########################################################################*/

#define PASS2(X)	X

PASS2(#include "wasm.h")	/* wasm configurations			 */
PASS2(#include "parser.h")	/* definitions of tokens from parser	 */


/*-------------------------------------------------------------------------
- local and global data
-------------------------------------------------------------------------*/

int 		i;		/* loop counter				 */



/**************************************************************************
* definitions of tokens and actions
**************************************************************************/
%}
%p 5000
%e 1400
%n 650
%%

#include "w_scan.h"

void                    return TVOID;
cstrvar			return TCSTRVAR;

cref			return TCREF;
var                     return TVAR;
global			return TGLOBAL;
const                   return TCONST;
gterm			return TGTERM;
ngterm			return TNGTERM;
eostr			return TEOSTR;


equ                     return EQU;
dw                      return DW;
ds                      return DS;
start                   return START;
org                     return ORG;
clause			return CLAUSE;
reduction		return RED;
pred			return PRED;
optim			return OPTIM;
noopt			return NOOPT;
symb			return SYMB;
include			return INCLUDE;

[a-zA-Z][a-zA-Z0-9_]*   { for (i=0; i<yyleng; i++) {
				if (i == M_SYMB_LENG - 1) break;
                                token[i] = yytext[i];
                          }
                          token[yyleng]='\0';
                          return LABEL;
			}

[0-9][0-9]*             { numval = 0;
                          for (i=0; i<yyleng; i++) {
                                numval = 10 * numval + yytext[i] - '0';
                          }
                          return NUMBER; 
		    	}

\"[^\"]*\"	        { for (i=1; i<yyleng-1; i++) {
                                token[i-1] = yytext[i];
                          }
                          token[yyleng-2]='\0';
                          return STRING;
			}

\:                      return ':';
\.                      return '.';
\,                      return ',';
\(                      return '(';
\)                      return ')';
\+                      return '+';
\-                      return '-';

\012                    { return NL; }
[ \t]			;
\/\*.*\*\/   		;

%%
