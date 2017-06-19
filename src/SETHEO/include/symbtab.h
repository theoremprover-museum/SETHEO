/******************************************************/
/*    S E T H E O                                     */
/*                                                    */
/* FILE: symbtab.h                                    */
/* VERSION:                                           */
/* DATE: 23.12.87                                     */
/*	27.1.88                                       */
/* AUTHOR: J. Schumann                                */
/* NAME OF FILE:                                      */
/* DESCR:                                             */
/* MOD:                                               */
/*		31.10.88                              */
/*              23.7.91 symbtab_last added by Goller  */
/*		29.6.93 jsc include-defines           */
/*		17.9.93	jsc	name = char *         */
/* BUGS:                                              */
/******************************************************/

#ifndef SYMBTAB_H
#define SYMBTAB_H

#include "codedef.h"


typedef struct {
	char 	*name;
	char 	type;
	int 	arity;
	} syel;

extern syel *symbtab;
extern syel *symbtab_last;

extern char symbflg;
extern int nr_symbols;

#endif
