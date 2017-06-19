/**************************************************************
;;;*****************************************
;;;MODULE NAME:   globals.c
;;;
;;;PARENT     :   setheo/mainpart
;;;
;;;PROJECT    :   pppppppppp
;;;
;;;AUTHOR     :   xxxxxxxxxx
;;;
;;;DATE       :   ddddddd
;;;
;;;DESCRIPTION:   xxxxxxxxxxxx
;;;               xxxxxxxxxxxxxx
;;;MODIF      :   xxxxx (when, what, who)
;;;			11.1.88
;;;			6.6.
;;;			29.7.
;;;			10.10.
;;;			7.11. global vars
;;;			20.2.89 built-ins
;;;			25.4. statistics
;;;			12.6.     "
;;;			24.7.89 DEF'd deref
;;;			25.7. built
;;;			30.8.  "
;;;			12.12.89 is_compl built-in
;;;			3.5.90 builttab --> conf.c
;;;                     26.2.92 local_inf-bounding added by bschorer
;;;			11.3.93 read in externals
;;;			16.9.93	johann	unitlemma
;;;
;;;REMARKS    :   xxxxxxx
;;;
;;;COMPILING  :   yn
;;;
;;;BUGS       :   never any !!
;;;**************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/times.h>
#include "constraints.h"
#include "tags.h"
#include "codedef.h"

#define extern 

#include "machine.h"
#include "symbtab.h"
#ifdef STATPART
#include "sps.h"
#endif

#ifdef UNITLEMMA
#include "unitlemma.h"
#endif

/**/
#include "globals.h"
