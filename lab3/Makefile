targets = server client

all: $(targets)

server:	server.c error.c heap.c
	gcc $^ -o $@ -Wall

server_shadow: server.c error.c heap.c
	gcc -g $^ -o $@ -Wall -Wshadow

client:	client.c heap.c
	gcc $^ -o $@ -Wall

client_shadow:	client.c heap.c
	gcc -g $^ -o $@ -Wall -Wshadow

.phony:	clean

clean:
	rm $(targets) 
