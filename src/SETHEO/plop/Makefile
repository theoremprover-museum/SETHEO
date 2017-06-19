CC=gcc

plop: j16.c j16_protos.h
	$(CC) -o plop -O j16.c

clean:
	rm -f core j16.o
	sccs clean

clobber: clean
	rm -f plop


install: plop
	cp plop $(SETHEOHOME)
	chmod a+rx $(SETHEOHOME)/plop
	@echo done

