/*########################################################################
# Include File:		setheo_hex.h
# SCCS-Info:		@(#)codedef.h	8.1 05 Jan 1996
# Author:		J. Schumann
# Date:			23.12.87
# Context:		wasm,setheo (SETHEO)
#
# Contents:		definition of the format of a .hex file
#
# Modifications:
#	(when)		(who)		(what)
#     	31.10.88 			common object file
#	16.11.89 			great length of symbol
#       28.09.91 	Goller		HEUMOD adapted from gh
#	23.10.91	Max Moser	`restyled'
#	12.1.95		jsc		LENGTH_CODE, NRSYMB_CODE
#
# Bugs:
#
# --- Forschungsgruppe KI, Inst. fuer Informatik, TU Muenchen ---
########################################################################*/

#ifndef SETHEO_HEX

#  define SETHEO_HEX

/*------------------------------------------------------------------------
- format for code in the ``.hex''-file: printf//scanf format
------------------------------------------------------------------------*/

#define FORMAT_CODE ":%c:%8.8x:%8.8x:%s\n"

/*
                       +------------------------ TAG (see below)
                          +--------------------- hex address
                                +--------------- contents (one WORD)
                                     +---------- string (if symbol)
*/

#define FORM_READ   ":%c:%x:%x:%s\n"

/*------------------------------------------------------------------------
- TAGS for the .hex file
------------------------------------------------------------------------*/

#define MEM_CODE 	'C'	/* put data into code memory	 */
#define HEAP_CODE	'H'	/* data into heap 		 */
#define EXT_CODE	'X'	/* external Def.		 */
#define SYMB_CODE	'Y'	/* symbol definition		 */
#define LAB_CODE	'L'	/* label definition		 */
#define CLAUSE_CODE	'K'	/* clause code			 */
#define RED_CODE	'R'	/* reduction label code		 */
#define START_CODE	'S'	/* Start code */
#define RESTART_CODE 	'I'	/* restart code			 */
#define END_CODE	'E'	/* end of code			 */
#define MAXADDR_CODE	'M'	/* max. memory address		 */
#define NRSYMB_CODE	'N'	/* number of symbols     	 */

/*------------------------------------------------------------------------
- maximum length of a symbol
------------------------------------------------------------------------*/
#define M_SYMB_LENG	42	/* max. length of symbols	 */

/*------------------------------------------------------------------------
- tags for coding the type of a symbol
------------------------------------------------------------------------*/

#define SYMB_P		1	/* symbol coding : predicate	 */
#define SYMB_V		2
#define SYMB_C		3
#define SYMB_F		4
#define SYMB_GLOBAL	5


#endif
/*######################################################################*/


