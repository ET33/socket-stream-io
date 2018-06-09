CFLAGS= -Wall -lpthread
all: clean sources
	@gcc sockets.o client.o -o client $(CFLAGS)
	@gcc sockets.o server.o -o server $(CFLAGS)
sources:	
	@gcc -c *.c $(CFLAGS)

clean:
	@rm -rf *.o server client solomon.zip 

zip:
	@rm -rf solomon.zip
	@zip solomon.zip *.c *.h Makefile

run:
	@./server &
	@./client
