/******************************************************
 *     PIL System
 *     $Id: functions.c,v 1.2 2002/07/23 17:43:48 schumann Exp $
 *
 *     (c) Johann Schumann, Klaus Wagner
 *     TU Muenchen and NASA Ames
 *
 *     Origin: 2.1 06/18/98
 *
 *     $Log: functions.c,v $
 *     Revision 1.2  2002/07/23 17:43:48  schumann
 *     bugfix: set first, second, third to ""
 *
 *     Revision 1.1.1.1  2001/07/16 18:16:09  schumann
 *     new CVS project
 *
******************************************************/
#include "pil.h"
#include "pily.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

struct symtab empty = {"","","","",0,0}; /* empty symbol-table-entry */

#define IGNORE -1
/*                                                 cons
						     believes
						       terms */
struct lop_formula ID_bel_formula =  {"bel(%s,%s)",0,1,1,IGNORE};
struct lop_formula ID_sees_formula = {"sees(%s,%s)",0,0,1,IGNORE};
struct lop_formula ID_said_formula = {"said(%s,%s)",0,0,1,IGNORE};
struct lop_formula ID_said_recently_formula = {"said_recently(%s,%s)",0,0,1,IGNORE};
struct lop_formula ID_controls_formula =  {"controls(%s,%s)",0,0,1,IGNORE};
struct lop_formula holds_formula = {"holds(%s)",0,0,1,0};
struct lop_formula fresh_formula =  {"fresh(%s)",0,0,1,0};
struct lop_formula recognizes_formula =  {"recog(%s)",0,0,1,0};
struct lop_formula sk_formula = {"sk(%s,%s,%s)",0,0,2,IGNORE};
struct lop_formula pub_formula =  {"pub(%s,%s)",0,0,2,IGNORE};
struct lop_formula ss_formula = {"ss(%s,%s,%s)",0,0,2,IGNORE};
struct lop_formula holds_bel_formula = {"holds(bel(%s,%s))",0,1,2,IGNORE};
struct lop_formula nonce_formula = {"%s",0,0,1,IGNORE};
struct lop_formula principal_formula = {"%s",0,0,1,IGNORE};
struct lop_formula holds_sees_formula = {"holds(sees(%s,%s))",0,0,2,IGNORE};
struct lop_formula encr_formula = {"encr(%s,%s)",0,0,1,IGNORE};
struct lop_formula comb_formula = {"comb(%s,%s)",0,0,1,IGNORE};
/* 
  liefert zu SHARKEY, PUBKEY den Symbolpointer des Principals i
  Bsp: symtab* zeigt auf Key_Alice_Bob,
       keysymlook(symtab,1) liefert Pointer auf Tabelleneintrag von Alice
 */
struct symtab *keysymlook(struct symtab *symtab, int i)
{
	struct symtab *erg;
	char *name;
	char *first;

	name=strdup(symtab->name);
	first=strtok(name,"_)");
	   first=strtok(NULL,"_)");

	while (i > 1)
	{
	   first=strtok(NULL,"_)");
	   i--;
	}

	erg=symlook(first,WORD);  
	return(erg);

}


/* build the ILF-Latex-Transformation of principals, keys, nonces */
void build_ilf(struct symtab *symtab)
{
       
	char *name = "";
	char *first = "";
        char *second = "";
        char *third = "";
        
	name=strdup(symtab->name);
	first=strtok(name,"_)");
        first[0] =toupper(first[0]);
        
        
           
	/* ersetze symtab->name mit symtab->sorts in sprintf(...)*/
        
        if (symtab->ground != 0) { /*Term is ground, only "name" */
	  symtab->ilf = (char *) malloc(strlen(string_ilf_name)+strlen(symtab->sorts)+strlen(first)+1);
	  sprintf (symtab->ilf, string_ilf_name
		   ,symtab->sorts,first);
        } else
        switch (symtab->type) { /* only "name" */
	case ID: symtab->ilf = (char *) malloc(strlen(string_ilf_name)+strlen(symtab->sorts)+strlen(first)+1);
	         sprintf (symtab->ilf,  string_ilf_name
                                     ,symtab->sorts,first);
                 break;
        case PUBKEY:
        case NONCE: symtab->ilf = (char *) malloc(strlen(string_ilf_name_name)+strlen(symtab->sorts)
						  +strlen(first)+strlen(second)+1);
	            second=strtok(NULL,"_)"); /* "name_name" */
                    second[0]=toupper(second[0]);
                    sprintf (symtab->ilf,  string_ilf_name_name
                                        ,symtab->sorts,first,second);
                 break;
        case SHARKEY: 
		symtab->ilf = (char *) malloc(strlen(string_ilf_name_name_name)+strlen(symtab->sorts)
						  +strlen(first)+strlen(second)+strlen(third)+1);
	              second=strtok(NULL,"_)"); /* "name_name_name" */
                      second[0]=toupper(second[0]);
                      third =strtok(NULL,"_)");
                      third[0]=toupper(third[0]);
                      sprintf (symtab->ilf, string_ilf_name_name_name
                                        ,symtab->sorts,first,second,third);
                 break;
	}

	return;

}



void SyntaxError(char *err,char *s)
{
    char line[200]; 
    sprintf(line,"%s: %s\nSyntax Error",err,s);
    yyerror(line);
    return;
}




/* look up a symbol table entry, add if not present */
struct symtab *symlook(char *s, int stype)
{
	
  
	int oldstate; 
        const char* p_sign = privkey_sign; /* this char will be added to pubkey to create privkey */  
	char *sorts;
        char *sorts2;
	char *dummy1;
	char *dummy2;
        struct symtab *sp;
	char *pt;

	

	for (pt=s;pt[0];pt++)
	   pt[0]=tolower(pt[0]); /* symbol-table-entries are in lowercase */
	
	if (variable) 
	   s[0]=toupper(s[0]); /* expect "variable" symbols */ 


        for(sp = symtab; sp < &symtab[NSYMS]; sp++) { /* is it already here? */
           if(sp->name && !strcasecmp(sp->name, s)) {
                        return sp; 
           }
           /* is it free */
           if(!sp->name) {
	      if (state != DEFS_BEGIN) { /* we are not in the objects-section */
		 SyntaxError("Undefined Symbol",s);
		 return(&empty);
	      }
              sp->name = strdup(s);
              sp->ground = variable; /* active variable-atribute? */
	      if (variable == FALSE)
		 switch (stype) {
		 case WORD1:         /* "name" from lexer */
				if (symtype == ID) /* symtype from parser */
				   sp->type = symtype;
			        else {
				   SyntaxError("Wrong Syntax of Principal",s);
				   return (&empty);
				}   
				break;
		 case WORD2:         /* "name_name" */
				if (symtype == PUBKEY || symtype == NONCE)
				{
				    sp->type = symtype;
   				    oldstate=state;  /* lookup, if second name in symtab */
				    state=INIT_BEGIN;
				    keysymlook(sp,1);
				    state=oldstate;

			            if (sp->type ==  PUBKEY)
			            {
			               
				      dummy1=(char *) malloc(strlen(s)+strlen(p_sign)+1);
				      strcpy(dummy1,s);
				      strcat(dummy1,p_sign);
				      dummy2=(char *) malloc(strlen(s)+strlen(privkey_string)+1);
				      sprintf(dummy2,privkey_string,s);
				      
				      symtype=PRIVKEY; /* add "name_" and inv(name) to symtab */
				      symlook(dummy1,WORD2_);
				      symlook(dummy2,WORD2_);
				      symtype=sp->type;	
                            	    }   
				}
			        else {
				   SyntaxError("Wrong Syntax of PublicKey or Statement",s);
                                   return(&empty);
				}
				break;
		 case WORD2_:       /* "name_name_" */
				if (symtype == PRIVKEY)
				{   
				    sp->type = symtype;
   				    oldstate=state; /* lookup second name */
				    state=INIT_BEGIN;
				    keysymlook(sp,1);
				    state=oldstate;
			        }
				else {
				   SyntaxError("Wrong Syntax of PrivateKey",s);
				   return(&empty);
				}   
				break;
		 case WORD3: /* "name_name_name" */
				if (symtype == SHARKEY)
				{   
				    sp->type = symtype;
   				    oldstate=state;
				    state=INIT_BEGIN;
				    keysymlook(sp,1); /*lookup second name */
				    keysymlook(sp,2); /*lookup third name */
 				    state=oldstate;
				}
			        else {
				   SyntaxError("Wrong Syntax of SharedKey",s);
				   return(&empty);
				}   
				break;
		 default:
				SyntaxError("Unknown Symbol",s);
				return(&empty);
				break;
				
		 }
	      else /* variable == TRUE */
		 switch (stype) {
		 case WORD1:  /* name */
				if (symtype == ID || symtype == SHARKEY ||
				    symtype == PUBKEY || symtype == NONCE)
				   sp->type = symtype;
			        else {
				   SyntaxError("Wrong Syntax of Variable Symbol",s);
				   return(&empty);
				}   	
			            if (sp->type ==  PUBKEY)
			            {
			               
  
					dummy1=(char *) malloc(strlen(s)+strlen(p_sign)+1);
					strcpy(dummy1,s);
					strcat(dummy1,p_sign);
					dummy2=(char *) malloc(strlen(s)+strlen(privkey_string)+1);
					sprintf(dummy2,privkey_string,s);
					symtype=PRIVKEY;
			       		symlook(dummy1,WORD1_);
			       		symlook(dummy2,WORD1_);
					symtype=sp->type;	
                            	    }   
				break;
		 case WORD1_:
				if (symtype == PRIVKEY)
				{
				    sp->type = symtype;
				}
			        else {
				   SyntaxError("Wrong Syntax of Variable PrivKey",s);
				   return(&empty);
				}
				break;
		 default:
				SyntaxError("Unknown Symbol",s);
				return(&empty);
				break;
		 }
			
		 sorts=""; /* sort-information */    			    
		 sorts2="";
                 if (sorts_minimal_flag) 
		    switch (sp->type) {
		    case ID:
				sorts="__principal";
                                sorts2="principal";
				break;
		    case NONCE:
				sorts="__nonce";
                                sorts2="formula";
				break;
		    case PUBKEY: case SHARKEY: case PRIVKEY:
				sorts="__key";
				sorts2="key";
				break;
		    }
		 if (sorts_flag) 
		    switch (sp->type) {
		    case ID:
				sorts="__principal";
				sorts2="principal";
				break;
		    case NONCE:
				sorts="__nonce";
				sorts2="formula";
				break;
		    case PUBKEY:
			   	sorts="__pubkey";
				sorts2="key";
				break; 
		    case SHARKEY: 
			   	sorts="__sharkey";
				sorts2="key";
				break;
		    case PRIVKEY:
				sorts="__privkey";
				sorts2="key";
				break;
		    }
		 
		 if (sorts2_flag) 
		    switch (sp->type) {
		    case ID:
		                sorts2="principal";
		                break;
		    case NONCE:			
                                sorts2="formula";
				break;
		    case PUBKEY: case SHARKEY: case PRIVKEY:		     
				sorts2="key";
				break;
		    }


		    sp->sorts = (char *) malloc(strlen(sp->name)+strlen(sorts)+1);

		    sp->sorts2 = (char *) malloc(strlen(sorts2)+1);
		    strcpy (sp->sorts2,sorts2);

		    if ((sp->type == PRIVKEY) && (sp->name[strlen(sp->name)-1] == ')')){
		      /*special handling of inv(key) */
			strcpy (sp->sorts,sp->name);
			sp->sorts[strlen(sp->name)-1] = '\0';
			   
		        strcat (sp->sorts,sorts);

			strcat (sp->sorts,")");	   

		    }
		    else {
  			strcpy (sp->sorts,sp->name);
			strcat (sp->sorts,sorts);
		    }
                    if (sp->type != PRIVKEY)
                        build_ilf(sp); /* build ilf-information */
                    return sp;
                }
                /* otherwise continue to next */
        }
        yyerror("Too many symbols");
        exit(1);        /* cannot continue */
} /* symlook */



/* creates a new list of pointer to symbols 
   used for (A,B) believes ...
            ^^^^^
*/
struct symlist *new_list_of_symp(struct symtab *symp)
{
	struct symlist *slp;
	slp = (struct symlist *) malloc(sizeof(struct symlist));
        slp->next = NULL;
	slp->symp = symp;
	return (slp);
}



/* add another pointer to symbol to the end of the list */
struct symlist *add_list_of_symp(struct symlist *symlist, struct symtab *symp)
{
	struct symlist *slp,*search;
        slp = (struct symlist *) malloc(sizeof(struct symlist));
        slp->next = NULL;
        slp->symp = symp;
	search=symlist;
        while (search->next != NULL)
		search=search->next;
	search->next=slp;
	return (symlist);	
}

/* delete the list of symbolpointers */
void del_list_of_symp(struct symlist *symlist)
{
	struct symlist *slp,*search;
	search=symlist;
	while (search->next != NULL)
	{
		slp=search;
		search=search->next;
		free(slp);
	}
	free (search);

}


/*create a list of pointer to strings. these strings hold the
  lop-formulas, also the information of cons, terms, etc.
  used for A believes (fresh T_s; fresh T_a);
                      ^^^^^^^^^^^^^^^^^^^^^^^                */
struct strlist *new_list_of_strp(int d_cons,int d_believes,int d_terms, int ground,char *fmt, ...)
{
	va_list ap;
	char *p ,*sval;
	int i,j;
      	struct strlist *slp;
	i =strlen(fmt);
	va_start(ap, fmt);
	for (p= fmt; *p; p++) {
		if (*p != '%') {
			continue;
		}
		switch(*++p) {
		case 's':
			sval = va_arg(ap, char *);
				
			i += strlen(sval);
			i -= 2;
			break;
		default:
			break;
		}
	}
	va_end(ap);
	va_start(ap, fmt);


        
        slp = (struct strlist *) malloc(sizeof(struct strlist));
        slp->next = NULL;
	slp->first = 1;
        slp->string = (char *) malloc(i+1);

        slp->depth_cons=d_cons;
        slp->depth_believes=d_believes;
        slp->depth_terms=d_terms;
        slp->ground=ground;

	j=vsprintf(slp->string,fmt, ap);
	va_end(ap);
	if (i!=j)
		printf("%i <> %i\n",i,j);

        return (slp);


}


/* modify list of pointer to strings */
struct strlist *mod_list_of_strp(int d_cons,int d_believes,int d_terms, int ground,struct strlist *strlist,char *fmt, ...)
{
	
	va_list ap;
	char *p ,*sval;
	int i,j;

      
	i =strlen(fmt);
	va_start(ap, fmt);
	for (p= fmt; *p; p++) {
		if (*p != '%') {
			continue;
		}
		switch(*++p) {
		case 's':
			sval = va_arg(ap, char *);
				
			i += strlen(sval);
			i -= 2;
			break;
		default:
			break;
		}
	}
	va_end(ap);
	va_start(ap, fmt);


        

	sval = (char *) malloc(i+1);




	j=vsprintf(sval,fmt, ap);
	va_end(ap);
	if (i!=j)
		printf("%i <> %i\n",i,j);

	free(strlist->string);

	strlist->string=sval;
        strlist->depth_cons=d_cons;
        strlist->depth_believes=d_believes;
        strlist->depth_terms=d_terms;
        strlist->ground=ground;

	return (strlist);


}

/* add a new pointer to the list of strings */
struct strlist *add_list_of_strp(struct strlist *strlist, struct strlist *slp)

{

      	struct strlist *search;
	slp->first = 0;

	search=strlist;
        while (search->next != NULL)
		search=search->next;
	search->next=slp;
	return (strlist);	
}


/* delete list of pointer to strings */
void del_list_of_strp(struct strlist *strlist)
{
	struct strlist *slp,*search;

	search=strlist;
	while (search->next != NULL)
	{
		slp=search;
		search=search->next;
		free (slp->string);
		free(slp);

	}

	free (search->string);
	free (search);
}


struct strlist *mod_statement_ID (struct symtab *symp , struct strlist *strlist, struct lop_formula formula)
{
  struct strlist *result;
  
  while (strlist) {
    strlist = mod_list_of_strp(
			       formula.cons + strlist->depth_cons,
			       formula.believes + strlist->depth_believes ,
			       formula.terms + strlist->depth_terms,
			       symp->ground + strlist->ground,
			       strlist,
			       formula.formula,
			       symp->sorts,
			       strlist->string
			     );
    if (strlist->first) 
      result=strlist;
    strlist = strlist->next;
  }
  
  return result;

}

struct strlist *mod_statement (struct strlist *strlist, struct lop_formula formula)
{
  struct strlist *result;

  while (strlist) {
    strlist = mod_list_of_strp(
			       formula.cons  + strlist->depth_cons,
			       formula.believes + strlist->depth_believes,
			       formula.terms + strlist->depth_terms,
			       formula.ground + strlist->ground,
			       strlist,
			       formula.formula,
			       strlist->string
			       );
    if (strlist->first) 
      result=strlist;
    strlist = strlist->next;
  }
    
  return result;
  
}


struct strlist *new_statement_key (struct symtab *symp1, struct symtab *symp2,
				   struct symtab *symp3, struct lop_formula formula)
{
  return new_list_of_strp(formula.cons,
			  formula.believes,
			  formula.terms,
                          symp1->ground + symp2->ground + symp3->ground,
			  formula.formula,
                          symp1->sorts,
                          symp2->sorts,
                          symp3->sorts);
}


struct strlist *new_statement_ID_list (struct symtab *symp , struct strlist *strlist, struct lop_formula formula)
{

  /* die schleife ueber strlist wird im pil.y ausgefuehrt */
  return new_list_of_strp(
					       strlist->depth_cons + formula.cons,
                                               strlist->depth_believes + formula.believes,
                                               strlist->depth_terms + formula.terms,
                                               symp->ground  
                                                  + strlist->ground,
                                               formula.formula,
                                               symp->sorts,
                                               strlist->string
                                               );
}


struct strlist *mod_protocol_ID_list (struct symtab *symp , struct strlist *strlist, struct lop_formula formula)
{
  return mod_list_of_strp(
					       strlist->depth_cons + formula.cons,
                                               strlist->depth_believes + formula.believes,
                                               strlist->depth_terms + formula.terms,
                                               symp->ground  
                                                  + strlist->ground,
					       strlist,
                                               formula.formula,
                                               symp->sorts,
                                               strlist->string
					       
                                               );
}


struct strlist *mod_protocol_list_ID (struct strlist *strlist, struct symtab *symp, struct lop_formula formula)
{
  return mod_list_of_strp(
					       strlist->depth_cons + formula.cons,
                                               strlist->depth_believes + formula.believes,
                                               strlist->depth_terms + formula.terms,
                                               symp->ground  
                                                  + strlist->ground,
					       strlist,
                                               formula.formula,
					       strlist->string,
                                               symp->sorts
                                               
                                               );
}


struct strlist *new_statement_nonce (struct  symtab *symp, struct lop_formula formula)
{
  return new_list_of_strp( formula.cons,
			   formula.believes,
			   formula.terms,
			   symp->ground,
			   formula.formula,
			   symp->sorts);


}



struct strlist *process_strlist (struct strlist *strlist)
{
  char *dummy;
  int rekursion =0;
  int fehlende_klammer[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  int fehlende_klammer_aussen=0;
  char klammer[20]=")))))))))))))))))))";
  char klammera[20]=")))))))))))))))))))";
  char copy_klammer[20];  
  char *erg=(char *)malloc(1000);
 
    int d_cons=0;
  int max_d_cons=0;
  int d_believes=0;
  int max_d_believes=0;
  int d_terms=0;
  int max_d_terms=0;
  int ground=0;
  erg[0]='\0';
dummy=(char *)malloc(1000);

/* if this list contains only ONE element, return it */
  if (strlist && !strlist->next){
	return strlist;
	}

  while (strlist != NULL) {
    d_cons++;
    if (max_d_cons < d_cons + strlist->depth_cons)
      max_d_cons = d_cons + strlist->depth_cons;
    
    
    if (d_believes < strlist->depth_believes)
      d_believes = strlist->depth_believes;
    
    d_terms++;
    if (max_d_terms < d_terms + strlist->depth_terms)
      max_d_terms = d_terms + strlist->depth_terms;
    

    if (strlist->ground)
      ground += strlist->ground;

    sprintf (dummy,"and(%s,",strlist->string);
    strcat (erg,dummy);
    strlist = strlist->next;
					    
    
  }
  strcat (erg,"nil");
  
  strcpy(copy_klammer,"");
  
  strncpy(copy_klammer,klammer,
	  d_cons);
  copy_klammer[d_cons]='\0';
  strcat (erg,copy_klammer);
 
  return new_list_of_strp(max_d_cons,d_believes,max_d_terms,ground,"%s",erg);
  
}




































































































