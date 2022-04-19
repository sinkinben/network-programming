SHELL=/bin/bash
CC=gcc -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast
all:
	$(CC) server.c -lpthread -o server
	$(CC) client.c -o client

clean:
	rm ./server ./client

clients:
	for ((i=0; i<2000; i+=1)); do ./client; done;