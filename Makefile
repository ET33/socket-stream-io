all: clean tcp_server.o tcp_client.o
	@gcc tcp_client.o -o tcp_client -g -Wall -std=c99
	@gcc tcp_server.o -o tcp_server -g -Wall -std=c99

tcp_server.o:	
	@gcc -c tcp_server.c -g -Wall -std=c99

tcp_client.o:
	@gcc -c tcp_client.c -g -Wall -std=c99

clean:
	@rm -rf *.o tcp_server tcp_client solomon.zip

zip:
	@rm -rf solomon.zip
	@zip solomon.zip *.c *.h 

run:
	@./tcp_server &
	@./tcp_client