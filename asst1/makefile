all: memgrind

clean:
	rm -f *.o
	rm -f memgrind
	rm -f *.h.gch

malloc.o: 
	gcc -c mymalloc.c mymalloc.h

memgrind: malloc.o
	gcc -o memgrind mymalloc.o memgrind.c
