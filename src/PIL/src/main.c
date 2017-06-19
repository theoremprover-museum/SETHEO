/******************************************************
 *     PIL System
 *     $Id: main.c,v 1.1.1.1 2001/07/16 18:16:09 schumann Exp $
 *
 *     (c) Johann Schumann, Klaus Wagner
 *     TU Muenchen and NASA Ames
 *
 *     Origin: 2.1 06/18/98
 *
 *     $Log: main.c,v $
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
#include "pily.h"

extern FILE *yyin;   /* yacc-infile */

struct strlist *strlist; /* linked lists with lop-formulas */
struct symlist *symlist; /* linked symbol-list */
struct strlist *strlist_proto, /*linked list with protocol-formulas */ 
               *strlist_assump, /*linked list with assumptions-formulas */
               *strlist_conject; /*linked list with conjecture-formulas */

struct symtab symtab[NSYMS];  /* symbol-table */

int sorts_flag = TRUE;  
int sorts_minimal_flag = FALSE;
int sorts2_flag = FALSE;

enum which_logic used_logic;

#include "help.c" /*helping functions for doing output go there */ 

main(int argc, char *argv[])
{
  
  
  
  struct symtab *sp; /* symbol-table-pointer */
  
  struct strlist *strlist; /* pointer to conjecture-list */
  
  int number, i;
  char c;
  char *prog =argv[0];
  FILE *fp_out, *fp_make, *fp_rules, *fp_skel, *fp_ilf; /*file-handlers*/
  char *outfilename, *outfile, *infile, *infilename, *makefile, *rulesfile,
    *skelfile, *ilffilename; /*strings for filenames */
 
  int ilf_flag = FALSE;         /* option-flags */  
  int outfile_flag = FALSE;
  int skelfile_flag = FALSE;
  int makefile_flag = FALSE;
  int info_flag = TRUE;
  int rulesfile_flag = FALSE;
  int use_proofed_flag = TRUE;
                                /* option-flags end here */

  int arg_error_flag = FALSE;   /* argument-error */

  int outfilenumber=1;          /* counters */
  int conjecture_number=1;
  int assumption_number=1;

  char *default_rulesfile;

  infile=NULL;

	/* which logic is used? */
	used_logic = BAN;
             
  fprintf(stderr,"%s (%s)\n",prog,"$Id: main.c,v 1.1.1.1 2001/07/16 18:16:09 schumann Exp $");

  if (argc < 2)                /* only <infile> */
    arg_error_flag = TRUE;
  else                         /* other options */
    for (i=1; i < argc; i++) {
      if (strcmp(argv[i], "-make") == 0) {
	i++;
	if (i >= argc)
	  arg_error_flag = TRUE;
	else
	  makefile=strdup(argv[i]);
	makefile_flag = TRUE;

      } else
	if (strcmp(argv[i], "-out") == 0) {
	  i++;
	  if (i >= argc)
	    arg_error_flag = TRUE;
	  else
	    outfile=strdup(argv[i]);
	  outfile_flag=TRUE;
	} else
	  if (strcmp(argv[i], "-skel") == 0) {
	    i++;
	    if (i >= argc)
	      arg_error_flag = TRUE;
	    else
	      skelfile=strdup(argv[i]);
	    skelfile_flag=TRUE;
	  } else
            if (strcmp(argv[i], "-no_sorts") == 0) {
	      sorts_flag = FALSE;
            } else
	      if (strcmp(argv[i], "-sorts_minimal") == 0) {
		sorts_minimal_flag = TRUE;
		sorts_flag = FALSE;
	      } else 
		if (strcmp(argv[i], "-sorts2") == 0) {
		  sorts2_flag = TRUE;
		} else                   
		  if (strcmp(argv[i], "-noinfo" ) == 0) {
		    info_flag = FALSE;
		  } else
		    if (strcmp(argv[i], "-ilf" ) == 0) {
		      ilf_flag = TRUE;
		    } else
		      if (strcmp(argv[i], "-rules" ) == 0) {
			i++;
			if (i >= argc)
			  arg_error_flag = TRUE;
			else
			  rulesfile=strdup(argv[i]);
			rulesfile_flag = TRUE;			
		      } else                     
			if (strcmp(argv[i], "-no_use_proofed" ) == 0) {
			  use_proofed_flag = FALSE;
			} else
			  if (infile == NULL) 
			    infile = strdup(argv[i]);
			  else
			    arg_error_flag = TRUE;
    }
  if (sorts_minimal_flag && sorts_flag)
    arg_error_flag = TRUE;

  if (infile == NULL) 
    arg_error_flag = TRUE;

  
  if (arg_error_flag) {
    fprintf(stderr, 
            "usage: %s [-make <makefile>] [-skel <skelfile>] [-rules <rulefile] [-noinfo] [-no_sorts | -sorts_minimal] [-sorts2] [-no_use_proofed] [-out <outfile>] [-ilf] <infile> \n",prog);
    exit(1);
  }
     
  
  if (!outfile_flag)
    outfile=strdup(infile);/* use infilename for outfilename, if not spezified */
  
  
  infilename = (char *) malloc(strlen(infile)+strlen(default_pil_extension)+1);
  sprintf(infilename,"%s%s",infile,default_pil_extension); /* infilename without extension */
  if ((yyin=fopen(infilename,"r"))==NULL) {  /* open infile */
    fprintf (stderr, "%s: can't open %s\n",prog,infilename);
    exit(1);
  }        

  
  
  
  
  if (! makefile_flag) {
    makefile = (char *)malloc(strlen(default_makefile)+strlen(outfile)+1);
    sprintf(makefile,"%s%s",default_makefile,outfile);  /* default Makefile */
  }	
  
  if (! skelfile_flag) {
    skelfile = (char *)malloc(
		strlen(LIBDIR)+
		strlen(default_skelfile)+
		2);
    sprintf(skelfile,"%s/%s",LIBDIR,default_skelfile);       /* default Skeletonfile */
  }
  
  
  
  
  
  yydebug=0;                               
  yyparse();                                   /* parse infile */
  
  /* veraendert:
     strlist_conject: alle conjectures im lop-format
     strlist_assump:  alle assumptions im lop-format
     strlist_proto:   protokoll im lop-format
     symlist:         Symboltabelle
     used_logic
     */            

  if (! rulesfile_flag) {
	if (used_logic == BAN){
		default_rulesfile=default_rulesfile_BAN;
		}
	else {
		default_rulesfile=default_rulesfile_AUTLOG;
		}
	
    rulesfile = (char *) malloc(
		strlen(LIBDIR)+
		strlen(default_rulesfile)+
		2);
    sprintf(rulesfile,"%s/%s",LIBDIR,default_rulesfile); /*default rules-file */
  }
  
   
  
  
  
  /* creates outfile-x.lop */
  strlist=strlist_conject;
  outfilenumber=1;
  while(strlist) {   /* go over conjectures */
    
    if (ilf_flag) { /* first, we create outfile-x.ilf */
      create_ilf_file(prog,outfile,outfilenumber,symtab);        
    }
    
    /* create outfile-i.lop */
    fp_out=open_out_file(prog,outfile,outfilename,outfilenumber,fp_out);
    outfilenumber++;
    
    
    if (sorts2_flag) /* special sortinformation exclusive for J.S. */
      append_out_file_sorts2(fp_out,symtab);
     
    /* query */
    append_out_file_query(fp_out,strlist,outfilenumber,info_flag);
    
    
    /* go over protocol */
    append_out_file_proto(fp_out,strlist_proto,
			  strlist->number,info_flag);
    
    /* go over assumptions */
    append_out_file_assump(fp_out,strlist_assump,
			   info_flag);
    
    /* go over proofen conjectures */
    if (use_proofed_flag)
      append_out_file_conject(fp_out,strlist_conject,strlist,
			      info_flag, outfile, outfilenumber);
    
    /* copy ban-rules */
    append_out_file_rules(fp_out,prog,rulesfile); 
    
    /* close outfile */
    
    close_out_file(prog, outfilename, fp_out);
    strlist=strlist->next;
    
  }
  /* all done in outfile */
  
	   
  /* creates Makefile.outfile */
  create_make_file(prog, makefile, skelfile, outfile, outfilenumber);
  
  
}
/* all done */









