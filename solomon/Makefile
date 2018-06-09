CFLAGS= -Wall -l pthread
all: clean sources
	@gcc queue.o bytestream.o sockets.o client.o -o client $(CFLAGS)
	@gcc queue.o bytestream.o sockets.o server.o -o server $(CFLAGS)

sources:	
	@gcc -c *.c $(CFLAGS)

clean:
	@rm -rf *.o server client queue solomon.zip 

zip:
	@rm -rf solomon.zip
	@zip solomon.zip *.c *.h Makefile

run:
	@./server &
	@./client
