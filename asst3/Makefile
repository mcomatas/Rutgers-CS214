all: client server
clean:
	rm -f *.o
	rm -f server
	rm -f client
	rm -f *.h.gch
client:
	gcc DUMBclient.c -o client
server:
	gcc -lpthread DUMBserver.c -o server
