all:
	echo please specify proc or thread
clean:
	rm -f *.o
	rm -f searchtest
	rm -f *.h.gch

proc:
	ulimit -S -u 500
	gcc -c multitest_proc.c multitest.h
	gcc -o searchtest multitest_proc.o searchtest.c

thread:
	gcc -c multitest_thread.c multitest.h
	gcc -lpthread -o searchtest multitest_thread.o searchtest.c
