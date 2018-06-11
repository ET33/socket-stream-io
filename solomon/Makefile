CFLAGS= -Wall -g -l pthread -I ./include
all: clean sources
	@gcc queue.o bytestream.o sockets.o client_interface.o client.o -o client $(CFLAGS)
	@gcc queue.o bytestream.o sockets.o server_interface.o server.o -o server $(CFLAGS)
	@rm -r *.o 

sources:	
	@gcc -c ./src/*.c $(CFLAGS)

clean:
	@rm -rf *.o server client solomon.zip 

zip:
	@rm -rf solomon.zip
	@zip solomon.zip *.c *.h Makefile

run:
	@./server &
	@./client
