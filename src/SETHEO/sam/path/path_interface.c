/******************************************************
 *    S E T H E O                                     
 *                                                    
 * FILE: 	path_interface.c
 * VERSION:
 * DATE: 	16.9.93
 * AUTHOR: 	J. Schumann
 * DESCR:
 * MOD:
 * BUGS:
 ******************************************************/

#include <stdio.h>
#include "path/path_interface.h"

int term_with_all_variables(T)
WORD *T;
{
  if ((GETTAG(*T) == T_CONST) || (ISVAR(*T))){
    printf("object is var or const\n");
    return 0;
  }
  T++;
  while (GETTAG(*T) != T_EOSTR){
    if(!ISVAR(*ref_head(T))){
      return 0;
    }
    T++;
  }
  return 1;
}

