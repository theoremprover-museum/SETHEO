#******************************************************
#*     PIL System
#*     $Id: Makefile,v 1.3 2002/09/13 17:50:08 schumann Exp $
#*
#*     (c) Johann Schumann, Klaus Wagner
#*     TU Muenchen and NASA Ames
#*
#*     $Log: Makefile,v $
#*     Revision 1.3  2002/09/13 17:50:08  schumann
#*     added -g
#*
#*     Revision 1.2  2002/07/05 16:26:23  schumann
#*     bugfix for sun /bin/sh
#*
#*     Revision 1.1.1.1  2001/07/16 18:16:09  schumann
#*     new CVS project
#*
#*****************************************************/
LIBDIR=$(PILHOME)/lib/
BINDIR=$(PILHOME)/bin/

PL = swipl
CC = gcc
LIBS = -L/opt/local/lib -lfl
LEX = flex
YACC = bison -y -d -v 
CFLAGS = -O -DLIBDIR=\"${LIBDIR}\" -g

PROGRAM = pil 
SETHEO2BLOCK = setheo2block

all:	FORCE
	PILHOME=$(PILHOME); \
	export PILHOME; \
         if [ -z "$$PILHOME$$EMPTY" ] ; then PILHOME=`pwd`/..; fi; \
	 echo $$PILHOME; \
	 make install; 
	
	
all_binaries: ${PROGRAM} ${SETHEO2BLOCK}

install: all_binaries
	cp ${PROGRAM} ${BINDIR}
	cp ${SETHEO2BLOCK} ${BINDIR}
	
$(SETHEO2BLOCK) : FORCE
	echo '[setheo2block].' \\n'qsave_program(setheo2block,[goal=start_working]).' >L ; 
	bash -c "cat L | $(PL) ";
#	echo '[setheo2block].qsave_program($(SETHEO2BLOCK),[goal=start_working]).' | $(PL) ; 

grammar: pil.gr

pil.gr: pil.y
	sed -e '/[	]\{/,/[ 	]\}/d' pil.y >pil.gr

pil: 	pill.o pily.o  functions.o main.o pil.h 
	${CC} ${CFLAGS} -o pil pill.o pily.o functions.o main.o ${LIBS}

clean::
	rm -f *.o core pill.c pily.c pily.h y.output

clobber: clean
	rm -f ${PROGRAM} ${SETHEO2BLOCK}

pill.c:	pil.l pil.h
	${LEX} pil.l
	mv lex.yy.c pill.c

pill.o:	pill.c pily.h pil.h

pily.h pily.c:	pil.y pil.h
	${YACC} pil.y
	mv y.tab.c pily.c
	mv y.tab.h pily.h

main.o: pil.h help.c main.c

FORCE:
