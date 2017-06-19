/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: i_negate.c
/* VERSION:
/* DATE:
/* AUTHOR:
/* NAME OF FILE:
/* DESCR:
/* MOD:
/* BUGS:
/******************************************************/

#include <stdio.h>
#include "tags.h"
#include "machine.h"
#include "unification.h"
#include "symbtab.h"
#include "disp.h"

#define INSTR_LENGTH		3
#define FAIL_U {return failure;}
#define UN_WITHGLOBAL

extern WORD *heapgen();

instr_result i_negate()
{
int fu, unsigned_fu;
WORD *tmp;

if (!ISCONST(*ARGPTR(1))) {
	return failure;
	}

  fu = GETSYMBOL(*ARGPTR(1));
  if (ISNEGATED(fu))
    unsigned_fu = fu;
  CHANGE_THE_SIGN(unsigned_fu);
  else unsigned_fu = fu;

printf("type %c\n", symbtab[unsigned_fu].type);

  if (!symbtab[unsigned_fu].type == SYMB_P)
    return failure;
  GENOBJ(*hp, fu, T_CONST);
  CHANGE_THE_SIGN(*hp);
  hp++;
  tmp = hp-1;
  UN_CONST(tmp, NULL, ARGPTR(2), bp);

pc +=INSTR_LENGTH;
return success;
}
