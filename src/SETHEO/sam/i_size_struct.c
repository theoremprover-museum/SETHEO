/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: i_size_struct.c
/* VERSION:
/* DATE:
/* AUTHOR:
/* NAME OF FILE:
/* DESCR:
/* MOD:
/* BUGS:
/******************************************************/

#include <stdio.h>
#include "constraints.h"
#include "tags.h"
#include "machine.h"

#define INSTR_LENGTH		2


instr_result i_size_struct()
{
    WORD           *ga;
    int             s;

    ga = ARGV (0);

    s = get_size (ga);


    ga = ARGV (1);

    if (GETTAG (*ga) == T_CONST) {
	if (GETVAL (*ga) != s)
	    return 1;
	else
	    return 0;
    }
    else if (ISVAR (*ga)) {
	trail_var (ga);
	GENOBJ (*ga, (WORD) s, T_CONST);
        pc +=INSTR_LENGTH;
	return success;
    }
    else
	sam_error ("size: second arg must be const or var", ga, 1);
    return failure;
}

/************************************************************/
static int      get_size (ga)
    WORD           *ga;
{
    WORD           *gaa;
    int             s;

    gaa = deref (ga, bp);
    switch (GETTAG (*gaa)) {
      case T_CRTERM:
      case T_GTERM:
      case T_NGTERM:
	gaa++;
	s = 0;
	while (GETTAG (*gaa) != T_EOSTR) {
	    s += get_size (gaa);
	    gaa++;
	}
	return s + 1;
	break;
      default:
	return 1;
    }
}
