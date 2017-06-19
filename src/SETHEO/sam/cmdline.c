/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: cmdline.c                                    */
/* VERSION:                                           */
/* DATE: 10.11.92                                     */
/* AUTHOR: J. Schumann                                */
/* NAME OF FILE:                                      */
/* DESCR:                                             */
/* MOD:                                               */
/*	10.3.93		jsc	usage w/trailer       */
/*	18.4.95		wolfa	ctype raus            */
/* BUGS:                                              */
/******************************************************/

#include <stdio.h>
#include "cmdline.h"
/**/  
#include <ctype.h>
#include <string.h>


/* #define TEST_DRIVER */

#define DEBUG(X) 


int parsearg(argvp,argcp,intvalp,stringvalp,negated,negstring)

char 	**argvp[];		/*	pointer to argv		*/
int	*argcp;			/*	pointer to argc		*/
int	*intvalp;		/* pointer to int-return val	*/
char	**stringvalp;		/*            string -"-	*/
int	*negated;		/* is result negated?		*/
char	*negstring;		/* negation string, e.g. no	*/
{
  
  int	i;
  char 	*cp;
  int	sign;
  int	val;
  int 	found;
  
  DEBUG(printf("argv[0]=%s argc=%d\n",*argvp[0],*argcp);)

    if (*argcp == 0){
      /* we processed all arguments */
      return noarg;
    }

    if (**argvp[0] != '-'){
      /* this is an absolute argument	*/
      return absarg;
    }
    
    *negated = 0;
    
    
    /* cp points to current parameter */
    cp = *argvp[0] + 1;
    
    /* check if the argument is negated */
    if (negstring){
      /* compare the first letters of *argv[0] +1 */
      /* with the negstring */
      if (!strncmp(cp,negstring,strlen(negstring))){
	/* yes, it is negated */
	*negated = 1;
	cp += strlen(negstring);
      }
    }
    
    /* search for a matching argument */
    for (i=0,found=-1;i < size_argtab;i++){
      if (!strcmp(cp,argtab[i].name)){
	found = i;
	break;
      }
    }
    
    if (found < 0){
      /* argument was not found in table */
      return argnotfound;
    }
    
    /* adjust the argv,argc */
    (*argcp)--;
    (*argvp)++;
    
    /* process (optional) arguments for that parameter */

    
    if (argtab[found].arg_type == none){
      /* we have no argument */
      /* we are done */
      *intvalp = 0;
      *stringvalp = NULL;
      
      return argtab[found].ident;
    }
    
    if (argtab[found].opt_arg == opt){
      /* this is an optional parameter */
      /* so, first set the default values */
      if (argtab[found].arg_type == integer){
	*intvalp = argtab[found].defint;
      }
      else {
	*stringvalp = argtab[found].defstr;
      }
    }
    
    /* try to get the argument */
    /* are we at the end of the argument list */
    if (!*argcp){
      /* yes */
      if (argtab[found].opt == opt){
	/* we are done */
	return argtab[found].ident;
      }
      else {
	/* required argument is missing */
	return argmissing;
      }
    }
    
    
    cp = *argvp[0];		/* in this position, the argument is */
    /* supposed */
    
    if (argtab[found].arg_type == integer){
      /* get the integer, if possible */
      if (*cp == '-'){
	/* is probably a signed integer */
	sign = -1;
	cp++;
      }
      else sign = 1;
      
      val = 0;
      while (*cp){
	if (!isdigit(*cp)){
	  /* is not a number */
	  if (argtab[found].opt_arg == opt){
	    /* we are done */
	    return argtab[found].ident;
	  }
	  else {
	    return intexpected;	
	  }
	}
	val = 10 * val + (*cp) - '0';
	cp++;
      }
      /* return the signed value */
      *intvalp = sign *val;
      (*argcp)--;
      (*argvp)++;
      return argtab[found].ident;
    }
    else {
      /* this is a string */
      *stringvalp = cp;
      (*argcp)--;
      (*argvp)++;
      return argtab[found].ident;
    }
}


void  usage(fp,av0,trailer)
FILE 	*fp;
char	*av0;
char	*trailer;
{
  int i;
  
  fprintf(fp,"usage: %s %s\n",av0,trailer);

  for (i=0; i< size_argtab;i++){
    if (argtab[i].opt == opt){
      fprintf(fp,"\t[");
    }
    
    fprintf(fp," -%s ",argtab[i].name);
    switch(argtab[i].arg_type){
    case integer:
      if (argtab[i].opt_arg == opt){
	fprintf(fp,"[");
      }
      fprintf(fp,"number");
      
      if (argtab[i].opt_arg == opt){
	fprintf(fp,"]   (default= %d)",
		argtab[i].defint);
      }
      break;
    case string:
      if (argtab[i].opt_arg == opt){
	fprintf(fp,"[");
      }
      fprintf(fp,"string");
      
      if (argtab[i].opt_arg == opt){
	fprintf(fp,"]   (default= \"%s\")",
		argtab[i].defstr);
      }
      break;
    case none:
      break;
    }
    if (argtab[i].opt == opt){
      fprintf(fp,"]");
    }
    
    fprintf(fp,"\n");
  }
  fprintf(fp,"\t%s\n",trailer);
}

char *cmd_line_errtxt(res)
cmdline_retval res;
{
  switch (res){
  case noarg:
    return "all arguments processed";
  case absarg:
    return "this is an argument without a '-'";
  case argnotfound:
    return "unknown option";
  case argmissing:
    return "argument missing";
  case intexpected:
    return "integer expected";
  default:
    return "no error";
  }
}


#ifdef TEST_DRIVER

#define VERBOSE		1
#define DEPTH		2
#define TREE		3
#define OUTFILE		4

argentry argtab[] = {
  { VERBOSE,	opt,	"verbose",	integer,	opt,	"",	128},
  { DEPTH,	opt,	"depth",	integer,	req,	"",	0},
  { TREE,		opt,	"t",	none,	opt,	"",	0},
  { OUTFILE,	opt,	"o",	string,	req,	"txt.out",	0},
};

int size_argtab = sizeof(argtab)/sizeof(argentry);


main(argc,argv)
char **argv;
int argc;
{
  int res;
  int neg, iv;
  char *istr = NULL;
  char *progname = argv[0];
  
  printf("%s\n",argv[0]);
  
  argv++;
  argc--;
  
  while ((res = parsearg(&argv,&argc,&iv,&istr,&neg,"no"))>=0 ){
    printf("argument %s (%d) parsed. result=%d \n",argv[0],argc,res);
    printf("iv=%d istr=%s neg=%d\n", iv,istr,neg);
  }
  printf("parsearg failed: res=%d\n",res);
  printf("error: %s\n",cmd_line_errtxt(res));
  usage(stderr,argv[0],"test-setheo");
}

#endif
