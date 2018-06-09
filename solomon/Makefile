CFLAGS= -Wall
all: clean sources
	@gcc client.o -o client $(CFLAGS)
	@gcc server.o main.o -o server $(CFLAGS)
sources:	
	@gcc -c *.c $(CFLAGS)

clean:
	@rm -rf *.o server client solomon.zip 

zip:
	@rm -rf solomon.zip
	@zip solomon.zip *.c *.h 

run:
	@./server &
	@./client
