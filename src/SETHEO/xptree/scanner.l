%{
#define _NO_X_
#include "xptree.h"
#include "parser_if.h"
#include <string.h>
/* letter		lcletter|ucletter|digit*/
%}

lcletter	[a-z]
ucletter	[A-Z_]
minus		[\-]
digit		[0-9]
letter		[A-Za-z_0-9]
wsc		[ \n\t]
other		.

%%

fac__				return FAC;

ext__				return EXT;

red__				return RED;

built_in			return BUILT_IN;

not_touched                     return NOT_TOUCHED;

{lcletter}{letter}*		{ yylval.str = strdup(yytext);
				  return PRED_FUNCT_SYMB;
				}

{ucletter}{letter}*		{ yylval.str = strdup(yytext);
				  return VAR;
				}

{digit}+			{ yylval.number = atoi(yytext);
				  return NUMBER;
				}

{minus}{digit}+			{ yylval.number = atoi(yytext);
				  return NUMBER;
				}

{wsc}+				;

{other}				return yytext[0];

