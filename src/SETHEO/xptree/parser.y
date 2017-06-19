/*########################################################################
# File:			parser.y
# SCCS-Info:		@(#)parser.y	2.7 08/22/95
# Author:		R. Handl
# Date:			1994
# Context:		xptree
# 
# Contents:		list handling and conversion term->string
#
# Modifications:	
#	(when)		(who)		(what)
#	94		jsc		new handling of terms
#	29.7.94		jsc		bugs
#	1.2.95		jsc		changes in syntax ext__()
#	22.3.95		jsc		changes in syntax
#					red__(),fac__()
#
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
########################################################################*/

%{
#include <string.h>
#include "xptree.h"
#include "infix.h"

char buf[1000];
int td=0;
int tdmax;
struct label_str dummy_label;
%}

%union {
  char *str;
  struct expr *expr;
  char *s;
  int number;
}


%start	proof

%token		RED, EXT, FAC, BUILT_IN, NOT_TOUCHED
%token	<str>	VAR, PRED_FUNCT_SYMB
%token	<number> NUMBER

%type	<expr>	term, seq_of_terms, atom, literal, p_seq_of_terms
%type   <expr> lseq_of_terms , list_of_terms
%type	<str>	infdescr
%type	<number> infnumber


%%

proof		: treelist '.'
		;


treelist	: '[' seq_of_trees ']'
		;


seq_of_trees	: '[' tree ']'
		| seq_of_trees ',' '[' tree ']'
		;


tree		: literal
			{ 
			  insert_node1(convert_to_string($1), strdup("entry literal"), 
				       stay);
			}
		| literal ',' '[' infnumber ',' infdescr ']'
			{ 
			  sprintf(buf, "InfNr %d\n%s", $4, $6);
			  insert_node1(convert_to_string($1), strdup(buf), stay);
			}
		| root ',' treelist
			{ 
			  insert_node1(dummy_label, NULL, up);
			}
		;

root		: literal ',' '[' infnumber ',' infdescr ']'
			{ 
			  sprintf(buf, "InfNr %d\n%s", $4, $6);
			  insert_node1(convert_to_string($1), strdup(buf), down);
			}
		;



literal		: atom
		| '~' atom
			{ 
			  $$=mknode("~",$2);
			}
		| NUMBER
			{ 
			  sprintf(buf,"%d",$1);
			  $$ = mkleaf(strdup(buf));
			}
		;


atom		: PRED_FUNCT_SYMB 
			{ $$ = mkleaf($1);
			}
		| p_seq_of_terms
		| term '=' term
			{ 
			$$ = mknode3("=",$1,$3);
			}
		;


seq_of_terms	: term
		| term ','  seq_of_terms
			{ 
			  $$ = mkcons($1,$3);
			}
		;

lseq_of_terms	: seq_of_terms
		| term '|' seq_of_terms
			{ 
			  $$ = mknode3("|",$1,$3);
			}
		;


term		: VAR
			{ $$ = mkleaf($1);
			}
		| '[' ']'
			{ $$ = mkleaf("[]");
			}
		| PRED_FUNCT_SYMB
			{ $$ = mkleaf($1);
			}
		| NUMBER
			{ 
			sprintf(buf,"%d",$1);
			$$ = mkleaf(strdup(buf));
			}
		| p_seq_of_terms
		| list_of_terms
		;


p_seq_of_terms	: PRED_FUNCT_SYMB '(' seq_of_terms ')'
                        { 
			  $$=mknode($1,$3);
			}
                ;


list_of_terms	: '[' lseq_of_terms ']'
                        {
			$$=mknode("[",$2);
			}
		;



infdescr	: EXT '(' NUMBER '.' NUMBER ',' NUMBER '.' NUMBER ')'
			{ sprintf(buf, "ext(%d.%d,%d.%d)", $3, $5,$7,$9);
			  $$ = strdup(buf);
			}
		| RED '(' NUMBER '.' NUMBER ',' NUMBER
			')'
			{ sprintf(buf, "red(%d.%d,%d)", $3,$5,$7);
			  $$ = strdup(buf);
			}
		| FAC '(' NUMBER '.' NUMBER ',' NUMBER
			')'
			{ sprintf(buf, "fac(%d.%d,%d)", $3,$5,$7);
			  $$ = strdup(buf);
			}
		| NOT_TOUCHED
		        { $$ = "not_touched";
			}
		| BUILT_IN
			{ $$ = "built in";
			}
		;


infnumber	: NUMBER
		;
			
%%

#include <stdio.h>

#ifdef linux
int yylineno;
#else
extern int yylineno;
#endif
extern char yytext[];

#ifdef __ANSI__
int yyerror(char *s) {
#else
int yyerror(s)
  char *s;
{
#endif

  fprintf(stderr,"%s, line %d, token `%s'\n",s,yylineno,yytext);
  exit(1);
}



