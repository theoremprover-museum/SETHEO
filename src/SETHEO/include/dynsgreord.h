#ifndef DYNSGREORD_H
#define DYNSGREORD_H

#include "errors.h"

int  select_subgoal();
/* int  select_subgoal(int delay_mode); */

void  delay_subgoal();
/* void  delay_subgoal(); */

void  set_delay_option();
/* void  set_delay_option(int option); */

/********************************************************************/
/* Makros for the levels of dynsgreord:                             */
/********************************************************************/

#define DYNSGREORD_1\
	for ( sg_index = first_open_sg+1 ; sg_index < nr_sgls ; sg_index++ ) {\
	    sgi = sgls+sg_index;\
	    chk = subgoal_check(sgi,dl_offset,0,0,0,sel.dl);\
	    if (chk.tc > sel.tc) {\
		sel = chk;\
		selected_sg = sg_index;\
	    }\
	}

#define DYNSGREORD_2\
	for ( sg_index = first_open_sg+1 ; sg_index < nr_sgls ; sg_index++ ) {\
	    sgi = sgls+sg_index;\
	    chk = subgoal_check(sgi,dl_offset,sel.ground,0,0,sel.dl);\
	    if (   (chk.ground && !sel.ground)\
		|| (   chk.ground == sel.ground\
		    && (   chk.tc > sel.tc\
			|| (   chk.tc == sel.tc\
			    && chk.fvars < sel.fvars)))) {\
		sel = chk;\
		selected_sg = sg_index;\
	    }\
	}

#define DYNSGREORD_3\
	for ( sg_index = first_open_sg+1 ; sg_index < nr_sgls ; sg_index++ ) {\
	    sgi = sgls+sg_index;\
	    chk = subgoal_check(sgi,dl_offset,0,0,0,sel.dl);\
	    if (   chk.fvars < sel.fvars\
		|| (   chk.fvars == sel.fvars\
		    && chk.tc > sel.tc)) {\
		sel = chk;\
		selected_sg = sg_index;\
	    }\
	}

#define DYNSGREORD_4\
	for (sg_index = first_open_sg+1;\
	     sg_index < nr_sgls  &&  chk.ps == sel.ps;\
	     sg_index++ ) {\
	    sgi = sgls+sg_index;\
	    chk = subgoal_check(sgi,dl_offset,0,1,sel.ps,sel.dl);\
	    if (   chk.ps == sel.ps\
		&& chk.tc > sel.tc) {\
		sel = chk;\
		selected_sg = sg_index;\
	    }\
	}

#define DYNSGREORD_5\
	for (sg_index = first_open_sg+1;\
	     sg_index < nr_sgls  &&  chk.ps == sel.ps;\
	     sg_index++ ) {\
	    sgi = sgls+sg_index;\
	    chk = subgoal_check(sgi,dl_offset,sel.ground,1,sel.ps,sel.dl);\
	    if (   chk.ps == sel.ps\
		&& (   (!sel.ground  &&  chk.ground)\
		    || (   sel.ground == chk.ground\
			&& (   chk.tc > sel.tc\
			    || (   chk.tc == sel.tc\
				&& chk.fvars < sel.fvars))))) {\
		sel = chk;\
		selected_sg = sg_index;\
	    }\
	}

#define DYNSGREORD_6\
	for (sg_index = first_open_sg+1;\
	     sg_index < nr_sgls  &&  chk.ps == sel.ps;\
	     sg_index++ ) {\
	    sgi = sgls+sg_index;\
	    chk = subgoal_check(sgi,dl_offset,0,1,sel.ps,sel.dl);\
	    if (   chk.ps == sel.ps\
		&& (   chk.fvars < sel.fvars\
		    || (   chk.fvars == sel.fvars\
			&& chk.tc > sel.tc))) {\
		sel = chk;\
		selected_sg = sg_index;\
	    }\
	}


/********************************************************************/
/* Makros for the levels of dynsgreord in combination with delay of */
/* subgoals:                                                        */
/********************************************************************/

#define DYNSGREORD_0_DELAY\
	for ( sg_index = first_open_sg+1 ; sg_index < nr_sgls ; sg_index++ ) {\
	    sgi = sgls+sg_index;\
	    chk = subgoal_check(sgi,dl_offset,0,force_ps,sel.ps,sel.dl);\
	    if (    (!force_ps  ||  sel.ps == chk.ps)\
		&&  (!force_fvar  ||  chk.var_common  ||  chk.ground)\
		&&  chk.dl < sel.dl) {\
		sel = chk;\
		selected_sg = sg_index;\
	    }\
	}

#define DYNSGREORD_1_DELAY\
        for ( sg_index = first_open_sg+1 ; sg_index < nr_sgls ; sg_index++ ) {\
	    sgi = sgls+sg_index;\
	    chk = subgoal_check(sgi,dl_offset,0,force_ps,sel.ps,sel.dl);\
	    if (    (!force_ps  ||  sel.ps == chk.ps)\
		&&  (!force_fvar  ||  chk.var_common  ||  chk.ground)\
		&&  (   chk.dl < sel.dl\
		     || (    chk.dl == sel.dl\
			 &&  chk.tc > sel.tc))) {\
		sel = chk;\
		selected_sg = sg_index;\
	    }\
	}

#define DYNSGREORD_2_DELAY\
	for ( sg_index = first_open_sg+1 ; sg_index < nr_sgls ; sg_index++ ) {\
	    sgi = sgls+sg_index;\
	    chk = subgoal_check(sgi,dl_offset,sel.ground,force_ps,sel.ps,sel.dl);\
	    if (    (!force_ps  ||  sel.ps == chk.ps)\
		&&  (!force_fvar  ||  chk.var_common  ||  chk.ground)\
		&&  (  chk.dl < sel.dl\
		     || (   chk.dl == sel.dl\
			 && (   (chk.ground && !sel.ground)\
			     || (   chk.ground == sel.ground\
				 && (   chk.tc > sel.tc\
				     || (   chk.tc == sel.tc\
					 && chk.fvars < sel.fvars))))))) {\
		sel = chk;\
		selected_sg = sg_index;\
	    }\
	}

#define DYNSGREORD_3_DELAY\
	for ( sg_index = first_open_sg+1 ; sg_index < nr_sgls ; sg_index++ ) {\
	    sgi = sgls+sg_index;\
	    chk = subgoal_check(sgi,dl_offset,0,force_ps,sel.ps,sel.dl);\
	    if (    (!force_ps  ||  sel.ps == chk.ps)\
		&&  (!force_fvar  ||  chk.var_common  ||  chk.ground)\
		&&  (   chk.dl < sel.dl\
		     || (   chk.dl == sel.dl\
			 && (   chk.fvars < sel.fvars \
			     || (   chk.fvars == sel.fvars\
				 && chk.tc > sel.tc))))) {\
		sel = chk;\
		selected_sg = sg_index;\
	    }\
	}

#define DYNSGREORD_4_DELAY\
	for (sg_index = first_open_sg+1;\
	     sg_index < nr_sgls  &&  chk.ps == sel.ps;\
	     sg_index++ ) {\
	    sgi = sgls+sg_index;\
	    chk = subgoal_check(sgi,dl_offset,0,1,sel.ps,sel.dl);\
	    if (    (!force_ps  ||  sel.ps == chk.ps)\
		&&  (!force_fvar  ||  chk.var_common  ||  chk.ground)\
		&&  (   chk.dl < sel.dl\
		     || (   chk.dl == sel.dl\
			 && (   chk.ps == sel.ps\
			     && chk.tc > sel.tc)))) {\
		sel = chk;\
		selected_sg = sg_index;\
	    }\
	}

#define DYNSGREORD_5_DELAY\
	for (sg_index = first_open_sg+1;\
	     sg_index < nr_sgls  &&  chk.ps == sel.ps; \
	     sg_index++ ) {\
	    sgi = sgls+sg_index;\
	    chk = subgoal_check(sgi,dl_offset,sel.ground,1,sel.ps,sel.dl);\
	    if (    (!force_ps  ||  sel.ps == chk.ps)\
		&&  (!force_fvar  ||  chk.var_common  ||  chk.ground)\
		&&  (   chk.dl < sel.dl\
		     || (   chk.dl == sel.dl\
			 && (   chk.ps == sel.ps\
			     && (   (!sel.ground  &&  chk.ground)\
				 || (   sel.ground == chk.ground\
				     && (   chk.tc > sel.tc\
					 || (   chk.tc == sel.tc  \
					     && chk.fvars < sel.fvars)))))))){\
		sel = chk;\
		selected_sg = sg_index;\
	    }\
	}

#define DYNSGREORD_6_DELAY\
	for (sg_index = first_open_sg+1;\
	     sg_index < nr_sgls  &&  chk.ps == sel.ps;\
	     sg_index++ ) {\
	    sgi = sgls+sg_index;\
	    chk = subgoal_check(sgi,dl_offset,0,1,sel.ps,sel.dl);\
	    if (    (!force_ps  ||  sel.ps == chk.ps)\
		&&  (!force_fvar  ||  chk.var_common  ||  chk.ground)\
		&&  (   chk.dl < sel.dl\
		     || (   chk.dl == sel.dl\
			 && (   chk.ps == sel.ps\
			     && (   chk.fvars < sel.fvars\
				 || (   chk.fvars == sel.fvars\
				     && chk.tc > sel.tc)))))) {\
		sel = chk;\
		selected_sg = sg_index;\
	    }\
	}

#define SELECT_SUBGOAL\
	switch (dynsgreord) {\
	case 0:\
	    if (delay_mode)\
		DYNSGREORD_0_DELAY\
	    break;\
	case 1:\
	    if (delay_mode)\
		DYNSGREORD_1_DELAY\
	    else\
		DYNSGREORD_1\
	    break;\
	case 2:\
	    if (delay_mode)\
		DYNSGREORD_2_DELAY\
	    else\
		DYNSGREORD_2\
	    break;\
	case 3:\
	    if (delay_mode)\
		DYNSGREORD_3_DELAY\
	    else\
		DYNSGREORD_3\
	    break;\
	case 4:\
	    if (delay_mode)\
		DYNSGREORD_4_DELAY\
	    else\
		DYNSGREORD_4\
	    break;\
	case 5:\
	    if (delay_mode)\
		DYNSGREORD_5_DELAY\
	    else\
		DYNSGREORD_5\
	    break;\
	case 6:\
	    if (delay_mode)\
		DYNSGREORD_6_DELAY\
	    else\
		DYNSGREORD_6\
	    break;\
	default: sam_error("call:select_subgoal:\
                            No such level of dynsgreord",0,0);\
	}

#endif
