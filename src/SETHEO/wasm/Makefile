#########################################################################
# makefile for `wasm"
# SCCS:		@(#)makefile	7.1 01/02/97
#
# Max Moser, 11.12.91
#########################################################################

include ../make.conf


LINCLDIR=./
INCLDIR=./
#BINDIR=./
GFLAGS=

############################################
PREPROC		= -P -C $(DEFINES) -I$(GINCLDIR)
CFLAGS		= $(G_CFLAGS) -I$(LINCLDIR) -DCONSTRAINTS -I../include
LFLAGS		= $(G_LFLAGS)


CSOURCES 	= geninstr.c opt.c optim.c symbtab.c globals.c main.c error.c
YSOURCES 	= parser.y
LSOURCES 	= scanner.l

SOURCES 	= $(CSOURCES) $(YSOURCES) $(LSOURCES)

OBJS 		= geninstr.o opt.o optim.o symbtab.o globals.o main.o\
                  error.o parser.o scanner.o

LOCALHEADERS	= instr.h wasm.h setheo_hex.h


compile: wasm
	@echo "*** wasm done"

wasm:	$(OBJS) ../sam/cmdline.o
	$(CC) -o wasm $(OBJS) ../sam/cmdline.o $(LFLAGS)
	chmod a+rx wasm

install: wasm
	cp wasm $(BINDIR)/wasm
	chmod a+rx $(BINDIR)/wasm

newrelease:
	sccs edit -r$(WASMRELEASE) SCCS
	sccs delta -y"new major release $(WASMRELEASE)" SCCS

parser.h: FORCE
	$(MAKE) parser.c

parser.c: parser.y wasm.h $(GINCLDIR)/w_parse1.h $(GINCLDIR)/w_parse2.h
	$(CPP) -I. -I$(LINCLDIR) -I$(GINCLDIR) parser.y | \
		sed -e '/^# [0-9]/d' >parser.y.i
	$(YACC) -d parser.y.i
	if [ -f parser.y.i.tab.c ] ; then mv parser.y.i.tab.c parser.c; else mv y.tab.c parser.c ; fi
	if [ -f parser.y.i.tab.h ] ; then mv parser.y.i.tab.h parser.h; else mv y.tab.h parser.h ; fi

scanner.c: scanner.l parser.h wasm.h $(GINCLDIR)/w_scan.h
	$(CPP) -I. -I$(GINCLDIR) $(PREPROC) scanner.l |\
		sed -e '/^# [0-9]/d' |\
		$(LEX) -v -t  >scanner.c

main.c: wasm.h parser.h $(GINCLDIR)/cmdline.h

../sam/cmdline.o: ../sam/cmdline.c
	(cd ../sam; $(MAKE) cmdline.o)
clean:
	rm -f *.o y.output core parser.c scanner.c parser.h
	rm -f scanner.c parser.y.i y.tab.c lex.yy.c
	sccs clean

geninstr.c: wasm.h setheo_hex.h

get:	$(SOURCES) $(LOCALHEADERS)

FORCE:
