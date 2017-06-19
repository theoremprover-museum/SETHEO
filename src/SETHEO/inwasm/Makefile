MAKEFILE = makefile

include ../make.conf

#CFLAGS = -c $(INWASM_CFLAGS)
CFLAGS = -g -c $(INWASM_CFLAGS)
YFLAGS = 
#YFLAGS = -v -t 
LFLAGS =
LINTFLAGS = -cxh
BINSUFFIX = o

YACC=yacc

INDIR = .
PREDIR = preproc
CODEDIR = codegen
INCLDIR = $(GINCLDIR)

HSOURCES = defines.h extern.h otypes.h \
	   symb.h types.h w_types.h

MAINSOURCES = main.c memoryhandler.c signals.c filehandler.c generator.c output.c globals.c symb.c built_ins.c \
	wunif.c newunify.c unify.c lopout.c

CSOURCES = $(MAINSOURCES) y.tab.c 

PRESOURCES = cl_reordering.c connopt.c fanning.c \
	hlpfkt.c oconstr.c purity.c sconstr.c \
	sg_reordering.c symconstr.c tconstr.c weightfunc.c \
	constrsu.c

CODESOURCES = cgen.c cgutil.c cgbuilt.c dumpst.c cgarith.c cgconstr.c \
	 instrs.c hexout.c

WASMSOURCES = ../wasm/optim.c

SOURCES = $(CSOURCES) $(PRESOURCES) $(CODESOURCES)

OBJS = main.o memoryhandler.o signals.o filehandler.o generator.o output.o globals.o symb.o built_ins.o \
       wunif.o newunify.o unify.o lopout.o y.tab.o \
       cl_reordering.o connopt.o fanning.o \
       hlpfkt.o oconstr.o purity.o sconstr.o \
       sg_reordering.o symconstr.o tconstr.o weightfunc.o \
       constrsu.o cgen.o cgutil.o cgbuilt.o dumpst.o cgarith.o \
       cgconstr.o instrs.o hexout.o optim.o

.KEEP_STATE:

install: inwasm
	cp inwasm $(BINDIR)
	chmod a+rx $(BINDIR)/inwasm
	@echo done

inwasm: $(OBJS) 
	$(CC) -o inwasm $(OBJS) $(G_LFLAGS)

newrelease:
	sccs edit -r$(INWASMRELEASE) SCCS
	sccs delta -y"new major release $(INWASMRELEASE)" SCCS
	(cd $(PREDIR); \
		sccs edit -r$(INWASMRELEASE) SCCS && \
	sccs delta -y"new major release $(INWASMRELEASE)" SCCS)
	(cd $(CODEDIR); \
		sccs edit -r$(INWASMRELEASE) SCCS && \
	sccs delta -y"new major release $(INWASMRELEASE)" SCCS)

#
# parser.y includes the scanner (lex.yy.c)
#
y.tab.o: y.tab.c lex.yy.c 
	$(CC) $(CFLAGS) y.tab.c

y.tab.c: pars.y 
	$(YACC) pars.y 
	@if [ -f pars.tab.c ] ; then mv pars.tab.c y.tab.c ; fi

lex.yy.c: scan.l
	$(LEX) -v scan.l

optim.o: $(WASMSOURCES)
	$(CC) $(CFLAGS) $(WASMSOURCES)

clean:
	rm -f *.o y.output y.tab.c lex.yy.c core $(GINCLDIR)/uopcodes.h
	(cd $(PREDIR); sccs clean)
	(cd $(CODEDIR); sccs clean)
	sccs clean


tags: $(MAINSOURCES) $(CODEDIR)/*.c  $(PREDIR)/*.c 
	@echo ctags -twuf ...
	@rm -f tags; touch tags
	@ctags -twuf $(INDIR)/tags pars.y scan.l $(MAINSOURCES)
	@ctags -twuf $(INDIR)/tags $(GINCLDIR)/*.h
	@ctags -twuf $(INDIR)/tags $(PREDIR)/*.c
	@ctags -twuf $(INDIR)/tags $(CODEDIR)/*.c

lint:
	@echo lint $(LINTFLAGS) -Iinclude -DLINT -DBLIBDIR=\"$(BINDIR)\" ...
	@lint -Iinclude -DLINT -DBLIBDIR=\"$(BINDIR)\"  $(CSOURCES) $(CODEDIR)/*.c $(PREDIR)/*.c

delta:
	sccs delta `sccs tell`
	(cd $(PREDIR); sccs delta `sccs tell`)
	(cd $(CODEDIR); sccs delta `sccs tell`)
	(cd $(INCLDIR); sccs delta `sccs tell`)

#
# this file contains stubs for all undefined opcodes
# OPCODE_.
# This file is included in built_ins.c and instrs.c
# and is generated from information contained in these
# files

$(INCLDIR)/uopcodes.h: codegen/instrs.c built_ins.c makefile
	fgrep 'OPCODE_' codegen/instrs.c built_ins.c | \
	sed -e 's/^.*\(OPCODE_[A-Za-z_0-9]*\).*$$/\1/' | \
	awk 'BEGIN{print "/* uopcodes.h: do not edit by hand */"} \
	{print "#ifndef " $$1 ; print "#define "$$1" OPCODE_break"; \
	      print "#endif"}'  >$(INCLDIR)/uopcodes.h

depend:
# do not consider conditional included files
# and <> included files
#
# do not resolve dependencies among include files
# makefile MUST be readable
#
	-sccs get $(MAINSOURCES)
	@for i in ${MAINSOURCES}; do \
		( /bin/grep '^#[ 	]*include[ 	]*"' $$i | sed -n \
			-e 's/[^"]*"\([^"]*\)".*/$$(INCLDIR)\/\1/' \
			-e H -e '$$g' -e '$$s/\n/ /g' \
			-e '$$s/.*/'$$i': '$$i' &\
				$$(CC) $$(CFLAGS) '$$i/\
			-e '$$s/\.c:/.$$(BINSUFFIX):/p' \
			>> makedep); done
	-(cd $(CODEDIR); sccs get $(CODESOURCES))
	@for i in ${CODESOURCES}; do \
		( /bin/grep '^#[ 	]*include[ 	]*"' ${CODEDIR}/$$i | sed -n \
			-e 's/[^"]*"\([^"]*\)".*/$$(INCLDIR)\/\1/' \
			-e H -e '$$g' -e '$$s/\n/ /g' \
			-e '$$s/.*/'$$i': $$(CODEDIR)'\\/$$i' &\
				$$(CC) $$(CFLAGS) $$(CODEDIR)'\\/$$i/\
			-e '$$s/\.c:/.$$(BINSUFFIX):/p' \
			>> makedep); done
	-(cd $(PREDIR); sccs get $(PRESOURCES))
	@for i in ${PRESOURCES}; do \
		( /bin/grep '^#[ 	]*include[ 	]*"' ${PREDIR}/$$i | sed -n \
			-e 's/[^"]*"\([^"]*\)".*/$$(INCLDIR)\/\1/' \
			-e H -e '$$g' -e '$$s/\n/ /g' \
			-e '$$s/.*/'$$i': $$(PREDIR)'\\/$$i' &\
				$$(CC) $$(CFLAGS) $$(PREDIR)'\\/$$i/\
			-e '$$s/\.c:/.$$(BINSUFFIX):/p' \
			>> makedep); done
	@echo '/^# DO NOT DELETE THIS LINE/+2,$$d' >eddep
	@echo '$$r makedep' >>eddep
	@echo 'w' >>eddep
	@cp $(MAKEFILE) $(MAKEFILE).bak
	ed - $(MAKEFILE) < eddep
	@rm -f eddep makedep
	@echo '# DEPENDENCIES MUST END AT END OF FILE' >> $(MAKEFILE)
	@echo '# IF YOU PUT STUFF HERE IT WILL GO AWAY' >> $(MAKEFILE)
	@echo '# see make depend above' >> $(MAKEFILE)
	@rm -f $(MAKEFILE).bak


#
# dependencies for .h files
#
$(INCLDIR)/symb.h: $(INCLDIR)/types.h

$(INCLDIR)/types.h: $(INCLDIR)/defines.h

$(INCLDIR)/w_types.h: $(INCLDIR)/types.h


# DO NOT DELETE THIS LINE
# DEPENDENCIES MUST END AT END OF FILE
main.$(BINSUFFIX): main.c  $(INCLDIR)/types.h $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) main.c
memoryhandler.$(BINSUFFIX): memoryhandler.c  $(INCLDIR)/types.h $(INCLDIR)/extern.h 
			$(CC) $(CFLAGS) memoryhandler.c
signals.$(BINSUFFIX): signals.c  $(INCLDIR)/types.h $(INCLDIR)/extern.h 
			$(CC) $(CFLAGS) signals.c
filehandler.$(BINSUFFIX): filehandler.c  $(INCLDIR)/types.h $(INCLDIR)/extern.h 
			$(CC) $(CFLAGS) filehandler.c
generator.$(BINSUFFIX): generator.c  $(INCLDIR)/types.h $(INCLDIR)/extern.h 
			$(CC) $(CFLAGS) generator.c
output.$(BINSUFFIX): output.c  $(INCLDIR)/symb.h $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) output.c
globals.$(BINSUFFIX): globals.c  $(INCLDIR)/symb.h $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) globals.c
symb.$(BINSUFFIX): symb.c  $(INCLDIR)/symb.h $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) symb.c
built_ins.$(BINSUFFIX): built_ins.c  $(INCLDIR)/types.h $(INCLDIR)/extern.h $(INCLDIR)/opcodes.h $(INCLDIR)/uopcodes.h
			$(CC) $(CFLAGS) built_ins.c
wunif.$(BINSUFFIX): wunif.c  $(INCLDIR)/symb.h $(INCLDIR)/extern.h $(INCLDIR)/defines.h
			$(CC) $(CFLAGS) wunif.c
newunify.$(BINSUFFIX): newunify.c  $(INCLDIR)/symb.h $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) newunify.c
unify.$(BINSUFFIX): unify.c  $(INCLDIR)/symb.h $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) unify.c
lopout.$(BINSUFFIX): lopout.c  $(INCLDIR)/symb.h $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) lopout.c
cgen.$(BINSUFFIX): $(CODEDIR)/cgen.c  $(INCLDIR)/types.h $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) $(CODEDIR)/cgen.c
cgutil.$(BINSUFFIX): $(CODEDIR)/cgutil.c  $(INCLDIR)/symb.h $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) $(CODEDIR)/cgutil.c
cgbuilt.$(BINSUFFIX): $(CODEDIR)/cgbuilt.c  $(INCLDIR)/symb.h $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) $(CODEDIR)/cgbuilt.c
dumpst.$(BINSUFFIX): $(CODEDIR)/dumpst.c  $(INCLDIR)/symb.h $(INCLDIR)/extern.h $(INCLDIR)/codedef.h
			$(CC) $(CFLAGS) $(CODEDIR)/dumpst.c
cgarith.$(BINSUFFIX): $(CODEDIR)/cgarith.c  $(INCLDIR)/symb.h $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) $(CODEDIR)/cgarith.c
cgconstr.$(BINSUFFIX): $(CODEDIR)/cgconstr.c  $(INCLDIR)/types.h $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) $(CODEDIR)/cgconstr.c
instrs.$(BINSUFFIX): $(CODEDIR)/instrs.c  $(INCLDIR)/types.h $(INCLDIR)/extern.h $(INCLDIR)/hexout.h $(INCLDIR)/opcodes.h $(INCLDIR)/uopcodes.h
			$(CC) $(CFLAGS) $(CODEDIR)/instrs.c
hexout.$(BINSUFFIX): $(CODEDIR)/hexout.c  $(INCLDIR)/types.h $(INCLDIR)/hexout.h $(INCLDIR)/extern.h $(INCLDIR)/tags.h $(INCLDIR)/codedef.h $(INCLDIR)/optim.h
			$(CC) $(CFLAGS) $(CODEDIR)/hexout.c
cl_reordering.$(BINSUFFIX): $(PREDIR)/cl_reordering.c  $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) $(PREDIR)/cl_reordering.c
connopt.$(BINSUFFIX): $(PREDIR)/connopt.c  $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) $(PREDIR)/connopt.c
fanning.$(BINSUFFIX): $(PREDIR)/fanning.c  $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) $(PREDIR)/fanning.c
hlpfkt.$(BINSUFFIX): $(PREDIR)/hlpfkt.c  $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) $(PREDIR)/hlpfkt.c
oconstr.$(BINSUFFIX): $(PREDIR)/oconstr.c  $(INCLDIR)/extern.h $(INCLDIR)/otypes.h $(INCLDIR)/defines.h
			$(CC) $(CFLAGS) $(PREDIR)/oconstr.c
purity.$(BINSUFFIX): $(PREDIR)/purity.c  $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) $(PREDIR)/purity.c
sconstr.$(BINSUFFIX): $(PREDIR)/sconstr.c  $(INCLDIR)/extern.h $(INCLDIR)/defines.h
			$(CC) $(CFLAGS) $(PREDIR)/sconstr.c
sg_reordering.$(BINSUFFIX): $(PREDIR)/sg_reordering.c  $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) $(PREDIR)/sg_reordering.c
symconstr.$(BINSUFFIX): $(PREDIR)/symconstr.c  $(INCLDIR)/extern.h $(INCLDIR)/defines.h
			$(CC) $(CFLAGS) $(PREDIR)/symconstr.c
tconstr.$(BINSUFFIX): $(PREDIR)/tconstr.c  $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) $(PREDIR)/tconstr.c
weightfunc.$(BINSUFFIX): $(PREDIR)/weightfunc.c  $(INCLDIR)/w_types.h
			$(CC) $(CFLAGS) $(PREDIR)/weightfunc.c
constrsu.$(BINSUFFIX): $(PREDIR)/constrsu.c  $(INCLDIR)/extern.h
			$(CC) $(CFLAGS) $(PREDIR)/constrsu.c
# DEPENDENCIES MUST END AT END OF FILE
# IF YOU PUT STUFF HERE IT WILL GO AWAY
# see make depend above
