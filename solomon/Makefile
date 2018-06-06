CFLAGS= -I ./
all: clean server.o client.o main.o
	@gcc client.o -o client -g -Wall -std=c99 $(CFLAGS)
	@gcc server.o main.o -o server -g -Wall -std=c99 $(CFLAGS)
server.o:	
	@gcc -c server.c -g -Wall -std=c99 $(CFLAGS)

client.o:
	@gcc -c client.c -g -Wall -std=c99 $(CFLAGS)

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
