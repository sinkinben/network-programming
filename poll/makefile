SHELL=/bin/bash
all:
	gcc server.c -lpthread -o server
	gcc client.c -o client

clean:
	rm ./server ./client

clients:
	for ((i=0; i<2000; i+=1)); do ./client; done;