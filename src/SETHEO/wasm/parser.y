/*#########################################################################
# File:			parser.y
# SCCS-Info:		01/16/97, @(#)parser.y	7.2
# Author:		J.Schumann
# Date:			23.12.87
# Context:		wasm (SETHEO)
# 
# Contents:		parser with direct translation of
#				setheo assembler files into binary files
#
# Modifications:	
#	(when)		(who)		(what)
#	28.09.92	jsc
#	25.2.93		Goller		TSTRVAR -> TCSTRVAR
#					T_STRVAR -> T_CSTRVAR
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
#########################################################################*/


/**************************************************************************
* Definition of tokens (terminal symbols)
**************************************************************************/

%{
#define PASS2(X) X

#define YYSTYPE_IS_DECLARED	1
#ifndef YYSTYPE_DEFINED
#define YYSTYPE_DEFINED
typedef union {
        struct {
        int	val;		/* value of expression			*/
        short	tag;		/* type of expression			*/
        } EXPRVAL;
        } YYSTYPE;
#endif

PASS2(#include "wasm.h")
PASS2(#include "parser.h")
PASS2(#include "tags.h")

#define DEBUG(X)

/*-------------------------------------------------------------------------
- Definition of attribute types of tokens
-------------------------------------------------------------------------*/

#ifndef YYSTYPE_DEFINED
#define YYSTYPE_DEFINED
typedef union {
        struct {
        int	val;		/* value of expression			*/
        short	tag;		/* type of expression			*/
        } EXPRVAL;
        } YYSTYPE;
#endif


	/* for temp. storage of equ tokens				*/
static	char            eqtoken[TOKENLENGTH];
	/* 0 if no optimization takes place				*/
static	int		optim = 0;
	/* temp. value							*/
static	int		defval;
int   yylex();
extern char yytext[];
%}

/********************************************************/
/* general tokens 					*/
%token LABEL NL 
%token NUMBER
%token EQU DW DS START ORG 
%token OPTIM NOOPT INCLUDE
%token CLAUSE RED
%token SYMB PRED STRING 

/********************************************************/
/* tokens for types				*/
%token TVOID TVAR TCONST TGLOBAL TGTERM TNGTERM TCSTRVAR
%token TCREF TEOSTR TCRTERM

/********************************************************/
/* tokens for instructions				*/
#include "w_parse1.h"

/********************************************************/
/* YYSTACK-type 					*/

%type <EXPRVAL> expr numexpr symbtype


%left '+' '-'

%%
program : line 
	| program line
        ;

line    : NL 
        | '.' directive NL
	| '.' INCLUDE STRING NL
		{ if (incl_ptr == MAX_INCL -1) {
			nferror("Illegal nesting of include files");
			exit(1);
		  }
		  if ((infiles[incl_ptr+1] = fopen(token,"r")) == NULL) {
			nferror("could not open include file");
			fprintf(stderr,"File name: %s\n",token);
		  }
		  else {
			DEBUG(printf("INCLUDE>>%s\n",token));
			yyin = infiles[++incl_ptr];
		  }
		}
        | LABEL ':' NL 
                { /* define label */
		  if (optim) {
			/* put the label into the Optimiser	*/
			if (getdef(token,ISLABEL,&defval) == UNDEF) 
				o_name(defval,UNDEF);
			else
				o_name(defval,VALUE);
		  }
		  else {
			/* directly put the label into the output */
                	if (putdef(token,ISLABEL,curr_pc)) {
                        	nferror("Label multiply defined");
                        	prttoken(token);
                       	}
		  }
                }
        | statement NL 
		{ optim = 0; 
		}

        | error NL 
                { errors++; 
		}   
        ;


statement :
# include "w_parse2.h"
        ;

directive :  
        EQU LABEL 
                { strcpy(eqtoken,token);
                }
        numexpr 
                { /*define value */
                  if (putdef(eqtoken,ISEQU,$4.val)) {
                        nferror("EQU name multiply defined");
                        prttoken(eqtoken);
                  }
                }
        | DW exprlist 
        | DS numexpr 
                { /* define storage space */
		  if ($2.tag == UNDEF)
			nferror(".ds numexpr: numexpr must be defined");
		  else
                	curr_pc += $2.val;
                }
        | START 
                { /* mark start address */
                  enter_start(curr_pc);
                }
        | ORG  numexpr
                { /* set curr_pc */
		  if ($2.tag == UNDEF)
			nferror(".org numexpr: numexpr must be defined");
		  else
                	curr_pc = $2.val;
                }
	| SYMB STRING 
                { codestr(eqtoken,token);
		}
	  ',' symbtype ',' numexpr
		{ 
		put_symbol_code($5.val,$7.val,eqtoken);
		/* fprintf(fout,FORMAT_CODE,SYMB_CODE,$5.val,$7.val,eqtoken);*/
		}

	| CLAUSE numexpr ',' numexpr
		{ /* enter clause info into deb-file */
		  if ($2.tag == UNDEF || $4.tag == UNDEF)
			nferror(".clause: numexpr must be defined");
		  put_clause_code($2.val,$4.val);
		}
	| RED numexpr
		{ /* enter reduction info into deb-file */
		  if ($2.tag == UNDEF )
			nferror(".reduct: numexpr must be defined");
		  put_reduct_code($2.val);
		}
	| OPTIM
		{ if (optflg)
			optim = 1;
		}
	| NOOPT
		{ if (optim) 
			list2tree();
		  optim = 0;
		}

        ;

symbtype: PRED
		{ $$.val = SYMB_P;
		}
	| TVAR
		{ $$.val = SYMB_V;
		}
	| TCONST
		{ $$.val = SYMB_C;
		}
	| TGLOBAL
		{ $$.val = SYMB_GLOBAL;
		}
	| TGTERM
		{ $$.val = SYMB_F;
		}
	| TNGTERM
		{ $$.val = SYMB_F;
		}
	;


exprlist: datexpr
        | exprlist ',' datexpr
        ;

datexpr : numexpr 
                { if (optim){
			enter_opt($1.val,$1.tag);
		  }
		  else
                	put_tag_word(curr_pc++,$1.val,$1.tag,0);
                }
/*
        | TVOID numexpr
                { put_tag_word(curr_pc++,$2.val,$2.tag,T_TTVAR);
                }
*/
        | TCREF numexpr
                { put_tag_word(curr_pc++,$2.val,$2.tag,T_CREF);
                }
        | TEOSTR numexpr
                { put_tag_word(curr_pc++,$2.val,$2.tag,T_EOSTR);
                }
        | TCONST numexpr
                { put_tag_word(curr_pc++,$2.val,$2.tag,T_CONST);
                }
        | TVAR numexpr
                { put_tag_word(curr_pc++,$2.val,$2.tag,T_VAR);
                }
        | TGTERM numexpr
                { put_tag_word(curr_pc++,$2.val,$2.tag,T_GTERM);
                }
        | TGLOBAL numexpr
                { put_tag_word(curr_pc++,$2.val,$2.tag,T_HREF);
                }
        | TNGTERM numexpr
                { put_tag_word(curr_pc++,$2.val,$2.tag,T_NGTERM);
                }
        | TCRTERM numexpr
                { put_tag_word(curr_pc++,$2.val,$2.tag,T_CRTERM);
                }
        | TCSTRVAR numexpr
                { put_tag_word(curr_pc++,$2.val,$2.tag,T_CSTRVAR);
                }
        ;

expr    : LABEL 
                { /* expr is a label */
                  if (getdef(token,ISLABEL,&defval) == UNDEF) {
                        $$.tag = UNDEF;
                  }
                  else { /* label was already defined */
                        $$.tag = VALUE;
                  }
                  $$.val = defval;
                }
        | NUMBER 
                { $$.tag = VALUE;
                  $$.val = numval;
                }
        ;

numexpr : NUMBER 
                { /* have read a number */
                  $$.tag = VALUE;
                  $$.val = numval;
                }       
        | LABEL 
                { /* have read a label, look if it is defined */
                  if (getdef(token,ISLABEL,&defval) == UNDEF) {
                	$$.val = defval;
                        $$.tag = UNDEF;
                  }
                  else {
                        $$.val = defval;
                        $$.tag = VALUE;
                  }
                }
        | numexpr '+' numexpr
                { /* add only, if both values are defined */
                  if (($1.tag == UNDEF) || ($3.tag == UNDEF)) {
                        warning("sum: undefined values: 0 taken");
                        $$.val = 0;
                        $$.tag = UNDEF;
                  }
                  else {
                        $$.val = $1.val + $3.val;
                        $$.tag = VALUE;
                  }
                }
        | numexpr '-' numexpr 
                { /* sub only, if both values are defined */
                  if (($1.tag == UNDEF) || ($3.tag == UNDEF)) {
                        warning("minus: undefined values: 0 taken");
                        $$.val = 0;
                        $$.tag = UNDEF;
                  }
                  else {
                        $$.val = $1.val - $3.val;
                        $$.tag = VALUE;
                  }
                }
        ;


        
%%

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ yyerror
+ - error reporting of the parser
+
+ special remarks:
+ - none
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int yyerror ()

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

    fprintf (stderr,"Syntax error occurred in line : %d\n", yylineno);
    fprintf(stderr,"near token %s\n",yytext);
    errors++;

} /*=====================================================================*/







/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ yywrap
+ - called whenever an EOF is encountered
+
+ special remarks:
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

int yywrap ()

{ /*=====================================================================*/
/*-----------------------------------------------------------------------*/

	/* are we processing an include file ? */
    if (incl_ptr > 0) {
 	/* close current file */
	fclose (yyin);
 	/* go one level out   */
	incl_ptr--;
	yyin = infiles[incl_ptr];
	/* resume parsing with the outer file */
	return 0;
    }

    return 1;

}



/*#######################################################################*/
