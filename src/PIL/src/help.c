/******************************************************
 *     PIL System
 *     $Id: help.c,v 1.1.1.1 2001/07/16 18:16:09 schumann Exp $
 *
 *     (c) Johann Schumann, Klaus Wagner
 *     TU Muenchen and NASA Ames
 *
 *     Origin: 2.1 06/18/98
 *
 *     $Log: help.c,v $
 *     Revision 1.1.1.1  2001/07/16 18:16:09  schumann
 *     new CVS project
 *
******************************************************/

int intlen(int n)
{
  int i, sign, res;
  
  if ((sign = n) < 0)
    n = -n;

  i = res = 0;
  do { res++; 
  } while ((n /= 10) > 0);
  if (sign < 0)
    res++;
  return res;
}


/* create outfile-x.ilf */

void create_ilf_file(char *prog, char *outfile,int outfilenumber,struct symtab symtab[NSYMS])
{        

        struct symtab *sp; /* symbol-table-pointer */
        FILE *fp_ilf; /* file-handle */
	char *ilffilename = (char *) malloc(strlen(outfile)+
                                            strlen(default_ilf_extension)+
                                            intlen(outfilenumber)+2);
	sprintf(ilffilename,"%s-%i%s",outfile,outfilenumber,default_ilf_extension);
	if ((fp_ilf=fopen(ilffilename,"w"))==NULL) {
	   fprintf (stderr,"%s: can't write %s\n",prog,ilffilename);
	   exit(1);
	}
	for(sp = symtab; sp < &symtab[NSYMS]; sp++) {
           if (sp->name && sp->type != PRIVKEY && !sp->ground)
               fprintf(fp_ilf,"%s",sp->ilf);
        } 
        if (fclose(fp_ilf)) {
	   fprintf (stderr,"%s: can't close %s\n",prog,ilffilename);
	   exit(1);
	}
        free(ilffilename);
}

FILE *open_out_file(char *prog, char *outfile,char *outfilename, 
		    int outfilenumber,FILE *fp_out)
{ /* open outfile */
   outfilename = (char *) malloc(strlen(outfile)+
				      strlen(default_pil_extension)+
                                      intlen(outfilenumber)+2);
   sprintf(outfilename,"%s-%i%s",outfile,outfilenumber,default_lop_extension);
   if ((fp_out=fopen(outfilename,"w"))==NULL) {
      fprintf (stderr,"%s: can't write %s\n",prog,outfilename);
      exit(1);
   }
   fprintf (fp_out,"/*  lop-file, produced by pil     */\n\n");
   return(fp_out);
}


void close_out_file(char *prog, char *outfilename,FILE *fp_out)
{   /* close outfile */
    if (fclose(fp_out)) {
       fprintf (stderr,"%s: can't close %s\n",prog,outfilename);
       exit(1);
    }
   
}

void append_out_file_sorts2(FILE *fp_out,struct symtab symtab[NSYMS])
{

/* special sortinformation exclusive for J.S. */

   struct symtab *sp; /* symbol-table-pointer */

#ifdef WITH_GCC_ONLY

   fprintf (fp_out,
" sorts : (
        principal,
        key,
        message,
        formula
        ).

subsorts: (
        principal subsort_of formula,
        key subsort_of formula,
        message subsort_of formula
        ).

functions: (
        bel: (principal,formula) => formula,
        sees: (principal,formula) => formula,
        said: (principal,formula) => formula,
        controls: (principal,formula) => formula,
        fresh: (formula) => formula,
        sk: (principal,principal,key) =>formula,
        encr: (formula,key) =>formula,
        and: (formula,formula) => formula");

     for(sp = symtab; sp < &symtab[NSYMS]; sp++) {
       if (sp->name && sp->type != PRIVKEY)
	 fprintf(fp_out,",\n        %s: %s",sp->name,sp->sorts2);
     }

  
   fprintf(fp_out,
"
        ).

relations: (
        holds: (formula),
        subset: (formula,formula),
        oneof: (formula,formula)
        ).
      \n\n\n");
#endif

} /* sorts2 ends here */

void apppend_out_file_info(FILE *fp_out, struct strlist *strlist)
{  /*append info about cons, belives, term-depth, ground */
   fprintf (fp_out,"/* cons: %i believes: %i terms: %i ground: %i */\n",
                              strlist->depth_cons,strlist->depth_believes,
                              strlist->depth_terms,strlist->ground);
}


void append_out_file_query(FILE *fp_out,struct strlist *strlist,
                           int outfilenumber, int info_flag)
{
  /* append query */
   fprintf (fp_out,"\n/*  query     */\n\n");
   if (info_flag)
      apppend_out_file_info(fp_out,strlist);
   fprintf (fp_out,"#clausename query_%i\n",outfilenumber-1);
   fprintf (fp_out,"<- %s.\n\n",strlist->string); 
}




void append_out_file_proto(FILE *fp_out,struct strlist *strlist,
                           int number,int info_flag)
{          /* after message #strlist->number# */
           /* go over protocol */
   while(strlist) {
      if(strlist->number <= number) {  /* only protocolsteps, we need */
	 if (info_flag)
            apppend_out_file_info(fp_out,strlist);
	 fprintf (fp_out,"#clausename message_%i\n",strlist->number);
	 fprintf (fp_out,"%s<-.\n\n",strlist->string); 
      }
      strlist=strlist->next;
   }
}

	 
void append_out_file_assump(FILE *fp_out,struct strlist *strlist,
                                  int info_flag)
{
    /* go over assumptions */
    int assumption_number=1;
    
    fprintf (fp_out,"\n/*  assumptions     */\n\n");
    while(strlist) {
       if (info_flag)
          apppend_out_file_info(fp_out,strlist);
        fprintf (fp_out,"#clausename assumption_%i\n",assumption_number++);
	fprintf (fp_out,"%s<-.\n\n",strlist->string); 
	strlist=strlist->next;
    }
}


void append_out_file_conject(FILE *fp_out,struct strlist *strlist_conject,
			     struct strlist *strlist, int info_flag, 
			     char *outfile, int outfilenumber)
{  /* go over proofen conjectures */
   int conjecture_number=1;	
   fprintf (fp_out,"/* proofen ?! conjectures from %s-1 to %s-%i */\n",
				outfile,outfile,outfilenumber-2);
   while(strlist_conject != strlist) {
       if (info_flag)
          apppend_out_file_info(fp_out,strlist_conject);
       fprintf (fp_out,"#clausename conjecture_%i\n",conjecture_number++);
       fprintf (fp_out,"%s<-.\n\n",strlist_conject->string); 
       strlist_conject=strlist_conject->next;
   }
}   



           
void append_out_file_rules(FILE *fp_out, char *prog, char *rulesfile)
{  /* copy ban-rules */

//JSC port2linuxppc   char c;
   int c;
   FILE *fp_rules;

   fprintf (fp_out,"\n\n/*  rules     */\n\n");

   if ((fp_rules=fopen(rulesfile,"r"))==NULL) {
      fprintf (stderr, "%s: can't open rulesfile %s\n",prog, rulesfile);
      exit(1);
   } else {
      while ((c = getc(fp_rules)) != EOF)
         putc((char)c,fp_out);
   }
}



	/* creates Makefile.outfile */
void create_make_file(char *prog, char *makefile, char *skelfile, 
		      char *outfile,
		      int outfilenumber)
{   /* creates Makefile.outfile */

   FILE *fp_make, *fp_skel;
   int i;
//JSC port2linuxppc   char c;
   int c;

   if ((fp_make=fopen(makefile,"w"))==NULL) {
      fprintf (stderr,"%s: can't write %s\n",prog,makefile);
      exit(1);
   }
        
   fprintf (fp_make,"# Makefile produced by pil     \n\n");

   if ((fp_skel=fopen(skelfile,"r"))==NULL) {
      fprintf (stderr, "%s: can't open %s, using defaults for Makefile\n",
                            prog, skelfile);
      fprintf (fp_make,"SETHEO = ILFBANSETHEO\n");  /* default skeleton */
      fprintf (fp_make,"MKREPORT = mkreport\n");  /* default skeleton */
   } else {  /*copy skeleton-Makefile */
      while ((c = getc(fp_skel)) != EOF)
	 putc((char)c,fp_make);
      if (fclose(fp_skel)) {
         fprintf (stderr,"%s: can't close %s\n",prog,skelfile);
         exit(1);
      }
   }

   /*generate dependencies */
   fprintf (fp_make,"\n\nPROOFS = ");
   for (i=1; i < outfilenumber; i++) 
      fprintf (fp_make,"%s-%i.tree ",outfile,i);
   fprintf (fp_make,"\n\n");
   fprintf (fp_make,"proofs:	$(PROOFS)\n");
   fprintf (fp_make,"\t$(MKREPORT) $(PROOFS)\n\n");
   fprintf (fp_make,"all:	$(PROOFS)\n\n");
   for (i=1; i < outfilenumber; i++) {
      if (i == 1)
	 fprintf (fp_make,"%s-%i.tree: %s-%i.lop\n",outfile,i,outfile,i);
      else
	 fprintf (fp_make,"%s-%i.tree: %s-%i.lop %s-%i.tree\n",
                                outfile,i,outfile,i,outfile,i-1);	
      fprintf (fp_make,"	$(SETHEO) %s-%i\n\n",outfile,i);
   }

   if (fclose(fp_make)) {
      fprintf (stderr,"%s: can't close %s\n",prog,makefile);
      exit(1);
   }
        /* makefile done */	
}
