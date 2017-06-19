//JSC #include <malloc.h>
#include <stdlib.h>
#include "xptree.h"
#include <stdio.h>
#include <string.h>


typedef struct str_varlist *varlist;
struct str_varlist {
  varlist next;
  char *name;
  int number;
};

#ifdef __ANSI__
char *symb(char * var);
#else
char *symb();
#endif

bool symbs;
char vbuf[20];
varlist vl = NULL;

#ifdef __ANSI__
char *symb(char * var) {
#else
char *symb(var)
  char * var;
{
#endif
  varlist l0, l1;
  int number;

  if (!symbs) return (var);
  for (l0=l1=vl; l0 && !streq(l0->name, var); l1=l0, l0=l0->next);
  if (!l0) {
    l0 = (varlist) malloc(sizeof(struct str_varlist));
    l0->name = strdup(var);  
    l0->next=NULL;
    if (l1) {
      l1->next=l0;
      number = l0->number = l1->number+1;
    } else {
      vl=l0;
      number = l0->number = 0;
    }
  } else {
    number = l0->number;
  }
  sprintf(vbuf, "X%d", number);
  return vbuf;
}

#ifdef __ANSI__
bool streq(char *s1, char *s2) {
#else
bool streq(s1, s2)
  char *s1;
  char *s2;
{
#endif
  
  while (*s1==*s2 && *s1 != '\0') { s1++; s2++; }
  return (*s1==*s2);
}
