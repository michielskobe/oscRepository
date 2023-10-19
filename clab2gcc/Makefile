all: lab2main.c lab2func.c lab2func.h
	gcc -g -Wall -o lab2 stringmain.c lab2func.c
	
stepwise: lab2main.o lab2func.o lab2func.h
	gcc -Wall -o size lab2main.o lab2func.o lab2func.h

lab2main.o: lab2main.c
	gcc -Wall -c lab2main.c
	
lab2func.o: lab2func.c lab2func.h
	gcc -Wall -c sizefunc.c lab2func.h
	
clean:
	rm -rf *.o
