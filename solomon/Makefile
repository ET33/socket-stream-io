CFLAGS= -Wall
all: clean server.o client.o main.o
	@gcc client.o -o client $(CFLAGS)
	@gcc server.o main.o -o server $(CFLAGS)
server.o:	
	@gcc -c server.c $(CFLAGS)

client.o:
	@gcc -c client.c $(CFLAGS)

clean:
	@rm -rf *.o server client solomon.zip 

zip:
	@rm -rf solomon.zip
	@zip solomon.zip *.c *.h 

run:
	@./server &
	@./client

main.o:
	@gcc -c main.c $(CFLAGS)
