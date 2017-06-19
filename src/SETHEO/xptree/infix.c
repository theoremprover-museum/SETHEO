/*########################################################################
# File:			infix.c
# SCCS-Info:		@(#)infix.c	2.3 11/28/94
# Author:		J. Schumann
# Date:			1994
# Context:		xptree
# 
# Contents:		list handling and conversion term->string
#
# Modifications:	
#	(when)		(who)		(what)
#	29.7.94		jsc		bugs
#	28.11.94	jsc		bugs (w. assoc operations)
#
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
########################################################################*/

#include <stdio.h>
#include <stdlib.h>

#include "xptree.h"
#include "infix.h"

#define DEBUG(X) 
/* #define STANDARD_CONVERSION /**/

char *do_convert();
	
/***************************************************************/
/***************************************************************/
/* globals for the conversion table
/***************************************************************/
/***************************************************************/
symbtab conversiontable;
int nrentries;

/***************************************************************/
/***************************************************************/
/*  functions to construct the term-tree
/***************************************************************/
/***************************************************************/

/***************************************************************/
/* generate a leaf node and return its address                 */
/***************************************************************/
exprptr mkleaf(s)
char *s;
{
exprptr ep;

ep=(exprptr)malloc(sizeof(struct expr));
ep->name = s;
ep->next = NULL;
ep->down = NULL;
return ep;
}

/***************************************************************/
/* generate a node with one down-pointer. e.g. for f(X)        */
/* the address of the node is returned                         */
/***************************************************************/
exprptr mknode(s,down)
char *s;
exprptr down;
{
exprptr ep;

ep=(exprptr)malloc(sizeof(struct expr));
ep->name = s;
ep->next = NULL;
ep->down = down;
return ep;
}

/***************************************************************/
/* generate a node  op(ST1,ST2) and return its address         */
/***************************************************************/
exprptr mknode3(s,st1,st2)
char *s;
exprptr st1,st2;
{
exprptr ep;

ep=(exprptr)malloc(sizeof(struct expr));
ep->name = s;
ep->next = NULL;
ep->down = st1;
st1->next = st2;
return ep;
}

/***************************************************************/
/* concatenate 2 nodes via their next-link and return the
/* address of the first one    
/***************************************************************/
exprptr mkcons(st1,st2)
exprptr st1,st2;
{
st1->next = st2;
return st1;
}

/***************************************************************/
/***************************************************************/
/*  functions for the conversion table
/***************************************************************/
/***************************************************************/

/***************************************************************/
/* initialize the conversion table
/*	and enter ~ and '|'
/***************************************************************/
init_conversion_table()
{
conversiontable = (symbtab)malloc(MAXENTRIES_CONVERSIONTABLE*
			sizeof(symbtabel));

CONVENTER(0,"~","~ ",50,prefix);
CONVENTER(1,"|"," | ",50,prefix);
nrentries = 2;
}

/***************************************************************/
/* read  the conversion table from a named file
/* returns: number of conversions read
/***************************************************************/
int read_conversion_table(fn)
char *fn;
{
FILE *f;
static char line[255];
static char printname[255];
int bp;
char type;
char *cp1;
char *cp2;

if (!(f=fopen(fn,"r"))){
	fprintf(stderr,"xptree: could not open %s\n",fn);
	return 0;
	}
DEBUG(fprintf(stderr,"reading >%s<\n",fn);)

while(fgets(line,255,f)){
	/*
	/* read conversion in successfully
	/**/

	if (nrentries == MAXENTRIES_CONVERSIONTABLE){
	  fprintf(stderr,"xptree: too many entries in conversion table\n");
	  fclose(f);
	  return nrentries;
	  }
	
	cp1=cp2=line;

	/* parse parse-name */
	while (*cp1 && *cp1 != ':') cp1++;
	if (!*cp1){
		fprintf(stderr,"xptree: error in ctab file\n");
		fclose(f);
		return 0;
		}
	*cp1 = '\0';
	conversiontable[nrentries].parsename = strdup(line);
	DEBUG(fprintf(stderr,"read: parsename >%s<\n",conversiontable[nrentries].parsename);)

	/* parse print-name */
	cp1++;
	cp2=cp1;
	while (*cp1 && *cp1 != ':') cp1++;
	if (!*cp1){
		fprintf(stderr,"xptree: error in ctab file\n");
		fclose(f);
		return 0;
		}
	*cp1 = '\0';
	conversiontable[nrentries].printname = strdup(cp2);
	DEBUG(fprintf(stderr,"read: printname >%s<\n",conversiontable[nrentries].printname));

	cp1++;
	/* parse binding power */
	bp=0;
	while (*cp1 && *cp1 != ':'){
		if (isdigit(*cp1)){
			bp = 10*bp+*cp1 - '0';
			}
		else {
			fprintf(stderr,"xptree: error in ctab file\n");
			fclose(f);
			return 0;
			}
		cp1++;
		}	
	conversiontable[nrentries].bp = bp;
	DEBUG(fprintf(stderr,"read: bp  >%d<\n",bp);)
	if (!*cp1){
		fprintf(stderr,"xptree: error in ctab file\n");
		return 0;
		}
	cp1++;
	switch (*cp1){
	case 'I':
	case 'i':
		conversiontable[nrentries].type = infix;
		break;
	case 'P':
	case 'p':
		conversiontable[nrentries].type = prefix;
		break;
	case 'Q':
	case 'q':
		conversiontable[nrentries].type = postfix;
		break;
	default:
	  fprintf(stderr,"xptree: illegal type: %c\n",*cp1);
	  fclose(f);
	  return nrentries;
	  }
	nrentries++;
	}
fclose(f);
return nrentries;
}
		


/***************************************************************/
/* look the parse-name up in the conversion table
/* return: ptr to symbol table element if found, NULL otherwise
/***************************************************************/
symbtab getsymbtabentry(s)
char *s;
{
int i;

DEBUG(fprintf(stderr,"getsymbtabentry: >%s<\n",s);)

for (i=nrentries-1;i>=0;i--){
	DEBUG(fprintf(stderr,"compare i=%d >%s< >%s<\n",i,s,conversiontable[i].parsename);)
	if (!strcmp(s,conversiontable[i].parsename)){
		return conversiontable+i;
		}
	}

return NULL;
}

/***************************************************************/
/* convert a term into a string and return the xptree-structure
/* label_str (a new one) with that string
/***************************************************************/
struct label_str convert_to_string(term)
exprptr term;
{
struct label_str *x;
static char buf[1000];
char *cp;

DEBUG(fprintf(stderr,">>>entering convert_to...\n");)
DEBUG(fflush(stderr);)

x=(struct label_str *)malloc(sizeof(struct label_str));

/* convert to full depth */
cp =do_convert(term,buf,sizeof(buf),9999,0);
*cp = '\0';
x->exp = strdup(buf);

/* convert to the given depth only */
cp =do_convert(term,buf,sizeof(buf),tdmax,0);
*cp = '\0';
x->abbr = strdup(buf);

return *x;
}

#ifdef STANDARD_CONVERSION
/***************************************************************/
/* standard conversion                                         */
/* term->string (in buf)
/* returns: to end of string (= 1+last)
/* Note: the string in buf is NOT \0-terminated
/*       the buffer-size is NOT checked at the moment
/***************************************************************/
char *do_convert(term,buf,bufsize,termdepth)
exprptr term;
char *buf;
int bufsize;
int termdepth;
{
char *cp;
int closepar;

if (!term) {
	DEBUG(fprintf(stderr,"END\n");)
	DEBUG(fflush(stderr);)
	return buf;
	}

DEBUG(fprintf(stderr,"%s",term->name);)
cp=term->name;
while (*buf++=*cp++);
buf--;

if (term->down){
	closepar=0;
	if (strcmp(term->name,"~")){
		*buf++ = '(';
		DEBUG(fprintf(stderr,"(");)
		closepar=1;
		}
	buf=do_convert(term->down,buf,9999,9999);
	if (closepar) {
		*buf++ = ')';
		DEBUG(fprintf(stderr,")");)
		}
	}
if (term->next){
	while (term->next){
		DEBUG(fprintf(stderr,",");)
		*buf++ = ',';
		buf=do_convert(term->next,buf,9999,9999);
		term=term->next;
		}
	}
DEBUG(fprintf(stderr,"END\n");)
DEBUG(fflush(stderr);)
return buf;
}
#else
/***************************************************************/
/* conversion of term into a string                            */
/* using the conversion table
/* term->string (in buf)
/* Params:
/*	term 	= input term
/*	buf  	= place where to put the resulting string
/*	bufsize = sizeof(buf)
/*	termdepth = maximal allowable display depth
/*	prio    = current binding power (should be 0 on entry)
/*
/* returns:
/*	pointer to end of string (= 1+last)
/* Note: the string in buf is NOT \0-terminated
/*       the buffer-size is NOT checked at the moment
/***************************************************************/
char *do_convert(term,buf,bufsize,termdepth,prio)
exprptr term;
char *buf;
int bufsize;
int termdepth;
int prio;
{
char *cp;
int closepar;
exprptr t;
symbtab st;
char paropen,parclose;

if (!term) {
	DEBUG(fprintf(stderr,"END\n");)
	DEBUG(fflush(stderr);)
	return buf;
	}

DEBUG(fprintf(stderr,"%s",term->name);)

/*
/* we have to check, if we have to decrease the
/* termdepth:
/* in case of '~', we do not decrease it
/**/
if (*(term->name) != '~') termdepth--;

if (!termdepth){
	/*
	/* we have reached the boundary
	/* print "..." and we are done
	/**/
	*buf++='.';
	*buf++='.';
	*buf++='.';
	return buf;
	}

if (!term->down){
	/*
	/* leaf
	/* nothing to do, just print the name
	/* V1.X: printnames for operators ONLY
	/**/
	cp=term->name;
	while (*buf++=*cp++);
	buf--;
	return buf;
	}

/*
/* we have subterms
/**/


/*
/* we have a compound term w/ subterms which we have to
/* process
/**/
if (!(st=getsymbtabentry(term->name))){
	 /*
	/* this functor is not in the symbol table:
	/* convert it as full parenthsized prefix
	/**/
	if (*term->name == '[' && !*(term->name +1)){
		/*
		/* we have a list
		/**/
		paropen='[';
		parclose=']';
		}
	else {
		cp=term->name;
		while (*buf++=*cp++);
		buf--;
		paropen='(';
		parclose=')';
		}
	*buf++ = paropen;
	t=term->down;
	while (t){
		buf=do_convert(t,buf,9999,termdepth,prio);
		if (t->next){
			*buf++ = ',';
			}
		t=t->next;
		}
	*buf++ = parclose;
	return buf;
	}

/*
/* this is a functor which is in the conversion table:
/* at st
/**/
switch (st->type){
case prefix:
	/* convert it to prefix w/parenthesis if necessary
	/**/
	cp=st->printname;
	while (*buf++=*cp++);
	buf--;
	closepar=0;
	if (prio > st->bp){
		/*
		/* we have to set parenthesis
		/**/
		closepar=1;
		*buf++ = '(';
		}
	t=term->down;
	while (t){
		buf=do_convert(t,buf,9999,termdepth,st->bp);
		if (t->next){
			*buf++ = ',';
			}
		t=t->next;
		}
	if (closepar) *buf++ = ')';
	return buf;

case infix:
	/* convert it to prefix w/parenthesis if necessary
	/**/
	closepar=0;
	if (prio >= st->bp){
		/*
		/* we have to set parenthesis
		/**/
		closepar=1;
		*buf++ = '(';
		}
	t=term->down;
	while (t){
		buf=do_convert(t,buf,9999,termdepth,st->bp);
		if (t->next){
			cp=st->printname;
			while (*buf++=*cp++);
			buf--;
			}
		t=t->next;
		}
	if (closepar) *buf++ = ')';
	return buf;

case postfix:
	/* convert it to postfix w/parenthesis if necessary
	/**/
	closepar=0;
	if (prio > st->bp){
		/*
		/* we have to set parenthesis
		/**/
		closepar=1;
		*buf++ = '(';
		}
	t=term->down;
	while (t){
		buf=do_convert(t,buf,9999,termdepth,st->bp);
		if (t->next){
			*buf++ = ' ';
			}
		t=t->next;
		}
	if (closepar) *buf++ = ')';
	cp=st->printname;
	while (*buf++=*cp++);
	buf--;
	return buf;
	}
}
#endif
