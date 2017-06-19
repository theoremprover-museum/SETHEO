#########################################################################
# makefile for `setheo" include-dir
# SCCS:		@(#)makefile	15.2 10/28/97
#
# J. Schumann 29.6.93
#
#	3/12/96		inwasm headers
#########################################################################

MAKEFILE=makefile
BINSUFFIX = o

include ../make.conf

SAMHEADERS= antilemma.h bounds.h c_check_1.h c_check_2.h \
	constraints.h debug.h defaults.h deref.h disp.h \
	dynsgreord.h errors.h full.h fvars.h genlemma.h \
	globals.h i_fail.h i_nalloc.h i_ndealloc.h i_orbranch.h \
	init.h instr.h iteration.h itime.h lcode.h main.h \
	machine.h match.h proto_unify.h ptree.h reg_constr.h \
	sampars.h sighand.h signals.h symbtab.h taut_constr.h \
	termcomplexity.h unifeq.h unification.h unifred.h unify.h \
	unifyred.h unifyt.h unitlemma.h univ_constr.h weighted_depth.h 

SPTHEOHEADERS= pvm3.h sps.h 

IMPORTEDHEADERS = \
	path/path_interface.h	\
	path/define.h		\
	path/list.h		\
	path/newmemory.h	\
	path/path.h		\
	path/path_types.h	\
	path/types.h

WASMHEADERS= optim.h

INWASMHEADERS= defines.h extern.h ficonst.h hexout.h otypes.h stypes.h \
	symb.h types.h w_types.h

COMMONHEADERS= cmdline.h codedef.h tags.h unistd.h

CREATEDHEADERS= conf.h conf.old.h parser.h w_parse1.h w_parse1.old.h \
	w_parse2.h w_parse2.old.h w_scan.h w_scan.old.h opcodes.h \
	opcodes.old.h uopcodes.h

#########################################################################
newrelease:
	sccs edit -r$(INCLUDERELEASE) SCCS
	sccs delta -y"new major release $(INCLUDERELEASE)" SCCS
	(cd path; \
		sccs edit -r$(INCLUDERELEASE) SCCS && \
	sccs delta -y"new major release $(INCLUDERELEASE)" SCCS)

clean:
	rm -f *.o y.output y.tab.c lex.yy.c core $(CREATEDHEADERS)
	sccs clean
	(cd path; sccs clean)

clobber:
	rm -f setheo *.o y.output y.tab.c lex.yy.c core $(CREATEDHEADERS)
	sccs clean
	(cd path; sccs clean)

delget:
	sccs delget $(SAMHEADERS) $(WASMHEADERS) $(INWASMHEADERS) \
	$(COMMONHEADERS) $(SPTHEOHEADERS) 
	(cd path; sccs delget SCCS)

get:
	sccs get $(SAMHEADERS) $(WASMHEADERS) $(INWASMHEADERS) \
	$(COMMONHEADERS) $(SPTHEOHEADERS)
	(cd path; sccs get SCCS)
