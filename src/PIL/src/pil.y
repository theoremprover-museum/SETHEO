%{
/******************************************************
 *     PIL System
 *     $Id: pil.y,v 1.1.1.1 2001/07/16 18:16:09 schumann Exp $
 *
 *     (c) Johann Schumann, Klaus Wagner
 *     TU Muenchen and NASA Ames
 *
 *     Origin: 2.1 06/18/98
 *
 *     $Log: pil.y,v $
 *     Revision 1.1.1.1  2001/07/16 18:16:09  schumann
 *     new CVS project
 *
******************************************************/
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#define YYDEBUG 1 
#include "pil.h"

char *dummy, *dummy2;
int state=DEFS_BEGIN;
int i;
int errline;
int yylex_line=1;



%}

%union {
	char cval;
	int ival;
	char *strval;
	struct symtab *symp;
	struct symlist *list_of_symp;
	struct strlist *list_of_strp;
}


%token USE_LOGIC, USE_BAN, USE_AUTLOG

%token OBJ ASSUMP PROTO CONJECT
%token PRINCIPAL PUBLICKEY SHAREDKEY STATEMENT
%token BELIEVES SEES SAID SECRET
%token SAID_RECENTLY, RECOGNIZES
%token FRESH CONTROLS
%token MESSAGE AFTER LEFTARROW RIGHTARROW
%token UNDEFINED
%token VAR
%token <ival> NUMBER
%token <symp> ID
%token <symp> WORD
%token <symp> NONCE
%token <symp> PRIVKEY
%token <symp> SHARKEY
%token <symp> PUBKEY

%type <symp> key
%type <list_of_symp> id_list
%type <list_of_symp> sharedkey_list
%type <list_of_symp> publickey_list
%type <list_of_symp> nonce_list

%type <list_of_strp> statement
%type <list_of_strp> statement_list
%type <list_of_strp> assumption 
%type <list_of_strp> assumption_list
%type <list_of_strp> protocol 
%type <list_of_strp> protocol_list
%type <list_of_strp> protocol_part
%type <list_of_strp> protocol_object_list
%type <list_of_strp> protocol_sentence
%type <list_of_strp> protocol_object
%type <list_of_strp> conjecture 
%type <list_of_strp> conjecture_list

%token WORD1 WORD1_ WORD2 WORD2_ WORD3




%%
pil_file:	    
	logic_section
	objects_section
        assumption_section
        protocol_section
        conjectures_section
	;

logic_section:
	| USE_LOGIC USE_BAN
		{
		used_logic=BAN;
		}
	| USE_LOGIC USE_AUTLOG
		{
		used_logic=AUTLOG;
		}
	;

objects_section:    OBJ principals objects_list      
		{
			symtype = UNDEFINED;
		}
	;

principals:	    PRINCIPAL           
			{
                                   variable=FALSE;
                                   symtype = ID;
                                         errline=@1.first_line;
                                        } 
                    id_list ';' 
                                        {
                                         del_list_of_symp ($3);
                                        }
	|	    VAR PRINCIPAL       
					{
                                         variable=TRUE;
                                         symtype = ID;
                                         errline=@1.first_line;
	                                } 
                    id_list ';' 
                                        {
                                         del_list_of_symp ($4);
                                         variable=FALSE;
                                        }
	;

id_list:	    ID		        
					{
                                         $$ = new_list_of_symp ($1);
                                         errline=@1.first_line;
                                        }
	|	    id_list ',' ID      
					{
                                         $$ = add_list_of_symp ($1,$3);
                                         errline=@1.first_line; // was @3 JSC 2007
                                        }  
	;

objects_list:	    key_objects 	
	|	    statement_objects
	|	    objects_list key_objects
	|	    objects_list statement_objects 
	;

key_objects:        SHAREDKEY           
					{
                                         variable=FALSE;
                                         symtype = SHARKEY;
                                         errline=@1.first_line;
                                        } 
                    sharedkey_list ';' 
                                        {
                                         del_list_of_symp($3);
                                        }
	|	    PUBLICKEY           
					{
                                         variable=FALSE;
                                         symtype = PUBKEY;
                                         errline=@1.first_line;
                                        } 
                    publickey_list ';'  
                                        {
                                         del_list_of_symp($3);
                                        }
	|	    VAR SHAREDKEY       
					{
                                         variable=TRUE;
                                         symtype = SHARKEY;
                                         errline=@1.first_line;
                                        } 
                    sharedkey_list ';' 
                                        {
                                         del_list_of_symp($4);
                                         variable=FALSE;
                                        }
	|	    VAR PUBLICKEY       
					{
                                         variable=TRUE;
                                         symtype = PUBKEY;
                                         errline=@1.first_line;
                                        }  
                    publickey_list ';' 
                                        { 
                                         del_list_of_symp($4);
                                         variable=FALSE;
                                        }
	;

sharedkey_list:	    SHARKEY		
					{
                                         $$ = new_list_of_symp ($1);
                                        }
	|	    sharedkey_list ',' SHARKEY	
                                        {
                                         $$ = add_list_of_symp($1,$3);
                                        }
	;

publickey_list:	    PUBKEY		
					{
                                         $$ = new_list_of_symp ($1);
                                        }
	|	    publickey_list ',' PUBKEY 	
                                        {
                                         $$ = add_list_of_symp ($1,$3);
                                        }
	;

statement_objects:  STATEMENT           
					{
                                         variable=FALSE;
                                         symtype = NONCE;
                                         errline=@1.first_line;
                                        } 
                    nonce_list ';'   
                                        {
                                         del_list_of_symp($3);
                                        }
	|	    VAR STATEMENT       
					{
                                         variable=TRUE;
                                         symtype = NONCE;
                                         errline=@1.first_line;
                                        } 
                    nonce_list ';' 
                                        {
                                         del_list_of_symp($4);
                                         variable=FALSE;
                                        }
	;


nonce_list:         NONCE		
					{
                                         $$ = new_list_of_symp ($1);
                                        }
	|	    nonce_list ',' NONCE	
                                        {
                                         $$ = add_list_of_symp ($1,$3);
                                        }
	;








assumption_section: ASSUMP              
					{
                                         state = INIT_BEGIN; 
                                        }
                    assumption_list ';' 
		                        {
		                         strlist_assump=$3;
		                        }
	;

assumption_list:    assumption          
					{
                                         $$=$1;
                                         errline=@1.first_line;
                                        }
	|	    assumption_list ';' assumption 
                                        {
                                         $$=add_list_of_strp($1,$3);
                                         errline=@1.first_line; // was @3 JSC 2007
                                        }
	;

assumption: 	 

   '(' id_list ')' BELIEVES statement
		                        {   
					  strlist = $5;
					  errline=@1.first_line; // was @3 JSC 2007
					  symlist = $2;
					  $$=new_statement_ID_list(symlist->symp,strlist,holds_bel_formula);
					  symlist = symlist->next;	 
					  while (strlist) {
			                    while (symlist) {
					      add_list_of_strp(
                                                  $$,
						  new_statement_ID_list(symlist->symp,strlist,holds_bel_formula)
                                                  );
					      symlist=symlist->next;
			                    }
			                    symlist = $2;
		                            strlist=strlist->next;
					  }
					  del_list_of_symp($2);
					  del_list_of_strp($5);	
		                        }	
	|	
    statement           
					{   
                                         strlist = $1;
                                         errline=@1.first_line; // was @3 JSC 2007
					 $$ = mod_statement(strlist,holds_formula);
                                        }
	;



statement: 
	ID
			{      
                        errline=@1.first_line; // was @3 JSC 2007
                        $$ = new_statement_nonce ($1,principal_formula);
                        }

	|         ID BELIEVES statement  
		                        {   
                                         strlist = $3;
	                                 errline=@1.first_line; // was @3 JSC 2007
		                         $$ = mod_statement_ID($1,strlist,ID_bel_formula);
                                        }
	|	    ID SEES statement   
					{   
                                         strlist = $3;
                                         errline=@1.first_line; // was @3 JSC 2007
		                         $$ = mod_statement_ID($1,strlist,ID_sees_formula);
                                        }
	|	    ID SAID statement   
					{   
                                         strlist = $3;
                                         errline=@1.first_line; // was @3 JSC 2007
		                         $$ = mod_statement_ID($1,strlist,ID_said_formula);
		                        }
	|	    ID SAID_RECENTLY statement   
					{   
                                         strlist = $3;
                                         errline=@1.first_line; // was @3 JSC 2007
		                         $$ = mod_statement_ID(
						$1,strlist,
						ID_said_recently_formula);
		                        }
	|	    ID CONTROLS statement
		                        {   
                                         strlist = $3;
                                         errline=@1.first_line; // was @3 JSC 2007
		                         $$ = mod_statement_ID($1,strlist,ID_controls_formula);
		                        }
	|	    FRESH statement     
					{   
                                         strlist = $2;
                                         errline=@1.first_line; // was @3 JSC 2007
					 $$ = mod_statement(strlist,fresh_formula);
	                                }
	|	    RECOGNIZES statement     
					{   
                                         strlist = $2;
                                         errline=@1.first_line; // was @3 JSC 2007
					 $$ = mod_statement(strlist,
						recognizes_formula);
	                                }
	|	    SHAREDKEY '(' ID ',' ID ',' SHARKEY ')'
                                        {      
                                         errline=@1.first_line; // was @3 JSC 2007
					 $$ = new_statement_key($3,$5,$7,sk_formula);
		                        }
	|	    SHAREDKEY SHARKEY   
					{      
                                         errline=@1.first_line;  // was @3 JSC 2007
					 $$ = new_statement_key( keysymlook($2,1),keysymlook($2,2),$2,sk_formula);
		                        }
	| 	    PUBLICKEY '(' ID ',' PUBKEY ')'
                                        {
                                         errline=@1.first_line;       // was @3 JSC 2007
					 $$ = new_statement_key($3,$5,&empty,pub_formula); 
		                        }
	|	    PUBLICKEY PUBKEY    
					{      
                                         errline=@1.first_line; // was @3 JSC 2007
					 $$ = new_statement_key(keysymlook($2,1),$2,&empty,pub_formula); 
		                        }
	|	    SECRET '(' ID ',' ID ',' NONCE ')'
                                        {      
					  errline=@1.first_line;   // was @3 JSC 2007
					  $$ = new_statement_key($3,$5,$7,ss_formula);
		                        }
	|	    NONCE               
					{      
                                         errline=@1.first_line; // was @3 JSC 2007
                                         $$ = new_statement_nonce ($1,nonce_formula);
		                        }
/*
	|	    '(' statement_list ')'  
                                        {
                                         $$ = $2;
                                         errline=@1.first_line; // was @3 JSC 2007
                                        }
*/
	|	    '|' protocol_object_list '|' 
                                        { 
                                        
					  $$=process_strlist($2);
                                        }
	|	    '|' protocol_object '|' 
                                        {
					  $$ = $2;
                                        }
								          						
;
	



statement_list:     statement           
					{
                                         $$=$1;
                                         errline=@1.first_line; // was @3 JSC 2007
                                        }
	|	    statement_list ';' statement
                                        {
                                         $$=add_list_of_strp($1,$3)
                                         ;errline=@1.first_line; // was @3 JSC 2007
                                        }
	;








protocol_section:   PROTO protocol_list 
					{
                                         strlist_proto=$2;
                                        }
        ;
                 
protocol_list:      protocol            
					{
                                         $$=$1;
                                         errline=@1.first_line;
                                        }
        |           protocol_list protocol
		                        {
                                         $$=add_list_of_strp($1,$2);
                                         errline=@2.first_line;
                                        }
        ;        
                 
protocol:           MESSAGE NUMBER ID LEFTARROW ID protocol_part ';'
		                        {
				
	                                 mod_protocol_ID_list($3,$6,holds_sees_formula);
		                       
		                         $6->number=$2;
		                         $$=$6;
		                        }
        |           MESSAGE NUMBER ID LEFTARROW ID protocol_sentence ';'
                                         {
					 strlist=process_strlist($6);
                                         mod_protocol_ID_list($3,strlist,holds_sees_formula);

		                         strlist->number=$2;
		                         $$=strlist;
		                        }
        |           MESSAGE NUMBER ID RIGHTARROW ID protocol_part ';'
		                        {
					  mod_protocol_ID_list($5,$6,holds_sees_formula);
		                      
		                         $6->number=$2;
		                         $$=$6;

					}
        |           MESSAGE NUMBER ID RIGHTARROW ID protocol_sentence ';'
		                        {                                        



					 strlist=process_strlist($6);
					 mod_protocol_ID_list($5,strlist,holds_sees_formula);
		                         strlist->number=$2;
		                         $$=strlist;



                                        }
        ;        
                 
protocol_part:      '{' protocol_object_list '}' '(' key ')'
                                        {
					
 
					 strlist=process_strlist($2);
					 $$=mod_protocol_list_ID(strlist,$5,encr_formula);
                                        
                                         errline=@1.first_line; // was @3 JSC 2007
					 
                                        }
 
/*
       |           '{'  protocol_object '}' '(' key ')'
                                        {
 
                                         $$=mod_protocol_list_ID($2,$5,encr_formula);
                                        
                                         errline=@1.first_line; // was @3 JSC 2007
                                        } 
*/
	|           '<' protocol_object_list '>' '(' NONCE ')'
                                        {

					 strlist=process_strlist($2);
					 $$=mod_protocol_list_ID(strlist,$5,comb_formula);
                                        
                                         errline=@1.first_line; // was @3 JSC 2007
					
                                        }
/*
       |           '<' protocol_object'>' '(' NONCE ')'
                                        {
					  $$=mod_protocol_list_ID($2,$5,comb_formula);
                                        
					  errline=@1.first_line; // was @3 JSC 2007
					 
   
                                        } 
*/
	|           '[' protocol_object_list ']'
                                        {

			 strlist=process_strlist($2);
/*
			 $$=mod_protocol_list_ID(strlist,$5,comb_formula);
*/
			$$=strlist;
                                        
                         errline=@1.first_line; // was @3 JSC 2007
					
                                        }

       ;        


protocol_object:    statement           
					{
                                         $$=$1;
                                        }
        |           protocol_part       
					{
                                         $$= $1;
                                        } 
        ;        
                 
protocol_object_list: protocol_object   
					{
                                         $$=$1;
					
                                        }
        |             protocol_object_list ',' protocol_object
                                        {
                                         $$=add_list_of_strp($1,$3);
                                       
                                        }
        ;        
                 
                 
protocol_sentence:  protocol_part       
					{
                                         $$=$1;
                                         errline=@1.first_line;
                                        }
        |           protocol_sentence ',' protocol_part 
			                {
					 $$=add_list_of_strp($1,$3);
                                         errline=@1.first_line; // was @3 JSC 2007
                                        }
        ;        

key:                PRIVKEY             
					{	

                                         dummy=(char *) malloc(strlen($1->name)+strlen(privkey_string)+1);
					 dummy2=(char *) malloc(strlen($1->name));
					 dummy2[0]='\0';
					 strncat(dummy2,
                                            $1->name,strlen($1->name)-1);
					 
					 sprintf(dummy,privkey_string,dummy2);
					 

                                         $$=symlook(dummy,PRIVKEY);
                                         errline=@1.first_line; // was @3 JSC 2007
			                }
        |           PUBKEY
        |           SHARKEY
        ;

conjectures_section: CONJECT conjecture_list 
		                        {
		                         strlist_conject=$2;
		                        }
	;

conjecture_list:    conjecture          
					{
                                         $$=$1;
                                         errline=@1.first_line;
                                        }
	|	    conjecture_list conjecture
		                        {
                                         $$=add_list_of_strp($1,$2);
                                         errline=@2.first_line;
                                        }
	;

conjecture:	    AFTER NUMBER assumption_list ';'
		                        {
                                         errline=@1.first_line; // was @3 JSC 2007
		                         strlist=$3;
		                         while(strlist) {
			                    strlist->number = $2;
			                    strlist=strlist->next;
		                         }
		                         $$=$3;
		                        }
	;





%%










yyerror(s)
char *s;
{
    fprintf(stderr, "%s between line %i and line %i, near Token \"%s\"\n", s,errline,yylex_line,yytext);
}




















