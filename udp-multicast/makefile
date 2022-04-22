SHELL=/bin/bash
all:
	gcc server.c -lpthread -o server
	gcc client.c -o client

clean:
	rm ./server ./client

test:
	for ((i=0; i<10; i+=1)); do (./client &); done;
	./server
	