CFLAGS= -Wall
all: clean queue.o server.o client.o main.o
	@gcc queue.o client.o -o client $(CFLAGS)
	@gcc queue.o server.o main.o -o server $(CFLAGS)
server.o:	
	@gcc -c server.c $(CFLAGS)

client.o:
	@gcc -c client.c $(CFLAGS)

queue.o:
	@gcc -c queue.c $(CFLAGS)

clean:
	@rm -rf *.o server client queue solomon.zip 

zip:
	@rm -rf solomon.zip
	@zip solomon.zip *.c *.h 

run:
	@./server &
	@./client
