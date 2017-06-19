%{
/****************************************************
;;; MODULE NAME : sca.lex
;;;
;;; PARENT      :
;;;
;;; PROJECT     : MPLOP
;;;
;;; SCCS        : @(#)scan.l	18.4 06/19/98 
;;;
;;; AUTHOR      : Janos Breiteneicher
;;;
;;; DATE        : 15.5.91
;;;
;;; DESCRIPTION : scanner definition
;;;
;;; REMARKS     :
;;;
****************************************************/

%}

%a 3000 
%o 3500

%start BEG EN CONSTR 
Digit	[0-9]+
Rest	[a-zA-Z0-9_]*
Small	[a-z0-9]
Big	[A-Z_]
Var	{Big}{Rest}
Name	{Small}{Rest}
Other	.

%%
\@clstart       {
                if (inwasm_flags.ignoreclstart == FALSE) {
                  cl_reord_start = curr_clausenr;
                }
                }
\?-		return QUERY;
\<-		return ARROW;
\:-		return COLMIN;
\:=		return ASSIGN;
\:is		return EVASSIGN;
is		return IS;
\:		{ 
		BEGIN CONSTR; 
		return CONSTRSEP;
		}
{Digit}		{
		yylval.N = enter_token(yytext);
		numval = atoi(yytext);
		return NUMBER;
		}
{Name}		{
		if (yyleng > NAMELENGTH)
		   warning(yytext," token too long");
		yylval.N = enter_token(yytext);
	   	return NAME;
		}
\${Name}	{
		if (yyleng > NAMELENGTH)
		   warning(yytext," token too long");
		yylval.N = enter_token(yytext);
	   	return NAME;
		}
\@${Name}	{
		if (yyleng > NAMELENGTH)
		   warning(yytext," token too long");
		yylval.N = enter_token(yytext);
	   	return NAME;
		}
\@{Name}	{
		if (yyleng > NAMELENGTH)
		   warning(yytext," token too long");           
		yylval.N = enter_token(yytext);
	   	return NAME;
		}
\^{Name}	{
		if (yyleng > NAMELENGTH)
		   warning(yytext," token too long");           
		yylval.N = enter_token(yytext);
	   	return NAME;
		}
{Var}		{
		yylval.N = enter_token(yytext);
		return VAR;
		}
\${Var}		{
		yylval.N = enter_token(yytext);
		return GLOBVAR;
		}
<CONSTR>\#{Var}	{
		yylval.N = enter_token(yytext);
		return STRUCVAR;
		}
\"[^\"]*\"	{
		yytext[yyleng-1] = '\0';
		yylval.N = enter_token(yytext+1);
		return STRING;
		}
[ \t\n]+	{}
\%.*		{}
^#.*$		{}
"/*"		{
		loop:
			while (input() != '*');
			switch (input())
			{
			case '/': break;
			case '*': unput('*');
			default : goto loop;
			}
		}
"=/="		{
		yylval.N = NQAL_SYMB;
		return CONSTRAIN;
		}
">"		{
		yylval.N = GREA_SYMB;
		return RELOP;
		}
"<"		{
		yylval.N = LESS_SYMB;
		return RELOP;
		}
"=<"		{
		yylval.N = LEQU_SYMB;
		return RELOP;
		}
">="		{
		yylval.N = GRQU_SYMB;
		return RELOP;
		}
\+		{
		yylval.N = PLUS_SYMB;
		return '+';
		}
\-		{
		yylval.N = MINUS_SYMB;
		return '-';
		}
\*		{
		yylval.N = MULT_SYMB;
		return '*';
		}
\/		{
		yylval.N = DIV_SYMB;
		return '/';
		}
{Other}		{
		return yytext[0];
		}
