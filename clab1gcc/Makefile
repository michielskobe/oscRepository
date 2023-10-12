all: stringmain.c stringfunc.c stringfunc.h
	gcc -Wall -o string stringmain.c stringfunc.c
	
stepwise: sizemain.o sizefunc.o sizefunc.h
	gcc -Wall -o size sizemain.o sizefunc.o sizefunc.h

sizemain.o: sizemain.c
	gcc -Wall -c sizemain.c
	
sizefunc.o: sizefunc.c sizefunc.h
	gcc -Wall -c sizefunc.c sizefunc.h
	
clean:
	rm -rf *.o
