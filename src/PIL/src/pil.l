%{
/******************************************************
 *     PIL System
 *     $Id: pil.l,v 1.1.1.1 2001/07/16 18:16:09 schumann Exp $
 *
 *     (c) Johann Schumann, Klaus Wagner
 *     TU Muenchen and NASA Ames
 *
 *     Origin: 2.1 06/18/98
 *
 *     $Log: pil.l,v $
 *     Revision 1.1.1.1  2001/07/16 18:16:09  schumann
 *     new CVS project
 *
******************************************************/


#include "pily.h"	/* token codes from the parser */
extern YYLTYPE yylloc;
#include "pil.h"
%}

ID 	[A-Za-z][A-Za-z0-9]*

%%
Objects:        {yylloc.first_line=yylex_line;return(OBJ);}
Assumptions:	{yylloc.first_line=yylex_line;return(ASSUMP);}
"Idealized Protocol:"	{yylloc.first_line=yylex_line;return(PROTO);}
Conjectures:	{yylloc.first_line=yylex_line;return(CONJECT);}
variable	{yylloc.first_line=yylex_line;return(VAR);}
principal	{yylloc.first_line=yylex_line;return(PRINCIPAL);}
sharedkey	{yylloc.first_line=yylex_line;return(SHAREDKEY);}
publickey	{yylloc.first_line=yylex_line;return(PUBLICKEY);}
statement	{yylloc.first_line=yylex_line;return(STATEMENT);}
secret          {yylloc.first_line=yylex_line;return(SECRET);}
believes	{yylloc.first_line=yylex_line;return(BELIEVES);}
sees		{yylloc.first_line=yylex_line;return(SEES);}
said		{yylloc.first_line=yylex_line;return(SAID);}
said_recently	{yylloc.first_line=yylex_line;return(SAID_RECENTLY);}
controls        {yylloc.first_line=yylex_line;return(CONTROLS);}
recognizes      {yylloc.first_line=yylex_line;return(RECOGNIZES);}
fresh		{yylloc.first_line=yylex_line;return(FRESH);}
message		{yylloc.first_line=yylex_line;return(MESSAGE);}
"<-"		{yylloc.first_line=yylex_line;return(LEFTARROW);}
"->"		{yylloc.first_line=yylex_line;return(RIGHTARROW);}
"after message"	{yylloc.first_line=yylex_line;return(AFTER);}
use		{yylloc.first_line=yylex_line;return(USE_LOGIC);}
BAN		{yylloc.first_line=yylex_line;return(USE_BAN);}
AUTLOG		{yylloc.first_line=yylex_line;return(USE_AUTLOG);}



{ID}	{yylval.symp = symlook (yytext,WORD1);yylloc.first_line=yylex_line;
         return(yylval.symp->type);}

{ID}"_"  {yylval.symp = symlook (yytext,WORD1_);yylloc.first_line=yylex_line;
         return(yylval.symp->type);} 

{ID}"_"{ID}"_"	{yylval.symp = symlook (yytext,WORD2_);yylloc.first_line=yylex_line;
         return(yylval.symp->type);}

{ID}"_"{ID}	{yylval.symp = symlook (yytext,WORD2);yylloc.first_line=yylex_line;
         return(yylval.symp->type);}

{ID}"_"{ID}"_"{ID}	{yylval.symp = symlook (yytext,WORD3);yylloc.first_line=yylex_line;
         return(yylval.symp->type);}

[ \t] ;
\n  {yylex_line++;
/*    printf("line %i\n",yylex_line);*/
      yylloc.first_line=yylex_line;}
.	{return yytext[0];}
[0-9]+: {yylval.ival = atoi(yytext); yylloc.first_line=yylex_line;return (NUMBER);}
"//".*\n {yylex_line++;
/*    printf("line %i\n",yylex_line);*/
     yylloc.first_line=yylex_line;}
%%


























