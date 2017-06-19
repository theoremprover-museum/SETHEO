XPTREEHOME = /home/schumann/setheo/xptree



#STATLIBS = -L. -Bstatic -lXawRol -lXmu -lXt -lXext -lX11 -Bdynamic -lm -ll -ly
#LIBS = -lXaw -lXmu -lXt -lXext -lX11 -lm -ll -ly
#DEFS = 

###############################################
# for solaris 2.5, SunPro CC
#
DEFS = -I/usr/local/include
CC = gcc
#STATLIBS = -L. -L/usr/openwin/lib -Bstatic -lXawRol \
#JSC STATLIBS = -L. -L/usr/X11/lib -Bstatic -lXawRol \
STATLIBS = -L. -L/Developer/SDKs/MacOSX10.4u.sdk/usr/X11R6/lib -Bstatic -lXawRol \
-lXmu \
-lXt -lXext -lX11 \
-Bdynamic \
-lSM -lsocket \
-lICE \
-lw -lnsl \
-lresolv  \
-lm -ll -ly
#LIBS = -L/usr/X11/lib -lXaw -lXmu -lXt -lXext -lX11 -lresolv -lm -ll -ly
#JSCLIBS = -L/usr/X11/lib -lXaw -lXmu -lXt -lXext -lX11 -lresolv -lm -lfl
#JSC LIBS = -L/Developer/SDKs/MacOSX10.4u.sdk/usr/X11R6/lib -lXaw -lXmu -lXt -lXext -lX11 -lresolv -lm -lfl
LIBS = -L/usr/lib/x86_64-linux-gnu/ -lXaw -lXmu -lXt -lXext -lX11 -lresolv -lm -lfl
#SW = -fast
SW = -Dlinux
AWLIBPATH=/usr/lib/x86_64-linux-gnu/
RANLIB=echo

###############################################
# for solaris 2.3, SunPro CC
#
#DEFS = -I/usr/local/include
#CC = /usr/local/jessen/bin/cc
#STATLIBS = -L. -L/usr/openwin/lib -Bstatic -lXawRol -lXmu -Bdynamic -lXt -lXext -lX11 -lm -ll -ly
#LIBS = -L/usr/openwin/lib -lXaw -lXmu -lXt -lXext -lX11 -lm -ll -ly
#SW = -fast
#AWLIBPATH=/usr/local/lib
#RANLIB=echo

###############################################
# for sunOS4.1, cc
#
#DEFS = -I/usr/local/include
#CC = cc
#STATLIBS = -L. -L/usr/openwin/lib -Bstatic -lXawRol -lXmu -lXt -lXext -lX11 -Bdynamic -lm -ll -ly
#LIBS = -L/usr/openwin/lib -lXaw -lXmu -lXt -lXext -lX11 -lm -ll -ly
#SW = -O
#AWLIBPATH=/lib
#RANLIB=ranlib

COPTS = 
#hphalle: COPTS = -I/usr/local/X11R5/include -L/usr/local/X11R5/lib

OBJS = infix.o xptree.o parser.o scanner.o symbtab.o fold.o utils.o
#JSC2008 OBJS2 = $(OBJS) Tree_dynamic.o
OBJS2 = $(OBJS)

OBJS2_STATIC = $(OBJS) Tree.o


all: xptree


xptree_static: xptree.h $(OBJS2_STATIC) libXawRol.a
	$(CC) $(SW) -o xptree_static $(OBJS2_STATIC) $(STATLIBS) $(DEFS) $(COPTS)

libXawRol.a: Tree.o
	cp $(AWLIBPATH)/libXaw7.a libXawRol.a
	ar d libXawRol.a $(XPTREEHOME)/Tree.o
	$(RANLIB) libXawRol.a

xptree: xptree.h $(OBJS2)
	$(CC) $(SW) -o xptree $(OBJS2) $(LIBS) $(DEFS) $(COPTS)

origtree: xptree.h $(OBJS)
	$(CC) $(SW) -o xptree $(OBJS) $(LIBS) $(DEFS) $(COPTS)

ot: xptree.h $(OBJS) Treem.o
	$(CC) $(SW) -o xptree $(OBJS) Treem.o $(LIBS) $(DEFS) $(COPTS)

xptree.o: xptree.c xptree.h xptree.ad.h
	$(CC) $(SW) -c xptree.c $(DEFS) $(COPTS)

scanner.o: scanner.c parser_if.h
	$(CC) $(SW) -c scanner.c $(DEFS) $(COPTS)

Tree.o: Tree.c TreeP.h
	$(CC) $(SW) -c Tree.c $(DEFS) $(COPTS)

fold.c: treeh.xbm treeh_hl.xbm treev.xbm treev_hl.xbm

infix.c: xptree.h infix.h

TreeP.h: Tree.h

Tree_dynamic.o: Tree_dynamic.c TreeP.h
	$(CC) $(SW) -c Tree_dynamic.c $(DEFS) $(COPTS)

.c.o: xptree.h
	$(CC) $(SW) -c  $(DEFS)  $(COPTS) $<

parser.c parser_if.h: parser.y
	yacc -d parser.y
	mv y.tab.c parser.c
	mv y.tab.h parser_if.h

scanner.c: scanner.l
	lex scanner.l
	mv lex.yy.c scanner.c

xptree.ad.h: xptree.ad ad2c
	chmod +x ad2c
	./ad2c xptree.ad > xptree.ad.h

clean:
	rm -f *.o scanner.c parser.c parser_if.h xptree.ad.h libXawRol.a
